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

/// Converts Java exception to C++ exception
void AndroidAccess::CheckJavaException(JNIEnv * env)
{
 if (!env->ExceptionCheck()) {
    return;
 }

 jthrowable e = env->ExceptionOccurred();
 env->ExceptionClear();
 ASSERT(e, "got a Java Exception, but could not get detailed information");

 jmethodID toString = env->GetMethodID(env->FindClass("java/lang/Object"), "toString", "()Ljava/lang/String;");
 jstring estring = (jstring)env->CallObjectMethod(e, toString);

 throw AndroidAccess::JavaException() << "Java Exception: " << env->GetStringUTFChars(estring, nullptr);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *       class JClass:                                                                   *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

JClass::JClass(const char * classPath, jobject instance, JNIEnv * env):
    myClassPath(classPath),
    env(env),
    javaLocalRef(env->FindClass(classPath)),
    globalClassObject(nullptr, env)
{
 AndroidAccess::CheckJavaException(env);

 SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);
 SYS_DEBUG(DL_INFO1, "Class '" << myClassPath << "': " << (const void *)getClass() << ", jni=" << env);

 ASSERT(javaLocalRef, "could not find Java class '" << myClassPath << "'");
 globalClassObject = javaLocalRef;
 ASSERT(globalClassObject.get(), "could not make global reference for Java class '" << myClassPath << "'");

 instantiate(instance);

 SYS_DEBUG(DL_INFO1, "Created " << myClassPath);
}

JClass::~JClass()
{
 SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);

 SYS_DEBUG(DL_INFO1, "Deleted " << myClassPath);
}

void JClass::instantiate(jobject instance)
{
 SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);

 if (instance) {
    globalClassInstance = JGlobalRef::Create(instance, env);
    env->DeleteLocalRef(instance);
 } else {
    jmethodID constructor = env->GetMethodID(javaLocalRef, "<init>", "()V");   // currently void constructor only (TODO)
    ASSERT(constructor, "could not get constructor for " << myClassPath);
    jobject javaInstance = env->NewObject(javaLocalRef, constructor);
    globalClassInstance = JGlobalRef::Create(javaInstance, env);
 }
 SYS_DEBUG(DL_INFO1, "Instantiated " << myClassPath);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *       class JFunctionBase:                                                            *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

JFunctionBase::JFunctionBase(const JClass & jClass, const char * functionName, const char * signature):
    jClass(jClass),
    env(jClass.getEnv()),
    javaFunction(env->GetMethodID(jClass.getClass(), functionName, signature))
{
 AndroidAccess::CheckJavaException(env);

 SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);
 SYS_DEBUG(DL_INFO1, "Function '" << functionName << "', signature: " << signature << ", id: " << (const void *)javaFunction);
 ASSERT(javaFunction, "could not get Java function '" << functionName << ", signature: " << signature);
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

 int status = AndroidAccess::jvm->AttachCurrentThread(&jni, nullptr);
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
