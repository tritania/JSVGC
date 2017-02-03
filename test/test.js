var jsvgc = require("../jsvgc");
var result;

result = jsvgc.convertSync("./test/drawing.svg", "./test/test2.png", 500, 500);
if (result) {
	console.log("Sync Good");
} else {
	console.log("Sync Error");
}

jsvgc.convertSVG("./test/drawing.svg", "./test/test.png", 500, 500, function (err, res) {
	if (err) {
		console.log("Async Error: "+ err);
	} else {
		console.log("Async Good");
	}
});