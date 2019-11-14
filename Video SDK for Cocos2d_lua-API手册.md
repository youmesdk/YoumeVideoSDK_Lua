# Video SDK for Cocos2d Lua API接口手册

## 相关异步/同步处理方法介绍

游密语音引擎SDK提供的全部为Lua接口,接口调用都会立即返回,凡是本身需要较长耗时的接口调用都会采用异步回调的方式,所有接口都可以在主线程中直接使用。回调在主线程。

## API调用说明

添加如下代码：
`cc.exports.youmetalk = cc.YTalk:create()`
API的调用可使用“cc.exports.youmetalk:”来直接操作。
接口使用的基本流程为`初始化`->`收到初始化成功回调通知`->`加入语音频道`->`收到加入频道成功回调通知`->`使用其它接口`->`离开语音频道`->`反初始化`，要确保严格按照上述的顺序使用接口。

## 实现回调

需要实现以下回调：

  ``` lua
  -- 通用事件回调
  -- event：参见YouMeEvent定义
  -- errorcode：参见YouMeErrorCode定义
  cc.exports.youmetalk.OnEvent = function( event,  errorcode, channel, param )
     print("收到Talk回调:"..event..","..errorcode..","..channel..","..param)
  end

  -- RestApi回调
  cc.exports.youmetalk.OnRequestRestApi  = function( requestid, errcode, query, result )

  end

 -- 频道内用户列表的回调
 --  channel: 频道
 --  member_list_json: 变化的用户列表
 --  is_update: false为第一次查询，true为增量通知
 cc.exports.youmetalk.OnMemberChange = function ( channel,  member_list_json, is_update)
    print("OnMemberChange:"..channel..",json:"..member_list_json)
    -- member_list_json 样例：
    -- {"channelid":"123","memchange":[{"isJoin":true,"userid":"u541"},{"isJoin":true,"userid":"u948"}]}
  end
  
  cc.exports.youmetalk.OnBroadcast = function( bctype, channel, param1, param2, content )
    print("OnBroadcast:"..bctype..",channel:"..channel..",P1:"..param1..",P2"..param2..",content:"..content)
end
  ```

* **注册回调**

``` lua
cc.exports.youmetalk.registerScriptHandler(cc.exports.youmetalk,
	cc.exports.youmetalk.OnEvent,
	cc.exports.youmetalk.OnRequestRestApi,
	cc.exports.youmetalk.OnMemberChange,
	cc.exports.youmetalk.OnBroadcast );
```

## 初始化

* **语法**

```
init( strAppKey, strAPPSecret, serverRegionId, strExtServerRegionName );
```

* **功能**
初始化语音引擎，做APP验证和资源初始化。（只有appkey,appsecret,serverRegionId 都一样时,才可以进行语音互通,改任意一个视为另外一个区服）;

* **参数说明**
`strAPPKey`：string，从游密后台申请到的 app key, 这个是你们应用程序的唯一标识。
`strAPPSecret`：string， 对应 strAPPKey 的私钥, 这个需要妥善保存，不要暴露给其他人。
`serverRegionId`：int，设置首选连接服务器的区域码，参见状态码的YOUME_RTC_SERVER_REGION定义。如果在初始化时不能确定区域，可以填10001，后面确定时通过 setServerRegion 设置。如果YOUME_RTC_SERVER_REGION定义的区域码不能满足要求，可以把这个参数设为10000，然后通过后面的参数strExtServerRegionName 设置一个自定的区域值（如中国用 "cn" 或者 “ch"表示），然后把这个自定义的区域值同步给游密，我们将通过后台配置映射到最佳区域的服务器。
`strExtServerRegionName`：string,自定义的扩展的服务器区域名。可为空字符串“”。只有前一个参数serverRegionId设为10000时，此参数才有效（否则都将当空字符串“”处理）。

* **返回值**
int，返回0才会有异步回调通知。其它返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。

* **异步回调**

```
--  涉及到的主要回调事件有：
--  OnEvent
--  event: （0） - 表明初始化成功
--  event: （1） - 表明初始化失败，最常见的失败原因是网络错误或者 AppKey-AppSecret 错误

```
## 语音频道管理

### 加入语音频道（单频道）

* **语法**

``` lua
joinChannelSingleMode ( strUserID, strChannelID, roleType);
```
* **功能**
加入语音频道（单频道模式，每个时刻只能在一个语音频道里面）。

* **参数说明**
`strUserID`：string,全局唯一的用户标识，全局指在当前应用程序的范围内。
`strChannelID`：string, 全局唯一的频道标识，全局指在当前应用程序的范围内。
`roleType`：int,用户在语音频道里面的角色，见YouMeUserRole定义。


