--region YoumeSDK.lua
--Author : Y.X.
--Date   : 2017/7/5
--此文件由[BabeLua]插件自动生成

local youme = class("youme")

-- other require
--local UserData = require("src/app/controller/UserData")

local function cclog(...)
    release_print(string.format(...))
end

youme.instance = nil

function youme:getInstance()
    if not youme.instance then
        youme.instance = youme:create()
        youme.instance:initData()
    end
    return youme.instance
end

local function onYoumeEvent(event,  errorcode, channel, param)
    youme:getInstance():onEvent(event, errorcode, channel, param)
end

local function onYoumeRequestRestAPI(requestID,  iErrorCode, strQuery, strResult)
    youme:getInstance():onRequestRestAPI(requestID, iErrorCode, strQuery, strResult)
end

local function onYoumeMemberChange(channel,  listMemberChange, bUpdate)
    youme:getInstance():onMemberChange(channel, listMemberChange, bUpdate)
end

function youme:initData()
    -- do something
    self.sdk = ym.YouMeTalk:getInstance()
    self.sdk:registerScriptHandler(onYoumeEvent, onYoumeRequestRestAPI, onYoumeMemberChange)
end

function youme:initSDK()
    self:sendtip("开始初始化......")
    self.sdk:init(
            "YOUME5BE427937AF216E88E0F84C0EF148BD29B691556",
            "y1sepDnrmgatu/G8rx1nIKglCclvuA5tAvC0vXwlfZKOvPZfaUYOTkfAdUUtbziW8Z4HrsgpJtmV/RqhacllbXD3abvuXIBlrknqP+Bith9OHazsC1X96b3Inii6J7Und0/KaGf3xEzWx/t1E1SbdrbmBJ01D1mwn50O/9V0820BAAE=",
            0,
            "")
end

function youme:onEvent(event,  errorcode, channel, param)
    print("[onEvent]%d %d %s %s", event, errorcode, channel, param)
    if event == 0 then
        cclog("Talk 初始化成功");
        self:sendtip("初始化成功");
        self:sendmsg_init(errorcode)
    elseif event == 1 then
        cclog("Talk 初始化失败");
        self:sendtip("初始化失败");
        self:sendmsg_init(errorcode)
    elseif event == 2 then
        cclog("Talk 进入房间成功");
        self:sendtip("进入房间成功");
        self:sendmsg_join(errorcode, param)
    elseif event == 3 then
        cclog("Talk 进入房间失败");
        self:sendtip("进入房间失败");
        self:sendmsg_join(errorcode, param)
    elseif event == 4 then
        cclog("Talk 离开单个房间");
        self:sendtip("离开单个房间:"..errorcode)
    elseif event == 5 then
        cclog("Talk 离开所有房间");
        self:sendtip("离开所有房间:"..errorcode)
        self:sendmsg_leave(errorcode, param)
    elseif event == 10 then
        cclog("Talk 正在重连");
        self:sendtip("正在重连");
    elseif event == 11 then
        cclog("Talk 重连成功");
        self:sendtip("重连成功");
    elseif event == 15 then
        cclog("Talk 频道成员变化["..channel.."]"..param)
        self:sendtip("频道成员变化:"..param)
    elseif event == 200 then
        cclog("Talk 用户视频打开");
        self:sendtip("用户视频打开:"..param)
        self:sendmsg_video(errorcode, param, 1)
    elseif event == 201 then
        cclog("Talk 用户视频关闭");
        self:sendtip("用户视频关闭");
    elseif event == 202 then
        cclog("Talk 其他用户摄像头暂停");
        self:sendtip("其他用户摄像头暂停:"..param);
        self:sendmsg_video(errorcode, param, 3)
    elseif event == 203 then
        cclog("Talk 其他用户摄像头恢复");
        self:sendtip("其他用户摄像头恢复:"..param);
        self:sendmsg_video(errorcode, param, 4)
    elseif event == 204 then
        cclog("Talk 视频被其他用户屏蔽");
        self:sendtip("视频被其他用户屏蔽:"..param);
    elseif event == 205 then
        cclog("Talk 视频被其他用户恢复");
        self:sendtip("视频被其他用户恢复:"..param);
    elseif event == 206 then
        cclog("Talk 屏蔽了谁的视频");
        self:sendtip("屏蔽了谁的视频:"..param);
        self:sendmsg_video(errorcode, param, 5)
    elseif event == 207 then
        cclog("Talk 恢复了谁的视频");
        self:sendtip("恢复了谁的视频:"..param);
        self:sendmsg_video(errorcode, param, 6)
    elseif event == 208 then
        cclog("Talk 视频断开");
        self:sendtip("视频断开:"..param);
        self:sendmsg_video(errorcode, param, 2)
    end
end

function youme:onRequestRestAPI(requestID, iErrorCode, strQuery, strResult)
end

function youme:onMemberChange(channel,  listMemberChange, bUpdate)
    cclog("MemberChange %s %s" ,channel,bUpdate)
    for i,v in pairs(listMemberChange) do
		local isjoin = listMemberChange[i].IsJoin;
		if(isjoin == 0) then
		    local userid = listMemberChange[i].UserID;
			self:sendmsg_memchange(userid);
		end
	end
end

function youme:sendmsg_init(errcode)
    local event = cc.EventCustom:new("YMInit")
    event._usedata = {["errorcode"]=errcode}
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)
end

function youme:sendmsg_join(errcode, userid)
    local event = cc.EventCustom:new("YMEnterRoom")
    event._usedata = {["errorcode"]=errcode, ["uid"]=userid}
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)
end

function youme:sendmsg_leave(errcode, userid)
    local event = cc.EventCustom:new("YMLeaveRoom")
    event._usedata = {["errorcode"]=errcode, ["uid"]=userid}
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)
end

function youme:sendmsg_memchange(userid)
    local event = cc.EventCustom:new("YMMemChange")
    event._usedata = {["uid"]=userid}
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)
end

function youme:sendmsg_video(errcode, userid, state)
    local event = cc.EventCustom:new("YMVideo")
    event._usedata = {["errorcode"]=errcode, ["uid"]=userid, ["state"]=state}
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)
end

function youme:sendtip(str, iserr)
    local berr = iserr or false
    local event = cc.EventCustom:new("YMTip")
    event._usedata = {["iserr"]=berr, ["str"]=str}
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)
end

return youme

--endregion
