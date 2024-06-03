#ifndef JNI_DATA_H
#define JNI_DATA_H

struct JNI_DATA{
    JNIEnv* env;
    jclass gsPlayback;
    jmethodID setCurrFrameId;
    jmethodID getPlayState;
    jclass jNum;
    jmethodID jNumInit;

    JNI_DATA(JNIEnv* env) :
        env(env)
          , gsPlayback(env->FindClass("dev/nateschieber/groovesprings/actors/GsPlaybackThread"))
          , setCurrFrameId(env->GetStaticMethodID (gsPlayback, "setCurrFrameId", "(Ljava/lang/Long;)V"))
          , getPlayState(env->GetStaticMethodID (gsPlayback, "getPlayState", "()I"))
          , jNum(env->FindClass("java/lang/Long"))
          , jNumInit(env->GetMethodID(jNum, "<init>", "(J)V")) {}
};

#endif