* **返回值**
int，返回0才会有异步回调通知。其它返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。

* **异步回调**

``` lua
-- 涉及到的主要回调事件有：
-- OnEvent
-- event:2 - 成功进入语音频道
-- event:3 - 进入语音频道失败，可能原因是网络或服务器有问题，
```

### 加入语音频道（多频道）

* **语法**

```
joinChannelMultiMode ( strUserID,strChannelID, roleType);
```
* **功能**
加入语音频道（多频道模式，可以同时听多个语音频道的内容，但每个时刻只能对着一个频道讲话）。

* **参数说明**
`strUserID`：string,全局唯一的用户标识，全局指在当前应用程序的范围内。
`strChannelID`：string,全局唯一的频道标识，全局指在当前应用程序的范围内。
`roleType`：YouMeUserRole_t，用户在语音频道里面的角色，见YouMeUserRole定义


* **返回值**
int，返回0才会有异步回调通知。其它返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。

* **异步回调**

```
-- 涉及到的主要回调事件有：
--  OnEvent
--  event: 2 - 成功进入语音频道
--  event: 3 - 进入语音频道失败，可能原因是网络或服务器有问题
```

### 指定讲话频道

* **语法**

``` lua
speakToChannel ( strChannelID);
```
* **功能**
多频道模式下，指定当前要讲话的频道。

* **参数说明**
`strChannelID`：string,全局唯一的频道标识，全局指在当前应用程序的范围内。

* **返回值**
int，返回0才会有异步回调通知。其它返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。

* **异步回调**

``` lua
-- 涉及到的主要回调事件有：
--  OnEvent
--  event: 8 - 成功切入到指定语音频道
--  event: 9 - 切入指定语音频道失败，可能原因是网络或服务器有问题

```

### 退出指定的语音频道

* **语法**

```
leaveChannelMultiMode (strChannelID);
```
* **功能**
多频道模式下，退出指定的语音频道。

* **参数说明**
`strChannelID`：string,全局唯一的频道标识，全局指在当前应用程序的范围内。

* **返回值**
int，返回0才会有异步回调通知。其它返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。

* **异步回调**

``` lua
-- 涉及到的主要回调事件有：
--  OnEvent
--  event: 4 - 退出指定语音频道完成
```

### 退出所有语音频道

* **语法**

``` lua
leaveChannelAll ();
```
* **功能**
退出所有的语音频道（单频道模式下直接调用此函数离开频道即可）。

* **返回值**
int，返回0才会有异步回调通知。其它返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。

* **异步回调**

``` lua
-- 涉及到的主要回调事件有：
--  OnEvent
--  event: 5 - 退出所有语音频道完成
```

## 设备状态管理

### 设置扬声器状态

* **语法**

```
setSpeakerMute (mute);
```
* **功能**
打开/关闭扬声器。**该状态值在加入房间成功后设置才有效。**（该接口是异步接口,调用后会有一段时间生效）


* **参数说明**
`mute`:true——关闭扬声器，false——开启扬声器。


### 获取扬声器状态

* **语法**

```
getSpeakerMute();
```

* **功能**
获取当前扬声器状态。（该接口为异步接口,尽量避免逻辑上设置了扬声器状态后立刻调用该接口）

* **返回值**
true——扬声器关闭，false——扬声器开启。


### 设置麦克风状态

* **语法**

```
setMicrophoneMute (mute);
```

* **功能**
打开／关闭麦克风。**该状态值在加入房间成功后设置才有效。**（该接口是异步接口,调用后会有一段时间生效）


* **参数说明**
`mute`:true——关闭麦克风，false——开启麦克风。


### 获取麦克风状态
* **语法**

```
getMicrophoneMute ();
```

* **功能**
获取当前麦克风状态。（该接口为异步接口,尽量避免逻辑上设置了扬声器状态后立刻调用该接口）

* **返回值**
true——麦克风关闭，false——麦克风开启。

### 设置是否通知别人麦克风和扬声器的开关

* **语法**

```
setAutoSendStatus( bAutoSend );
```

* **功能**
设置是否通知别人,自己麦克风和扬声器的开关状态（建议进入频道后调用，设置后其他当前在同一个频道的用户可以获取该用户的麦克风和扬声器的开关状态 对应回调事件YouMeEvent.YOUME_EVENT_OTHERS_MIC_ON,YouMeEvent.YOUME_EVENT_OTHERS_MIC_OFF,YouMeEvent.YOUME_EVENT_OTHERS_SPEAKER_ON,YouMeEvent.YOUME_EVENT_OTHERS_SPEAKER_OFF,事件对应具体详细请参考状态码）

