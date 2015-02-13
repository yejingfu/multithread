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
#include "worker.h"
#include "sharedbuffer.h"

using namespace v8;

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

NAN_METHOD(CreateWorker) {
  NanScope();
  Worker *worker = Worker::Start(args);
  if (!worker) {
    printf("Jingfu: Failed to start a worker");
    NanReturnUndefined();
  }
  NanReturnValue(worker->GetJSObject());
}

NAN_METHOD(CreateSharedBuffer) {
  NanScope();
  if (args.Length() == 0 || !args[0]->IsInt32()) {
    NanThrowTypeError("Input buffer size to create SharedBuffer object");
  }
  Local<Integer> size = Local<Integer>::Cast(args[0]);
  int bufSize = (int)size->Int32Value();
  SharedBuffer *buf = SharedBuffer::createSharedBuffer(bufSize);
  NanReturnValue(buf->getJSObject());
}

void Init(Handle<Object> exports) {
  exports->Set(NanNew("createThread"), NanNew<FunctionTemplate>(CreateThread)->GetFunction());
  exports->Set(NanNew("createWorker"), NanNew<FunctionTemplate>(CreateWorker)->GetFunction());
  exports->Set(NanNew("createSharedBuffer"), NanNew<FunctionTemplate>(CreateSharedBuffer)->GetFunction());
}

NODE_MODULE(multithread, Init)

