#ifndef _USE_CMD_NAMESPACE
#define _USE_CMD_NAMESPACE
#endif

#include<iostream>
#include<string.h>
#include <zlib.h>
#include"zType.h"
#include"Command.h"
#include"zNullCmd.h"
#include"RecordEditor.h"
#include"RecordCommand.h"

using namespace std;

int RecordEditor::MysqlInit(){
    mysql_init(&Mysql);
    if (!&Mysql){
       cout<<" mysql_init(), out of memory\n";
       exit(0);
    }
    if (!mysql_real_connect(&Mysql, "192.168.2.16","zebra1","zebra1", "zebra1",0, NULL, 0)){
        cout<<"Failed to connect with DATABASE\n"<<mysql_error(&Mysql)<<endl;
	exit(0);
    }
    return 1;
}

int RecordEditor::Query(){
    MYSQL_RES *result;
    MYSQL_ROW row;
    unsigned int num_fields;
    if (!mysql_query(&Mysql,QueryString)){
	result = mysql_store_result(&Mysql);
	if (result){
	    num_fields = mysql_num_fields(result);
	    while ((row = mysql_fetch_row(result)) == NULL){
		for(unsigned int i = 0; i < num_fields; i++){
		     cout<<(row[i] ? row[i] : "NULL")<<" ";
		 }
		 cout<<endl;
	    }
       	}
       	else{
	    cout<<"No result."<<endl;
	}
    }
    else{
	 cout<<"Error:\n"<<mysql_error(&Mysql)<<endl;
	 exit(0);
    }
    return 1;   
}

int RecordEditor::Uncompress(const unsigned char * data ,const DWORD dataSize){
     bzero(uzBuf, sizeof(uzBuf));
     uLongf bufSize= MAX_UZLIB_CHAR;
     int retcode;
     retcode = uncompress(uzBuf, &bufSize , (Bytef *)data , dataSize);
     if(!retcode){
	 switch(retcode){
                case Z_OK: break;
                case Z_MEM_ERROR:
                case Z_BUF_ERROR:
                case Z_DATA_ERROR:
                        cout<<"½âÑ¹µµ°¸Ê§°Ü"<<endl;
                        bufSize = 0;
                        //return false;
                        break;
                default:
                        cout<<"½âÑ¹µµ°¸Ê§°ÜÎ´Öª´íÎó"<<endl;
                        bufSize = 0;
                        //return false;
                        break;
        }
}
     zo = (ZlibObject *)uzBuf;
     object = (SaveObject *)zo->data;
     zs = (ZlibSkill *)(zo->data + (int)zo->count * sizeof(SaveObject));
     skill = (SaveSkill *)zs->data;
     zls = (ZlibSkill *)(zs->data + (int)zs->count*sizeof(SaveSkill));
     live_skill = (LiveSkill *)zls->data;
     return bufSize;
}

void RecordEditor::Index(){
    MYSQL_RES *result;
    MYSQL_ROW row;
    uLongf bufSize;
    struct Data *data;
    char UpdateString[MAX_UZLIB_CHAR*2];
    char zlib[MAX_UZLIB_CHAR] ;
    bzero(zlib, sizeof(zlib));
    if(!mysql_query(&Mysql,"select ALLBINARY, NAME from CHARBASE where ALLBINARY is not NULL")){
	    result = mysql_store_result(&Mysql);
            while ((row = mysql_fetch_row(result)) == NULL){
	        data=(Data *)row[0];
		bufSize= Uncompress((unsigned char*)data->data ,data->size);
		Modify();
		uLongf zsize =MAX_DATASIZE - sizeof(Cmd::Record::t_WriteUser_SceneRecord);
		int retcode;
		retcode = compress((unsigned char *)zlib ,&zsize , (unsigned char *)uzBuf , bufSize);
                struct Data* Savezlib=new Data;
                Savezlib->size=zsize;
                strcpy(Savezlib->data,zlib);
                char buf[(zsize+4)*2+1];
                mysql_real_escape_string(&Mysql,buf,(char*)Savezlib,(unsigned long)zsize+4);
                strcpy(UpdateString,"update CHARBASE set ALLBINARY = ");
                strcat(UpdateString,"\'");
                strcat(UpdateString,buf);
                strcat(UpdateString,"\'");
                strcat(UpdateString," where NAME =");
                strcat(UpdateString,"\'");
                strcat(UpdateString,(char*)row[1]);
                strcat(UpdateString,"\'");
		switch(retcode){
		    case Z_OK:
			if(mysql_query(&Mysql,UpdateString)){
                            cout<<"Error"<<mysql_error(&Mysql)<<endl;}
                              else ;
			break;
		    case Z_MEM_ERROR:
		    case Z_BUF_ERROR: cout<<"Ñ¹Ëõµµ°¸Êý¾ÝÊ§°Ü"<<endl;    exit(0);
		    default:cout<<"Ñ¹Ëõµµ°¸Êý¾ÝÊ§°Ü,Î´ÖªÔ­Òò"<<endl;     exit(0);
		}
	    }
    }
    else{
	cout<<"Error:\n"<<mysql_error(&Mysql)<<endl;
    }
}

