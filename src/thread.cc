// yejingfu@hotmail.com

#include "thread.h"
#include "util.h"

// #include <pthread.h>
// #include <unistd.h>
// //#ifndef uv_cond_t
// #define uv_cond_signal(x) pthread_cond_signal(x)
// #define uv_cond_init(x) pthread_cond_init(x, NULL)
// #define uv_cond_wait(x,y) pthread_cond_wait(x, y)
// //#endif

using namespace v8;

static const PropertyAttribute attribute_ro_dd = (PropertyAttribute)(ReadOnly | DontDelete);
static const PropertyAttribute attribute_ro_de_dd = (PropertyAttribute)(ReadOnly | DontEnum | DontDelete);
#define JSObjFn(obj, name, fnname) \
	obj->Set(NanNew<String>(name), NanNew<FunctionTemplate>(fnname)->GetFunction(), attribute_ro_dd);

static int g_thread_id = 1;
static Persistent<ObjectTemplate> thread_template;
static Persistent<String> id_symbol;

NAN_METHOD(Evaluate) {
  NanScope();
  if (!args.Length()) {
    return NanThrowTypeError("Evaluate: missing arguments");
  }
  Thread *thread = Thread::AsThread(args.This());  // args.This() means the obj who is called on this function
  if (!thread) {
    return NanThrowTypeError("Evaluate: the callee must be thread");
  }
  if (args.Length() < 2 || !args[1]->IsFunction()) {
    return NanThrowTypeError("Evaluate: the first argment must be function");
  }
  EvalTask *task = new EvalTask();
  task->m_script = new String::Utf8Value(args[0]);
  task->SetCallback(args[1]->ToObject());
  thread->QueueEvalTask(task);
  NanReturnValue(args.This());
}

NAN_METHOD(threadTestSharedVariable) {
  printf("Jingfu: threadTestSharedVariable\n");
  NanReturnUndefined();
}

NAN_METHOD(postMessage) {
  printf("Jingfu: postMessage\n");
  NanReturnUndefined();
}

NAN_METHOD(DestroyThread) {
  NanScope();
  Thread *thread = Thread::AsThread(args.This());
  if (!thread) {
    return NanThrowTypeError("DestroyThread: missing thread object as input");
  }
  thread->Destroy();
}

Thread::Thread() {

}

Thread::~Thread() {
  CleanEvalTasks();
}

//static 
Thread* Thread::CreateInstance() {
  Thread* inst = new Thread();
  if (!inst->Init()) {
    inst->Destroy();
    inst = NULL;
  }
  return inst;
}

//static
Thread* Thread::AsThread(Local<Object> obj) {
  Thread *thread  = NULL;
  if (obj->IsObject() && obj->InternalFieldCount() == 1) {
    thread = (Thread*)NanGetInternalFieldPointer(obj, 0);
  }
  return thread;
}

//static
void Thread::Callback(uv_async_t *watcher, int status) {
/*
  Thread *thread = (Thread*)watcher->data;
  if (thread->m_sig_kill) {
    thread->Destroy();
    return;
  }
  NanScope();
  EvalTask *task = NULL;
  Local<Value> argv[2];
  Local<Value> null = NanNull();
  String::Utf8Value *str;
  TryCatch trycatch;
  //while () {}
*/

}

void* ThreadProc_g(void *arg) {
  Thread::ThreadProc(arg);
  //Thread *pThread = (Thread*)arg;
  // printf("Jingfu: ThreadProc\n");
  // pThread->m_isolate = Isolate::New();
  // NanSetIsolateData(pThread->m_isolate, pThread);
  // if (v8::Locker::IsActive()) {
  //   v8::Locker locker(pThread->m_isolate);    // ensure one most thread accecc isolate per time 
  //   EventLoop(pThread);
  // }
  // // on exit event loop
  // if (pThread->m_tasks.size() == 0) {
  //   uv_async_send(&pThread->m_async_watcher);
  // }
  // pThread->m_isolate->Dispose();
  // delete pThread;
  // printf("Jingfu: exiting sub thread...\n");
  return NULL;
}

//static 
void Thread::ThreadProc(void *arg) {
  Thread *pThread = (Thread*)arg;
  printf("Jingfu: ThreadProc\n");

  pThread->m_isolate = Isolate::New();
  NanSetIsolateData(pThread->m_isolate, pThread);


  if (v8::Locker::IsActive()) {
    v8::Locker locker(pThread->m_isolate);    // ensure one most thread accecc isolate per time 
    pThread->m_isolate->Enter();
    EventLoop(pThread);
  }
  printf("Jingfu: ThreadProc 1\n");
  pThread->m_isolate->Exit();
    printf("Jingfu: ThreadProc 2\n");
  //pThread->m_isolate->Dispose();
    printf("Jingfu: ThreadProc 3\n");
  pThread->m_isolate = NULL;

  // on exit event loop
  printf("Jingfu: exiting sub thread...\n");
  if (pThread->m_tasks.size() == 0) {
    //uv_async_send(&pThread->m_async_watcher);
  }
  //pThread->m_isolate->Dispose();
  //delete pThread;
  printf("Jingfu: exiting sub thread 2...\n");
}