* **参数说明**
`bAutoSend`:true——通知，false——不通知。

* **相关回调**

``` lua
--  涉及到的主要回调事件有(房间里的其他人会收到)：
--  OnEvent
--  event: 16 - 其他用户麦克风打开
--  event: 17 - 其他用户麦克风关闭
--  event: 18 - 其他用户扬声器打开
--  event: 19 - 其他用户扬声器关闭
```

### 设置音量

* **语法**

```
setVolume (uiVolume);
```

* **功能**
设置当前程序输出音量大小。建议该状态值在加入房间成功后按需再重置一次。

* **参数说明**
`uiVolume`:unsigned int，当前音量大小，范围[0-100]。

### 获取音量

* **语法**

```
getVolume ();
```

* **功能**
获取当前程序输出音量大小。

* **返回值**
unsigned int ，当前音量大小，范围[0-100]。

## 设置网络

### 设置是否允许使用移动网络

* **语法**

```
setUseMobileNetworkEnabled (bEnabled);
```

* **功能**
设置是否允许使用移动网络。在WIFI和移动网络都可用的情况下会优先使用WIFI，在没有WIFI的情况下，如果设置允许使用移动网络，那么会使用移动网络进行语音通信，否则通信会失败。


* **参数说明**
`bEnabled`:true——允许使用移动网络，false——禁止使用移动网络。


### 获取是否允许使用移动网络

* **语法**

```
getUseMobileNetworkEnabled () ;
```

* **功能**
获取是否允许SDK在没有WIFI的情况使用移动网络进行语音通信。

* **返回值**
true——允许使用移动网络，false——禁止使用移动网络，默认情况下允许使用移动网络。

## 设置是否听某人的语音

* **语法**

```
setListenOtherVoice (strUserID, on);
```

* **功能**
设置是否听某人的语音。

* **参数说明**
`strUserID`：string,要控制的用户ID。
`on`：true表示开启接收指定用户的语音，false表示屏蔽指定用户的语音。

* **返回值**
int，如果成功返回0，否则返回错误码，请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。

* **异步回调**

``` lua
--  涉及到的主要回调事件有(被控制方收到)：
--  OnEvent
--  event: 27 - 取消屏蔽某人语音
--  event: 28 - 屏蔽某人语音
```

## 设置当麦克风静音时，是否释放麦克风设备

* **语法**
```
setReleaseMicWhenMute(enabled);
```

* **功能**
设置当麦克风静音时，是否释放麦克风设备（需要在初始化成功后，加入房间之前调用）

* **参数说明**
`enabled`： true--当麦克风静音时，释放麦克风设备，此时允许第三方模块使用麦克风设备录音。在Android和iOS上，语音由通话音轨切换到媒体音轨，回归到双声道高音质输出体验（如果媒体音量和通话音量不同时切换会产生音量变大变小的感觉）；false--不管麦克风是否静音，麦克风设备都会被占用。

* **返回值**
返回YOUME_SUCCESS才会有异步回调通知。其它返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。


## 通话管理

### 暂停通话

* **语法**

```
pauseChannel();
```

* **功能**
暂停通话，释放对麦克风等设备资源的占用。当需要用第三方模块临时录音时，可调用这个接口。

* **返回值**
int，返回0才会有异步回调通知。其他返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。

* **异步回调**

``` lua
-- 主要回调事件：
--  OnEvent
--  event: 6 - 暂停语音频道完成
```

### 恢复通话

* **语法**

```
resumeChannel();
```

* **功能**
恢复通话，调用PauseChannel暂停通话后，可调用这个接口恢复通话。

* **返回值**
int，返回0才会有异步回调通知。其他返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。

* **异步回调**

``` lua
-- 主要回调事件：
--  OnEvent
--  event: 7 - 恢复语音频道完成
```

## 设置语音检测

* **语法**

```
setVadCallbackEnabled(bEnabled);
```

* **功能**
设置是否开启语音检测回调，开启后频道内有人正在讲话与结束讲话都会发起相应回调通知。（该状态值在加入房间成功后设置才有效，并且需要联系游密官方人员进行开启后方才生效）

* **参数说明**
`bEnabled`:true——打开，false——关闭。

* **返回值**
int，如果成功则返回0，其他返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。


## 设置是否开启讲话音量回调


* **语法**


```
setMicLevelCallback(int maxLevel)

```

* **功能**
设置是否开启监听自己讲话音量回调, 并设置相应的参数。

