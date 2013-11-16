#include <pebble.h>

#include "Blockslide-Date_2.0.h"


#define TRUE  1
#define FALSE 0

// Languages
#define LANG_DUTCH 0
#define LANG_ENGLISH 1
#define LANG_FRENCH 2
#define LANG_GERMAN 3
#define LANG_SPANISH 4
#define LANG_MAX 5
// Language for the day of the week
#define LANG_CUR LANG_SPANISH

#define USDATE FALSE
#define WEEKDAY TRUE


#define TILEW 22
#define TILEH 13
#define DTILEW 5
#define DTILEH 4
#define HSPACE 8
#define DHSPACE 4
#define VSPACE 8
#define DIGIT_CHANGE_ANIM_DURATION 800
#define STARTDELAY 1500
#define SCREENW 144
#define SCREENH 168
#define CX 72
#define CY 84
#define NUMSLOTS 10

#if LANG_CUR == LANG_DUTCH
const char weekDay[7][3] = { "ZO", "MA", "DI", "WO", "DO", "VR", "ZA" };	// Dutch
#elif LANG_CUR == LANG_ENGLISH
const char weekDay[7][3] = { "SU", "MO", "TU", "WE", "TH", "FR", "SA" };	// English
#elif LANG_CUR == LANG_FRENCH
const char weekDay[7][3] = { "DI", "LU", "MA", "ME", "JE", "VE", "SA" };	// French
#elif LANG_CUR == LANG_GERMAN
const char weekDay[7][3] = { "SO", "MO", "DI", "MI", "DO", "FR", "SA" };	// German
#elif LANG_CUR == LANG_SPANISH
const char weekDay[7][3] = { "DO", "LU", "MA", "MI", "JU", "VI", "SA" };	// Spanish
#else // Fallback to English
const char weekDay[7][3] = { "SU", "MO", "TU", "WE", "TH", "FR", "SA" };	// English
#endif

typedef struct {
	Layer *layer;
	int   prevDigit;
	int   curDigit;
	int   tileWidth;
	int   tileHeight;
	uint32_t normTime;
} digitSlot;

Window *window;

digitSlot slot[NUMSLOTS]; // 4 big digits for the hour, 6 small for the date
int startDigit[NUMSLOTS] = {
	'B'-'0',
	'L'-'0',
	'O'-'0',
	'C'-'0',
	'K'-'0',
	'S'-'0',
	'L'-'0',
	'I'-'0',
	'D'-'0',
	'E'-'0'
};

bool clock_12 = false;
bool splashEnded = false;

AnimationImplementation animImpl;
Animation *anim;

GRect slotFrame(int i) {
	int x, y;
	int w = slot[i].tileWidth*3;
	int h = slot[i].tileHeight*5;
	
	if (i<4) {
		// Hour slot -> big digits
		if (i%2) {
			x = CX + HSPACE/2; // i = 1 or 3
		} else {
			x = CX - HSPACE/2 - w; // i = 0 or 2
		}
		
		if (i<2) {
			y = 1;
		} else {
			y = 1 + h + VSPACE;
		}
	} else {
		// Date slot -> small digits
		x = CX + (i-7)*(w+DHSPACE) + DHSPACE/2 - ((i<6)?16:0) + ((i>7)?16:0);
		y = SCREENH - h - VSPACE/2;
	}
	
	return GRect(x, y, w, h);
}

digitSlot *findSlot(Layer *layer) {
	int i;
	for (i=0; i<NUMSLOTS; i++) {
		if (slot[i].layer == layer) {
			return &slot[i];
		}
	}
	
	return NULL;
}

void updateSlot(Layer *layer, GContext *ctx) {
	int t, tx1, tx2, ty1, ty2, ox, oy;
	GRect bounds;
	digitSlot *slot;
		
	slot = findSlot(layer);
	graphics_context_set_fill_color(ctx, GColorBlack);
	bounds = layer_get_bounds(slot->layer);
	graphics_fill_rect(ctx, GRect(0, 0, bounds.size.w, bounds.size.h), 0, GCornerNone);
	
	for (t=0; t<13; t++) {
		if (digits[slot->curDigit][t][0] != digits[slot->prevDigit][t][0]
			|| digits[slot->curDigit][t][1] != digits[slot->prevDigit][t][1]) {
			if (slot->normTime == ANIMATION_NORMALIZED_MAX) {
				ox = digits[slot->curDigit][t][0]*slot->tileWidth;
				oy = digits[slot->curDigit][t][1]*slot->tileHeight;
			} else {
				tx1 = digits[slot->prevDigit][t][0]*slot->tileWidth;
				tx2 = digits[slot->curDigit][t][0]*slot->tileWidth;
				ty1 = digits[slot->prevDigit][t][1]*slot->tileHeight;
				ty2 = digits[slot->curDigit][t][1]*slot->tileHeight;
				
				ox = slot->normTime * (tx2-tx1) / ANIMATION_NORMALIZED_MAX + tx1;
				oy = slot->normTime * (ty2-ty1) / ANIMATION_NORMALIZED_MAX + ty1;
			}
		} else {
			ox = digits[slot->curDigit][t][0]*slot->tileWidth;
			oy = digits[slot->curDigit][t][1]*slot->tileHeight;
		}
		
		graphics_context_set_fill_color(ctx, GColorWhite);
		graphics_fill_rect(ctx, GRect(ox, oy, slot->tileWidth, slot->tileHeight-1), 0, GCornerNone);
	}
}

