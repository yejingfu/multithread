#ifndef MT_SHAREDBUFFER_H_
#define MT_SHAREDBUFFER_H_

#include <uv.h>
#include <v8.h>
#include <node.h>

class SharedBuffer {
public:
  SharedBuffer(int sz);
  ~SharedBuffer();

  static SharedBuffer* createSharedBuffer(int bufSize);
  static void releaseSharedBuffer(int id);
  static SharedBuffer* getSharedBuffer(int id);

  int id() const { return m_id; }
  int size() const { return m_size; }
  v8::Persistent<v8::Object>& getJSObject() { return m_jsobject; }

private:
  int m_id;
  int m_size;

  v8::Persistent<v8::Object> m_jsobject;
};

#endif  // MT_SHAREDBUFFER_H_
