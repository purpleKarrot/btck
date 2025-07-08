// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "TransactionOutput.h"

#include <btck/btck.h>

static jfieldID get_impl_field(JNIEnv* env, jobject obj)
{
  jclass cls = (*env)->GetObjectClass(env, obj);
  return (*env)->GetFieldID(env, cls, "impl", "J");
}

static struct btck_ScriptPubkey* get_scriptpubkey_ptr(JNIEnv* env, jobject scriptPubkeyObj)
{
  if (!scriptPubkeyObj) {
    return NULL;
  }

  jclass cls = (*env)->GetObjectClass(env, scriptPubkeyObj);
  jfieldID fid = (*env)->GetFieldID(env, cls, "impl", "J");
  jlong handle = (*env)->GetLongField(env, scriptPubkeyObj, fid);
  return (struct btck_ScriptPubkey*)(uintptr_t)handle;
}

JNIEXPORT jlong JNICALL
Java_TransactionOutput_construct(JNIEnv* env, jclass clazz, jlong amount, jobject scriptPubkeyObj)
{
  (void)clazz;
  struct btck_ScriptPubkey* script_pubkey = get_scriptpubkey_ptr(env, scriptPubkeyObj);
  struct btck_TransactionOutput* ptr = btck_TransactionOutput_New((int64_t)amount, script_pubkey);
  return (jlong)(uintptr_t)ptr;
}

JNIEXPORT void JNICALL Java_TransactionOutput_close(JNIEnv* env, jobject self)
{
  jfieldID fid = get_impl_field(env, self);
  jlong handle = (*env)->GetLongField(env, self, fid);

  struct btck_TransactionOutput* ptr = (struct btck_TransactionOutput*)(uintptr_t)handle;
  if (ptr) {
    btck_TransactionOutput_Release(ptr);
    (*env)->SetLongField(env, self, fid, (jlong)0);
  }
}

JNIEXPORT jlong JNICALL Java_TransactionOutput_getAmount(JNIEnv* env, jobject self)
{
  jfieldID fid = get_impl_field(env, self);
  jlong handle = (*env)->GetLongField(env, self, fid);

  struct btck_TransactionOutput* ptr = (struct btck_TransactionOutput*)(uintptr_t)handle;
  if (!ptr) {
    return 0;
  }

  return (jlong)btck_TransactionOutput_GetAmount(ptr);
}

JNIEXPORT jobject JNICALL Java_TransactionOutput_getScriptPubkey(JNIEnv* env, jobject self)
{
  jfieldID fid = get_impl_field(env, self);

  jlong handle = (*env)->GetLongField(env, self, fid);
  struct btck_TransactionOutput* ptr = (struct btck_TransactionOutput*)(uintptr_t)handle;
  if (!ptr) {
    return NULL;
  }

  struct btck_ScriptPubkey* spk = btck_TransactionOutput_GetScriptPubkey(ptr);
  if (!spk) {
    return NULL;
  }

  jclass spkClass = (*env)->FindClass(env, "ScriptPubkey");
  if (!spkClass) {
    return NULL;
  }

  jmethodID ctor = (*env)->GetMethodID(env, spkClass, "<init>", "(J)V");
  if (!ctor) {
    return NULL;
  }

  jobject spkObj = (*env)->NewObject(env, spkClass, ctor, (jlong)(uintptr_t)spk);
  return spkObj;
}