void initSlot(int i, Layer *parent) {
	slot[i].normTime = ANIMATION_NORMALIZED_MAX;
	slot[i].prevDigit = 0;
	slot[i].curDigit = startDigit[i];
	if (i<4) {
		// Hour slots -> big digits
		slot[i].tileWidth = TILEW;
		slot[i].tileHeight = TILEH;
	} else {
		// Date slots -> small digits
		slot[i].tileWidth = DTILEW;
		slot[i].tileHeight = DTILEH;
	}
	slot[i].layer = layer_create(slotFrame(i));
	layer_set_update_proc(slot[i].layer, updateSlot);
	layer_add_child(parent, slot[i].layer);
}

void deinitSlot(int i) {
	layer_destroy(slot[i].layer);
}

void animateDigits(struct Animation *anim, const uint32_t normTime) {
	int i;
	
	for (i=0; i<NUMSLOTS; i++) {
		if (slot[i].curDigit != slot[i].prevDigit) {
			slot[i].normTime = normTime;
			layer_mark_dirty(slot[i].layer);
		}
	}
}

void handle_tick(struct tm *now, TimeUnits units_changed) {
	int h, m;
    int D, M;
    int i;
#if WEEKDAY
    int wd;
#else
    int Y;
#endif
	
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Entering handle_tick");

    if (splashEnded) {
        if (animation_is_scheduled(anim))
            animation_unschedule(anim);
                
        h = now->tm_hour;
        m = now->tm_min;
        D = now->tm_mday;
        M = now->tm_mon+1;
#if WEEKDAY
		wd = now->tm_wday;
#else
        Y = now->tm_year%100;
#endif
        
        if (clock_12) {
            h = h%12;
            if (h == 0) {
                h = 12;
            }
        }
        
        for (i=0; i<NUMSLOTS; i++) {
            slot[i].prevDigit = slot[i].curDigit;
        }
        
        // Hour slots
        slot[0].curDigit = h/10;
        slot[1].curDigit = h%10;
        slot[2].curDigit = m/10;
        slot[3].curDigit = m%10;
        
        // Date slots
#if WEEKDAY && USDATE
        slot[4].curDigit = weekDay[wd][0] - '0';
        slot[5].curDigit = weekDay[wd][1] - '0';
        slot[6].curDigit = M/10;
        slot[7].curDigit = M%10;
        slot[8].curDigit = D/10;
        slot[9].curDigit = D%10;
#elif WEEKDAY && !USDATE
        slot[4].curDigit = weekDay[wd][0] - '0';
        slot[5].curDigit = weekDay[wd][1] - '0';
        slot[6].curDigit = D/10;
        slot[7].curDigit = D%10;
        slot[8].curDigit = M/10;
        slot[9].curDigit = M%10;
#elif !WEEKDAY && USDATE
        slot[4].curDigit = M/10;
        slot[5].curDigit = M%10;
        slot[6].curDigit = D/10;
        slot[7].curDigit = D%10;
        slot[8].curDigit = Y/10;
        slot[9].curDigit = Y%10;
#else
        slot[4].curDigit = D/10;
        slot[5].curDigit = D%10;
        slot[6].curDigit = M/10;
        slot[7].curDigit = M%10;
        slot[8].curDigit = Y/10;
        slot[9].curDigit = Y%10;
#endif
        animation_schedule(anim);
    }
}

void handle_timer(void *data) {
	time_t curTime;
	struct tm *now;
	
    splashEnded = true;
	curTime = time(NULL);
	now = localtime(&curTime);
    handle_tick(now, 0);
}

void handle_init() {
	Layer *rootLayer;
	int i;
	
	window = window_create();
	window_stack_push(window, true);
	window_set_background_color(window, GColorBlack);
	
	rootLayer = window_get_root_layer(window);
	
	for (i=0; i<NUMSLOTS; i++) {
		initSlot(i, rootLayer);
	}
	
	clock_12 = !clock_is_24h_style();
	
	animImpl.setup = NULL;
	animImpl.update = animateDigits;
	animImpl.teardown = NULL;
	
	anim = animation_create();
	animation_set_delay(anim, 0);
	animation_set_duration(anim, DIGIT_CHANGE_ANIM_DURATION);
	animation_set_implementation(anim, &animImpl);
	
	app_timer_register(STARTDELAY, handle_timer, NULL);

	tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
}

void handle_deinit() {
	int i;
	
	tick_timer_service_unsubscribe();
	
	for (i=0; i<NUMSLOTS; i++) {
		deinitSlot(i);
	}
	
	window_destroy(window);
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}
