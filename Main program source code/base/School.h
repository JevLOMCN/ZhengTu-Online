/**
 * \file
 * \version  $Id: School.h $
 * \author  
 * \date 
 * \brief 定义帮会的基本结构
 *
 */


#ifndef _SCHOOL_H_
#define _SCHOOL_H_

#include "zType.h"

#pragma pack(1)

namespace School
{

#define MAX_BULLETIN_CHAR_NUMBER 1000          // 公告牌上允许的最大字符数
#define MAX_PRENTICE 20							// 一个人最多可以招收的徒弟数目
#define TEACHER_LEVEL 30						// 招收徒弟需要等级
#define CREATE_SCHOOL_REQUEST_LEVEL 60			// 创建门派等级需要 60
#define CREATE_SCHOOL_REQUEST_ITEM_ID 684      // 创建门派需要道具目前为天羽令
#define CREATE_SCHOOL_REQUEST_PRICE_GOLD 100000 // 创建门派需要的金钱数目
#define FIRST_LAYER_PRENTICE_REQUEST_LEVEL 50   // 创建门派者的第一层徒弟需要的角色级别
#define SECOND_LAYER_PRENTICE_REQUEST_LEVEL 40  // 创建门派者的第二层徒孙需要的角色级别

struct stSchoolInfo
{
	DWORD dwSchoolID;
	char  name[MAX_NAMESIZE+1];
	DWORD dwMasterSerialID;
	DWORD  size;
	//char data[0];
}__attribute__ ((packed));
	
struct stSchoolMemberInfo
{
	DWORD dwSerialID;
	DWORD dwMasterID;
	DWORD dwPreSerialID;
	DWORD dwCharID;
	char  name[MAX_NAMESIZE+1];
	WORD  wdLevel;
	DWORD dwJoinTime;
	WORD  wdDegree;
	DWORD dwLastTime;
	DWORD dwSchoolID;
	BYTE  byTag;
	WORD  wdOccupation;
	DWORD master_total;
	DWORD master_balance;
	DWORD prentice_lastlevel;
	DWORD prentice_total;
}__attribute__ ((packed));


}

#pragma pack()

#endif





