var svgc = require('./build/Release/jsvgc');
var fs = require('fs');

module.exports.convertSVG = convertSVG;

function convertSVG(inFile, outFile, width, height, callback) {
	fs.readFile(inFile, (err, data) => {
		if (err) return callback(err, null);
		svgc.Convert(data, data.length, width, height, (err, data) => {
				if (err) return callback(err, null);
				fs.writeFile(outFile, data, (err) => {
						if (err) return callback(err, null);
						return callback(null, true); //all good!
				})
		});
	});
}
