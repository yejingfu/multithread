// yejingfu@hotmail.com

#ifndef MT_THREAD_H_
#define MT_THREAD_H_

#include <vector>

#include <uv.h>
#include <v8.h>
#include "nan.h"

NAN_METHOD(Evaluate);

class EvalTask;

class Thread {
public:
  ~Thread();

  static Thread* CreateInstance();
  static Thread* AsThread(v8::Local<v8::Object> obj);
  static void ThreadProc(void *arg);
  static void EventLoop(Thread *thread);

  void Destroy();
  v8::Persistent<v8::Object>& GetJSObject() { return m_jsobject; }

  void QueueEvalTask(EvalTask *task);
  EvalTask* DequeueEvalTask();
  void CleanEvalTasks();
  

private:
  explicit Thread();
  static void Callback(uv_async_t *watcher, int revents);

  bool Init();
  void CreateJSObject();

  uv_async_t    m_async_watcher;
  int           m_id;
  uv_thread_t   m_thread;
  volatile int  m_sig_kill;
  volatile int  m_idle;
  uv_cond_t     m_idle_cv;
  uv_mutex_t    m_idle_mutex;
  v8::Isolate*  m_isolate;
  v8::Persistent<v8::Context>   m_ctx;
  v8::Persistent<v8::Object>    m_jsobject;
  v8::Persistent<v8::Object>    m_thread_jsobject;

  std::vector<EvalTask*> m_tasks; 
};

class EvalTask {
public:
  explicit EvalTask() {}
  ~EvalTask() {}

  void SetCallback(v8::Local<v8::Object> obj);

  v8::Persistent<v8::Object>  m_cb;
  v8::String::Utf8Value   *m_script;
  v8::String::Utf8Value   *m_result;
};

#endif  // MT_THREAD_H_

