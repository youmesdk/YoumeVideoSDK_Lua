#include "lb_youmetalk.hpp"
#include "YouMeTalk.h"
#include "scripting/lua-bindings/manual/tolua_fix.h"
#include "scripting/lua-bindings/manual/LuaBasicConversions.h"
#include "scripting/lua-bindings/manual/CCLuaEngine.h"
#include "cocos2d.h"

USING_NS_CC;

int  EVENT_YTALK_EVENT = 0;
int  EVENT_YTALK_RestAPI = 0;
int  EVENT_YTALK_MemberChange = 0;
int  EVENT_YTALK_Broadcast = 0;

bool g_FirstVisit = true;

class TalkClass;
TalkClass* g_SingleInstance = NULL;

class TalkClass : public IYouMeEventCallback
	,public IYouMePcmCallback
	,public IRestApiCallback
	,public IYouMeMemberChangeCallback
	,public IYouMeChannelMsgCallback
{
public:
	~TalkClass();
	static TalkClass* getInstance();
private:
	TalkClass();
public:

	bool m_bUninit = false;

	void onEvent(const YouMeEvent event, const YouMeErrorCode error, const char * channel, const char * param) override;
	void onPcmData(int channelNum, int samplingRateHz, int bytesPerSample, void* data, int dataSizeInByte) ;//override;

	void onRequestRestAPI(int requestID, const YouMeErrorCode &iErrorCode, const char* strQuery, const char*  strResult);//override;

	void onMemberChange(const char* channel, std::list<MemberChange>& listMemberChange, bool bUpdate)override;
	
	void onBroadcast(const YouMeBroadcast bc, const char* channel, const char* param1, const char* param2, const char* strContent) override;
};


void TalkClass::onEvent(const YouMeEvent event, const YouMeErrorCode error, const char * channel, const char * param)
{
	if (EVENT_YTALK_EVENT == 0){
		return;
	}

	int nEvent = event;
	int nError = error;
	std::string strChannel = channel;
	std::string strParam = param;

	Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
		int handler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)TalkClass::getInstance(), (ScriptHandlerMgr::HandlerType)EVENT_YTALK_EVENT);
		if (0 != handler) {
			LuaStack *pStack = LuaEngine::getInstance()->getLuaStack();
			if (NULL == pStack) {
				return;
			}

			lua_State *tolua_s = pStack->getLuaState();
			if (NULL == tolua_s) {
				return;
			}

			pStack->pushInt(nEvent);
			pStack->pushInt(nError);
			pStack->pushString(strChannel.c_str());
			pStack->pushString(strParam.c_str());
			
			pStack->executeFunctionByHandler(handler, 4);
			pStack->clean();
		}
	});
}

void TalkClass::onPcmData(int channelNum, int samplingRateHz, int bytesPerSample, void* data, int dataSizeInByte)
{
	//todo

}

void TalkClass::onRequestRestAPI(int requestID, const YouMeErrorCode &iErrorCode, const char* strQuery, const char*  strResult)
{
	if (EVENT_YTALK_RestAPI == 0){
		return;
	}
    
    std::string query = strQuery;
    std::string result = strResult;

	Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
		int handler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)TalkClass::getInstance(), (ScriptHandlerMgr::HandlerType)EVENT_YTALK_RestAPI);
		if (0 != handler) {
			LuaStack *pStack = LuaEngine::getInstance()->getLuaStack();
			if (NULL == pStack) {
				return;
			}

			lua_State *tolua_s = pStack->getLuaState();
			if (NULL == tolua_s) {
				return;
			}

			pStack->pushInt(requestID);
			pStack->pushInt(iErrorCode);
			pStack->pushString(query.c_str());
			pStack->pushString(result.c_str());

			pStack->executeFunctionByHandler(handler, 4);
			pStack->clean();
		}
	});
}

void TalkClass::onMemberChange( const  char*  channel, std::list<MemberChange>& listMemberChange, bool bUpdate )
{
    std::string strChannel = channel;
    LuaStack *pStack = LuaEngine::getInstance()->getLuaStack();
    if (NULL == pStack) {
        return;
    }
    
    lua_State *tolua_s = pStack->getLuaState();
    if (NULL == tolua_s) {
        return;
    }
    
    cocos2d::LuaValueArray luaMsgLists;
    for (auto it = listMemberChange.begin() ; it != listMemberChange.end(); ++it)
    {
        const MemberChange& rChange = *it;
        cocos2d::LuaValueDict luaMsg;
        luaMsg["UserID"] = cocos2d::LuaValue::stringValue( std::string(rChange.userID) );
        luaMsg["IsJoin"] = cocos2d::LuaValue::intValue( rChange.isJoin );
        
        luaMsgLists.push_back(cocos2d::LuaValue::dictValue(luaMsg));
    }
    
    
    Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
        int handler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)TalkClass::getInstance(), (ScriptHandlerMgr::HandlerType)EVENT_YTALK_MemberChange);
        if (0 != handler) {
            LuaStack *pStack = LuaEngine::getInstance()->getLuaStack();
            if (NULL == pStack) {
                return;
            }
            
            lua_State *tolua_s = pStack->getLuaState();
            if (NULL == tolua_s) {
                return;
            }
            
            pStack->pushString( strChannel.c_str() );
            
            pStack->pushLuaValueArray(luaMsgLists);
            pStack->pushBoolean(bUpdate);
            pStack->executeFunctionByHandler(handler, 3);
            pStack->clean();
        }
    });
}

