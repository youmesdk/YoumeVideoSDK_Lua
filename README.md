# Video SDK for Cocos2d Lua 使用指南

游密实时视频SDK，如果用git克隆到本地需要使用：  
git lfs clone https://github.com/youmesdk/YoumeVideoSDK_Lua

## 适用范围

本规范仅适用于使用游密实时音视频引擎Cocos2d-lua接口开发多人实时语音功能的开发者。

## SDK目录概述

语音SDK中有两个子文件夹：VideoEngine_SDK，cocos_lua_SDK,下面依次介绍下这两个子文件夹。

### VideoEngine_SDK:游密video的SDK
1. `include`：SDK的头文件。
详细介绍下inlude，所有接口都在这个文件夹中。
    * `IYouMeVoiceEngine.h`封装了语音SDK的全部功能接口，集成方可通过IYouMeVoiceEngine::getInstance ()->…来调用语音SDK接口。
    * `IYouMeEventCallback.h`包含语音SDK的所有回调事件接口，例如初始化结果，频道加入成功等，都将通过此接口通知集成方
    * `YouMeConstDefine.h`包含语音SDK的所有枚举类型定义，如错误码等。
    * `IYouMeVideoCallback.h`包含音视频相关的回调接口。
2. `lib`：库文件，分为Android平台和iOS平台。
    *  Android平台下包括armeabi、armeabi-v7a,armeabi-v8a和X86四种CPU架构下的libyoume_voice_engine.so文件，还包括youme_voice_engine.jar。
        
    * iOS平台下包含libyoume_voice_engine.a，libffmpeg3.3.a, libYouMeCommon.a文件。

### cocos_lua_SDK：lua相关封装
此SDK是在引擎SDK基础上封装的为cocos-lua使用的SDK，包括一部分c++文件和lua文件。
 1. `YoumeClasses`：对应cocos-lua里的frameworks\runtime-src\Classes目录，包括一些封装过的c++文件以及c++转lua的文件
 2. `YoumeSrc`：对应cocos-lua里的src目录，里面的YoumeSDK.lua就是提供给游戏使用的lua文件

## 开发环境集成
* **引擎SDK集成**
Cocos2d-lua开发环境生成的目录结构如下图所示，将SDK/VideoEngine_SDK目录更名为youme_voice_engine（内含“include”和“lib”两个子文件夹），并复制到游戏的根目录下，这个目录下包含了Android和iOS两个平台所需的所有C++头文件和库文件。
![](/doc/images/talk_cocos_project_directory.png)

* **lua SDK集成**
把lua相关sdk的YoumeClasses目录放入cocos-lua项目的frameworks\runtime-src\Classes目录，将src目录放入cocos-lua项目的src目录，并加入项目。

* **lua注册**
修改cocos-lua原有项目中的frameworks\runtime-src\Classes\AppDelegate.cpp文件：
 1. 增加：`#include "lb/lb_youmetalk.hpp"`
 2. 在`AppDelegate::applicationDidFinishLaunching()`方法内加上：
 `register_all_youmetalk(stack->getLuaState();`


### Android系统开发环境配置

1. 修改proj.android/jni/Android.mk文件，对应位置增加指定内容，分别对游密实时语音SDK的动态库进行预编译处理、添加头文件路径、链接动态库。

  ``` Shell
    LOCAL_PATH := $(call my-dir)
    include $(CLEAR_VARS)
    $(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
    $(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
    $(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)

    #＝＝＝＝＝＝Youme 添加＝＝＝＝＝＝＝＝＝
    LOCAL_MODULE := youme_voice_engine
    LOCAL_SRC_FILES := ../$(LOCAL_PATH)/../../../../youme_voice_engine/lib/android/$(TARGET_ARCH_ABI)/libyoume_voice_engine.so
    include $(PREBUILT_SHARED_LIBRARY)
    #＝＝＝＝＝＝结束 Youme 添加＝＝＝＝＝＝＝

    LOCAL_MODULE := cocos2dcpp_shared
    LOCAL_MODULE_FILENAME := libcocos2dlua
    LOCAL_SRC_FILES := ../../Classes/AppDelegate.cpp \
    ../../Classes/YMRenderTexture.cpp \
    ../../Classes/YoumeTalk.cpp \
    ../../Classes/lb/lb_youmetalk.cpp \
    hellolua/main.cpp

    #＝＝＝＝＝＝==Youme修改＝＝＝＝＝＝＝＝＝
    LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes \
                        $(LOCAL_PATH)/../../../../youme_voice_engine/include
    #＝＝＝＝＝＝结束 Youme 修改＝＝＝＝＝＝＝

    # _COCOS_HEADER_ANDROID_BEGIN
    # _COCOS_HEADER_ANDROID_END
    LOCAL_STATIC_LIBRARIES := cocos2d_lua_static

    #＝＝＝＝＝＝Youme 添加＝＝＝＝＝＝＝＝＝
    LOCAL_SHARED_LIBRARIES := youme_voice_engine
    #＝＝＝＝＝＝结束 Youme 添加＝＝＝＝＝＝＝

    # _COCOS_LIB_ANDROID_BEGIN
    # _COCOS_LIB_ANDROID_END
    include $(BUILD_SHARED_LIBRARY)
    $(call import-module,.)
    # _COCOS_LIB_IMPORT_ANDROID_BEGIN
    # _COCOS_LIB_IMPORT_ANDROID_END

  ```

