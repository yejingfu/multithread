// yejingfu@hotmail.com

#ifndef MT_WORKER_H_
#define MT_WORKER_H_

#include <vector>

#include <uv.h>
#include <v8.h>
#include <node.h>
#include "nan.h"


class Worker {
public:
  Worker();
  ~Worker();

  static Worker* Start(const v8::Arguments &args);
  static void WorkerProc(uv_work_t *req);
  static void PostWorkerProc(uv_work_t *req, int result);

  v8::Persistent<v8::Object>& GetJSObject() { return m_jsobject; }
  void ReleaseIsolate();

private:
  uv_work_t                     m_work;
  int                           m_error;
  v8::String::Utf8Value         *m_result;
  v8::Isolate                   *m_isolate;
  v8::Persistent<v8::Object>    m_jsobject;
  v8::String::Utf8Value         *m_script;
  v8::Persistent<v8::Object>    m_callback;
};

#endif  // MT_WORKER_H_