void TalkClass::onBroadcast(const YouMeBroadcast bc, const char* channel, const char* param1, const char* param2, const char* strContent)
{
	if (EVENT_YTALK_Broadcast == 0){
		return;
	}
	
	int nbc = bc;
    
    std::string strChannel = channel;
    std::string strParam1 = param1;
    std::string strParam2 = param2;
    std::string content = strContent;
	
	Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
		int handler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)TalkClass::getInstance(), (ScriptHandlerMgr::HandlerType)EVENT_YTALK_Broadcast);
		if (0 != handler) {
			LuaStack *pStack = LuaEngine::getInstance()->getLuaStack();
			if (NULL == pStack) {
				return;
			}

			lua_State *tolua_s = pStack->getLuaState();
			if (NULL == tolua_s) {
				return;
			}

			pStack->pushInt(nbc);
			pStack->pushString(strChannel.c_str());
			pStack->pushString(strParam1.c_str());
			pStack->pushString(strParam2.c_str());
			pStack->pushString(content.c_str());
			
			pStack->executeFunctionByHandler(handler, 5);
			pStack->clean();
		}
	});	
}

TalkClass::TalkClass(){

}

TalkClass::~TalkClass(){

}

TalkClass* TalkClass::getInstance()
{
	static TalkClass* s_instance = NULL;
	if (NULL == s_instance)
	{
		s_instance = new TalkClass;
	}
	return s_instance;
}

static ScriptHandlerMgr::HandlerType _bindLuaFunction(lua_State *tolua_S, int index, void *cobj)
{
    int handler = toluafix_ref_function(tolua_S, index, 0);
    return ScriptHandlerMgr::getInstance()->addCustomHandler(cobj, handler);
}

int lua_cocos2dx_extension_YouMeTalk_registerScriptHandler(lua_State* tolua_S)
{
	//TalkClass *self = (TalkClass*)tolua_tousertype(tolua_S, 1, 0);
	void *obj = tolua_tousertype(tolua_S, 1, 0);
	TalkClass *self = TalkClass::getInstance();
	int iCallbackCount = lua_gettop(tolua_S) - 1;
	
	assert(iCallbackCount >= 1);
	if (iCallbackCount < 1)
	{
		return 0;
	}

	EVENT_YTALK_EVENT = (int)_bindLuaFunction(tolua_S, 2, self);
	if (iCallbackCount >= 2){
		EVENT_YTALK_RestAPI = (int)_bindLuaFunction(tolua_S, 3, self);
	}
	
	if (iCallbackCount >= 3){
		EVENT_YTALK_MemberChange = (int)_bindLuaFunction(tolua_S, 4, self);
	}

	if(iCallbackCount >= 4){
		EVENT_YTALK_Broadcast = (int)_bindLuaFunction(tolua_S, 5, self);
	}
	return 0;
}

int lua_cocos2dx_extension_YouMeTalk_leaveChannelAll(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_leaveChannelAll'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_leaveChannelAll'", nullptr);
            return 0;
        }
        int ret = cobj->leaveChannelAll();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:leaveChannelAll",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_leaveChannelAll'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_getVolume(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_getVolume'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_getVolume'", nullptr);
            return 0;
        }
        unsigned int ret = cobj->getVolume();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:getVolume",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_getVolume'.",&tolua_err);
