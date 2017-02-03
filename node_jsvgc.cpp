#include <nan.h>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <iterator>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"
#include <uv.h>
#include <fcntl.h>

using namespace std;
using namespace Nan;
using namespace v8;

int writeFile(string out, vector<unsigned char> png) {
    uv_fs_t open_req;
    uv_fs_t write_req;
    uv_fs_t close_req;

    uv_buf_t buff;

    int op;
    int wr;

    char *buf = new char[png.size()];
    std::copy(png.begin(), png.end(), buf);
    buff = uv_buf_init(buf, png.size());
    

    op = uv_fs_open(uv_default_loop(), &open_req, out.c_str(), O_WRONLY | O_CREAT, 0, NULL); //need proper file handles here
    if (op != -1) {
        wr = uv_fs_write(uv_default_loop(), &write_req, op, &buff, sizeof(buf), -1, NULL); //Error Here
        if (wr != -1) {
            return uv_fs_close(uv_default_loop(), &close_req, open_req.result, NULL);
        } else {
            fprintf(stderr, "Error writing file!");
            return -1;
        }
    } else {
        fprintf(stderr, "Error opening file!");
        return -1;
    }
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

int readFile(string arg, vector<unsigned char> & data) {
    static char str_buf[65];
    static char data_buf[64];
    uv_buf_t buff;
    uv_fs_t open_req; 
    uv_fs_t read_req;   
    uv_fs_t close_req;

    int op;
    int re;

    bool loop = true;

    string svgfile = "";

    buff = uv_buf_init(data_buf, sizeof(data_buf));
    op = uv_fs_open(uv_default_loop(), &open_req, arg.c_str(), O_RDONLY, 0, NULL);
    if (op != -1) {
        while (loop) {
            re = uv_fs_read(uv_default_loop(), &read_req, open_req.result, &buff, 1, -1, NULL);
            if (re < 0) {
                fprintf(stderr, "Read error!");
            }
            else if (re == 0) {
                loop = false;
                copy(svgfile.begin(), svgfile.end(), back_inserter(data));
                return uv_fs_close(uv_default_loop(), &close_req, open_req.result, NULL);
            }
            else {
                memset(str_buf, 0, sizeof(str_buf));
                memcpy(str_buf, data_buf, sizeof(data_buf));
                svgfile += string(str_buf);
                memset(data_buf, 0, sizeof(data_buf));
            }
        }
    } else {
        fprintf(stderr, "Error opening file!");
    }
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    return 1;
}

bool svg_convert(vector<unsigned char> input_data, vector<unsigned char> & png, int width, int height)
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
		return false;
	}
	
	int ws = (int)image->width;
	float scale = (float) w / (float) ws;

	rast = nsvgCreateRasterizer();
	if (rast == NULL) {
		return false;
	}

	img = new unsigned char[w*h*4];
	if (img == NULL) {
		return false;
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
    ConWorker(Callback * callback, string in_file, string out_file, int w, int h) : AsyncWorker(callback) {
        width = w;
        height = h;
        in = in_file;
        out = out_file;
    }

    void Execute() {
       png = new vector<unsigned char>();
       int rr = readFile(in, svg_data);
       bool sr = svg_convert(svg_data, *png, width, height);
       int wr = writeFile(out, *png);
       if (rr != 0 || sr != true || wr != 0) {
            HandleErrorCallback();
       } 
       return;
    }

    void HandleOKCallback () {
        Local<Value> argv[] = { Nan::Null(), Nan::True() };
        callback->Call(2, argv);
    }

    void HandleErrorCallback() {
        Local<Value> argv[] = { Nan::New("Conversion Error").ToLocalChecked(), Nan::Null() };
        callback->Call(2, argv);
    }

    private:
        string in;
        string out;
		int width;
		int height;
        vector<unsigned char> svg_data;
        vector<unsigned char> * png;
};

NAN_METHOD(Convert) {
    Callback *callback;

    if (info[4]->IsFunction()) {
        callback = new Callback(info[4].As<Function>());
    } else {
        Nan::ThrowTypeError("Wrong Arguments");
        return;
    }

    if (info.Length() < 5) {
        Nan::ThrowTypeError("Wrong amount of arguments");
        return;
    }

    if (!info[0]->IsString() || !info[1]->IsString() || !info[2]->IsNumber() || !info[3]->IsNumber()) {
        Nan::ThrowTypeError("Wrong arguments");
        return;
    }

    String::Utf8Value param1(info[0]->ToString());
    string in_file = std::string(*param1);

    String::Utf8Value param2(info[1]->ToString());
    string out_file = std::string(*param2);

    int width = info[2]->NumberValue();
    int height = info[3]->NumberValue();
    
    AsyncQueueWorker(new ConWorker(callback, in_file, out_file, width, height));
}

NAN_METHOD(ConvertSync) {
    if (info.Length() < 4) {
        Nan::ThrowTypeError("Wrong amount of arguments");
        return;
    }

    if (!info[0]->IsString() || !info[1]->IsString() || !info[2]->IsNumber() || !info[3]->IsNumber()) {
        Nan::ThrowTypeError("Wrong arguments");
        return;
    }

    String::Utf8Value param1(info[0]->ToString());
    string in_file = std::string(*param1);

    String::Utf8Value param2(info[1]->ToString());
    string out_file = std::string(*param2);

    int width = info[2]->NumberValue();
    int height = info[3]->NumberValue();

    vector<unsigned char> svg_data;
    vector<unsigned char> * png  = new vector<unsigned char>();;

    int rres = readFile(in_file, svg_data);
    bool sc = svg_convert(svg_data, *png, width, height);
    int res = writeFile(out_file, *png);
    if (res == 0 && sc == true && rres == 0) {
        info.GetReturnValue().Set(true);
    } else {
        info.GetReturnValue().Set(false);
    }
}

NAN_MODULE_INIT(Init) {
         
   Nan::Set(target, New<String>("convertSVG").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(Convert)).ToLocalChecked());

   Nan::Set(target, New<String>("convertSync").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(ConvertSync)).ToLocalChecked());
        
}

NODE_MODULE(jsvgc, Init)
