/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// OpenGL ES 2.0 code

#include <jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>

using namespace std;

#define  LOG_TAG    "libCrawler"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
         = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

struct ShaderManager
{
    GLint program;
    vector<string> mVertexShaders;
    vector<string> mPixelShaders;
    ShaderManager() : program(0){}
    ~ShaderManager()
    {
        if (program)
            glDeleteProgram(program);
    }
private:
    GLuint loadShader(GLenum shaderType, const char* pSource) 
    {
        if (!program)
            program = glCreateProgram();
        if (!program)
            return 0;
        
        GLuint shader = glCreateShader(shaderType);
        if (!shader)
            return 0;
        
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) 
        {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE("Could not compile shader %d:\n%s\n",
                         shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
        return shader;
    }
public:
    bool createProgram(GLuint shaderType, const char* pShaderSource) {
        if (!program)
            program = glCreateProgram();
        if (!program){
            checkGlError("++++++ message 1 ");
            return false;
        }
        
        GLuint shader = loadShader(shaderType, pShaderSource);
        if (!shader) {
            checkGlError("++++++ message 2");
            return false;
        }
        
        GLint retVal = program;
        glAttachShader(program, shader);
        checkGlError("++++++ message 3");
        glLinkProgram(program);
        return true;
    }
    
    GLint linkIt()
    {
        GLint linkStatus = GL_FALSE;
        GLint retVal = program;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = new char[bufLength];
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE("Could not link program:\n%s\n", buf);
                    delete[] buf;
                }
            }
            retVal = 0;
        }
        return retVal;
    }
    void addPixelShaderSource(const char* source)
    {
        string temp(source);
        mPixelShaders.push_back(temp);
    }
                    
    void addVertexShaderSource(const char* source)
    {
        string temp(source);
        mVertexShaders.push_back(temp);
    }
    
    const char* getVertexShader(int index) const{
        return mVertexShaders[index].c_str();
    }
    
    const char* getPixelShader(int index) const{
        return mPixelShaders[index].c_str();
    }
};

static ShaderManager gSM;

GLuint gProgram;
GLuint gvPositionHandle;

bool setupGraphics(int w, int h) {
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);
    
    if (!gSM.createProgram(GL_VERTEX_SHADER, gSM.getVertexShader(0))) {
        LOGE("Could not create program.");
        return false;
    }
    if (!gSM.createProgram(GL_FRAGMENT_SHADER, gSM.getPixelShader(0))) {
        LOGE("Could not create program.");
        return false;
    }
    gProgram = gSM.linkIt();
    
    gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
    checkGlError("glGetAttribLocation");
    LOGI("glGetAttribLocation(\"vPosition\") = %d\n",
         gvPositionHandle);
    
    glViewport(0, 0, w, h);
    checkGlError("glViewport");
    return true;
}

const GLfloat gTriangleVertices[] = { 0.0f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };

void renderFrame() {
    float grey = 0.01f;
    glClearColor(grey, grey, grey, 1.0f);
    checkGlError("glClearColor");
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");
    
    glUseProgram(gProgram);
    checkGlError("glUseProgram");
    
    glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, gTriangleVertices);
    checkGlError("glVertexAttribPointer");
    glEnableVertexAttribArray(gvPositionHandle);
    checkGlError("glEnableVertexAttribArray");
    glDrawArrays(GL_TRIANGLES, 0, 3);
    checkGlError("glDrawArrays");
}

extern "C" {
    JNIEXPORT void JNICALL Java_com_games_Crawler_CrawlerLib_Initialize(JNIEnv * env, jobject obj,  jint width, jint height);
    JNIEXPORT void JNICALL Java_com_games_Crawler_CrawlerLib_Render(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_games_Crawler_CrawlerLib_LoadPixelShaders(JNIEnv * env, jobject obj, jstring shader);
    JNIEXPORT void JNICALL Java_com_games_Crawler_CrawlerLib_LoadVertexShaders(JNIEnv * env, jobject obj, jstring shader);
};

JNIEXPORT void JNICALL Java_com_games_Crawler_CrawlerLib_LoadVertexShaders(JNIEnv * env, jobject obj, jstring shader)
{
    const char *nativeString = env->GetStringUTFChars(shader, 0);
    gSM.addVertexShaderSource(nativeString);
    env->ReleaseStringUTFChars(shader, nativeString);    
}

JNIEXPORT void JNICALL Java_com_games_Crawler_CrawlerLib_LoadPixelShaders(JNIEnv * env, jobject obj, jstring shader)
{
    const char *nativeString = env->GetStringUTFChars(shader, 0);
    gSM.addPixelShaderSource(nativeString);
    env->ReleaseStringUTFChars(shader, nativeString);    
}

JNIEXPORT void JNICALL Java_com_games_Crawler_CrawlerLib_Initialize(JNIEnv * env, jobject obj,  jint width, jint height)
{
    setupGraphics(width, height);
}

JNIEXPORT void JNICALL Java_com_games_Crawler_CrawlerLib_Render(JNIEnv * env, jobject obj)
{
    renderFrame();
}