#endif

    return 0;
}
//int lua_cocos2dx_extension_YouMeTalk_avoidOtherVoiceStatus(lua_State* tolua_S)
//{
//    int argc = 0;
//    YouMeTalk* cobj = nullptr;
//    bool ok  = true;
//
//#if COCOS2D_DEBUG >= 1
//    tolua_Error tolua_err;
//#endif
//
//
//#if COCOS2D_DEBUG >= 1
//    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
//#endif
//
//    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);
//
//#if COCOS2D_DEBUG >= 1
//    if (!cobj) 
//    {
//        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_avoidOtherVoiceStatus'", nullptr);
//        return 0;
//    }
//#endif
//
//    argc = lua_gettop(tolua_S)-1;
//    if (argc == 2) 
//    {
//        const char* arg0;
//        bool arg1;
//
//        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "YouMeTalk:avoidOtherVoiceStatus"); arg0 = arg0_tmp.c_str();
//
//        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "YouMeTalk:avoidOtherVoiceStatus");
//        if(!ok)
//        {
//            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_avoidOtherVoiceStatus'", nullptr);
//            return 0;
//        }
//        int ret = cobj->avoidOtherVoiceStatus(arg0, arg1);
//        tolua_pushnumber(tolua_S,(lua_Number)ret);
//        return 1;
//    }
//    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:avoidOtherVoiceStatus",argc, 2);
//    return 0;
//
//#if COCOS2D_DEBUG >= 1
//    tolua_lerror:
//    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_avoidOtherVoiceStatus'.",&tolua_err);
//#endif
//
//    return 0;
//}
int lua_cocos2dx_extension_YouMeTalk_requestRestApi(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_requestRestApi'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        std::string arg0;
        std::string arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "YouMeTalk:requestRestApi");

        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "YouMeTalk:requestRestApi");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_requestRestApi'", nullptr);
            return 0;
        }
        int ret = cobj->requestRestApi(arg0.c_str(), arg1.c_str());
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    if (argc == 3) 
    {
        std::string arg0;
        std::string arg1;
        int* arg2;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "YouMeTalk:requestRestApi");

        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "YouMeTalk:requestRestApi");

        #pragma warning NO CONVERSION TO NATIVE FOR int*
		ok = false;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_requestRestApi'", nullptr);
            return 0;
        }
        int ret = cobj->requestRestApi(arg0.c_str(), arg1.c_str(), arg2);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:requestRestApi",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_requestRestApi'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_setSpeakerMute(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_setSpeakerMute'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "YouMeTalk:setSpeakerMute");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_setSpeakerMute'", nullptr);
            return 0;
        }
        cobj->setSpeakerMute(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:setSpeakerMute",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_setSpeakerMute'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_setMicrophoneMute(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_setMicrophoneMute'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "YouMeTalk:setMicrophoneMute");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_setMicrophoneMute'", nullptr);
            return 0;
        }
        cobj->setMicrophoneMute(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:setMicrophoneMute",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_setMicrophoneMute'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_pauseChannel(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_pauseChannel'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_pauseChannel'", nullptr);
            return 0;
        }
        int ret = cobj->pauseChannel();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:pauseChannel",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_pauseChannel'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_createRender(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_createRender'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "YouMeTalk:createRender"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_createRender'", nullptr);
            return 0;
        }
        int ret = cobj->createRender(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:createRender",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_createRender'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_resumeChannel(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_resumeChannel'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_resumeChannel'", nullptr);
            return 0;
        }
        int ret = cobj->resumeChannel();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:resumeChannel",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_resumeChannel'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_maskVideoByUserId(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_maskVideoByUserId'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        const char* arg0;
        int arg1;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "YouMeTalk:maskVideoByUserId"); arg0 = arg0_tmp.c_str();

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "YouMeTalk:maskVideoByUserId");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_maskVideoByUserId'", nullptr);
            return 0;
        }
        int ret = cobj->maskVideoByUserId(arg0, arg1);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:maskVideoByUserId",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_maskVideoByUserId'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_setVadCallbackEnabled(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_setVadCallbackEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "YouMeTalk:setVadCallbackEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_setVadCallbackEnabled'", nullptr);
            return 0;
        }
        int ret = cobj->setVadCallbackEnabled(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:setVadCallbackEnabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_setVadCallbackEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_setReverbEnabled(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_setReverbEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "YouMeTalk:setReverbEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_setReverbEnabled'", nullptr);
            return 0;
        }
        int ret = cobj->setReverbEnabled(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:setReverbEnabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_setReverbEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_getSDKVersion(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_getSDKVersion'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_getSDKVersion'", nullptr);
            return 0;
        }
        int ret = cobj->getSDKVersion();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:getSDKVersion",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_getSDKVersion'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_setBackgroundMusicVolume(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_setBackgroundMusicVolume'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "YouMeTalk:setBackgroundMusicVolume");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_setBackgroundMusicVolume'", nullptr);
            return 0;
        }
        int ret = cobj->setBackgroundMusicVolume(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:setBackgroundMusicVolume",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_setBackgroundMusicVolume'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_setHeadsetMonitorOn(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_setHeadsetMonitorOn'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        bool arg0;
        bool arg1;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "YouMeTalk:setHeadsetMonitorOn");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_setHeadsetMonitorOn'", nullptr);
            return 0;
        }
        
        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "YouMeTalk:setHeadsetMonitorOn");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_setHeadsetMonitorOn'", nullptr);
            return 0;
        }
        
        int ret = cobj->setHeadsetMonitorOn(arg0, arg1 );
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:setHeadsetMonitorOn",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_setHeadsetMonitorOn'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_setServerRegion(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_setServerRegion'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 3) 
    {
        int arg0;
        const char* arg1;
        bool arg2;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "YouMeTalk:setServerRegion");

        std::string arg1_tmp; ok &= luaval_to_std_string(tolua_S, 3, &arg1_tmp, "YouMeTalk:setServerRegion"); arg1 = arg1_tmp.c_str();

        ok &= luaval_to_boolean(tolua_S, 4,&arg2, "YouMeTalk:setServerRegion");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_setServerRegion'", nullptr);
            return 0;
        }
        cobj->setServerRegion(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:setServerRegion",argc, 3);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_setServerRegion'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_switchCamera(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_switchCamera'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_switchCamera'", nullptr);
            return 0;
        }
        int ret = cobj->switchCamera();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:switchCamera",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_switchCamera'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_speakToChannel(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_speakToChannel'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "YouMeTalk:speakToChannel"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_speakToChannel'", nullptr);
            return 0;
        }
        int ret = cobj->speakToChannel(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:speakToChannel",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_speakToChannel'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_startCapture(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_startCapture'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_startCapture'", nullptr);
            return 0;
        }
        int ret = cobj->startCapture();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:startCapture",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_startCapture'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_getSpeakerMute(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_getSpeakerMute'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_getSpeakerMute'", nullptr);
            return 0;
        }
        bool ret = cobj->getSpeakerMute();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:getSpeakerMute",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_getSpeakerMute'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_cleanTextures(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_cleanTextures'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_cleanTextures'", nullptr);
            return 0;
        }
        cobj->cleanTextures();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:cleanTextures",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_cleanTextures'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_setGrabMicOption(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_setGrabMicOption'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 5) 
    {
        const char* arg0;
        int arg1;
        int arg2;
        int arg3;
        unsigned int arg4;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "YouMeTalk:setGrabMicOption"); arg0 = arg0_tmp.c_str();

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "YouMeTalk:setGrabMicOption");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "YouMeTalk:setGrabMicOption");

        ok &= luaval_to_int32(tolua_S, 5,(int *)&arg3, "YouMeTalk:setGrabMicOption");

        ok &= luaval_to_uint32(tolua_S, 6,&arg4, "YouMeTalk:setGrabMicOption");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_setGrabMicOption'", nullptr);
            return 0;
        }
        int ret = cobj->setGrabMicOption(arg0, arg1, arg2, arg3, arg4);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:setGrabMicOption",argc, 5);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_setGrabMicOption'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_setInviteMicOption(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_setInviteMicOption'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 3) 
    {
        const char* arg0;
        int arg1;
        int arg2;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "YouMeTalk:setInviteMicOption"); arg0 = arg0_tmp.c_str();

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "YouMeTalk:setInviteMicOption");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "YouMeTalk:setInviteMicOption");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_setInviteMicOption'", nullptr);
            return 0;
        }
        int ret = cobj->setInviteMicOption(arg0, arg1, arg2);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:setInviteMicOption",argc, 3);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_setInviteMicOption'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_stopBackgroundMusic(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_stopBackgroundMusic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_stopBackgroundMusic'", nullptr);
            return 0;
        }
        int ret = cobj->stopBackgroundMusic();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:stopBackgroundMusic",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_stopBackgroundMusic'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_setUseMobileNetworkEnabled(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_setUseMobileNetworkEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "YouMeTalk:setUseMobileNetworkEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_setUseMobileNetworkEnabled'", nullptr);
            return 0;
        }
        cobj->setUseMobileNetworkEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:setUseMobileNetworkEnabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_setUseMobileNetworkEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_init(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_init'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 4) 
    {
        const char* arg0;
        const char* arg1;
        int arg2;
        const char* arg3;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "YouMeTalk:init"); arg0 = arg0_tmp.c_str();

        std::string arg1_tmp; ok &= luaval_to_std_string(tolua_S, 3, &arg1_tmp, "YouMeTalk:init"); arg1 = arg1_tmp.c_str();

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "YouMeTalk:init");

        std::string arg3_tmp; ok &= luaval_to_std_string(tolua_S, 5, &arg3_tmp, "YouMeTalk:init"); arg3 = arg3_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_init'", nullptr);
            return 0;
        }
        int ret = cobj->init(arg0, arg1, arg2, arg3);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:init",argc, 4);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_init'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_stopInviteMic(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_stopInviteMic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_stopInviteMic'", nullptr);
            return 0;
        }
        int ret = cobj->stopInviteMic();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:stopInviteMic",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_stopInviteMic'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_responseInviteMic(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_responseInviteMic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 3) 
    {
        const char* arg0;
        bool arg1;
        const char* arg2;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "YouMeTalk:responseInviteMic"); arg0 = arg0_tmp.c_str();

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "YouMeTalk:responseInviteMic");

        std::string arg2_tmp; ok &= luaval_to_std_string(tolua_S, 4, &arg2_tmp, "YouMeTalk:responseInviteMic"); arg2 = arg2_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_responseInviteMic'", nullptr);
            return 0;
        }
        int ret = cobj->responseInviteMic(arg0, arg1, arg2);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:responseInviteMic",argc, 3);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_responseInviteMic'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_setMicLevelCallback(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_setMicLevelCallback'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "YouMeTalk:setMicLevelCallback");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_setMicLevelCallback'", nullptr);
            return 0;
        }
        int ret = cobj->setMicLevelCallback(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:setMicLevelCallback",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_setMicLevelCallback'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_getChannelUserList(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_getChannelUserList'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 3) 
    {
        const char* arg0;
        int arg1;
        bool arg2;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "YouMeTalk:getChannelUserList"); arg0 = arg0_tmp.c_str();

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "YouMeTalk:getChannelUserList");

        ok &= luaval_to_boolean(tolua_S, 4,&arg2, "YouMeTalk:getChannelUserList");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_getChannelUserList'", nullptr);
            return 0;
        }
        int ret = cobj->getChannelUserList(arg0, arg1, arg2);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:getChannelUserList",argc, 3);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_getChannelUserList'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_setVolume(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_setVolume'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        unsigned int arg0;

        ok &= luaval_to_uint32(tolua_S, 2,&arg0, "YouMeTalk:setVolume");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_setVolume'", nullptr);
            return 0;
        }
        cobj->setVolume(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:setVolume",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_setVolume'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_unbindTexture(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_unbindTexture'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "YouMeTalk:unbindTexture");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_unbindTexture'", nullptr);
            return 0;
        }
        cobj->unbindTexture(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:unbindTexture",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_unbindTexture'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_openVideoEncoder(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_openVideoEncoder'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "YouMeTalk:openVideoEncoder"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_openVideoEncoder'", nullptr);
            return 0;
        }
        int ret = cobj->openVideoEncoder(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:openVideoEncoder",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_openVideoEncoder'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_requestGrabMic(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_requestGrabMic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 4) 
    {
        const char* arg0;
        int arg1;
        bool arg2;
        const char* arg3;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "YouMeTalk:requestGrabMic"); arg0 = arg0_tmp.c_str();

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "YouMeTalk:requestGrabMic");

        ok &= luaval_to_boolean(tolua_S, 4,&arg2, "YouMeTalk:requestGrabMic");

        std::string arg3_tmp; ok &= luaval_to_std_string(tolua_S, 5, &arg3_tmp, "YouMeTalk:requestGrabMic"); arg3 = arg3_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_requestGrabMic'", nullptr);
            return 0;
        }
        int ret = cobj->requestGrabMic(arg0, arg1, arg2, arg3);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:requestGrabMic",argc, 4);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_requestGrabMic'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_leaveChannelMultiMode(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_leaveChannelMultiMode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "YouMeTalk:leaveChannelMultiMode"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_leaveChannelMultiMode'", nullptr);
            return 0;
        }
        int ret = cobj->leaveChannelMultiMode(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:leaveChannelMultiMode",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_leaveChannelMultiMode'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_setReleaseMicWhenMute(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_setReleaseMicWhenMute'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "YouMeTalk:setReleaseMicWhenMute");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_setReleaseMicWhenMute'", nullptr);
            return 0;
        }
        int ret = cobj->setReleaseMicWhenMute(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:setReleaseMicWhenMute",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_setReleaseMicWhenMute'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_playBackgroundMusic(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_playBackgroundMusic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        const char* arg0;
        bool arg1;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "YouMeTalk:playBackgroundMusic"); arg0 = arg0_tmp.c_str();

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "YouMeTalk:playBackgroundMusic");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_playBackgroundMusic'", nullptr);
            return 0;
        }
        int ret = cobj->playBackgroundMusic(arg0, arg1);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:playBackgroundMusic",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_playBackgroundMusic'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_startGrabMicAction(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_startGrabMicAction'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        const char* arg0;
        const char* arg1;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "YouMeTalk:startGrabMicAction"); arg0 = arg0_tmp.c_str();

        std::string arg1_tmp; ok &= luaval_to_std_string(tolua_S, 3, &arg1_tmp, "YouMeTalk:startGrabMicAction"); arg1 = arg1_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_startGrabMicAction'", nullptr);
            return 0;
        }
        int ret = cobj->startGrabMicAction(arg0, arg1);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:startGrabMicAction",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_startGrabMicAction'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_getUseMobileNetworkEnabled(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_getUseMobileNetworkEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_getUseMobileNetworkEnabled'", nullptr);
            return 0;
        }
        bool ret = cobj->getUseMobileNetworkEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:getUseMobileNetworkEnabled",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_getUseMobileNetworkEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_requestInviteMic(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_requestInviteMic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 3) 
    {
        const char* arg0;
        const char* arg1;
        const char* arg2;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "YouMeTalk:requestInviteMic"); arg0 = arg0_tmp.c_str();

        std::string arg1_tmp; ok &= luaval_to_std_string(tolua_S, 3, &arg1_tmp, "YouMeTalk:requestInviteMic"); arg1 = arg1_tmp.c_str();

        std::string arg2_tmp; ok &= luaval_to_std_string(tolua_S, 4, &arg2_tmp, "YouMeTalk:requestInviteMic"); arg2 = arg2_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_requestInviteMic'", nullptr);
            return 0;
        }
        int ret = cobj->requestInviteMic(arg0, arg1, arg2);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:requestInviteMic",argc, 3);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_requestInviteMic'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_setCaptureProperty(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_setCaptureProperty'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 3) 
    {
        int arg0;
        int arg1;
        int arg2;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "YouMeTalk:setCaptureProperty");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "YouMeTalk:setCaptureProperty");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "YouMeTalk:setCaptureProperty");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_setCaptureProperty'", nullptr);
            return 0;
        }
        int ret = cobj->setCaptureProperty(arg0, arg1, arg2);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:setCaptureProperty",argc, 3);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_setCaptureProperty'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_setNetCaptureProperty(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;
    
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    
    
