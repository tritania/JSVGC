#include <nan.h>
#include <iostream>
#include <string>
#include <fstream>

#include <stdio.h>
#include <string.h>
#include <float.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

using namespace std;
using namespace Nan;
using namespace v8;

bool svg_convert(vector<unsigned char> & input_data, vector<unsigned char> & png, int width, int height)
{
	NSVGimage *image = NULL;
	NSVGrasterizer *rast = NULL;
	unsigned char* img = NULL;
	unsigned char* imgPNG = NULL;
	int w, h;
	
	w = width;
	h = height;

	image = nsvgParse(reinterpret_cast<char*>(input_data.data()), "px", 96.0f);
	if (image == NULL) {
		//fail
	}
	
	int ws = (int)image->width;
	float scale = (float) w / (float) ws;

	rast = nsvgCreateRasterizer();
	if (rast == NULL) {
		//fail
	}

	img = new unsigned char[w*h*4];
	if (img == NULL) {
		//fail
	}

	nsvgRasterize(rast, image, 0, 0, scale, img, width, height, width*4);

	int len;
 	imgPNG = stbi_write_png_to_mem(img, width*4, width, height, 4, &len);
	vector<unsigned char> data(imgPNG, imgPNG + len);
	png = data;

	return true;
}


void buffer_delete_callback(char* data, void* used_vector) {
    delete reinterpret_cast<vector<unsigned char> *> (used_vector);
}


class ConWorker : public AsyncWorker {
    public:
    ConWorker(Callback * callback, Local<Object> &svgBuffer, int size, int w, int h) : AsyncWorker(callback) {
        unsigned char*buffer = (unsigned char*) node::Buffer::Data(svgBuffer);
        vector<unsigned char> tmp(buffer, buffer +  (unsigned int) size);
        svg_data = tmp;
        width = w;
        height = h;
    }
    void Execute() {
       png = new vector<unsigned char>();
       svg_convert(svg_data, *png, width, height);
    }
    void HandleOKCallback () {
        Local<Object> pngData = 
               NewBuffer((char *)png->data(), 
               png->size(), buffer_delete_callback, 
               png).ToLocalChecked();
        Local<Value> argv[] = { Nan::Null(), pngData };
        callback->Call(2, argv);
    }

    private:
		int width;
		int height;
        vector<unsigned char> svg_data;
        vector<unsigned char> * png;
};

NAN_METHOD(Convert) {
    int size = To<int>(info[1]).FromJust();
    Local<Object> svgBuffer = info[0]->ToObject();
    int width = info[2]->NumberValue();
    int height = info[3]->NumberValue();
    Callback *callback = new Callback(info[4].As<Function>());
    AsyncQueueWorker(new ConWorker(callback, svgBuffer ,size, width, height));
}

NAN_MODULE_INIT(Init) {
         
   Nan::Set(target, New<String>("Convert").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(Convert)).ToLocalChecked());
        
}

NODE_MODULE(jsvgc, Init)
