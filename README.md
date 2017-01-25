# JSVGC

JSVGC is a node modules for converting SVG files to PNG on the fly.

JSVGC uses [nanosvg](https://github.com/memononen/nanosvg) to do the actual conversion and then [stb](https://github.com/nothings/stb) to create the binary data for the PNGs. Conversion of files is done using full async code allowing fast creation of graphics at any given size without needing to pause your application.

## usage
```
jsvgc.convertSVG("in.svg", "out.png", 500, 500, function (err, res) {})
```