#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif
    
    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);
    
#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_setNetCaptureProperty'", nullptr);
        return 0;
    }
#endif
    
    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        int arg0;
        int arg1;
        
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "YouMeTalk:setNetCaptureProperty");
        
        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "YouMeTalk:setNetCaptureProperty");
        
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_setNetCaptureProperty'", nullptr);
            return 0;
        }
        int ret = cobj->setNetCaptureProperty(arg0, arg1);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:setNetCaptureProperty",argc, 2);
    return 0;
    
#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_setNetCaptureProperty'.",&tolua_err);
#endif
    
    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_setListenOtherVoice(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_setListenOtherVoice'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        const char* arg0;
        bool arg1;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "YouMeTalk:setListenOtherVoice"); arg0 = arg0_tmp.c_str();

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "YouMeTalk:setListenOtherVoice");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_setListenOtherVoice'", nullptr);
            return 0;
        }
        int ret = cobj->setListenOtherVoice(arg0, arg1);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:setListenOtherVoice",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_setListenOtherVoice'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_stopGrabMicAction(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_stopGrabMicAction'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        const char* arg0;
        const char* arg1;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "YouMeTalk:stopGrabMicAction"); arg0 = arg0_tmp.c_str();

        std::string arg1_tmp; ok &= luaval_to_std_string(tolua_S, 3, &arg1_tmp, "YouMeTalk:stopGrabMicAction"); arg1 = arg1_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_stopGrabMicAction'", nullptr);
            return 0;
        }
        int ret = cobj->stopGrabMicAction(arg0, arg1);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:stopGrabMicAction",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_stopGrabMicAction'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_setToken(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_setToken'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "YouMeTalk:setToken"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_setToken'", nullptr);
            return 0;
        }
        cobj->setToken(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:setToken",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_setToken'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_setAutoSendStatus(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_setAutoSendStatus'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "YouMeTalk:setAutoSendStatus");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_setAutoSendStatus'", nullptr);
            return 0;
        }
        cobj->setAutoSendStatus(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:setAutoSendStatus",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_setAutoSendStatus'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_joinChannelMultiMode(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_joinChannelMultiMode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 3)
    {
        const char* arg0;
        const char* arg1;
        int arg2;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "YouMeTalk:joinChannelMultiMode"); arg0 = arg0_tmp.c_str();

        std::string arg1_tmp; ok &= luaval_to_std_string(tolua_S, 3, &arg1_tmp, "YouMeTalk:joinChannelMultiMode"); arg1 = arg1_tmp.c_str();
        
        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "YouMeTalk:joinChannelMultiMode");
        
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_joinChannelMultiMode'", nullptr);
            return 0;
        }
        int ret = cobj->joinChannelMultiMode(arg0, arg1, arg2);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:joinChannelMultiMode",argc, 3);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_joinChannelMultiMode'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_getMicrophoneMute(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_getMicrophoneMute'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_getMicrophoneMute'", nullptr);
            return 0;
        }
        bool ret = cobj->getMicrophoneMute();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:getMicrophoneMute",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_getMicrophoneMute'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_unInit(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_unInit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_unInit'", nullptr);
            return 0;
        }
        int ret = cobj->unInit();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:unInit",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_unInit'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_joinChannelSingleMode(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_joinChannelSingleMode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 3) 
    {
        const char* arg0;
        const char* arg1;
        int arg2;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "YouMeTalk:joinChannelSingleMode"); arg0 = arg0_tmp.c_str();

        std::string arg1_tmp; ok &= luaval_to_std_string(tolua_S, 3, &arg1_tmp, "YouMeTalk:joinChannelSingleMode"); arg1 = arg1_tmp.c_str();

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "YouMeTalk:joinChannelSingleMode");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_joinChannelSingleMode'", nullptr);
            return 0;
        }
        int ret = cobj->joinChannelSingleMode(arg0, arg1, arg2);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:joinChannelSingleMode",argc, 3);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_joinChannelSingleMode'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_setCaptureFrontCameraEnable(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_setCaptureFrontCameraEnable'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "YouMeTalk:setCaptureFrontCameraEnable");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_setCaptureFrontCameraEnable'", nullptr);
            return 0;
        }
        int ret = cobj->setCaptureFrontCameraEnable(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:setCaptureFrontCameraEnable",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_setCaptureFrontCameraEnable'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_releaseGrabMic(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_releaseGrabMic'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "YouMeTalk:releaseGrabMic"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_releaseGrabMic'", nullptr);
            return 0;
        }
        int ret = cobj->releaseGrabMic(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:releaseGrabMic",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_releaseGrabMic'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_bindTexture(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_bindTexture'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 4) 
    {
        std::string arg0;
        int arg1;
        int arg2;
        cocos2d::RenderTexture* arg3;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "YouMeTalk:bindTexture");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "YouMeTalk:bindTexture");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "YouMeTalk:bindTexture");

        ok &= luaval_to_object<cocos2d::RenderTexture>(tolua_S, 5, "cc.RenderTexture",&arg3, "YouMeTalk:bindTexture");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_bindTexture'", nullptr);
            return 0;
        }
        int ret = cobj->bindTexture(arg0, arg1, arg2, arg3);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:bindTexture",argc, 4);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_bindTexture'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_updateTextures(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_updateTextures'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_updateTextures'", nullptr);
            return 0;
        }
        cobj->updateTextures();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:updateTextures",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_updateTextures'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_stopCapture(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_stopCapture'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_stopCapture'", nullptr);
            return 0;
        }
        int ret = cobj->stopCapture();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:stopCapture",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_stopCapture'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_destroy(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_destroy'", nullptr);
            return 0;
        }
        YouMeTalk::destroy();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "YouMeTalk:destroy",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_destroy'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_releaseMicSync(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;
    
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    
    
