#pragma	 once

#define DIRECT_UP								0
#define DIRECT_UPRIGHT							1
#define DIRECT_RIGHTUP							1
#define DIRECT_RIGHT							2
#define DIRECT_DOWNRIGHT						3
#define DIRECT_RIGHTDOWN						3
#define DIRECT_DOWN								4
#define DIRECT_LEFTDOWN							5
#define DIRECT_DOWNLEFT							5
#define DIRECT_LEFT								6
#define DIRECT_LEFTUP							7
#define DIRECT_UPLEFT							7

#define CEP_VERSION			4		// 版本
#define MAX_ACTION_COUNT	15		// 动作数量

enum enumPart
{	
	eBody=0,		//身体
	eLweapons,		//左手武器
	eRweapons,		//右手武器
	eHair,			//头发
	eHelm,			//头盔
};

struct stFrame
{
	BYTE aPartOrder[5];	// 
	BYTE aNone[3];		// 无用数据
	void Init(int iDirect)
	{
		if(iDirect == DIRECT_UP)
		{
			aPartOrder[0] = eLweapons;
			aPartOrder[1] = eRweapons;
			aPartOrder[2] = eBody;
			aPartOrder[3] = eHair;
			aPartOrder[4] = eHelm;
		}
		else if(DIRECT_UPRIGHT <= iDirect && iDirect <= DIRECT_DOWNRIGHT)
		{
			aPartOrder[0] = eLweapons;
			aPartOrder[1] = eBody;
			aPartOrder[2] = eRweapons;
			aPartOrder[3] = eHair;
			aPartOrder[4] = eHelm;
		}
		else if(iDirect == DIRECT_DOWN)
		{
			aPartOrder[0] = eBody;
			aPartOrder[1] = eRweapons;
			aPartOrder[2] = eLweapons;
			aPartOrder[3] = eHair;
			aPartOrder[4] = eHelm;
		}
		else
		{
			aPartOrder[0] = eRweapons;
			aPartOrder[1] = eBody;
			aPartOrder[2] = eLweapons;
			aPartOrder[3] = eHair;
			aPartOrder[4] = eHelm;
		}
	}
};

struct stAction
{
	long lFrameCountPreDirection;	//每个方向帧数
	stFrame stFrames[8][6];			//从人物面向正上方向开始,顺时针方向存放(每个方向最多6帧)
	stAction()
	{
		for(int i=0; i<8; i++)
		for(int j=0; j<6 ; j++)
			stFrames[i][j].Init(i);
	}
};

struct stCharacterSortStruct
{
	DWORD    dwTypeID;			//类型
	stAction	stActions[MAX_ACTION_COUNT];	//12组动作
	bool operator == (stCharacterSortStruct & v)
	{
		return (dwTypeID == v.dwTypeID);
	}
};

struct stCharacterFileHeader
{
	long lVersion;			//版本号
	long lCharacterCount;	//角色数量			
	stCharacterFileHeader()
	{
		lVersion=CEP_VERSION;
		lCharacterCount=0;
	}
};



