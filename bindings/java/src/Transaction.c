// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "Transaction.h"

#include <btck/btck.h>

#include <jni.h>
#include <stdint.h>
#include <stdlib.h>

static jfieldID get_impl_field(JNIEnv* env, jobject obj)
{
  jclass cls = (*env)->GetObjectClass(env, obj);
  return (*env)->GetFieldID(env, cls, "impl", "J");
}

jlong Java_Transaction_construct(JNIEnv* env, jclass clazz, jobject raw)
{
  (void)clazz;
  if (raw == NULL) {
    return 0;
  }

  void* data = (*env)->GetDirectBufferAddress(env, raw);
  jlong len = (*env)->GetDirectBufferCapacity(env, raw);
  if (data == NULL || len <= 0) {
    return 0;
  }

  return (jlong)(uintptr_t)BtcK_Transaction_New(data, (size_t)len, NULL);
}

void Java_Transaction_close(JNIEnv* env, jobject self)
{
  jfieldID fid = get_impl_field(env, self);
  jlong handle = (*env)->GetLongField(env, self, fid);
  struct BtcK_Transaction* ptr = (struct BtcK_Transaction*)(uintptr_t)handle;
  if (ptr != NULL) {
    BtcK_Transaction_Free(ptr);
    (*env)->SetLongField(env, self, fid, (jlong)0);
  }
}

jint Java_Transaction_size(JNIEnv* env, jobject self)
{
  jfieldID fid = get_impl_field(env, self);
  jlong handle = (*env)->GetLongField(env, self, fid);
  struct BtcK_Transaction* ptr = (struct BtcK_Transaction*)(uintptr_t)handle;
  if (ptr == NULL) {
    return 0;
  }

  return (jint)BtcK_Transaction_CountOutputs(ptr);
}

jobject Java_Transaction_get(JNIEnv* env, jobject self, jint idx)
{
  jfieldID fid = get_impl_field(env, self);
  jlong handle = (*env)->GetLongField(env, self, fid);
  struct BtcK_Transaction* ptr = (struct BtcK_Transaction*)(uintptr_t)handle;
  if (ptr == NULL) {
    return NULL;
  }

  struct BtcK_TransactionOutput* out =
    BtcK_Transaction_GetOutput(ptr, (size_t)idx, NULL);
  if (out == NULL) {
    return NULL;
  }

  jclass outClass = (*env)->FindClass(env, "TransactionOutput");
  if (outClass == NULL) {
    return NULL;
  }

  jmethodID ctor = (*env)->GetMethodID(env, outClass, "<init>", "(J)V");
  if (ctor == NULL) {
    return NULL;
  }

  jobject outObj =
    (*env)->NewObject(env, outClass, ctor, (jlong)(uintptr_t)out);
  return outObj;
}

jobject Java_Transaction_asByteBuffer(JNIEnv* env, jobject self)
{
  jfieldID fid = get_impl_field(env, self);
  jlong handle = (*env)->GetLongField(env, self, fid);
  struct BtcK_Transaction* ptr = (struct BtcK_Transaction*)(uintptr_t)handle;
  if (ptr == NULL) {
    return NULL;
  }

  size_t len = 0;
  void const* data = BtcK_Transaction_AsBytes(ptr, &len);
  if (data == NULL || len == 0) {
    return NULL;
  }

  return (*env)->NewDirectByteBuffer(env, (void*)data, (jlong)len);
}