#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif
    
    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);
    
#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_releaseMicSync'", nullptr);
        return 0;
    }
#endif
    
    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_releaseMicSync'", nullptr);
            return 0;
        }
        bool ret = cobj->releaseMicSync();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:releaseMicSync",argc, 0);
    return 0;
    
#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_releaseMicSync'.",&tolua_err);
#endif
    
    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_resumeMicSync(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;
    
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    
    
#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif
    
    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);
    
#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_resumeMicSync'", nullptr);
        return 0;
    }
#endif
    
    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_resumeMicSync'", nullptr);
            return 0;
        }
        bool ret = cobj->resumeMicSync();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:resumeMicSync",argc, 0);
    return 0;
    
#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_resumeMicSync'.",&tolua_err);
#endif
    
    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_openBeautify(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_openBeautify'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "YouMeTalk:openBeautify");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_openBeautify'", nullptr);
            return 0;
        }
        int ret = cobj->openBeautify(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:openBeautify",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_openBeautify'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_YouMeTalk_beautifyChanged(lua_State* tolua_S)
{
    int argc = 0;
    YouMeTalk* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_beautifyChanged'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "YouMeTalk:beautifyChanged");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_beautifyChanged'", nullptr);
            return 0;
        }
        int ret = cobj->beautifyChanged(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:beautifyChanged",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_beautifyChanged'.",&tolua_err);
#endif

    return 0;
}
// int lua_cocos2dx_extension_YouMeTalk_stretchFace(lua_State* tolua_S)
// {
//     int argc = 0;
//     YouMeTalk* cobj = nullptr;
//     bool ok  = true;

