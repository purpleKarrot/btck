// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "Block.h"

#include <btck/btck.h>

#include <jni.h>
#include <stddef.h>
#include <stdint.h>

static jfieldID get_impl_field(JNIEnv* env, jobject obj)
{
  jclass cls = (*env)->GetObjectClass(env, obj);
  return (*env)->GetFieldID(env, cls, "impl", "J");
}

jlong Java_Block_construct(JNIEnv* env, jclass clazz, jobject buffer)
{
  (void)clazz;
  if (buffer == NULL) {
    return 0;
  }

  void* data = (*env)->GetDirectBufferAddress(env, buffer);
  jlong len = (*env)->GetDirectBufferCapacity(env, buffer);
  if (data == NULL || len <= 0) {
    return 0;
  }

  return (jlong)(uintptr_t)BtcK_Block_New(data, (size_t)len, NULL);
}

void Java_Block_close(JNIEnv* env, jobject self)
{
  jfieldID fid = get_impl_field(env, self);
  jlong handle = (*env)->GetLongField(env, self, fid);
  struct BtcK_Block* ptr = (struct BtcK_Block*)(uintptr_t)handle;
  if (ptr != NULL) {
    BtcK_Block_Free(ptr);
    (*env)->SetLongField(env, self, fid, (jlong)0);
  }
}

jint Java_Block_size(JNIEnv* env, jobject self)
{
  jfieldID fid = get_impl_field(env, self);
  jlong handle = (*env)->GetLongField(env, self, fid);
  struct BtcK_Block* ptr = (struct BtcK_Block*)(uintptr_t)handle;
  if (ptr == NULL) {
    return 0;
  }

  return (jint)BtcK_Block_CountTransactions(ptr);
}

jobject Java_Block_get(JNIEnv* env, jobject self, jint idx)
{
  jfieldID fid = get_impl_field(env, self);
  jlong handle = (*env)->GetLongField(env, self, fid);
  struct BtcK_Block* ptr = (struct BtcK_Block*)(uintptr_t)handle;
  if (ptr == NULL) {
    return NULL;
  }

  struct BtcK_Transaction* tx =
    BtcK_Transaction_Copy(BtcK_Block_GetTransaction(ptr, (size_t)idx), NULL);
  if (tx == NULL) {
    return NULL;
  }

  jclass txClass = (*env)->FindClass(env, "Transaction");
  if (txClass == NULL) {
    return NULL;
  }

  jmethodID ctor = (*env)->GetMethodID(env, txClass, "<init>", "(J)V");
  if (ctor == NULL) {
    return NULL;
  }

  jobject txObj = (*env)->NewObject(env, txClass, ctor, (jlong)(uintptr_t)tx);
  return txObj;
}

jobject Java_Block_asByteBuffer(JNIEnv* env, jobject self)
{
  jfieldID fid = get_impl_field(env, self);
  jlong handle = (*env)->GetLongField(env, self, fid);
  struct BtcK_Block* ptr = (struct BtcK_Block*)(uintptr_t)handle;
  if (ptr == NULL) {
    return NULL;
  }

  size_t len = 0;
  void const* data = BtcK_Block_AsBytes(ptr, &len);
  if (data == NULL || len == 0) {
    return NULL;
  }

  return (*env)->NewDirectByteBuffer(env, (void*)data, (jlong)len);
}
