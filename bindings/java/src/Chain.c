// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "Chain.h"

#include <btck/btck.h>

#include <jni.h>
#include <stddef.h>
#include <stdint.h>

static jfieldID get_impl_field(JNIEnv* env, jobject obj)
{
  jclass cls = (*env)->GetObjectClass(env, obj);
  return (*env)->GetFieldID(env, cls, "impl", "J");
}

JNIEXPORT jlong JNICALL Java_Chain_construct(JNIEnv* env, jclass clazz)
{
  (void)env, (void)clazz;
  // BtcK_Chain* ptr = BtcK_Chain_New();
  // return (jlong)(uintptr_t)ptr;
  return 0;
}

JNIEXPORT void JNICALL Java_Chain_close(JNIEnv* env, jobject self)
{
  jfieldID fid = get_impl_field(env, self);
  jlong handle = (*env)->GetLongField(env, self, fid);
  struct BtcK_Chain* ptr = (struct BtcK_Chain*)(uintptr_t)handle;
  if (ptr != NULL) {
    BtcK_Chain_Release(ptr);
    (*env)->SetLongField(env, self, fid, (jlong)0);
  }
}

JNIEXPORT jint JNICALL Java_Chain_size(JNIEnv* env, jobject self)
{
  jfieldID fid = get_impl_field(env, self);
  jlong handle = (*env)->GetLongField(env, self, fid);
  struct BtcK_Chain* ptr = (struct BtcK_Chain*)(uintptr_t)handle;
  if (ptr == NULL) {
    return 0;
  }

  return (jint)BtcK_Chain_NumBlocks(ptr);
}

JNIEXPORT jobject JNICALL Java_Chain_get(JNIEnv* env, jobject self, jint idx)
{
  jfieldID fid = get_impl_field(env, self);
  jlong handle = (*env)->GetLongField(env, self, fid);
  struct BtcK_Chain* ptr = (struct BtcK_Chain*)(uintptr_t)handle;
  if (ptr == NULL) {
    return NULL;
  }

  struct BtcK_Block* blk = BtcK_Chain_GetBlock(ptr, (size_t)idx);
  if (blk == NULL) {
    return NULL;
  }

  jclass blkClass = (*env)->FindClass(env, "Block");
  if (blkClass == NULL) {
    return NULL;
  }

  jmethodID ctor = (*env)->GetMethodID(env, blkClass, "<init>", "(J)V");
  if (ctor == NULL) {
    return NULL;
  }

  jobject blkObj =
    (*env)->NewObject(env, blkClass, ctor, (jlong)(uintptr_t)blk);
  return blkObj;
}
