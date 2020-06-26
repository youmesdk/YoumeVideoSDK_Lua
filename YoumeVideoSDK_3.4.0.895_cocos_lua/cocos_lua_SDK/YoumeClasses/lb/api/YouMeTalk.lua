
--------------------------------
-- @module YouMeTalk
-- @parent_module ym

--------------------------------
-- 功能描述:退出所有语音频道
-- return 错误码，详见YouMeConstDefine.h定义
-- @function [parent=#YouMeTalk] leaveChannelAll 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述:获取音量大小,此音量值为程序内部的音量，与系统音量相乘得到程序使用的实际音量
-- return 音量值[0,100]
-- @function [parent=#YouMeTalk] getVolume 
-- @param self
-- @return unsigned int#unsigned int ret (return value: unsigned int)

--------------------------------
-- 功能描述:Rest API , 向服务器请求额外数据
-- param requestID: 回传id,回调的时候传回，标识消息
-- param strCommand: 请求的命令字符串
-- param strQueryBody: 请求需要的数据,json格式，内容参考restAPI
-- return YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] requestRestApi 
-- @param self
-- @param #string strCommand
-- @param #string strQueryBody
-- @param #int requestID
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述:设置扬声器静音
-- param bOn:true——静音，false——取消静音
-- return 无
-- @function [parent=#YouMeTalk] setSpeakerMute 
-- @param self
-- @param #bool bOn
-- @return YouMeTalk#YouMeTalk self (return value: YouMeTalk)
        
--------------------------------
-- 功能描述:设置麦克风静音
-- param bOn:true——静音，false——取消静音
-- return 无
-- @function [parent=#YouMeTalk] setMicrophoneMute 
-- @param self
-- @param #bool mute
-- @return YouMeTalk#YouMeTalk self (return value: YouMeTalk)
        
--------------------------------
-- 功能描述: 暂停通话，释放麦克风等设备资源
-- return YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] pauseChannel 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述: 创建渲染
-- param : userId 用户ID
-- return 大于等于0 - renderId
-- 小于0 - 具体错误码
-- @function [parent=#YouMeTalk] createRender 
-- @param self
-- @param #char userId
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述: 恢复通话
-- return YOUME_SUCCESS - 成功
--  其他 - 具体错误码
-- @function [parent=#YouMeTalk] resumeChannel 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#YouMeTalk] maskVideoByUserId 
-- @param self
-- @param #char userId
-- @param #int mask
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述: 设置是否开启语音检测回调
-- param enabled, true 开启，false 关闭
-- return YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] setVadCallbackEnabled 
-- @param self
-- @param #bool enabled
-- @return int#int ret (return value: int)
        
--------------------------------
--  功能描述: 设置是否开启语音检测回调
-- param enabled, true 开启，false 关闭
-- return YOUME_SUCCESS - 成功
--  其他 - 具体错误码
-- @function [parent=#YouMeTalk] setReverbEnabled 
-- @param self
-- @param #bool enabled
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述:获取SDK 版本号
-- return 整形数字版本号
-- @function [parent=#YouMeTalk] getSDKVersion 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述: 设置背景音乐播放的音量
-- param vol 背景音乐的音量，范围 0-100
-- return YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] setBackgroundMusicVolume 
-- @param self
-- @param #int vol
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述: 设置是否用耳机监听自己的声音或背景音乐，当不插耳机或外部输入模式时，这个设置不起作用
-- param micEnabled 是否监听麦克风 true 监听，false 不监听
-- param bgmEnabled 是否监听背景音乐 true 监听，false 不监听
-- return YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] setHeadsetMonitorOn 
-- @param self
-- @param #bool micEnabled
-- @param #bool bgmEnabled
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述:设置服务器区域
-- param region: YOUME_RTC_SERVER_REGION枚举可选的服务器区域
-- return 无
-- @function [parent=#YouMeTalk] setServerRegion 
-- @param self
-- @param #int regionId
-- @param #char extRegionName
-- @param #bool bAppend
-- @return YouMeTalk#YouMeTalk self (return value: YouMeTalk)
        
--------------------------------
-- 功能描述: 切换前置/后置摄像头
-- param<br>
-- return YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] switchCamera 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述：对指定频道说话
-- param pChannelID: 频道ID，要保证全局唯一
-- return 错误码，详见YouMeConstDefine.h定义
-- @function [parent=#YouMeTalk] speakToChannel 
-- @param self
-- @param #char pChannelID
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述: 开始camera capture
-- param<br>
-- return YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] startCapture 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述:获取扬声器静音状态
-- return true——静音，false——没有静音
-- @function [parent=#YouMeTalk] getSpeakerMute 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#YouMeTalk] cleanTextures 
-- @param self
-- @return YouMeTalk#YouMeTalk self (return value: YouMeTalk)
        
--------------------------------
-- 功能描述:    抢麦相关设置（抢麦活动发起前调用此接口进行设置）
-- param const char * pChannelID: 抢麦活动的频道id
-- param int mode: 抢麦模式（1:先到先得模式；2:按权重分配模式）
-- param int maxAllowCount: 允许能抢到麦的最大人数
-- param int maxTalkTime: 允许抢到麦后使用麦的最大时间（秒）
-- param unsigned int voteTime: 抢麦仲裁时间（秒），过了X秒后服务器将进行仲裁谁最终获得麦（仅在按权重分配模式下有效）
-- return   YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] setGrabMicOption 
-- @param self
-- @param #char pChannelID
-- @param #int mode
-- @param #int maxAllowCount
-- @param #int maxTalkTime
-- @param #unsigned int voteTime
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述:    连麦相关设置（角色是频道的管理者或者主播时调用此接口进行频道内的连麦设置）
-- param const char * pChannelID: 连麦的频道id
-- param int waitTimeout:等待对方响应超时时间（秒）
-- param int maxTalkTime: 最大通话时间（秒）
-- return   YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] setInviteMicOption 
-- @param self
-- @param #char pChannelID
-- @param #int waitTimeout
-- @param #int maxTalkTime
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述: 如果当前正在播放背景音乐的话，停止播放
-- return YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] stopBackgroundMusic 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述:启用/禁用移动网络
-- param bEnabled:true-可以启用，false-禁用，默认禁用
-- return 无
-- @function [parent=#YouMeTalk] setUseMobileNetworkEnabled 
-- @param self
-- @param #bool bEnabled
-- @return YouMeTalk#YouMeTalk self (return value: YouMeTalk)
        
--------------------------------
-- 
-- @function [parent=#YouMeTalk] init 
-- @param self
-- @param #char pAPPKey
-- @param #char pAPPSecret
-- @param #int serverRegionId
-- @param #char pExtServerRegionName
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述:    停止连麦
-- return   YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] stopInviteMic 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述:    对连麦请求做出回应（被动应答）
-- param const char * pUserID: 主叫方的用户id
-- param bool isAccept:是否同意连麦
-- param const char * pContent:游戏传入的上下文内容，通知回调会传回此内容（目前只支持纯文本格式）
-- return   YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] responseInviteMic 
-- @param self
-- @param #char pUserID
-- @param #bool isAccept
-- @param #char pContent
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述: 设置是否开启讲话音量回调, 并设置相应的参数
-- param maxLevel, , 音量最大时对应的级别，最大可设100。根据实际需要设置小于100的值可以减少回调的次数。
--                   比如你只在UI上呈现10级的音量变化，那就设10就可以了。
--                   设 0 表示关闭回调。
-- return YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] setMicLevelCallback 
-- @param self
-- @param #int maxLevel
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述:查询频道的用户列表
-- param channelID:要查询的频道ID
-- param maxCount:想要获取的最大数量，-1表示获取全部
-- param notifyMemChagne: 其他用户进出房间时，是否要收到通知
-- return 错误码，详见YouMeConstDefine.h定义
-- @function [parent=#YouMeTalk] getChannelUserList 
-- @param self
-- @param #char channelID
-- @param #int maxCount
-- @param #bool notifyMemChange
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述:设置音量，取值范围是[0-100] 100表示最大音量， 默认音量是100
-- return 无
-- @function [parent=#YouMeTalk] setVolume 
-- @param self
-- @param #unsigned int uiVolume
-- @return YouMeTalk#YouMeTalk self (return value: YouMeTalk)
        
--------------------------------
-- 
-- @function [parent=#YouMeTalk] unbindTexture 
-- @param self
-- @param #string userId
-- @return YouMeTalk#YouMeTalk self (return value: YouMeTalk)
        
--------------------------------
-- 功能描述: 设置是否开启视频编码器
-- param pFilePath: yuv文件的绝对路径
-- return YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] openVideoEncoder 
-- @param self
-- @param #char pFilePath
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述:    发起抢麦请求
-- param const char * pChannelID:  抢麦的频道id
-- param int score: 积分（权重分配模式下有效，游戏根据自己实际情况设置）
-- param bool isAutoOpenMic: 抢麦成功后是否自动开启麦克风权限
-- param const char * pContent: 游戏传入的上下文内容，通知回调会传回此内容（目前只支持纯文本格式）
-- return   YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] requestGrabMic 
-- @param self
-- @param #char pChannelID
-- @param #int score
-- @param #bool isAutoOpenMic
-- @param #char pContent
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述:退出多频道模式下的某个语音频道
-- param pChannelID:频道ID，要保证全局唯一
-- return 错误码，详见YouMeConstDefine.h定义
-- @function [parent=#YouMeTalk] leaveChannelMultiMode 
-- @param self
-- @param #char pChannelID
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述: 设置当麦克风静音时，是否释放麦克风设备，在初始化之后、加入房间之前调用
-- param enabled,<br>
-- true 当麦克风静音时，释放麦克风设备，此时允许第三方模块使用麦克风设备录音。在Android上，语音通过媒体音轨，而不是通话音轨输出。
-- false 不管麦克风是否静音，麦克风设备都会被占用。
-- return YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] setReleaseMicWhenMute 
-- @param self
-- @param #bool enabled
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述: 播放背景音乐，并允许选择混合到扬声器输出麦克风输入
-- param pFilePath 音乐文件的路径
-- param bRepeat 是否重复播放
-- return YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] playBackgroundMusic 
-- @param self
-- @param #char pFilePath
-- @param #bool bRepeat
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述:    发起抢麦活动
-- param const char * pChannelID:  抢麦活动的频道id
-- param const char * pContent: 游戏传入的上下文内容，通知回调会传回此内容（目前只支持纯文本格式）
-- return   YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] startGrabMicAction 
-- @param self
-- @param #char pChannelID
-- @param #char pContent
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述:是否可使用移动网络
-- return true-可以使用，false-禁用
-- @function [parent=#YouMeTalk] getUseMobileNetworkEnabled 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 功能描述:    发起与某人的连麦请求（主动呼叫）
-- param const char * pUserID: 被叫方的用户id
-- param const char * pContent: 游戏传入的上下文内容，通知回调会传回此内容（目前只支持纯文本格式）
-- return   YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] requestInviteMic 
-- @param self
-- @param #char pChannelID
-- @param #char pUserID
-- @param #char pContent
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述:     设置camera capture property
-- param<br>
-- return YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] setCaptureProperty 
-- @param self
-- @param #int nFps
-- @param #int nWidth
-- @param #int nHeight
-- @return int#int ret (return value: int)

