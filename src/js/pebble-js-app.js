var initialised = false;

function appMessageAck(e) {
    console.log("options sent to Pebble successfully");
}

function appMessageNack(e) {
    console.log("options not sent to Pebble: " + e.error.message);
}

Pebble.addEventListener("ready", function() {
    initialised = true;
});

Pebble.addEventListener("showConfiguration", function() {
    var options = JSON.parse(window.localStorage.getItem('options'));
    console.log("read options: " + JSON.stringify(options));
    console.log("showing configuration");
	 
	 /*if (options['invert'] == null) {
		 options['invert'] = "off";
	 }*/
	 
    var uri = 'http://corefault.de/pebble-conf/path.html?';// + 'invert=' + encodeURIComponent(options['invert']);
    Pebble.openURL(uri);
});

Pebble.addEventListener("webviewclosed", function(e) {
    console.log("configuration closed");
    if (e.response != '') {
	var options = JSON.parse(decodeURIComponent(e.response));
	console.log("storing options: " + JSON.stringify(options));
	window.localStorage.setItem('options', JSON.stringify(options));
	Pebble.sendAppMessage(options, appMessageAck, appMessageNack);
    } else {
	console.log("no options received");
    }
});