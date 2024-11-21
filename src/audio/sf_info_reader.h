#ifndef SF_INFO_READER_H
#define SF_INFO_READER_H

struct SfInfoReader
{
    JNIEnv* env;
    jclass jSfInfo;
    jmethodID jSfInfoInit;
    char const* filePath;
    SF_INFO sf_info;

    SfInfoReader(JNIEnv* env, jstring jFilePath):
        env(env)
        , jSfInfo(env->FindClass("dev/nateschieber/groovesprings/jni/SfInfo"))
        , jSfInfoInit(env->GetMethodID(jSfInfo, "<init>", "(IIIIII)V"))
        , filePath(env->GetStringUTFChars(jFilePath, 0)) {}

    SF_INFO read() {
      SF_INFO sf_info;
      sf_open(SfInfoReader::filePath, SFM_READ, &sf_info);
      return sf_info;
    };

    jobject jWrap(SF_INFO sf_info) {
//       std::cout << "\n sf_info:::::::: \n";
//       std::cout << "\n    " << SfInfoReader::filePath;
//       std::cout << "\n    " << (int) sf_info.frames;
//       std::cout << "\n    " << sf_info.samplerate;
//       std::cout << "\n    " << sf_info.channels;
//       std::cout << "\n    " << sf_info.format;
//       std::cout << "\n    " << sf_info.sections;
//       std::cout << "\n    " << sf_info.seekable;
//       std::cout << "\n    " << SfInfoReader::jSfInfo;
//       std::cout << "\n    " << SfInfoReader::jSfInfoInit;
      jobject res = SfInfoReader::env->NewObject(
        SfInfoReader::jSfInfo,
        SfInfoReader::jSfInfoInit,
         (int) sf_info.frames,
         sf_info.samplerate,
         sf_info.channels,
         sf_info.format,
         sf_info.sections,
         sf_info.seekable
      );

      std::cout << "\n        " << res;
      return res;
    }
};


#endif


