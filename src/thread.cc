// yejingfu@hotmail.com

#include "thread.h"

using namespace v8;

static int g_thread_id = 1;
static Persistent<ObjectTemplate> thread_template;
static Persistent<String> id_symbol;

void 

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
void Thread::Callback(uv_async_t *watcher, int status) {
  Thread *thread = (Thread*)watcher->data;
  if (thread->m_sig_kill) {
    thread->Destroy();
    return;
  }
  NanScope();
  Task *task = NULL;
  Local<Value> argv[2];
  Local<Value> null = NanNull();
  String::Utf8Value *str;
  TryCatch trycatch;
  //while () {}
}

bool Thread::Init() {
  m_id = g_thread_id++;
  Local<Object> jsobj = NanNew(thread_template)->NewInstance();
  jsobj->Set(NanNew("id"), NanNew<Integer>(m_id));
  NanSetInternalFieldPointer(jsobj, 0, this);
  NanAssignPersistent(m_jsobject, jsobj);

  uv_async_init(uv_default_loop(), &m_async_watcher, reinterpret_cast<uv_async_cb>(Callback));
  m_async_watcher.data = (void*)this;
  uv_ref((uv_handle_t*)&m_async_watcher);

  uv_cond_init(&m_idle_cv);
  uv_mutex_init(&m_idle_mutex);

  int err = uv_thread-create(&m_thread, ThreadProc, this);
  return !err;
}

void Thread::Destroy() {
  NanScope();
  m_sig_kill = 0;
  NanSetInternalFieldPointer(NanNew(m_jsobject), 0, NULL);
  NanDisposePersistent(m_jsobject);
  uv_unref((uv_handle_t*)&m_async_watcher);
  delete this;
}