//static 
void Thread::EventLoop(Thread *thread) {
  //Isolate::Scope isolate_scope(thread->m_isolate);
  //{
    NanScope();
    thread->m_ctx = Context::New();
    thread->m_ctx->Enter();

    //Local<FunctionTemplate> ftmpl = NanNew<FunctionTemplate>();
    //Local<ObjectTemplate> otmpl = ftmpl->InstanceTemplate();
    //Local<Context> ctx = NanNewContextHandle(NULL, otmpl);
    //NanAssignPersistent(thread->m_ctx, ctx);
    //thread->m_ctx->Enter();

    Local<Object> global = NanNew(thread->m_ctx)->Global();
    Handle<Object> consoleObj = NanNew<Object>();
    JSObjFn(consoleObj, "log", console_log);
    JSObjFn(consoleObj, "error", console_error);
    global->Set(NanNew<String>("console"), consoleObj, attribute_ro_dd);
    global->Set(NanNew<String>("global"), global);
    global->Set(NanNew<String>("postMessage"), NanNew<FunctionTemplate>(postMessage)->GetFunction());
    global->Set(NanNew<String>("testSharedVariable"), NanNew<FunctionTemplate>(threadTestSharedVariable)->GetFunction());
    
    Local<Object> theThread = NanNew<Object>();
    theThread->Set(NanNew<String>("id"), NanNew<Number>(thread->m_id));
    
    printf("Jingfu: Begin while\n");
    int count = 0;
    while(!thread->m_sig_kill) {
      printf("Jingfu: cycle: %d\n", count++);
      TryCatch tryCatch;
      String::Utf8Value *str;
      Local<String> source;
      Local<Script> script;
      Local<Value> result;

      EvalTask *tmpTask = NULL;
      while(thread->m_tasks.size() != 0) {
        tmpTask = *(thread->m_tasks.begin());
        thread->m_tasks.erase(thread->m_tasks.begin());
        str = tmpTask->m_script;
        source = NanNew<String>(**str, (*str).length());  // ???
        delete str;
        delete tmpTask;
        script = NanCompileScript(source);
        if (!tryCatch.HasCaught()) {
          result = NanRunScript(script);
        }
        // todo
      }

      // onIDLE
      //uv_mutex_lock(&(thread->m_idle_mutex));
      if (thread->m_tasks.size() == 0) {
        thread->m_idle = 1;
printf("Jingfu: uv_cond_wait begin\n");
        uv_cond_wait(&thread->m_idle_cv, &thread->m_idle_mutex);
printf("Jingfu: uv_cond_wait end\n");
        thread->m_idle = 0;
      }
      //uv_mutex_unlock(&(thread->m_idle_mutex));
    }
    //thread->m_ctx->Exit();
    //thread->m_ctx.Dispose();
    //NanDisposePersistent(thread->m_ctx);

  //}
}

bool Thread::Init() {
  m_id = g_thread_id++;
  m_sig_kill = 0;
  m_idle = 0;

  CreateJSObject();

  uv_async_init(uv_default_loop(), &m_async_watcher, reinterpret_cast<uv_async_cb>(Callback));
  m_async_watcher.data = (void*)this;
  uv_ref((uv_handle_t*)&m_async_watcher);

  uv_cond_init(&m_idle_cv);
  uv_mutex_init(&m_idle_mutex);
  int err = uv_thread_create(&m_thread, ThreadProc, this);


  /*
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  int err= pthread_create(&m_thread, &attr, ThreadProc_g, this);
  pthread_attr_destroy(&attr);
  */

  return !err;
}

void Thread::CreateJSObject() {
  Thread *thread = this;
  // initialize global variables: id_symbol and thread_template
  Local<String> localSymbol = NanNew<String>("id");
  NanAssignPersistent(id_symbol, localSymbol);
  Local<ObjectTemplate> localTpl = ObjectTemplate::New();
  localTpl->SetInternalFieldCount(1);
  localTpl->Set(localSymbol, NanNew<Integer>(0));
  localTpl->Set(NanNew<String>("destroy"), NanNew<FunctionTemplate>(DestroyThread));
  localTpl->Set(NanNew<String>("eval"), NanNew<FunctionTemplate>(Evaluate));
  NanAssignPersistent(thread_template, localTpl);
  Local<Object> jsobj = NanNew(thread_template)->NewInstance();
  jsobj->Set(NanNew("id"), NanNew<Integer>(thread->m_id));
  NanSetInternalFieldPointer(jsobj, 0, thread);  // ??
  NanAssignPersistent(thread->m_jsobject, jsobj);
}

void Thread::Destroy() {
  //NanScope();
  if (m_sig_kill == 0) {
    m_sig_kill = 1;
    uv_mutex_lock(&m_idle_mutex);
    if (m_idle) {
      uv_cond_signal(&m_idle_cv);
    }
    uv_mutex_unlock(&m_idle_mutex);
  }
  //printf("Jingfu: Destroy\n");
  // NanSetInternalFieldPointer(NanNew(m_jsobject), 0, NULL);
  // NanDisposePersistent(m_jsobject);
  // uv_unref((uv_handle_t*)&m_async_watcher);

  //delete this;
}

void Thread::QueueEvalTask(EvalTask *task) {
}

EvalTask* Thread::DequeueEvalTask() {
  return NULL;
}

void Thread::CleanEvalTasks() {
  for (size_t i = 0; i < m_tasks.size(); i++) {
    delete m_tasks[i];
  }
  m_tasks.clear();
}


void EvalTask::SetCallback(v8::Local<v8::Object> obj) {
  NanAssignPersistent(m_cb, obj);
}

