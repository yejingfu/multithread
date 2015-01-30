// yejingfu@hotmail.com

#include <v8.h>
#include <uv.h>
#include <node.h>
#include <node_object_wrap.h>
#include <stdlib.h>
#include <string>

#include "nan.h"
//#include "nan_isolate_data_accessor.h"

using namespace v8;

static Persistent<ObjectTemplate> thread_template;
static Persistent<String> id_symbol;

NAN_METHOD(SayHello) {
  NanScope();
  NanReturnValue(NanNew("Hello World"));
}

void Init(Handle<Object> exports) {
  exports->Set(NanNew("hello"), NanNew<FunctionTemplate>(SayHello)->GetFunction());
}

NODE_MODULE(multithread, Init)

