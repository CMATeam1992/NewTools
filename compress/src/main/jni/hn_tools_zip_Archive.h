#ifndef _Included_vn_hn_tools_Archive
#define _Included_vn_hn_tools_Archive

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_vn_cma_compress_Archive_init
  (JNIEnv * env, jclass cls);

JNIEXPORT jint JNICALL Java_vn_cma_compress_Archive_createArchive
  (JNIEnv *, jobject, jstring, jobjectArray,jint length,jint level,jint dictionary,
		  jint wordSize,jboolean orderMode,jboolean solid,jlong solidBlockSize,jstring method,
		  jstring encryptionMethod,jint findex,
		  jboolean encryptHeaders,jboolean encryptHeadersAllowed,jstring pass
		  ,jboolean multiThread ,jobject callback);

#ifdef __cplusplus
}
#endif
#endif
