/**
 * \file
 * \version  $Id: zScene.h  $
 * \author  
 * \date 
 * \brief 场景定义
 */

#ifndef _ZSCENE_H_
#define _ZSCENE_H_

#include "zEntry.h"

enum enumSceneRunningState{
	//SCENE_RUNNINGSTATE_NORMAL,//正常运行
	//SCENE_RUNNINGSTATE_UNLOAD,//正在卸载
	//SCENE_RUNNINGSTATE_REMOVE,//正在卸载

	SCENE_RUN_STATE_NORMAL       = 1,          //正常运行,场景可以进入
	SCENE_RUN_STATE_READY_CLOSE     = 2,          //准备关闭,场景无法进入
	SCENE_RUN_STATE_CLOSE           = 3,          //场景要关闭,场景无法进入
	SCENE_RUN_STATE_UNLOAD       = 4,          //正在卸载
	SCENE_RUN_STATE_REMOVE       = 5,          //正式卸载
};
/**
 * \brief 场景基本信息定义
 */
struct zScene:public zEntry
{
	private:
		DWORD running_state;
	public:
		zScene():running_state(SCENE_RUN_STATE_NORMAL){}
		DWORD getRunState() const
		{
			return running_state;
		}
//-----------------------------------------
    DWORD setRunState(DWORD state)
    {
        runState = state;
        return runState;
    }			
    bool isRunning() const { return runState == SCENE_RUN_STATE_NORMAL; } //副本新
    bool isClosing() const { return runState == SCENE_RUN_STATE_READY_CLOSE || runState == SCENE_RUN_STATE_CLOSE; } //副本关闭
    bool isUnloading() const { return runState == SCENE_RUN_STATE_UNLOAD || runState == SCENE_RUN_STATE_REMOVE; } //副本卸载
    private:
    DWORD runState;
};

#endif
