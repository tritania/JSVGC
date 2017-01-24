#include <node.h>
using namespace v8;

void Convert(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = args.GetIsolate();

	if (args.Length() < 3) {
		isolate->ThrowException(Exception::TypeError(
			String::NewFromUtf8(isolate, "To few arguments!")));
		return;
	}

	Local<String> str = String::NewFromUtf8(isolate, "PNG FILE NAME");
	args.GetReturnValue().Set(str);
}

void Init(Local<Object> exports) {
	NODE_SET_METHOD(exports, "convert", Convert);
}

NODE_MODULE(jsvgc, Init);
