# JSVGC

JSVGC is a node modules for converting SVG files to PNG on the fly.

[![NPM](https://nodei.co/npm/jsvgc.png?downloads=true&downloadRank=true)](https://nodei.co/npm/jsvgc/)

JSVGC uses [nanosvg](https://github.com/memononen/nanosvg) to do the actual conversion and then [stb](https://github.com/nothings/stb) to create the binary data for the PNGs. Conversion of files is done using full async code allowing fast creation of graphics at any given size without needing to pause your application. synchronous method is also offered.

## usage
convertSVG takes 5 arguments with an optional 6th that determines if existing files should be overwritten, set to false by default.
Res object will simply be true if conversion completed without issue. If not err will contain specific error code.
```
jsvgc.convertSVG("in.svg", "out.png", 500, 500, function (err, res) {})
jsvgc.convertSVG("in.svg", "out.png", 500, 500, true, function (err, res) {})
```

convertSync takes 4 arguments with an optional 5th that determines if existing files should be overwritten, set to false by default;
Function will return a value of true if conversion completed without issue.
```
jsvgc.convertSync("in.svg", "out.png", 500, 500)
jsvgc.convertSync("in.svg", "out.png", 500, 500, true)
```
