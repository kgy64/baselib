/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     Android Access Library - an easy to use interface between Java
 *              and native C++ code.
 * Purpose:     This project is going to be an easy to use interface between Java
 *              and native C++ code.
 * Author:      Copyright (c) 2014 - Gyorgy Kovesdi <kgy@teledigit.eu>
 * License:     GPLv2
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "access-base.h"

#include <android/log.h>
#include <stdio.h>

SYS_DEFINE_MODULE(DM_ANDROID_ACCESS);

static const char myTag[] = "Ducktor Navi Access";

JavaVM * AndroidAccess::jvm;

JNIEnv * AndroidAccess::jenv;

using namespace AndroidAccess;

jint AndroidAccess::Initialize(JavaVM * vm)
{
 jvm = vm;
 jenv = getJNIEnv();

 __android_log_write(ANDROID_LOG_INFO, myTag, "Android Access Library has been initialized");

 return JNI_VERSION_1_6;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *       class JClass:                                                                   *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

JClass::JClass(JNIEnv * env, const char * classPath, bool create_now):
    env(env),
    javaLocalRef(env->FindClass(classPath)),
    javaInstance(NULL),
    globalClassObject(env, javaLocalRef)
{
 SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);
 SYS_DEBUG(DL_INFO1, "Class '" << classPath << "': " << (const void *)getClass() << ", jni=" << env);

 ASSERT(javaLocalRef, "could not find Java class '" << classPath << "'");
 ASSERT(globalClassObject.get(), "could not make global reference for Java class '" << classPath << "'");

 if (create_now) {
    instantiate();
 }
}

JClass::~JClass()
{
 SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);
}

void JClass::instantiate(void)
{
 SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);

 char constructor_path[300];
 jmethodID constructor = env->GetMethodID(javaLocalRef, "<init>", "()V");   // currently void constructor only (TODO)
 ASSERT(constructor, "could not get constructor '" << constructor_path << "'");
 javaInstance = env->NewObject(javaLocalRef, constructor);

 globalClassInstance = JGlobalRef::Create(env, javaInstance);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *       class ThreadJNIEnv:                                                             *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

ThreadJNIEnv::ThreadJNIEnv(void):
    jni(nullptr)
{
 SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);
}

ThreadJNIEnv::~ThreadJNIEnv()
{
 SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);
}

/// Attach the current thread to the Java VM
/*! This function must be called at the beginning of each Android thread.
 *  In addition to the thread attach, it fetches the JNIEnv pointer of the current thread.
 *  Note that the JNIEnv pointer is thread-specific. */
void ThreadJNIEnv::attachAndroidThread(void)
{
 SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);

 int status = AndroidAccess::jvm->AttachCurrentThread(&jni, NULL);
 ASSERT(status >= 0, "cannot attach thread to JVM");

 SYS_DEBUG(DL_INFO1, "Thread attached: JavaVM=" << jvm << ", JNIEnv=" << getJNIEnv());
}

/// Attach the current thread to the Java VM
/*! This function must be called after each Android thread. */
void ThreadJNIEnv::detachAndroidThread(void)
{
 SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);

 if (jni) {
    int status = jvm->DetachCurrentThread();
    if (status < 0) {
        __android_log_write(ANDROID_LOG_ERROR, myTag, "Android Access Library: could not detach thread");
    }
 }
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