// #if COCOS2D_DEBUG >= 1
//     tolua_Error tolua_err;
// #endif


// #if COCOS2D_DEBUG >= 1
//     if (!tolua_isusertype(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
// #endif

//     cobj = (YouMeTalk*)tolua_tousertype(tolua_S,1,0);

// #if COCOS2D_DEBUG >= 1
//     if (!cobj) 
//     {
//         tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_YouMeTalk_stretchFace'", nullptr);
//         return 0;
//     }
// #endif

//     argc = lua_gettop(tolua_S)-1;
//     if (argc == 1) 
//     {
//         bool arg0;

//         ok &= luaval_to_boolean(tolua_S, 2,&arg0, "YouMeTalk:stretchFace");
//         if(!ok)
//         {
//             tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_stretchFace'", nullptr);
//             return 0;
//         }
//         int ret = cobj->stretchFace(arg0);
//         tolua_pushnumber(tolua_S,(lua_Number)ret);
//         return 1;
//     }
//     luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "YouMeTalk:stretchFace",argc, 1);
//     return 0;

// #if COCOS2D_DEBUG >= 1
//     tolua_lerror:
//     tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_stretchFace'.",&tolua_err);
// #endif

//     return 0;
// }
int lua_cocos2dx_extension_YouMeTalk_getInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"YouMeTalk",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_YouMeTalk_getInstance'", nullptr);
            return 0;
        }
        YouMeTalk* ret = YouMeTalk::getInstance();
		if (g_FirstVisit){
			g_FirstVisit = false;
			ret->SetEventCallback(TalkClass::getInstance());
			ret->setRestApiCallback(TalkClass::getInstance());
			ret->setMemberChangeCallback(TalkClass::getInstance());
			ret->setNotifyCallback(TalkClass::getInstance());
		}
        object_to_luaval<YouMeTalk>(tolua_S, "YouMeTalk",(YouMeTalk*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "YouMeTalk:getInstance",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_YouMeTalk_getInstance'.",&tolua_err);
#endif
    return 0;
}
static int lua_cocos2dx_extension_YouMeTalk_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (YouMeTalk)");
    return 0;
}