2. 如果需要显示指定CPU架构则修改proj.android/jni/Application.mk文件，增加指定部分的内容(v5版本为APP_ABI := armeabi)；如果不需要指定CPU架构Application.mk文件, 则不用修改。

``` shell
  #＝＝＝＝＝＝修改＝＝＝＝＝＝＝＝＝＝＝
  APP_ABI := armeabi-v7a
  #＝＝＝＝＝＝结束修改＝＝＝＝＝＝＝＝＝

  APP_STL := gnustl_static
  APP_CPPFLAGS := -frtti -DCC_ENABLE_CHIPMUNK_INTEGRATION=1 -std=c++11 -fsigned-char
  APP_LDFLAGS := -latomic
  ifeq ($(NDK_DEBUG),1)
   APP_CPPFLAGS += -DCOCOS2D_DEBUG=1
   APP_OPTIM := debug
  else
   APP_CPPFLAGS += -DNDEBUG
   APP_OPTIM := release
  endif

```

1. 复制youme_voice_engine/lib/android/youme_voice_engine.jar到proj.android/libs/ youme_voice_engine.jar。

2. 修改proj.android/AndroidManifest.xml文件，确保声明了如下的权限：

    ``` xml
    <uses-permission android:name="android.permission.INTERNET" />
    <uses-permission android:name="android.permission.ACCESS_WIFI_STATE" />
    <uses-permission android:name="android.permission.ACCESS_NETWORK_STATE" />
    <uses-permission android:name="android.permission.CHANGE_WIFI_STATE" />
    <uses-permission android:name="android.permission.CHANGE_NETWORK_STATE" />
    <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" />
    <uses-permission android:name="android.permission.RECORD_AUDIO" />
    <uses-permission android:name="android.permission.MODIFY_AUDIO_SETTINGS" />
    <uses-permission android:name="android.permission.READ_PHONE_STATE" />
    <uses-permission android:name="android.permission.BLUETOOTH" />
    <uses-permission android:name="android.permission.CAMERA" />
    <uses-permission android:name="android.permission.CAPTURE_VIDEO_OUTPUT" />
    <uses-permission android:name="android.permission.MOUNT_UNMOUNT_FILESYSTEMS"/>
    ```

3. 打开eclipse，导入上一步Android工程，在项目的第一个启动的AppActivity（找到AppActivity.java文件）中导入package:

``` java
import  com.youme.voiceengine.mgr.YouMeManager;
import  com.youme.voiceengine.*;
```

然后在onCreate方法里添加如下代码(没有此方法的话需要自己补上)：

``` java
 @Override
    public void onCreate(Bundle savedInstanceState)
    {
        //以下两个函数调用顺序不能错
        YouMeManager.Init(this);
        super.onCreate(savedInstanceState);

    }

```

### iOS系统XCode开发环境配置

添加头文件和依赖库:
1. 添加头文件路径：在`Build Settings` -> `Search Paths` -> `Header Search Paths`中添加:
    `../../../youme_voice_engine/include`  （建议直接将此include文件夹拖到xcode需要填入的位置，然后路径会自动生成)，
2. 添加库文件路径：在`Build Settings` -> `Search Paths` -> `Library Search Paths`中添加`../../../youme_voice_engine/lib/ios/Release-universal` （建议直接将此Release-universal文件夹拖到xcode需要填入的位置，然后路径会自动生成);
* **lua SDK配置**
参见[lua SDK集成](##开发环境集成)
3. 添加依赖库：在`Build Phases`  -> `Link Binary With Libraries`下添加：`libyoume_voice_engine.a`、`libYouMeCommon.a`,`libffmpeg3.3.a`,`libsqlite3.0.tbd`、,`libz.dylib`、`libz.1.2.5.tbd`、`libresolv.9.tbd`、`libc++.tbd`、`SystemConfiguration.framework`、`CoreTelephony.framework`、`AVFoundation.framework`、`AudioToolBox.framework`、`CFNetwork.framework`,`GLKit.framework`,`VideoToolbox.framework`,`OpenAL.framework`,`QuartzCore.framework`,`CoreMotion.framework`。

### 备注：
[详细接口介绍可查看“Video SDK for Cocos2d Lua API手册.md”文档](https://github.com/youmesdk/YoumeVideoSDK_Lua/blob/master/Video%20SDK%20for%20Cocos2d_lua-API%E6%89%8B%E5%86%8C.md)
