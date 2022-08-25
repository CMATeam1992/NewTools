#ifndef _Included_vn_cma_extract_Archive
#define _Included_vn_cma_extract_Archive

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_vn_cma_extract_Archive_init
  (JNIEnv * env, jclass cls);


JNIEXPORT jint JNICALL Java_vn_cma_extract_Archive_extractArchive
  (JNIEnv *, jobject, jstring, jstring,jobject);

JNIEXPORT jint JNICALL Java_vn_cma_extract_Archive_listArchive2
		(JNIEnv *, jobject, jstring, jobject, jstring);

#ifdef __cplusplus
}
#endif
#endif
