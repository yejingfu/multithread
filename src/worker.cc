#include "worker.h"
#include "sharedbuffer.h"
#include "nan.h"
#include "util.h"

using namespace v8;

static Persistent<ObjectTemplate> worker_template;
static Persistent<String> id_symbol;

NAN_METHOD(Eval) {
  NanScope();
  NanReturnUndefined();
}

Worker::Worker() {
  m_error = 0;
}

Worker::~Worker() {

}

//static 
Worker* Worker::Start(const v8::Arguments &args) {
  if (worker_template.IsEmpty()) {
    Local<String> localSymbol = NanNew<String>("id");
    NanAssignPersistent(id_symbol, localSymbol);
    Local<ObjectTemplate> tpl = ObjectTemplate::New();
    tpl->SetInternalFieldCount(1);
    tpl->Set(localSymbol, NanNew<Integer>(0));
    tpl->Set(NanNew<String>("eval"), NanNew<FunctionTemplate>(Eval));
    NanAssignPersistent(worker_template, tpl);
  }

  if (args.Length() < 3 || !args[2]->IsFunction()) {
    NanThrowTypeError("Usage: createWorker(script, sharedBufferObj, callback)");
    return NULL;
  }

  Worker *worker = new Worker();
  worker->m_script = new String::Utf8Value(args[0]);
  if (args[1]->IsNumber()) {
    printf("Jingfu: the sharedbuffer is valid\n");
    NanAssignPersistent(worker->m_sharedBufferId, args[1]->ToInt32());
  }
  NanAssignPersistent(worker->m_callback, args[2]->ToObject());
  Local<Object> jsObj = NanNew(worker_template)->NewInstance();
  jsObj->Set(NanNew<String>("id"), NanNew<Integer>(11));
  NanSetInternalFieldPointer(jsObj, 0, worker);
  NanAssignPersistent(worker->m_jsobject, jsObj);

  worker->m_work.data = worker;
  int ret = uv_queue_work(uv_default_loop(), &(worker->m_work), WorkerProc, PostWorkerProc);

  return worker;
}

//static 
void Worker::WorkerProc(uv_work_t *req) {
  // sub thread
  Worker *worker = (Worker*)(req->data);
  worker->m_isolate = Isolate::New();
  NanSetIsolateData(worker->m_isolate, worker);
  if (!Locker::IsActive()) {
    worker->ReleaseIsolate();
    return;
  }

  {  // wrap Lock within scope is very important
    Locker myLocker(worker->m_isolate);
    worker->m_isolate->Enter();
  
    NanScope();
    Persistent<Context> ctx = Context::New();

    ctx->Enter();

    Local<Object> ctx_global = ctx->Global();
    Handle<Object> consoleObj = NanNew<Object>();
    consoleObj->Set(NanNew<String>("log"), NanNew<FunctionTemplate>(console_log)->GetFunction());
    consoleObj->Set(NanNew<String>("error"), NanNew<FunctionTemplate>(console_error)->GetFunction());
    ctx_global->Set(NanNew<String>("console"), consoleObj, (PropertyAttribute)(ReadOnly | DontDelete));

    if (!worker->m_sharedBufferId.IsEmpty()) {
      printf("Jingfu SharedBuffer Id: %d\n", (int)worker->m_sharedBufferId->Value());
      SharedBuffer *buf = SharedBuffer::getSharedBuffer((int)worker->m_sharedBufferId->Value());
      if (buf) {
        printf("Jingfu: set SharedBuffer JS Object\n");

        Local<ObjectTemplate> sharedBufTpl = ObjectTemplate::New();
        //sharedBufTpl->SetInternalFieldCount(1);
        //sharedBufTpl->Set(NanNew<String>("id"), NanNew<Integer>(0));
        //NanAssignPersistent(buffer_template, tpl);

        Local<Object> jsObj = NanNew(sharedBufTpl)->NewInstance();
        jsObj->Set(NanNew<String>("id"), NanNew<Integer>(buf->id()), (PropertyAttribute)(ReadOnly | DontDelete));
        jsObj->Set(NanNew<String>("size"), NanNew<Integer>(buf->size()));
        NanSetInternalFieldPointer(jsObj, 0, buf);
        NanAssignPersistent(worker->m_sharedBufferObject, jsObj);

        //ctx_global->Set(NanNew<String>("sharedBuffer"), buf->getJSObject());
        ctx_global->Set(NanNew<String>("sharedBuffer"), jsObj);

        printf("Jingfu: set SharedBuffer JS Object Done\n");
      } else {
        printf("Jingfu: error: SharedBuffer is not valid\n");
        ctx_global->Set(NanNew<String>("sharedBuffer"), Null());
      }
    } else {
      printf("Jingfu: sharedBufferId is null\n");
      ctx_global->Set(NanNew<String>("sharedBuffer"), Null());
    }

    TryCatch tryCatch;
    String::Utf8Value *result;
    Local<String> source = NanNew<String>(**worker->m_script, worker->m_script->length());
    Local<Script> script = NanCompileScript(source);
    Local<Value> retValue = NanRunScript(script);

    if (!tryCatch.HasCaught()) {
      result = new String::Utf8Value(retValue->ToString());
    } else {
      
      worker->m_error = 1;
      Local<Value> err = tryCatch.Exception();
      result = new String::Utf8Value(err->ToString());
      printf("WorkerProc failed to run script: %s\n", **result);
    }
    worker->m_result = result;
    ctx.Dispose();
  }
  worker->ReleaseIsolate();
}

//static 
void Worker::PostWorkerProc(uv_work_t *req, int result) {
  Worker *worker = (Worker*)(req->data);
  NanScope();
  Local<Value> argv[2];
  if (worker->m_error) {
    argv[0] = String::New("error");
    argv[1] = Local<Value>::New(Null());
  } else {
    argv[0] = Local<Value>::New(Null());
    argv[1] = String::New("Succeed");
  }
  worker->m_callback->CallAsFunction(Object::New(), 2, argv);
}

void Worker::ReleaseIsolate() {
  m_isolate->Exit();
  m_isolate->Dispose();
  m_isolate = NULL;
}
