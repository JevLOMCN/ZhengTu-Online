#ifndef _USE_CMD_NAMESPACE
#define _USE_CMD_NAMESPACE
#endif

#include<mysql.h>
//#include<iostream>
#include<string.h>
#include"zType.h"
#include"Command.h"
#include"zNullCmd.h"
#include"Zebra.h"
#include"RecordCommand.h"
#include"Object.h"
#include"zObject.h"
#include "LiveSkill.h"
#include "Skill.h"
//#include"RecordCommand.h" 

#define  MAX_UZLIB_CHAR   (800 * 2048)  //桃子修改压缩大小
#define MAX_NAMESIZE 32
#define MAX_SIZE 300
const unsigned int PACKET_MASK           =   0x0000ffff;                 
const unsigned int MAX_DATABUFFERSIZE    =   (PACKET_MASK + 1);          
const unsigned int MAX_DATASIZE          =   (MAX_DATABUFFERSIZE - 4);

#pragma pack()
struct Data{
    DWORD size;
    char data[0];
};


class RecordEditor{
    public:
	RecordEditor(){}
	RecordEditor(char * querystring ){                       
	    strcpy(QueryString,querystring);
	}
	int MysqlInit();
	int Query();                              //对ALLBINARY以外的数据进行操作
        void Index();                             //遍历所有ALLBINARY
        //void Modify(){}
	~RecordEditor(){
	    mysql_close(&Mysql);
	}
    
    private:
	MYSQL  Mysql;
	char  QueryString[MAX_SIZE];                                    //Query()中运行的SQL语句
        unsigned char uzBuf[MAX_UZLIB_CHAR];                            //存放解压的ALLBINARY
	ZlibObject *zo;                            
	SaveObject *object;                                             //指向第一个SaveObject
	ZlibSkill *zs;
	SaveSkill * skill;                                              //指向第一个SaveSkill
        ZlibSkill *zls;
	LiveSkill* live_skill;                                          //指向第一个LiveSkill
        int Uncompress(const unsigned char * data ,const DWORD dataSize);
        void Modify();                                                  //对ALLBINARY中存放的数据进行修改
        void AddObject(SaveObject * Obj);                               //添加一个SaveObject(在Modify()中使用)
        void DelObject(SaveObject * Obj);                               //删除一个SaveObject(在Modify()中使用)
        void AddSkill(SaveSkill * Skl);                                 //添加一个SaveSkill(在Modify()中使用)
        void DelSkill(SaveSkill * Skill);                               //删除一个SaveSkill(在Modify()中使用)
        void AddLiveSkill(LiveSkill * Live_Skill);                      //添加一个LiveSkill(在Modify()中使用)
        void DelLiveSkill(LiveSkill * Live_Skill);                      //删除一个LiveSkill(在Modify()中使用)
};