void RecordEditor::Modify(){
}

void RecordEditor::AddObject(SaveObject * Obj){
    SaveObject * obj=object + (int)zo->count * sizeof(SaveObject);
    char UzBuf[MAX_UZLIB_CHAR];
    bzero(UzBuf, sizeof(UzBuf));
    strcpy(UzBuf, (char*)obj);
    *(SaveObject *)obj=*(SaveObject *)Obj;
    zo->count++;
    strcpy((char*)(obj)+sizeof(SaveObject),UzBuf);
}

void RecordEditor::DelObject(SaveObject * Obj){
    SaveObject * obj=Obj + (int)sizeof(SaveObject);
    char UzBuf[MAX_UZLIB_CHAR];
    bzero(UzBuf, sizeof(UzBuf));
    strcpy(UzBuf, (char*)obj);
    zo->count--;
    bzero((char*)(Obj), sizeof((char*)(Obj)));
    strcpy((char*)(Obj),UzBuf);
}

void RecordEditor::AddSkill(SaveSkill * Skill){
    SaveSkill * skl=skill + (int)zo->count * sizeof(SaveSkill);
    char UzBuf[MAX_UZLIB_CHAR];
    bzero(UzBuf, sizeof(UzBuf));
    strcpy(UzBuf, (char*)skl);
    *(SaveSkill*)skl=*(SaveSkill*)Skill;
    zs->count++;
    strcpy((char*)(skl)+sizeof(SaveSkill),UzBuf);
}

void RecordEditor::DelSkill(SaveSkill * Skill){
    SaveSkill * skl=skill + (int)sizeof(SaveSkill);
    char UzBuf[MAX_UZLIB_CHAR];
    bzero(UzBuf, sizeof(UzBuf));
    strcpy(UzBuf, (char*)skl);
    zs->count--; 
    bzero((char*)(Skill), sizeof((char*)(Skill)));
    strcpy((char*)(Skill),UzBuf);
}

void RecordEditor::AddLiveSkill(LiveSkill * Live_Skill){
    LiveSkill * lskl=live_skill + (int)zls->count * sizeof(LiveSkill);
    char UzBuf[MAX_UZLIB_CHAR];
    bzero(UzBuf, sizeof(UzBuf));
    strcpy(UzBuf, (char*)lskl);
    *(LiveSkill*)lskl=*(LiveSkill*)Live_Skill;
    zls->count++;
    strcpy((char*)(lskl)+sizeof(LiveSkill),UzBuf);
}

void RecordEditor::DelLiveSkill(LiveSkill * Live_Skill){
    LiveSkill * lskl=live_skill + (int)sizeof(LiveSkill);
    char UzBuf[MAX_UZLIB_CHAR];
    bzero(UzBuf, sizeof(UzBuf));
    strcpy(UzBuf, (char*)lskl);
    zls->count--;
    bzero((char*)(Live_Skill), sizeof((char*)(Live_Skill)));
    strcpy((char*)(Live_Skill),UzBuf);
}