* **参数说明**
`maxLevel`:音量最大时对应的级别，最大可设100。根据实际需要设置小于100的值可以减少回调的次数（注意设置较高的值可能会产生大量回调，特别在Unity上会影响其它事件到达，一般建议不超过30）。比如你只在UI上呈现10级的音量变化，那就设10就可以了。设 0 表示关闭回调。


* **返回值**
int，如果成功则返回0，其他返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。

## 背景音乐管理

### 播放背景音乐

* **语法**

```
playBackgroundMusic ( strFilePath, bRepeat);
```

* **功能**
播放指定的音乐文件。播放的音乐将会通过扬声器输出，并和语音混合后发送给接收方。这个功能必须进入频道传入身份为主播/指挥才能使用。

* **参数说明**
`strFilePath`：音乐文件的路径。
`bRepeat`：是否重复播放，true——重复播放，false——只播放一次就停止播放。

* **返回值**
int，返回0才会有异步回调通知。其他返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。

* **异步回调**

``` lua
-- 主要回调事件：
--  OnEvent
--  event: 13 - 通知背景音乐播放结束
--  event: 14 - 通知背景音乐播放失败
```

### 停止播放背景音乐

* **语法**

```
stopBackgroundMusic();
```

* **功能**
停止播放当前正在播放的背景音乐。
这是一个同步调用接口，函数返回时，音乐播放也就停止了。

* **返回值**
int，如果成功返回0，表明成功停止了音乐播放流程；否则返回错误码，请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。

### 设置背景音乐播放音量

* **语法**

```
setBackgroundMusicVolume(vol);
```

* **功能**
设定背景音乐的音量。这个接口用于调整背景音乐和语音之间的相对音量，使得背景音乐和语音混合听起来协调。
这是一个同步调用接口。

* **参数说明**
`vol`:int,背景音乐的音量，范围 [0-100]。

* **返回值**
int，如果成功（表明成功设置了背景音乐的音量）返回0，否则返回错误码，具体请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。

## 设置监听

* **语法**

```
setHeadsetMonitorOn(micEnabled, bgmEnabled);
```

* **功能**
设置是否用耳机监听自己的声音或背景音乐，当不插耳机时，这个设置不起作用
这是一个同步调用接口。

* **参数说明**
`micEnabled`:是否监听麦克风 true 监听，false 不监听。
`bgmEnabled`:是否监听背景音乐 true 监听，false 不监听。

* **返回值**
int，如果成功则返回0，其他返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。


## 设置混响音效

* **语法**

```
setReverbEnabled( bEnabled);
```

* **功能**
设置是否开启混响音效，这个主要对主播/指挥有用。

* **参数说明**
`bEnabled`:true——打开，false——关闭。

* **返回值**
int，如果成功则返回0，其他返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。

## 设置服务器区域

* **语法**

```
setServerRegion(serverRegionId,strExtRegionName);
```

* **功能**
设置首选连接服务器的区域码,初始化前调用，新版本以集成到Init里面，参数三，不填默认为中国

* **参数说明**
`serverRegionId`：int,如果YOUME_RTC_SERVER_REGION定义的区域码不能满足要求，可以把这个参数设为 10000，然后通过后面的参数strExtServerRegionName 设置一个自定的区域值（如中国用 "cn" 或者 “ch"表示），然后把这个自定义的区域值同步给游密，我们将通过后台配置映射到最佳区域的服务器。
`strExtServerRegionName`：string,自定义的扩展的服务器区域名。可为空字符串“”。只有前一个参数serverRegionId设为10000时，此参数才有效（否则都将当空字符串“”处理）。


## RestApi——支持主播相关信息查询
* **语法**

```
requestRestApi( strCommand , strQueryBody  );
```
* **功能**
Rest API , 向服务器请求额外数据。支持主播信息，主播排班等功能查询。需要的话，请联系我们获取命令详情文档。


* **参数说明**
`strCommand`：请求的命令字符串，标识命令类型。
`strQueryBody`：请求需要的参数,json格式。

* **返回值**
int，小于0，表示错误码，大于0，表示本次查询的requestID。

* **代码示例**

``` lua
local body = "{\"ChannelID\":\"123456\",\"AreaID\":0}"
youmetalk:requestRestApi("query_talk_channel_user_count", body );
```

* **异步回调**

``` lua
-- requestID:int,回传ID
-- iErrorCode:int,错误码
-- strQuery:string,回传查询请求，json格式，包含command（回传strCommand参数）和query（回传strQueryBody参数）字段。
-- strResult:string,查询结果，json格式。
onRequestRestAPI( requestID, iErrorCode, strQuery, strResult )
```

* **回调示例**

