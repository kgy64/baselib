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

#ifndef __SRC_ACCESS_BASE_H_INCLUDED__
#define __SRC_ACCESS_BASE_H_INCLUDED__

#include <Debug/Debug.h>

#include <android/native_window_jni.h>
#include <Memory/Memory.h>
#include <Exceptions/Exceptions.h>
#include <string>

SYS_DECLARE_MODULE(DM_ANDROID_ACCESS);

namespace AndroidAccess
{
    DEFINE_EXCEPTION(JavaException, nullptr, EX::Fatal);

    /// The Java Virtual Machine
    /*! This pointer is set in the initialization (see \ref AndroidAccess::Initialize()) and can
     *  be used anywhere in the C++ program, from any thread. Note that the JavaVM is shared globally. */
    extern JavaVM * jvm;

    /// The JNIEnv pointer of the main thread
    /*! Note that every thread has its own JNIEnv, and it is especially for the main thread. For other
     *  threads, call the function \ref AndroidAccess::getJNIEnv() or \ref AndroidAccess::ThreadJNIEnv::getJNIEnv()
     */
    extern JNIEnv * jenv;

    /*! This intitializer function must be called from the function \ref JNI_OnLoad()
     */
    jint Initialize(JavaVM * vm);

    /// Fetch the JNIEnv pointer of the current thread
    /*! This function can be used on any thread (including main one). Note that there is a more efficient way
     *  to do it: see \ref AndroidAccess::jenv for main thread, and \ref AndroidAccess::ThreadJNIEnv::getJNIEnv()
     *  for any other threads. However, calling this function needs only ~3 microseconds on my 1.7 GHz machine.
     *  */
    inline JNIEnv * getJNIEnv(void)
    {
        JNIEnv * jni;
        int status = AndroidAccess::jvm->GetEnv(reinterpret_cast<void**>(&jni), JNI_VERSION_1_6);
        ASSERT(status >= 0, "could not get JNIEnv pointer (thread not attached?)");
        return jni;
    }

    void CheckJavaException(JNIEnv * env);

    /// Class to represent JNIEnv for a thread
    /*! The main purpose of this class is to attach the thread to the Java VM and handle the JNIEnv for this thread.
     *  The destructor will detach accordingly.
     *  \note   Such a class must be created once for each thread. */
    class ThreadJNIEnv
    {
     public:
        ThreadJNIEnv(void);
        virtual ~ThreadJNIEnv();

        /// Get the JNIEnv pointer associated to this thread
        /*! \note   For the main thread, use the function AndroidAccess::getJNIEnv() */
        inline JNIEnv * getJNIEnv(void) const
        {
            return jni;
        }

     protected:
        void attachAndroidThread(void);
        void detachAndroidThread(void);

        JNIEnv * jni;

     private:
        SYS_DEFINE_CLASS_NAME("AndroidAccess::ThreadJNIEnv");

    }; // class ThreadJNIEnv

    class JString;
    typedef MEM::shared_ptr<JString> JStringPtr;

    /// Java String Accessor
    class JString
    {
        inline JString(JNIEnv * env, jobject obj):
            env(env),
            jstr(static_cast<jstring>(obj)),
            cstr(env->GetStringUTFChars(jstr, nullptr))
        {
        }

     public:
        inline ~JString()
        {
            env->ReleaseStringUTFChars(jstr, cstr);
        }

        static inline JStringPtr Create(JNIEnv * jenv, jobject obj)
        {
            return JStringPtr(new JString(jenv, obj));
        }

        const char * c_str(void) const
        {
            return cstr;
        }

     protected:
        JNIEnv * env;

        jstring jstr;

        const char * cstr;

    }; // class JString

    class JGlobalRef;
    typedef MEM::shared_ptr<JGlobalRef> JGlobalRefPtr;

    /// Holds a Global Reference for a Java Object
    class JGlobalRef
    {
     public:
        inline JGlobalRef(jobject obj, JNIEnv * env = AndroidAccess::getJNIEnv()):
            env(env),
            globalObj(obj ? env->NewGlobalRef(obj) : nullptr)
        {
            SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);
            SYS_DEBUG(DL_INFO1, "Global ref is " << globalObj << " to ref " << obj);
        }

