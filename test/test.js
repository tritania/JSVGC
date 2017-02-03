var jsvgc = require("../jsvgc");
var asyncv = false;

var hrTime = process.hrtime();
console.log("Start Time " + hrTime[0] * 1000000 + hrTime[1] / 1000);

jsvgc.Convert("./test/drawing.svg", "./test/test.png", 500, 500, function (err, res) {
	if (err) {
		console.log(err);
	} else {
		console.log("Created PNG!");
		hrTime = process.hrtime();
		console.log("Completed Time " + hrTime[0] * 1000000 + hrTime[1] / 1000);
		asyncv = true;
	}
});

hrTime = process.hrtime();
console.log("After Call Time " + hrTime[0] * 1000000 + hrTime[1] / 1000);