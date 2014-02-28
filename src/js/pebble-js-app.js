var dateorder = localStorage.getItem("dateorder");
if (!dateorder) {
	dateorder = 1;
}

var weekday = localStorage.getItem("weekday");
if (!weekday) {
	weekday = 0;
}

var lang = localStorage.getItem("lang");
if (!lang) {
	lang = 1;
}

Pebble.addEventListener("ready", function() {
	console.log("Ready Event");
	console.log("	dateorder: " + dateorder);
	console.log("	weekday: " + weekday);
	console.log("	lang: " + lang);

	
	Pebble.sendAppMessage(JSON.parse('{"dateorder":'+dateorder+',"weekday":'+weekday+',"lang":'+lang+'}'));
});

Pebble.addEventListener("showConfiguration", function(e) {
	console.log("showCOnfiguration Event");
	console.log("	dateorder: " + dateorder);
	console.log("	weekday: " + weekday);
	console.log("	lang: " + lang);
	Pebble.openURL("http://www.famillemattern.com/jnm/pebble/Blockslide-Date/Blockslide-Date_2.1.0.php?dateorder=" + dateorder + "&weekday=" + weekday + "&lang=" + lang);
});

Pebble.addEventListener("webviewclosed", function(e) {
	console.log("Configuration window closed");
	console.log(e.type);
	console.log(e.response);

	var configuration = JSON.parse(e.response);
	Pebble.sendAppMessage(configuration);
	
	dateorder = configuration["dateorder"];
	localStorage.setItem("dateorder", dateorder);
	
	weekday = configuration["weekday"];
	localStorage.setItem("weekday", weekday);

	lang = configuration["lang"];
	localStorage.setItem("lang", lang);
});