        VIRTUAL_IF_DEBUG inline ~JGlobalRef()
        {
            SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);
            Remove();
        }

        inline static JGlobalRefPtr Create(jobject obj, JNIEnv * env = AndroidAccess::getJNIEnv())
        {
            return JGlobalRefPtr(new JGlobalRef(obj, env));
        }

        inline jobject get(void) const
        {
            return globalObj;
        }

        inline JGlobalRef & operator=(jobject obj)
        {
            SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);
            Remove();
            globalObj = obj ? env->NewGlobalRef(obj) : nullptr;
            SYS_DEBUG(DL_INFO1, "Global ref is " << globalObj << " to ref " << obj);
            return *this;
        }

     protected:
        JNIEnv * env;

        jobject globalObj;

        inline void Remove(void)
        {
            if (globalObj) {
                env->DeleteGlobalRef(globalObj);
                SYS_DEBUG(DL_INFO1, "Global ref " << globalObj << " is removed");
                globalObj = nullptr;
            }
        }

     private:
        SYS_DEFINE_CLASS_NAME("AndroidAccess::JGlobalRef");

    }; // class JGlobalRef

    class JObject
    {
     public:
        inline JObject(jobject jobj, JNIEnv * env = AndroidAccess::getJNIEnv()):
            env(env),
            obj(AndroidAccess::JGlobalRef::Create(jobj, env))
        {
            SYS_DEBUG_MEMBER(DM_PACALIB);
            env->DeleteLocalRef(jobj);
        }

        inline JObject(AndroidAccess::JGlobalRefPtr obj, JNIEnv * env = AndroidAccess::getJNIEnv()):
            env(env),
            obj(obj)
        {
            SYS_DEBUG_MEMBER(DM_PACALIB);
        }

        VIRTUAL_IF_DEBUG inline ~JObject()
        {
            SYS_DEBUG_MEMBER(DM_PACALIB);
        }

        inline jobject getObject(void) const
        {
            return obj->get();
        }

     protected:
        JNIEnv * env;

        AndroidAccess::JGlobalRefPtr obj;

     private:
        SYS_DEFINE_CLASS_NAME("AndroidAccess::JObject");

    }; // class AndroidAccess::JObject

    class JClass;
    typedef MEM::shared_ptr<JClass> JClassPtr;

    /// Represents a Java class
    class JClass
    {
        JClass(const char * classPath, bool create_now, JNIEnv * env);

     public:
        VIRTUAL_IF_DEBUG ~JClass();

        inline static JClassPtr Create(const char * classPath, bool create_now = true, JNIEnv * env = AndroidAccess::jenv)
        {
            return JClassPtr(new JClass(classPath, create_now, env));
        }

        void instantiate(void);

        inline jclass getClass(void) const
        {
            return javaLocalRef;
        }

        inline jobject get(void) const
        {
            ASSERT(globalClassInstance, "java class is not instantiated yet");
            return globalClassInstance->get();
        }

        inline JNIEnv * getEnv(void) const
        {
            return env;
        }

     protected:
        const char * myClassPath;

        JNIEnv * env;

        jclass javaLocalRef;

        jobject javaInstance;

        JGlobalRef globalClassObject;

        JGlobalRefPtr globalClassInstance;

     private:
        SYS_DEFINE_CLASS_NAME("AndroidAccess::JClass");

    }; // class JClass

    /// Functor to call a Java member function
    class JFunctionBase
    {
     protected:
        JFunctionBase(const JClass & jClass, const char * functionName, const char * signature);

        VIRTUAL_IF_DEBUG inline ~JFunctionBase()
        {
            SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);
        }

        inline jmethodID get(void) const
        {
            return javaFunction;
        }

        const JClass & jClass;

        JNIEnv * env;

        jmethodID javaFunction;

     private:
        SYS_DEFINE_CLASS_NAME("AndroidAccess::JFunctionBase");

    }; // class JFunctionBase

    class JFuncVoid;
    typedef MEM::shared_ptr<JFuncVoid> JFuncVoidPtr;

    class JFuncVoid: public JFunctionBase
    {
     public:
        inline JFuncVoid(const JClass & jClass, const char * functionName, const char * signature):
            JFunctionBase(jClass, functionName, signature)
        {
        }

        VIRTUAL_IF_DEBUG inline ~JFuncVoid()
        {
        }

        inline static JFuncVoidPtr Create(const JClass & jClass, const char * functionName, const char * signature)
        {
            return JFuncVoidPtr(new JFuncVoid(jClass, functionName, signature));
        }

        inline void operator()(JNIEnv * jenv, ...)
        {
            va_list args;
            va_start(args, jenv);
            jenv->CallVoidMethodV(jClass.get(), javaFunction, args);
            va_end(args);
        }

     private:
        SYS_DEFINE_CLASS_NAME("AndroidAccess::JFuncVoid");

    }; // class JFuncVoid

    class JFuncObject;
    typedef MEM::shared_ptr<JFuncObject> JFuncObjectPtr;

    class JFuncObject: public JFunctionBase
    {
     public:
        inline JFuncObject(const JClass & jClass, const char * functionName, const char * signature):
            JFunctionBase(jClass, functionName, signature)
        {
        }

        VIRTUAL_IF_DEBUG inline ~JFuncObject()
        {
        }

        inline static JFuncObjectPtr Create(const JClass & jClass, const char * functionName, const char * signature)
        {
            return JFuncObjectPtr(new JFuncObject(jClass, functionName, signature));
        }

        inline jobject operator()(JNIEnv * jenv, ...)
        {
            va_list args;
            va_start(args, jenv);
            jobject obj = jenv->CallObjectMethodV(jClass.get(), javaFunction, args);
            va_end(args);
            return obj;
        }

     private:
        SYS_DEFINE_CLASS_NAME("AndroidAccess::JFuncObject");

    }; // class JFuncObject

    class JFuncBoolean;
    typedef MEM::shared_ptr<JFuncBoolean> JFuncBooleanPtr;

    class JFuncBoolean: public JFunctionBase
    {
     public:
        inline JFuncBoolean(const JClass & jClass, const char * functionName, const char * signature):
            JFunctionBase(jClass, functionName, signature)
        {
        }

        VIRTUAL_IF_DEBUG inline ~JFuncBoolean()
        {
        }

        static inline JFuncBooleanPtr Create(const JClass & jClass, const char * functionName, const char * signature)
        {
            return JFuncBooleanPtr(new JFuncBoolean(jClass, functionName, signature));
        }

        inline jboolean operator()(JNIEnv * jenv, ...)
        {
            va_list args;
            va_start(args, jenv);
            jboolean obj = jenv->CallBooleanMethodV(jClass.get(), javaFunction, args);
            va_end(args);
            return obj;
        }

     private:
        SYS_DEFINE_CLASS_NAME("AndroidAccess::JFuncBoolean");

    }; // class JFuncBoolean

    class JFuncByte;
    typedef MEM::shared_ptr<JFuncByte> JFuncBytePtr;

    class JFuncByte: public JFunctionBase
    {
     public:
        inline JFuncByte(const JClass & jClass, const char * functionName, const char * signature):
            JFunctionBase(jClass, functionName, signature)
        {
        }

        VIRTUAL_IF_DEBUG inline ~JFuncByte()
        {
        }

        inline jbyte operator()(JNIEnv * jenv, ...)
        {
            va_list args;
            va_start(args, jenv);
            jbyte obj = jenv->CallByteMethodV(jClass.get(), javaFunction, args);
            va_end(args);
            return obj;
        }

     private:
        SYS_DEFINE_CLASS_NAME("AndroidAccess::JFuncByte");

    }; // class JFuncByte

    class JFuncChar;
    typedef MEM::shared_ptr<JFuncChar> JFuncCharPtr;

    class JFuncChar: public JFunctionBase
    {
     public:
        inline JFuncChar(const JClass & jClass, const char * functionName, const char * signature):
            JFunctionBase(jClass, functionName, signature)
        {
        }

        VIRTUAL_IF_DEBUG inline ~JFuncChar()
        {
        }

        static inline JFuncCharPtr Create(const JClass & jClass, const char * functionName, const char * signature)
        {
            return JFuncCharPtr(new JFuncChar(jClass, functionName, signature));
        }

        inline jchar operator()(JNIEnv * jenv, ...)
        {
            va_list args;
            va_start(args, jenv);
            jchar obj = jenv->CallCharMethodV(jClass.get(), javaFunction, args);
            va_end(args);
            return obj;
        }

     private:
        SYS_DEFINE_CLASS_NAME("AndroidAccess::JFuncChar");

    }; // class JFuncChar

    class JFuncShort;
    typedef MEM::shared_ptr<JFuncShort> JFuncShortPtr;

    class JFuncShort: public JFunctionBase
    {
     public:
        inline JFuncShort(const JClass & jClass, const char * functionName, const char * signature):
            JFunctionBase(jClass, functionName, signature)
        {
        }

        VIRTUAL_IF_DEBUG inline ~JFuncShort()
        {
        }

        static inline JFuncShortPtr Create(const JClass & jClass, const char * functionName, const char * signature)
        {
            return JFuncShortPtr(new JFuncShort(jClass, functionName, signature));
        }

        inline jshort operator()(JNIEnv * jenv, ...)
        {
            va_list args;
            va_start(args, jenv);
            jshort obj = jenv->CallShortMethodV(jClass.get(), javaFunction, args);
            va_end(args);
            return obj;
        }

     private:
        SYS_DEFINE_CLASS_NAME("AndroidAccess::JFuncShort");

    }; // class JFuncShort

    class JFuncInt;
    typedef MEM::shared_ptr<JFuncInt> JFuncIntPtr;

    class JFuncInt: public JFunctionBase
    {
     public:
        inline JFuncInt(const JClass & jClass, const char * functionName, const char * signature):
            JFunctionBase(jClass, functionName, signature)
        {
        }

        VIRTUAL_IF_DEBUG inline ~JFuncInt()
        {
        }

        static inline JFuncIntPtr Create(const JClass & jClass, const char * functionName, const char * signature)
        {
            return JFuncIntPtr(new JFuncInt(jClass, functionName, signature));
        }

        inline jint operator()(JNIEnv * jenv, ...)
        {
            va_list args;
            va_start(args, jenv);
            jint obj = jenv->CallIntMethodV(jClass.get(), javaFunction, args);
            va_end(args);
            return obj;
        }

     private:
        SYS_DEFINE_CLASS_NAME("AndroidAccess::JFuncInt");

    }; // class JFuncInt

    class JFuncLong;
    typedef MEM::shared_ptr<JFuncLong> JFuncLongPtr;

    class JFuncLong: public JFunctionBase
    {
     public:
        inline JFuncLong(const JClass & jClass, const char * functionName, const char * signature):
            JFunctionBase(jClass, functionName, signature)
        {
        }

        VIRTUAL_IF_DEBUG inline ~JFuncLong()
        {
        }

        static inline JFuncLongPtr Create(const JClass & jClass, const char * functionName, const char * signature)
        {
            return JFuncLongPtr(new JFuncLong(jClass, functionName, signature));
        }

        inline jlong operator()(JNIEnv * jenv, ...)
        {
            va_list args;
            va_start(args, jenv);
            jlong obj = jenv->CallLongMethodV(jClass.get(), javaFunction, args);
            va_end(args);
            return obj;
        }

     private:
        SYS_DEFINE_CLASS_NAME("AndroidAccess::JFuncLong");

    }; // class JFuncLong

    class JFuncFloat;
    typedef MEM::shared_ptr<JFuncFloat> JFuncFloatPtr;

    class JFuncFloat: public JFunctionBase
    {
     public:
        inline JFuncFloat(const JClass & jClass, const char * functionName, const char * signature):
            JFunctionBase(jClass, functionName, signature)
        {
        }

        VIRTUAL_IF_DEBUG inline ~JFuncFloat()
        {
        }

        static inline JFuncFloatPtr Create(const JClass & jClass, const char * functionName, const char * signature)
        {
            return JFuncFloatPtr(new JFuncFloat(jClass, functionName, signature));
        }

        inline jfloat operator()(JNIEnv * jenv, ...)
        {
            va_list args;
            va_start(args, jenv);
            jfloat obj = jenv->CallFloatMethodV(jClass.get(), javaFunction, args);
            va_end(args);
            return obj;
        }

     private:
        SYS_DEFINE_CLASS_NAME("AndroidAccess::JFuncFloat");

    }; // class JFuncFloat

    class JFuncDouble;
    typedef MEM::shared_ptr<JFuncDouble> JFuncDoublePtr;

    class JFuncDouble: public JFunctionBase
    {
     public:
        inline JFuncDouble(const JClass & jClass, const char * functionName, const char * signature):
            JFunctionBase(jClass, functionName, signature)
        {
        }

        VIRTUAL_IF_DEBUG inline ~JFuncDouble()
        {
        }

        static inline JFuncDoublePtr Create(const JClass & jClass, const char * functionName, const char * signature)
        {
            return JFuncDoublePtr(new JFuncDouble(jClass, functionName, signature));
        }

        inline jdouble operator()(JNIEnv * jenv, ...)
        {
            va_list args;
            va_start(args, jenv);
            jdouble obj = jenv->CallDoubleMethodV(jClass.get(), javaFunction, args);
            va_end(args);
            return obj;
        }

     private:
        SYS_DEFINE_CLASS_NAME("AndroidAccess::JFuncDouble");

    }; // class JFuncDouble

} // namespace AndroidAccess

#endif /* __SRC_ACCESS_BASE_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