--------------------------------
-- 功能描述:     设置Net camera capture property<br>
-- param<br>
-- return YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] setNetCaptureProperty
-- @param self
-- @param #int nWidth
-- @param #int nHeight
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述:选择消除其他人的语音
-- param pUserID:用户ID，要保证全局唯一
-- param on: false屏蔽对方语音，true取消屏蔽
-- return 错误码，详见YouMeConstDefine.h定义
-- @function [parent=#YouMeTalk] setListenOtherVoice 
-- @param self
-- @param #char pUserID
-- @param #bool on
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述:    停止抢麦活动
-- param const char * pChannelID: 抢麦活动的频道id
-- param const char * pContent: 游戏传入的上下文内容，通知回调会传回此内容（目前只支持纯文本格式）
-- return   YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] stopGrabMicAction 
-- @param self
-- @param #char pChannelID
-- @param #char pContent
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述:设置身份验证的token
-- param pToken: 身份验证用token，设置为NULL或者空字符串，清空token值。
-- return 无
-- @function [parent=#YouMeTalk] setToken 
-- @param self
-- @param #char pToken
-- @return YouMeTalk#YouMeTalk self (return value: YouMeTalk)
        
--------------------------------
-- 功能描述:设置是否通知其他人自己的开关麦克风和扬声器的状态
-- param bAutoSend:true——通知，false——不通知
-- return 无
-- @function [parent=#YouMeTalk] setAutoSendStatus 
-- @param self
-- @param #bool bAutoSend
-- @return YouMeTalk#YouMeTalk self (return value: YouMeTalk)
        
--------------------------------
-- 功能描述：加入语音频道（多频道模式，可以同时在多个语音频道里面）
-- param pUserID:用户ID，要保证全局唯一
-- param pChannelID: 频道ID，要保证全局唯一
-- return 错误码，详见YouMeConstDefine.h定义
-- @function [parent=#YouMeTalk] joinChannelMultiMode 
-- @param self
-- @param #char pUserID
-- @param #char pChannelID
-- @param #int eUserRole
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述:获取麦克风静音状态
-- return true——静音，false——没有静音
-- @function [parent=#YouMeTalk] getMicrophoneMute 
-- @param self
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 功能描述:反初始化引擎
-- return 错误码，详见YouMeConstDefine.h定义
-- @function [parent=#YouMeTalk] unInit 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述:加入语音频道（单频道模式，每个时刻只能在一个语音频道里面）
-- param pUserID:用户ID，要保证全局唯一
-- param pChannelID: 频道ID，要保证全局唯一
-- param eUserRole: 用户角色，用于决定讲话/播放背景音乐等权限
-- return 错误码，详见YouMeConstDefine.h定义
-- @function [parent=#YouMeTalk] joinChannelSingleMode 
-- @param self
-- @param #char pUserID
-- @param #char pChannelID
-- @param #int eUserRole
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述: 设置是否前置摄像头
-- param<br>
-- return YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] setCaptureFrontCameraEnable 
-- @param self
-- @param #bool enable
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述:    释放抢到的麦
-- param const char * pChannelID:抢麦活动的频道id
-- return   YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] releaseGrabMic 
-- @param self
-- @param #char pChannelID
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#YouMeTalk] bindTexture 
-- @param self
-- @param #string userId
-- @param #int width
-- @param #int height
-- @param #cc.RenderTexture texture
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#YouMeTalk] updateTextures 
-- @param self
-- @return YouMeTalk#YouMeTalk self (return value: YouMeTalk)
        
--------------------------------
-- 功能描述: 开始camera capture
-- param
-- return YOUME_SUCCESS - 成功
-- 其他 - 具体错误码
-- @function [parent=#YouMeTalk] stopCapture 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 功能描述:销毁引擎实例，释放内存
-- return 无
-- @function [parent=#YouMeTalk] destroy 
-- @param self
-- @return YouMeTalk#YouMeTalk self (return value: YouMeTalk)
        
--------------------------------
-- 
-- @function [parent=#YouMeTalk] getInstance 
-- @param self
-- @return YouMeTalk#YouMeTalk ret (return value: YouMeTalk)

--------------------------------
-- @function [parent=#YouMeTalk] releaseMicSync
-- @param self
-- @return bool#bool ret (return value: bool)

--------------------------------
-- @function [parent=#YouMeTalk] resumeMicSync
-- @param self
-- @return bool#bool ret (return value: bool)
     
-- @function [parent=#YouMeTalk] openBeautify 
-- @param self
-- @param #bool open
-- @return YouMeTalk#YouMeTalk self (return value: int)

-- @function [parent=#YouMeTalk] beautifyChanged 
-- @param self
-- @param #double level
-- @return YouMeTalk#YouMeTalk self (return value: int)

return nil