``` lua
strQuery:{"command":"query_talk_channel_user_count","query":"{\"ChannelID\":\"123456\",\"AreaID\":0}"}
strResult:{"ActionStatus":"OK","ChannelID":"123456","ErrorCode":0,"ErrorInfo":"","UserCount":0}
```

## 安全验证码设置

* **语法**

```
setToken( strToken );
```

* **功能**
设置身份验证的token，需要配合后台接口。

* **参数说明**
`strToken`：string,身份验证用token，设置空字符串，清空token值，不进行身份验证。

## 查询频道用户列表

* **语法**

```
getChannelUserList( strChannelID,maxCount, notifyMemChange );
```

* **功能**
查询频道当前的用户列表， 并设置是否获取频道用户进出的通知。（必须自己在频道中）


* **参数说明**
`strChannelID`：string,频道ID。
`maxCount`：int,想要获取的最大人数。-1表示获取全部列表。
`notifyMemChange`：其他用户进出房间时，是否要收到通知。（设置为true，自己所在频道有人进出 都会有对应回调产生，反之false就没有对应回调）
* **返回值**
int，返回0才会有异步回调通知。其他返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。

* **异步回调**

``` lua
-- strChannel:string,频道ID
-- memchanges:对象数组，查询获得的用户列表，或变更列表。
-- 每一个数组的数据，包含userID和isJoin两个字段
onMemberChange(  strChannel,  memchanges )
```

## 视频相关接口
###  开始捕获本机摄像头数据

* **语法**

```
startCapture();
```

* **功能**
捕获本机摄像头数据，以便发送给房间内其他人

* **返回值**
int，返回0才会有异步回调通知。其他返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。

###  停止捕获本机摄像头数据

* **语法**

```
stopCapture();
```

* **功能**
停止捕获本机摄像头数据（比如退出房间、程序切换到后台时）

* **返回值**
int，返回0才会有异步回调通知。其他返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。

###  切换前后置摄像头

* **语法**

```
switchCamera();
```

* **功能**
切换前后置摄像头（默认使用的是前置摄像头）

* **返回值**
int，返回0才会有异步回调通知。其他返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。

###  设置是否前置摄像头

* **语法**

```
setCaptureFrontCameraEnable( enable );
```

* **功能**
设置是否前置摄像头

* **参数说明**
`enable`：是否默认使用前置摄像头，默认是true.

* **返回值**
int，返回0才会有异步回调通知。其他返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。

### 设置视频属性
* **语法**

```
setCaptureProperty(fps,  width, height );
```

* **功能**
设置本地视频渲染回调的分辨率及采集的帧率

* **参数说明**
`fps`：采集的帧率
`width`：宽
`height`：高

* **返回值**
int，返回0才会有异步回调通知。其他返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。

### 设置视频网络传输过程的分辨率
* **语法**

```
setNetCaptureProperty(  width, height );
```

* **功能**
设置视频网络传输过程的分辨率，低分辨率

* **参数说明**
`width`：宽
`height`：高

* **返回值**
int，返回0才会有异步回调通知。其他返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。


### 屏蔽/恢复他人视频

* **语法**

```
maskVideoByUserId(userId,  mask);
```

* **功能**
屏蔽他人视频（屏蔽后恢复他人视频也是调用此函数）

* **参数说明**
`userId`：要屏蔽的用户ID
`mask`:true是要屏蔽，false是要取消屏蔽

* **返回值**
int，返回0才会有异步回调通知。其他返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。

### 创建视频渲染

* **语法**

```
bindTexture(userId, width, height, texture )
```

* **功能**
为userid绑定texture。

* **参数说明**
`userId`：用户ID
`width`：视频显示的宽
`height`：视频显示的高
`texture`：显示视频画面的texture,用cc.RenderTexture:create创建。

* **返回值**
int，返回0才会有异步回调通知。其他返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。

### 删除视频渲染

* **语法**

```
unbindTexture(userId);
```

* **功能**
删除之前创建的渲染

* **参数说明**
`userId`：要删除的用户ID

* **返回值**
int，返回0才会有异步回调通知。其他返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。

### 清空所有渲染信息
* **语法**

```
cleanTextures();
```

* **功能**
删除之前创建的所有渲染


* **返回值**
int，返回0才会有异步回调通知。其他返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。


## 反初始化

* **语法**

```
unInit ();
```

* **功能**
反初始化引擎，可在退出游戏时调用，以释放SDK所有资源。

* **返回值**
int，如果成功则返回0，其他返回值请参考[YouMeErrorCode类型定义](TalkCocosLuaStatusCode.php#YouMeErrorCode定义)。


