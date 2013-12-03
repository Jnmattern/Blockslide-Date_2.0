#include <pebble.h>

#include "Blockslide-Date_2.0.h"

// Languages
#define LANG_DUTCH 0
#define LANG_ENGLISH 1
#define LANG_FRENCH 2
#define LANG_GERMAN 3
#define LANG_SPANISH 4
#define LANG_PORTUGUESE 5
#define LANG_MAX 6

enum {
	CONFIG_KEY_DATEORDER = 10,
	CONFIG_KEY_WEEKDAY = 11,
	CONFIG_KEY_LANG = 12
};


#define TILEW 22
#define TILEH 13
#define DTILEW 5
#define DTILEH 4
#define HSPACE 8
#define DHSPACE 4
#define VSPACE 8
#define DIGIT_CHANGE_ANIM_DURATION 800
#define STARTDELAY 1500
#define BATTERYDELAY 5000
#define SCREENW 144
#define SCREENH 168
#define CX 72
#define CY 84
#define NUMSLOTS 12

char weekDay[LANG_MAX][7][3] = {
	{ "ZO", "MA", "DI", "WO", "DO", "VR", "ZA" },	// Dutch
	{ "SU", "MO", "TU", "WE", "TH", "FR", "SA" },	// English
	{ "DI", "LU", "MA", "ME", "JE", "VE", "SA" },	// French
	{ "SO", "MO", "DI", "MI", "DO", "FR", "SA" },	// German
	{ "DO", "LU", "MA", "MI", "JU", "VI", "SA" }		// Spanish
};

int curLang = LANG_ENGLISH;
int showWeekday = 0;
int USDate = 1;

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
	SPACE,
	'S'-'0',
	'L'-'0',
	'I'-'0',
	'D'-'0',
	'E'-'0',
	SPACE
};

bool clock_12 = false;
bool splashEnded = false;
bool showBattery = false;

AnimationImplementation animImpl;
Animation *anim;

char buffer[256] = "";

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
		//x = CX + (i-7)*(w+DHSPACE) + DHSPACE/2 - ((i<6)?16:0) + ((i>7)?16:0);
		x = 5 + (i-4)*2 + (i-4)*w;
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
    int wd;
    int Y;
	
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Entering handle_tick");

    if (splashEnded) {
        if (animation_is_scheduled(anim))
            animation_unschedule(anim);
                
        h = now->tm_hour;
        m = now->tm_min;
        D = now->tm_mday;
        M = now->tm_mon+1;
		
		if (showWeekday) {
			wd = now->tm_wday;
		} else {
			Y = now->tm_year%100;
		}
        
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
        slot[6].curDigit = SPACE;
        slot[9].curDigit = SPACE;
		
        
        // Date slots
		if (showWeekday && USDate) {
			slot[4].curDigit = weekDay[curLang][wd][0] - '0';
			slot[5].curDigit = weekDay[curLang][wd][1] - '0';
			slot[7].curDigit = M/10;
			slot[8].curDigit = M%10;
			slot[10].curDigit = D/10;
			slot[11].curDigit = D%10;
		} else if (showWeekday && !USDate) {
			slot[4].curDigit = weekDay[curLang][wd][0] - '0';
			slot[5].curDigit = weekDay[curLang][wd][1] - '0';
			slot[7].curDigit = D/10;
			slot[8].curDigit = D%10;
			slot[10].curDigit = M/10;
			slot[11].curDigit = M%10;
		} else if (!showWeekday && USDate) {
			slot[4].curDigit = M/10;
			slot[5].curDigit = M%10;
			slot[7].curDigit = D/10;
			slot[8].curDigit = D%10;
			slot[10].curDigit = Y/10;
			slot[11].curDigit = Y%10;
		} else {
			slot[4].curDigit = D/10;
			slot[5].curDigit = D%10;
			slot[7].curDigit = M/10;
			slot[8].curDigit = M%10;
			slot[10].curDigit = Y/10;
			slot[11].curDigit = Y%10;
		}
		
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

void handle_tap(AccelAxisType axis, int32_t direction) {
	static BatteryChargeState chargeState;
	int i, s;
	
    if (splashEnded) {
		if (animation_is_scheduled(anim)) {
			animation_unschedule(anim);
		}
		
		for (i=0; i<NUMSLOTS; i++) {
            slot[i].prevDigit = slot[i].curDigit;
        }
        
        chargeState = battery_state_service_peek();
        s = chargeState.charge_percent / 10;
                
        if (s<1) {
			s = 1;
		} else if (s>9) {
			s = 9;
		}
		
		s--;
		
		for (i=0; i<4; i++) {
			slot[i].curDigit = BATTERYOFFSET + 4*s + i;
		}
		
		slot[4].curDigit = 'B' - '0';
		slot[5].curDigit = 'A' - '0';
		slot[6].curDigit = 'T' - '0';
		slot[7].curDigit = 'T' - '0';
		slot[8].curDigit = ':' - '0';
		slot[9].curDigit = s+1;
		slot[10].curDigit = 0;
		slot[11].curDigit = PERCENT;
		
        animation_schedule(anim);
       	app_timer_register(BATTERYDELAY, handle_timer, NULL);
	}
}

void applyConfig() {
	if (splashEnded) {
		handle_timer(NULL);
	}
}

bool checkAndSaveInt(int *var, int val, int key) {
	if (*var != val) {
		*var = val;
		persist_write_int(key, val);
		return true;
	} else {
		return false;
	}
}

void in_dropped_handler(AppMessageResult reason, void *context) {
}

void in_received_handler(DictionaryIterator *received, void *context) {
	bool somethingChanged = false;
	
	Tuple *dateorder = dict_find(received, CONFIG_KEY_DATEORDER);
	Tuple *weekday = dict_find(received, CONFIG_KEY_WEEKDAY);
	Tuple *lang = dict_find(received, CONFIG_KEY_LANG);
	
	if (dateorder && weekday && lang) {
		somethingChanged |= checkAndSaveInt(&USDate, dateorder->value->int32, CONFIG_KEY_DATEORDER);
		somethingChanged |= checkAndSaveInt(&showWeekday, weekday->value->int32, CONFIG_KEY_WEEKDAY);
		somethingChanged |= checkAndSaveInt(&curLang, lang->value->int32, CONFIG_KEY_LANG);
		
		snprintf(buffer, 256, "Received config (dateorder=%d, weekday=%d, lang=%d)", USDate, showWeekday, curLang);
		APP_LOG(APP_LOG_LEVEL_DEBUG, buffer);
		
		if (somethingChanged) {
			applyConfig();
		}
	}
}

void readConfig() {
	if (persist_exists(CONFIG_KEY_DATEORDER)) {
		USDate = persist_read_int(CONFIG_KEY_DATEORDER);
	} else {
		USDate = 1;
	}
	
	if (persist_exists(CONFIG_KEY_WEEKDAY)) {
		showWeekday = persist_read_int(CONFIG_KEY_WEEKDAY);
	} else {
		showWeekday = 0;
	}
	
	if (persist_exists(CONFIG_KEY_LANG)) {
		curLang = persist_read_int(CONFIG_KEY_LANG);
	} else {
		curLang = LANG_ENGLISH;
	}
}

static void app_message_init(void) {
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_open(64, 64);
}


void handle_init() {
	Layer *rootLayer;
	int i;
	
	window = window_create();
	window_stack_push(window, true);
	window_set_background_color(window, GColorBlack);
	
	app_message_init();
	readConfig();

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
	
	accel_tap_service_subscribe(handle_tap);
}

void handle_deinit() {
	int i;
	
	accel_tap_service_unsubscribe();
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