int lua_register_cocos2dx_extension_YouMeTalk(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"YouMeTalk");
    tolua_cclass(tolua_S,"YouMeTalk","YouMeTalk","",nullptr);

    tolua_beginmodule(tolua_S,"YouMeTalk");
        tolua_function(tolua_S,"leaveChannelAll",lua_cocos2dx_extension_YouMeTalk_leaveChannelAll);
        tolua_function(tolua_S,"getVolume",lua_cocos2dx_extension_YouMeTalk_getVolume);
//        tolua_function(tolua_S,"avoidOtherVoiceStatus",lua_cocos2dx_extension_YouMeTalk_avoidOtherVoiceStatus);
        tolua_function(tolua_S,"requestRestApi",lua_cocos2dx_extension_YouMeTalk_requestRestApi);
        tolua_function(tolua_S,"setSpeakerMute",lua_cocos2dx_extension_YouMeTalk_setSpeakerMute);
        tolua_function(tolua_S,"setMicrophoneMute",lua_cocos2dx_extension_YouMeTalk_setMicrophoneMute);
        tolua_function(tolua_S,"pauseChannel",lua_cocos2dx_extension_YouMeTalk_pauseChannel);
        tolua_function(tolua_S,"createRender",lua_cocos2dx_extension_YouMeTalk_createRender);
        tolua_function(tolua_S,"resumeChannel",lua_cocos2dx_extension_YouMeTalk_resumeChannel);
        tolua_function(tolua_S,"maskVideoByUserId",lua_cocos2dx_extension_YouMeTalk_maskVideoByUserId);
        tolua_function(tolua_S,"setVadCallbackEnabled",lua_cocos2dx_extension_YouMeTalk_setVadCallbackEnabled);
        tolua_function(tolua_S,"setReverbEnabled",lua_cocos2dx_extension_YouMeTalk_setReverbEnabled);
        tolua_function(tolua_S,"getSDKVersion",lua_cocos2dx_extension_YouMeTalk_getSDKVersion);
        tolua_function(tolua_S,"setBackgroundMusicVolume",lua_cocos2dx_extension_YouMeTalk_setBackgroundMusicVolume);
        tolua_function(tolua_S,"setHeadsetMonitorOn",lua_cocos2dx_extension_YouMeTalk_setHeadsetMonitorOn);
        tolua_function(tolua_S,"setServerRegion",lua_cocos2dx_extension_YouMeTalk_setServerRegion);
        tolua_function(tolua_S,"switchCamera",lua_cocos2dx_extension_YouMeTalk_switchCamera);
        tolua_function(tolua_S,"speakToChannel",lua_cocos2dx_extension_YouMeTalk_speakToChannel);
        tolua_function(tolua_S,"startCapture",lua_cocos2dx_extension_YouMeTalk_startCapture);
        tolua_function(tolua_S,"getSpeakerMute",lua_cocos2dx_extension_YouMeTalk_getSpeakerMute);
        tolua_function(tolua_S,"cleanTextures",lua_cocos2dx_extension_YouMeTalk_cleanTextures);
        tolua_function(tolua_S,"setGrabMicOption",lua_cocos2dx_extension_YouMeTalk_setGrabMicOption);
        tolua_function(tolua_S,"setInviteMicOption",lua_cocos2dx_extension_YouMeTalk_setInviteMicOption);
        tolua_function(tolua_S,"stopBackgroundMusic",lua_cocos2dx_extension_YouMeTalk_stopBackgroundMusic);
        tolua_function(tolua_S,"setUseMobileNetworkEnabled",lua_cocos2dx_extension_YouMeTalk_setUseMobileNetworkEnabled);
        tolua_function(tolua_S,"init",lua_cocos2dx_extension_YouMeTalk_init);
        tolua_function(tolua_S,"stopInviteMic",lua_cocos2dx_extension_YouMeTalk_stopInviteMic);
        tolua_function(tolua_S,"responseInviteMic",lua_cocos2dx_extension_YouMeTalk_responseInviteMic);
        tolua_function(tolua_S,"setMicLevelCallback",lua_cocos2dx_extension_YouMeTalk_setMicLevelCallback);
        tolua_function(tolua_S,"getChannelUserList",lua_cocos2dx_extension_YouMeTalk_getChannelUserList);
        tolua_function(tolua_S,"setVolume",lua_cocos2dx_extension_YouMeTalk_setVolume);
        tolua_function(tolua_S,"unbindTexture",lua_cocos2dx_extension_YouMeTalk_unbindTexture);
        tolua_function(tolua_S,"openVideoEncoder",lua_cocos2dx_extension_YouMeTalk_openVideoEncoder);
        tolua_function(tolua_S,"requestGrabMic",lua_cocos2dx_extension_YouMeTalk_requestGrabMic);
        tolua_function(tolua_S,"leaveChannelMultiMode",lua_cocos2dx_extension_YouMeTalk_leaveChannelMultiMode);
        tolua_function(tolua_S,"setReleaseMicWhenMute",lua_cocos2dx_extension_YouMeTalk_setReleaseMicWhenMute);
        tolua_function(tolua_S,"playBackgroundMusic",lua_cocos2dx_extension_YouMeTalk_playBackgroundMusic);
        tolua_function(tolua_S,"startGrabMicAction",lua_cocos2dx_extension_YouMeTalk_startGrabMicAction);
        tolua_function(tolua_S,"getUseMobileNetworkEnabled",lua_cocos2dx_extension_YouMeTalk_getUseMobileNetworkEnabled);
        tolua_function(tolua_S,"requestInviteMic",lua_cocos2dx_extension_YouMeTalk_requestInviteMic);
        tolua_function(tolua_S,"setCaptureProperty",lua_cocos2dx_extension_YouMeTalk_setCaptureProperty);
        tolua_function(tolua_S,"setNetCaptureProperty",lua_cocos2dx_extension_YouMeTalk_setNetCaptureProperty);
        tolua_function(tolua_S,"setListenOtherVoice",lua_cocos2dx_extension_YouMeTalk_setListenOtherVoice);
        tolua_function(tolua_S,"stopGrabMicAction",lua_cocos2dx_extension_YouMeTalk_stopGrabMicAction);
        tolua_function(tolua_S,"setToken",lua_cocos2dx_extension_YouMeTalk_setToken);
        tolua_function(tolua_S,"setAutoSendStatus",lua_cocos2dx_extension_YouMeTalk_setAutoSendStatus);
        tolua_function(tolua_S,"joinChannelMultiMode",lua_cocos2dx_extension_YouMeTalk_joinChannelMultiMode);
        tolua_function(tolua_S,"getMicrophoneMute",lua_cocos2dx_extension_YouMeTalk_getMicrophoneMute);
        tolua_function(tolua_S,"unInit",lua_cocos2dx_extension_YouMeTalk_unInit);
        tolua_function(tolua_S,"joinChannelSingleMode",lua_cocos2dx_extension_YouMeTalk_joinChannelSingleMode);
        tolua_function(tolua_S,"setCaptureFrontCameraEnable",lua_cocos2dx_extension_YouMeTalk_setCaptureFrontCameraEnable);
        tolua_function(tolua_S,"releaseGrabMic",lua_cocos2dx_extension_YouMeTalk_releaseGrabMic);
        tolua_function(tolua_S,"bindTexture",lua_cocos2dx_extension_YouMeTalk_bindTexture);
        tolua_function(tolua_S,"updateTextures",lua_cocos2dx_extension_YouMeTalk_updateTextures);
        tolua_function(tolua_S,"stopCapture",lua_cocos2dx_extension_YouMeTalk_stopCapture);
        tolua_function(tolua_S,"destroy", lua_cocos2dx_extension_YouMeTalk_destroy);
        tolua_function(tolua_S,"getInstance", lua_cocos2dx_extension_YouMeTalk_getInstance);
        tolua_function(tolua_S,"releaseMicSync", lua_cocos2dx_extension_YouMeTalk_releaseMicSync);
        tolua_function(tolua_S,"resumeMicSync", lua_cocos2dx_extension_YouMeTalk_resumeMicSync);
        tolua_function(tolua_S,"openBeautify", lua_cocos2dx_extension_YouMeTalk_openBeautify);
        tolua_function(tolua_S,"beautifyChanged", lua_cocos2dx_extension_YouMeTalk_beautifyChanged);
        // tolua_function(tolua_S,"stretchFace", lua_cocos2dx_extension_YouMeTalk_stretchFace);                
		//manual add, not auto binding
		tolua_function(tolua_S, "registerScriptHandler", lua_cocos2dx_extension_YouMeTalk_registerScriptHandler);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(YouMeTalk).name();
    g_luaType[typeName] = "YouMeTalk";
    g_typeCast["YouMeTalk"] = "YouMeTalk";
    return 1;
}
TOLUA_API int register_all_youmetalk(lua_State* tolua_S)
{
           lua_getglobal(tolua_S, "_G");
	tolua_open(tolua_S);
	
	tolua_module(tolua_S,"ym",0);
	tolua_beginmodule(tolua_S,"ym");

	lua_register_cocos2dx_extension_YouMeTalk(tolua_S);

	tolua_endmodule(tolua_S);
	return 1;
}

