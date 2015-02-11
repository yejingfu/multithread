//sharedbuffer.cc
#include "sharedbuffer.h"

#include <map>
#include "nan.h"

using namespace v8;

namespace {
  std::map<int, SharedBuffer*> g_sharedBuffers;
  int g_count = 1;

  static Persistent<ObjectTemplate> buffer_template;
}

NAN_METHOD(test) {
  NanScope();
  printf("Jingfu: test\n");
  NanReturnUndefined();
}


//static 
SharedBuffer* SharedBuffer::createSharedBuffer(int bufSize) {
  SharedBuffer *buf = new SharedBuffer(bufSize);
  g_sharedBuffers[buf->m_id] = buf;

  if (buffer_template.IsEmpty()) {
    Local<ObjectTemplate> tpl = ObjectTemplate::New();
    tpl->SetInternalFieldCount(1);
    tpl->Set(NanNew<String>("id"), NanNew<Integer>(0), (PropertyAttribute)(ReadOnly | DontDelete));
    tpl->Set(NanNew<String>("test"), NanNew<FunctionTemplate>(test));
    NanAssignPersistent(buffer_template, tpl);
  }

  Local<Object> jsObj = NanNew(buffer_template)->NewInstance();
  jsObj->Set(NanNew<String>("id"), NanNew<Integer>(buf->m_id));
  jsObj->Set(NanNew<String>("size"), NanNew<Integer>(buf->m_size));
  NanSetInternalFieldPointer(jsObj, 0, buf);
  NanAssignPersistent(buf->m_jsobject, jsObj);

  return buf;
}

//static 
void SharedBuffer::releaseSharedBuffer(int id) {
  SharedBuffer *buf = getSharedBuffer(id);
  delete buf;
  g_sharedBuffers.erase(id);
}

// static
SharedBuffer* SharedBuffer::getSharedBuffer(int id) {
  return g_sharedBuffers[id];
}

SharedBuffer::SharedBuffer(int sz) {
  m_size = sz;
  m_id = g_count++;
}

SharedBuffer::~SharedBuffer() {

}


