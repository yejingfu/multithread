// yejingfu@hotmail.com

#include <v8.h>
#include <uv.h>
#include <node.h>
#include <node_object_wrap.h>
#include <stdlib.h>
#include <string>

#include "nan.h"
//#include "nan_isolate_data_accessor.h"
#include "thread.h"

using namespace v8;


NAN_METHOD(SayHello) {
  NanScope();
  NanReturnValue(NanNew("Hello World"));
}

NAN_METHOD(CreateThread) {
  NanScope();
  Thread *thread = Thread::CreateInstance();
  if (!thread) {
    printf("Jingfu: Failed to create Thread\n");
    NanReturnUndefined();
  } else {
    NanReturnValue(thread->GetJSObject());
  }
}

void Init(Handle<Object> exports) {
  exports->Set(NanNew("hello"), NanNew<FunctionTemplate>(SayHello)->GetFunction());
  exports->Set(NanNew("createThread"), NanNew<FunctionTemplate>(CreateThread)->GetFunction());
}

NODE_MODULE(multithread, Init)

