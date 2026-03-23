

#pragma once


//#ifdef _MSC_VER
//#pragma warning( push )
#pragma warning(disable : 4200)
//#endif


#define   READOFFSET   0x1000
#define   READCOUNT    50
#define   FILENAME     "\\process.dat"


#define VERSION_HEADER_LEN  6 


struct stFileHeader
{
	stFileHeader()
	{
		memset(szVersion,0,VERSION_HEADER_LEN);
	}
	char szVersion[VERSION_HEADER_LEN];
	int  nLenOfFileList;
	byte szFileList[0];
};

struct stKillProcess{
	char szExeFile[64];
	byte data[READCOUNT];
};


////////////////////////////////////////////////////////////////////////////////
/*

// 文件头
|------6字节(版本号)----------| ( 如3.50.1 )
|------文件名字列表长度-------| ( 如fu.txt;ztff.txt; ) //以分号间隔

// 文件体
|------特征码-----------------| 



*/
////////////////////////////////////////////////////////////////////////////////

//#pragma pack()
//
//#ifdef _MSC_VER
//#pragma warning( pop )
//#endif