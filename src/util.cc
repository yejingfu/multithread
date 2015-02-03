#include "nan.h"

#include "util.h"
using namespace v8;

//  console section
static inline void console_common_1(const Handle<Value> &v, FILE* fd, const int deep) {
	char indent[36] = {};
	int i, n;
	int mark = 0;
	for (i=0; i<deep; ++i) {
		indent[mark++] = 0x20;
		indent[mark++] = 0x20;
	}

	Handle<Value> lv;
	if (v->IsFunction()) {
		fprintf(fd, "%s[Function]\n", indent);
	} else if (v->IsObject()) {
		Handle<Object> obj = Handle<Object>::Cast(v);
		Handle<Array> ar = obj->GetPropertyNames();
		fprintf(fd, "%s{Object}\n", indent);
		for (i=0, n=ar->Length(); i<n; ++i) {
			lv = obj->Get(ar->Get(i));
			fprintf(fd, "%s%s: ", indent, *(String::Utf8Value(Handle<String>::Cast(ar->Get(i)))));
			if (lv->IsFunction()) {
				fprintf(fd, "%s[Function]\n", indent);
			} else if (lv->IsObject() || lv->IsArray()) {
				//fprintf(fd, "\n");
				console_common_1(lv, fd, deep+1);
			} else {
				fprintf(fd, "%s%s\n", indent, *(String::Utf8Value(Handle<String>::Cast(lv))));
			}
		}
		fprintf(fd, "%s{/Object}\n", indent);

	} else if (v->IsArray()) {
		Handle<Array> obj = Handle<Array>::Cast(v);
		fprintf(fd, "%s[Array]\n", indent);
		for (i=0, n=obj->Length(); i<n; ++i) {
			lv = obj->Get(i);
			fprintf(fd, "%s%d: ", indent, i);
			if (lv->IsFunction()) {
				fprintf(fd, "%s[Function]\n", indent);
			} else if (lv->IsObject() || lv->IsArray()) {
				fprintf(fd, "\n");
				console_common_1(lv, fd, deep+1);
			} else {
				fprintf(fd, "%s%s\n", indent, *(String::Utf8Value(Handle<String>::Cast(lv))));
			}
		}
		fprintf(fd, "%s[/Array]\n", indent);
	} else {
		fprintf(fd, "%s%s\n", indent, *(String::Utf8Value(Handle<String>::Cast(v))));
	}
}

NAN_METHOD(console_log) {
	NanScope();
	
	TryCatch trycatch;
	
	for (int i=0, n=args.Length(); i<n; ++i) {
		console_common_1(args[i], stdout, 0);
	}

	if (trycatch.HasCaught()) {
		//ReportException(&trycatch);  // TODO
	}
	
	NanReturnUndefined();
}

NAN_METHOD(console_error) {
	NanScope();
	
	TryCatch trycatch;
	
	for (int i=0, n=args.Length(); i<n; ++i) {
		console_common_1(args[i], stderr, 0);
	}

	if (trycatch.HasCaught()) {
		//ReportException(&trycatch);  // TODO
	}
	
	NanReturnUndefined();
}


