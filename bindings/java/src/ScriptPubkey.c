// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "ScriptPubkey.h"

#include <btck/btck.h>

static jfieldID get_native_handle_field(JNIEnv* env, jobject obj)
{
  jclass cls = (*env)->GetObjectClass(env, obj);
  return (*env)->GetFieldID(env, cls, "nativeHandle", "J");
}

JNIEXPORT jlong JNICALL Java_ScriptPubkey_construct(JNIEnv* env, jclass clazz, jbyteArray raw)
{
  (void)clazz;

  jsize len = (*env)->GetArrayLength(env, raw);
  jbyte* bytes = (*env)->GetByteArrayElements(env, raw, NULL);
  struct btck_ScriptPubkey* ptr = btck_ScriptPubkey_New(bytes, (size_t)len);

  (*env)->ReleaseByteArrayElements(env, raw, bytes, JNI_ABORT);
  return (jlong)(uintptr_t)ptr;
}

JNIEXPORT void JNICALL Java_ScriptPubkey_close(JNIEnv* env, jobject self)
{
  jfieldID fid = get_native_handle_field(env, self);
  jlong handle = (*env)->GetLongField(env, self, fid);
  struct btck_ScriptPubkey* ptr = (struct btck_ScriptPubkey*)(uintptr_t)handle;
  if (ptr) {
    btck_ScriptPubkey_Release(ptr);
    (*env)->SetLongField(env, self, fid, (jlong)0);
  }
}

JNIEXPORT jboolean JNICALL Java_ScriptPubkey_equal(JNIEnv* env, jobject self, jobject other)
{
  jfieldID fid = get_native_handle_field(env, self);
  jlong handle1 = (*env)->GetLongField(env, self, fid);
  jlong handle2 = (*env)->GetLongField(env, other, fid);
  struct btck_ScriptPubkey* ptr1 = (struct btck_ScriptPubkey*)(uintptr_t)handle1;
  struct btck_ScriptPubkey* ptr2 = (struct btck_ScriptPubkey*)(uintptr_t)handle2;
  if (!ptr1 || !ptr2) {
    return JNI_FALSE;
  }
  return btck_ScriptPubkey_Equal(ptr1, ptr2) ? JNI_TRUE : JNI_FALSE;
}
