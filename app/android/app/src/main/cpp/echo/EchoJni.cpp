#include <jni.h>
#include <string>
#include <engine/core/render/gles/GLES.h>

// init
extern "C" JNIEXPORT void JNICALL Java_com_echo_app_GLESJniLib_init(JNIEnv *env, jclass clazz, jint width, jint height)
{
    int a = 10;
    //Echo::initRender(0);
    //Echo::initEngine( "", true);
}

// tick
extern "C" JNIEXPORT void JNICALL Java_com_echo_app_GLESJniLib_tick(JNIEnv *env, jclass clazz)
{
    int a = 1;
    //Echo::Engine::instance()->tick(0.1f);
}