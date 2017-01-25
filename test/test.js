var jsvgc = require("../jsvgc");

jsvgc.convertSVG("./test/drawing.svg", "./test/test.png", 500, 500, function (err, res) {
	if (err) {
		throw err;
	} else {
		console.log("Created PNG!");
	}
});
