// yejingfu@hotmail.com

#ifndef MT_THREAD_H_
#define MT_THREAD_H_

#include <uv.h>
#include <v8.h>

class Thread {
public:
  explicit Thread() {}
  ~Thread() {}

  static Thread* CreateInstance();

  void Destroy();
  v8::Persistent<Object>& GetJSObject() { return m_jsobject; }

private:
  static void Callback(uv_async_t *watcher, int revents);

  bool Init();

  uv_async_t    m_async_watcher;
  int           m_id;
  uv_thread_t   m_thread;
  volatile int  m_sig_kill;
  volatile int  m_idle;
  uv_cond_t     m_idle_cv;
  uv_mutex_t    m_idle_mutex;
  v8::Isolate*  m_isolate;
  v8::Persistent<Context>   m_ctx;
  v8::Persistent<Object>    m_jsobject;
  v8::Persistent<Object>    m_thread_jsobject;  
};

class Task {
public:
  explicit Task() {}
  ~Task() {}
};

#endif  // MT_THREAD_H_

