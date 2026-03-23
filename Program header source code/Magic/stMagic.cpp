#include "../engine/include/engine.h"
#include "cmagic.h"

//#include "iAnimation.h"
#include "iCharacter.h"
#include "iLight.h"
#include "math.h"
using namespace std;

#include "MapScene.h"

#include "magic_sound.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////
POINT c_aDirPoint[8] = 
{
	0,-1,
		1,-1,
		1,0,
		1,1,
		0,1,
		-1,1,
		-1,0,
		-1,-1,
};


#define GRIDSCREEN_WIDTH			13
#define GRIDSCREEN_HEIGHT			19
//////////////////////////////////////////////////////////////////////////
//movestruct
//////////////////////////////////////////////////////////////////////////
void GetTileCenter(POINT& ptDes);

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
stKeepImageControl::stKeepImageControl()
{
	nAlphaDec = 500;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param n : 描述
 * \param eBlockResult : 描述
 * \return 返回值的描述
 */
int stMoveStruct::OnBreak(stBlockStruct::BlockTypeEnum n,stBlockStruct::BlockCharacterResult eBlockResult)
{
	switch(n) 
	{
	case stBlockStruct::Block_Obstacle:
		xSpeed = 0;
		ySpeed = 0;
		SetLife(0);
		break;
	case stBlockStruct::Block_Character:
	case stBlockStruct::Block_Self:
		switch(eBlockResult)
		{
		case stBlockStruct::Block_Die:
			SetLife(0);
			break;
		case stBlockStruct::Block_Stop:
			xSpeed = 0;
			ySpeed = 0;
			break;
		case stBlockStruct::Block_Continue:
			break;
		case stBlockStruct::Block_NoBlockAgain:
			break;
		}
		break;		
	}
	return 0;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param ptS : 描述
 * \param ptD : 描述
 * \return 返回值的描述
 */
void stMoveStruct::GetFly(POINT& ptS,POINT& ptD)
{
	ptS = ptStart;
	ptD = ptDes;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
POINT stMoveStruct::GetDir()
{
	POINT ptTmp;
	if(eMove == Move_Static)
		ptTmp = ptStart;
	if(eMove == Move_Parabola)
		ptTmp = ptStart;
	else
		ptTmp = GetPos();

	if(ptTmp.x == ptDes.x && ptTmp.y == ptDes.y)
		ptTmp = ptStart;

	POINT ptDir = {ptDes.x - ptTmp.x,ptDes.y - ptTmp.y};
	return ptDir;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param st : 描述
 * \param iStartSpeed : 描述
 * \return 返回值的描述
 */
void stMoveStruct::Fly(const stFly& st,int iStartSpeed)
{
	stFly fly = st;
	ptStart = fly.ptStart;

	//计算生成位置
	if (ePos == Move_PosTile)
		GetTileCenter(fly.ptPos);
	xPos = fly.ptPos.x;
	yPos = fly.ptPos.y;
 
	switch(ePos)   
	{
	case Move_DesTile:
		GetTileCenter(fly.ptDes);
	case Move_Des:
		xPos = fly.ptDes.x;
		yPos = fly.ptDes.y;
		break;

	case Move_StartTile:
		GetTileCenter(fly.ptStart);
	case Move_Start:
		xPos = fly.ptStart.x;
		yPos = fly.ptStart.x;
		break;

	case Move_TargetCharacter:
		if (fly.pDes)
		{
			POINT pt = fly.pDes->GetPos();
			xPos = pt.x;
			yPos = pt.y;
		}
		else
		{
			xPos = fly.ptDes.x;
			yPos = fly.ptDes.y;
		}
		break;
	case Move_SendCharacter: 
		if (fly.pSender)
		{
			POINT pt = fly.pSender->GetPos();
			xPos = pt.x;
			yPos = pt.y;
		}
		break;
	case Move_SendCharacter_8_rand_1:
		{
			POINT pt;
			pt.x = c_aDirPoint[rand()%8].x * c_nMagicTileWidth + fly.pSender->GetPos().x;
			pt.y = c_aDirPoint[rand()%8].y * c_nMagicTileHeight + fly.pSender->GetPos().y;
			GetTileCenter(pt);
			xPos = pt.x;
			yPos = pt.y;
		}
		break;
	}

	//计算目的位置
	if (eDes == Move_DesTile)
		GetTileCenter(fly.ptDes);
	ptDes = fly.ptDes;
	switch(eDes) 
	{
	case Move_StartTile:
		GetTileCenter(fly.ptStart);
	case Move_Start:
		ptDes = fly.ptStart;
		break;

	case Move_PosTile:
		GetTileCenter(fly.ptPos);
	case Move_Pos:
		ptDes = fly.ptPos;
		break;
	case Move_TargetCharacter:
		if (fly.pDes)
			ptDes = fly.pDes->GetMagicPosition();
		break;
	case Move_SendCharacter:
		if (fly.pSender)
			ptDes = fly.pSender->GetMagicPosition();
		break;
	}

	switch (eMove)
	{
	case Move_Drop:			//从空中掉下		雷电术
	case Move_DropFollow:	//跟踪				火球术？
	case Move_DropStop:		//在空中静止
		{
			ptDes = fly.ptDes;
			nLife -= 1000;
			
			float fLife = nLife / 1000.f;
			float dis = fLife*nSpeed + fLife*fLife*fAcceleration/2;

			int alpha = atol(strParam1);
			xPos = ptDes.x + sinf(angle2radian(alpha)) * dis;
			yPos = ptDes.y - cosf(angle2radian(alpha)) * dis;

			nLife += 1000;
		}
		break;
	case Move_DropHere:		//从当前空中下落
		ptDes = fly.ptDes;
		break;
	case Move_Up:
		{
			ptDes = fly.ptDes;
			ptDes.y -= 1000;
			break;
		}
	case Move_MorePower:
		{ 
			Assert(fly.pSender);
			stMorePower.pSender = fly.pSender;   
		}
		break;
	}

	FlyTo(ptDes,iStartSpeed);
}

/**
 * \brief 运动方式:更新速度和加速度
 * 
 * 详细描述
 * 
 * \param ptD : 描述
 * \param iStartSpeed : 描述
 * \return 返回值的描述
 */
void stMoveStruct::FlyTo(POINT ptD,int iStartSpeed)
{
	ptDes = ptD;
	POINT ptDir = (POINT)ptDes - GetPos();

	if(iStartSpeed != -1)// 可以看出,如果iStartSpeed == -1 ,就是不调整
	{
		nSpeed = iStartSpeed;
	}

	switch (eMove)
	{
	case Move_LineCircle:  // 冰封球
	case Move_Line:		   // 直线运动			火球术
	case Move_LineDes:     // 直线运动到目的点	炎浪术
	case Move_LineFollow:  // 跟踪				火球术？
		{
			//POINT ptSpeed = {ptDes.x - ptStart.x,ptDes.y - ptStart.y};
			POINT ptSpeed = {ptDes.x - xPos,ptDes.y - yPos};
			if ( ptSpeed.x == 0 && ptSpeed.y == 0 )
				return;
			//float f = nSpeed / sqrtf( (float)(ptSpeed.x*ptSpeed.x + ptSpeed.y*ptSpeed.y*c_xy*c_xy) );
			float f = nSpeed / sqrtf((float)(ptSpeed.x*ptSpeed.x + ptSpeed.y*ptSpeed.y));
			xSpeed = ptSpeed.x * f;
			ySpeed = ptSpeed.y * f;
			float a = fAcceleration / sqrtf((float)(ptSpeed.x*ptSpeed.x + ptSpeed.y*ptSpeed.y));
			xAcceleration = ptSpeed.x * a;
			yAcceleration = ptSpeed.y * a;
			if (eMove == Move_LineCircle)
			{
				//放在圆周的最右
				stCircle.xCenter = xPos;
				stCircle.yCenter = yPos;
				stCircle.fAlpha = PI/4.f;
				stCircle.fRadius = 0.f;
				xPos += stCircle.fRadius;
			}
		}	
		break;
	case Move_DropStop:
	case Move_Drop:
	case Move_DropFollow:
	case Move_DropHere:
	case Move_Up:
		{
			//xPos = ptD.x;
			
			int alpha = atol(strParam1);

			xSpeed = -sinf(angle2radian(alpha)) * nSpeed;
			ySpeed = cosf(angle2radian(alpha)) * nSpeed;
			xAcceleration = - sinf(angle2radian(alpha)) * fAcceleration;
			yAcceleration = cosf(angle2radian(alpha)) * fAcceleration;
		}
		break;
	case Move_Parabola:
		{
			InitParabola();
		}
		break;
	case Move_MorePower: //  快速移动
		{
			
			POINT ptGrid = ptDes;
			Map_PixelToGrid(&ptGrid);
			Map_GridCenterToPixel(&ptGrid);
			ptDes = ptGrid;

			// 魔法位置
			POINT ptSpeed = {ptDes.x - xPos,ptDes.y - yPos};
			if ( ptSpeed.x == 0 && ptSpeed.y == 0 )
				return;
			float f = nSpeed / sqrtf((float)(ptSpeed.x*ptSpeed.x + ptSpeed.y*ptSpeed.y));
			xSpeed = ptSpeed.x * f;
			ySpeed = ptSpeed.y * f;
			//float a = fAcceleration / sqrtf((float)(ptSpeed.x*ptSpeed.x + ptSpeed.y*ptSpeed.y));
			//xAcceleration = ptSpeed.x * a;
			//yAcceleration = ptSpeed.y * a;

			xAcceleration = 0; // acceleration was ommitted 
			yAcceleration = 0;
			// 处理人物移动位置
			stMorePower.pSender->MoveTo(ptDes,Ani_Run,true);
			stMorePower.pSender->ChangeSpeed((int)xSpeed,(int)ySpeed);
			//stMorePower.pSender->SetRunAniSpeed(nSpeed);
		}
		break;
	}	
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param p : 描述
 * \param ptMagic : 描述
 * \return 返回值的描述
 */
//////////////////////////////////////////////////////////////////////////
//block
//////////////////////////////////////////////////////////////////////////
bool stBlockStruct::IsBreak(iCharacter* p,POINT ptMagic)
{
	switch(eBlockGeometry) 
	{
	case Block_Image:
	case Block_Grid:
		{
			POINT ptOffset = ptMagic;
			if(p == NULL)
			{
				ptMagic.x += ptCenter.x;
				ptMagic.y += ptCenter.y;
				CMapScene* pMapScene = GetMapScene();
				ptMagic = pMapScene->PixelPosToGridPos(ptMagic);
				if( ptMagic.x < 0 || ptMagic.x >= pMapScene->GetWidth() || ptMagic.y < 0 || ptMagic.y >= pMapScene->GetHeight())
				{
					// 超出地图范围
					return false;
				}
				if(pMapScene->IsMagicObstacle(ptMagic))
				{
					return true;
				}
				return false;
			}
			else
			{
				if (eBlockGeometry == Block_Image)
					ptOffset -= p->GetMagicAdjust();
				ptOffset -= p->GetPos();
				if (abs(ptOffset.x) < c_nMagicTileWidth + ptCenter.x && abs(ptOffset.y) < c_nMagicTileHeight + ptCenter.y)
					return true;
			}
			break;
		}
	case Block_Circle:
		{
			if(p == NULL) return false;
			ptMagic.x += ptCenter.x;
			ptMagic.y += ptCenter.y;
			if (fRadius == 0)
			{
				if (p->IsAt(ptMagic))				
					return true;
			}
			else
			{
				int offx[] = {-fRadius,0,fRadius,0};
				int offy[] = {0,-fRadius,0,fRadius};
				for (int i=0;i<4;i++)
				{
					POINT pt = ptMagic;
					pt.x += offx[i];
					pt.y += offy[i];
					if (p->IsAt(ptMagic))				
						return true;
				}
			}
		}
		break;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
//music
//////////////////////////////////////////////////////////////////////////

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pSound : 描述
 * \param ptPos : 描述
 * \param velocity : 描述
 * \param nInitLife : 描述
 * \param nLife : 描述
 * \param nMagicType : 描述
 * \return 返回值的描述
 */
int stMusicStruct::Run(CSceneSound* pSound, POINT ptPos,POINT velocity,int nInitLife, int nLife,size_t nMagicType)
{
	if(!bMusic)
	{
		if (eMusicEvent == Music_Event_Null)
			return false;
		if (idMusic == 0 && strcmp(szMusic,"null") == 0)
			return false;
		switch(eMusicEvent)
		{
		case Music_LifeEnd:
			if (nLife <= 0)
				bMusic = true;
			break;
		case Music_LifeStart:
			bMusic = true;
			break;
		case Music_LifePassed:
			if (nMusicTime > nLife)
				bMusic = true;
			break;
		case Music_LifePass:
			if (nInitLife - nLife > nMusicTime)
				bMusic = true;
			break;
		}

		if ( bMusic )
		{
			PlayMagicSound( pSound,ptPos,velocity,szMusic,nMagicType,(bMusicLoop != 0) );
			run_time = xtimeGetTime();
		}
	}
	else
	{
		//if(xtimeGetTime() - run_time > 100)
		{
			pSound->SetParam(ptPos);
			run_time = xtimeGetTime();
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
//generate
//////////////////////////////////////////////////////////////////////////

const int c_nRingNum = 8;
POINT c_aRingPoint[c_nRingNum] = 
{
	18,44,
		44,17,
		98,7,
		153,20,
		175,54,
		154,72,
		108,84,
		44,76,
};
const int c_width = c_nMagicTileWidth;
const int c_height = c_nMagicTileHeight;

/**
 * \brief 根据生成方式，生成子魔法
 * 
 * 详细描述
 * 
 * \param pFather : [in]父魔法
 * \param listGenerate : [out]用于存放生成的子魔法
 * \return 返回值: 无
 */
void stGenerateStructA::Generate(cMagic* pFather,tagListMagicGenerate& listGenerate)
{
	nGenerated++;			// 生成次数记录 ==0 代表第一次来生成子魔法
	int iStartSpeed = -1;
	switch(this->eGenerateSpeed)  //当前速度
	{
	case Generate_Speed_Current:
		iStartSpeed = pFather->m_stProperty.stMove.nSpeed;
		break;
	}
	switch(eGenerateGeometry)  // 生成方式
	{
	case Generate_OnCircle_3:// 生成环上三个
		{
			POINT ptPos = pFather->GetPos();
			int n = rand() % 360;
			float fAlpha = ( (float)n / 360.f )* ( 2.f * PI );
			float radius = 64.f;
			float addAlpha = 2.f * PI / 3.f;
			for (int i=0; i<3; i++)
			{
				float fx = sin(fAlpha) * radius;
				float fy = cos(fAlpha) * radius;
				fy /= 2;
				iMagic* p = GenerateAMagicByPos(pFather,strGenerateType,ptPos,ptPos + stPointI(fx,fy),NULL,iStartSpeed);
				if (p)
				{
					listGenerate.push_back(p);
					p->SetAlphaRadius(fAlpha,radius);
				}
				fAlpha += addAlpha;
			}
		}
		break;
	case Generate_Tangent://沿着切线生成
		{
			POINT ptPos = pFather->GetPos();
			if (pFather->GetLife() == 0) //almost die
			{
				for (int i=0; i<16; i++)
				{
					//float fSpeed = p->GetSpeed();
					float fAlpha = i * 2 * PI / 16;
					//speed is wrong...
					//float fx = fSpeed*sin(fAlpha) * c_nMagicTileWidth / sqrt(c_nMagicTileWidth * c_nMagicTileHeight);
					//float fy = fSpeed*cos(fAlpha) * c_nMagicTileHeight / sqrt(c_nMagicTileWidth * c_nMagicTileHeight);
					float fx = sin(fAlpha) * c_nMagicTileWidth;
					float fy = cos(fAlpha) * c_nMagicTileHeight;
					iMagic* p = GenerateAMagicByPos(pFather,strGenerateType,ptPos,ptPos + stPointI(fx,fy),NULL,iStartSpeed);
					if (p)
					{
						//p->SetSpeed(fx,fy);
						//p->SetPos(pFather->GetPos());
						listGenerate.push_back(p);
					}
				}
			}
			else
			{
				float fAlpha = pFather->m_stProperty.stMove.GetTangent();
				float fx = sin(fAlpha) * c_nMagicTileWidth;
				float fy = cos(fAlpha) * c_nMagicTileHeight;
				iMagic* p = GenerateAMagicByPos(pFather,strGenerateType,ptPos,ptPos + stPointI(fx,fy),NULL,iStartSpeed);
				if (p)
					listGenerate.push_back(p);
			}
		}
		break;
	case Generate_Point://生成一个点
		{
			stPointI ptOffset(0,0);
			if(strchr(szGenerateData,','))
			{
				ptOffset = stPointI(szGenerateData);
			}
			iMagic* p = GenerateAMagicDesOff(pFather,strGenerateType,ptOffset,iStartSpeed);
			if (p)
				listGenerate.push_back(p);
		}
		break;
	case Generate_PointUnCircle://逆时针生成一格
		{
			//start pos and end pos to get dir;
			POINT ptPos = pFather->GetPos();
			POINT ptdir = ptPos - pFather->GetStartPos(); 
			int dir = GetDir8(ptdir.x,ptdir.y);
			dir--;
			if (dir < 0)
				dir += 8;
			POINT ptAdd = c_aDirPoint[dir];
			ptAdd.x *= c_nMagicTileWidth;
			ptAdd.y *= c_nMagicTileHeight;
			iMagic* p = GenerateAMagic(pFather,strGenerateType,iStartSpeed);
			if (p)
			{
				p->SetPos(ptAdd+pFather->GetStartPos());
				listGenerate.push_back(p);
			}
		}
		break;
	case Generate_RandomPoint://生成一个随机点，冰风暴
		{
			iMagic* p = GenerateAMagic(pFather,strGenerateType,iStartSpeed);
			if (p)
			{
				POINT pt = p->GetPos();
				///*
				if(strcmp(szGenerateData,"烟花") == 0)
				{
					int dx,dy;
					dx = rand()%(5) - 2;
					dy = rand()%5 - (2) ;
					pt.x += dx * c_nMagicTileWidth + rand() % (c_nMagicTileWidth/2) - c_nMagicTileWidth/4;
					pt.y += dy * c_nMagicTileHeight+ rand() % (c_nMagicTileHeight/2) - c_nMagicTileHeight/4;
				}
				else if(strcmp(szGenerateData,"烟花2") == 0)
				{
					pt.x += rand() % (c_nMagicTileWidth*3) - c_nMagicTileWidth*3/2;
					pt.y += rand() % (c_nMagicTileHeight*3) - c_nMagicTileHeight*3/2;			
				}
				else
				//*/
				{
					pt.x += rand() % (c_nMagicTileWidth/2) - c_nMagicTileWidth/4;
					pt.y += rand() % (c_nMagicTileHeight/2) - c_nMagicTileHeight/4;
				}
				p->SetPos(pt);
				listGenerate.push_back(p);
			}
			break;
		}
	case Generate_FollowPoint://生成跟随点 火球术的火花
		{
			POINT ptStart,ptDes;
			pFather->GetFly(ptStart,ptDes);
			POINT ptPos = pFather->GetPos();

			POINT ptOffset;
			ptOffset.x = rand() % (c_nMagicTileWidth) - c_nMagicTileWidth/2;
			ptOffset.y = rand() % (c_nMagicTileHeight) - c_nMagicTileHeight/2;

			float fx,fy;
			pFather->GetSpeed(fx,fy);
			iMagic* p = GenerateAMagic(pFather,strGenerateType,iStartSpeed);
			if (p)
			{
				p->SetSpeedAndTurn(fx,fy);
				p->SetPos(ptPos+ptOffset);
				listGenerate.push_back(p);
			}
		}
		break;
	case Generate_Ring://生成一个环
		{
			POINT ptCenter = stPointI(128,64);
			for (int i=0; i<c_nRingNum; i++)
			{
				POINT ptDes = POINT(c_aRingPoint[i]) - ptCenter;
				ptDes.x += rand() % 8 - 4;
				ptDes.y += rand() % 4 - 2;
				iMagic* p = GenerateAMagicDesOff(pFather,strGenerateType,ptDes,iStartSpeed);
				if (p)
					listGenerate.push_back(p);
			}
		}
		break;
	case Generate_Fan:  // 扇形发射
		{
			int nDirNum = atoi(szGenerateData);
			if( nDirNum <= 0)
				break;
			if( nDirNum > 16) nDirNum = 16;
			nDirNum--;   // self 
			POINT ptPos = pFather->GetPos();
			POINT ptStart,ptDes;
			pFather->GetFly(ptStart,ptDes);
			POINT pos = pFather->GetPos();
			int curdir  = pFather->GetDir16(); 
			int nDir1 = curdir - nDirNum/2;
			int nDir2 = curdir + nDirNum/2;
			if (nDirNum%2) nDir2++;
			POINT pt;
			// 如果有目标人，只有其中一个箭飞向，其它箭都没有目标人,
			// 算了，散射干脆没有目标人,反正后面会计算出
			//iCharacter* pDes = pFather->m_pDes;
			//pFather->m_pDes = NULL;
			for(int nCookie = nDir1;nCookie <= nDir2; nCookie++)
			{
				int real = nCookie; 
				if( nCookie < 0) real += 16;
				if( nCookie >= 16) real -= 16;
				float radius = c_nMagicTileWidth * 1.42f;
				float fAlpha = 22.5f*PI/180.f*real;

				pt.x = radius * sinf(fAlpha);
				pt.y = -radius * cosf(fAlpha) * 1/2 ; // project
				//if(pDes && nCookie == curdir)
				//	pFather->m_pDes = pDes;
				//else
				//pFather->m_pDes = NULL;
				iMagic* p = GenerateAMagicByPos(pFather,strGenerateType,ptStart,pos+pt,NULL,iStartSpeed);
				if (p)
					listGenerate.push_back(p);				
			}		
		}
		break;
	case Generate_MulParallel:  // n平行
		{

			/////////////////////////////////////////
                        // 参数(x,y) : x表示平行数  y表示间隔
			stPointI _ptOffset(0,0);
			if(strchr(szGenerateData,','))
			{
				_ptOffset = stPointI(szGenerateData);
			}
			int nParallelNum = _ptOffset.x;
			if( nParallelNum == 0)
				nParallelNum = 3;
			int radius = _ptOffset.y;
			if( radius == 0)
			{
				radius = c_nMagicTileWidth * 1.42f;
			}
			/////////////////////////////////////////////
			POINT ptStart,ptEnd;
			pFather->GetFly(ptStart,ptEnd);
			POINT oldpos = pFather->GetPos();

			int curdir  = pFather->GetDir16(); 
			float ftheta = 22.5f*PI/180.f*curdir;	
			iCharacter * pOldCharacter = pFather->m_pDes;
			//if( pOldCharacter )
				//pFather->m_pDes = NULL;
			for(int i = 0; i< nParallelNum;i++)
			{
				stPointI ptOffset;
				ptOffset.x = - (nParallelNum - 1 - 2*i) / 2 * radius * cos(ftheta);
				ptOffset.y = - (nParallelNum - 1 - 2*i) / 2 * radius * sin(ftheta);

                                POINT pt = oldpos + ptOffset;
                                pFather->SetPos(oldpos + ptOffset);
				iMagic* p = GenerateAMagicByPos(pFather,strGenerateType,ptStart+ptOffset,ptEnd+ptOffset,NULL,iStartSpeed);
				if (p)
					listGenerate.push_back(p);
			}
			 pFather->SetPos(oldpos);
			 if(pOldCharacter)
				 pFather->m_pDes = pOldCharacter;
			break;
		}
	case Generate_MulTimeStamp: // n 连射
		{
			iMagic* p = GenerateAMagicDesOff(pFather,strGenerateType,stPointI(0,0),iStartSpeed);//strGenerateType：生成物体
			if (p)
				listGenerate.push_back(p);
		}
		break;

	case Generate_22:
		{
			stPointI pt[4];
			pt[0] = stPointI(0,0);
			pt[1] = stPointI(c_width,0);
			pt[2] = stPointI(0,c_height);
			pt[3] = stPointI(c_width,c_height);
			for (int i=0; i<4; i++)
			{
				pt[i].x += rand() % (c_nMagicTileWidth/4) - c_nMagicTileWidth/8;
				pt[i].y += rand() % (c_nMagicTileHeight/4) - c_nMagicTileHeight/8;
				iMagic* p = GenerateAMagicDesOff(pFather,strGenerateType,pt[i],iStartSpeed);
				if (p)
					listGenerate.push_back(p);
			}
		}
		break;
	case Generate_12:
	case Generate_13:
	case Generate_14:
	case Generate_15:
		{
			POINT ptStart = pFather->GetStartPos();
			POINT ptEnd = pFather->GetDes();
			POINT ptDir = ptEnd - ptStart;
			int dir = GetDir8(ptDir.x,ptDir.y);
			int num = eGenerateGeometry - Generate_12 + 2;
			for (int i=0; i<num; i++)
			{
				POINT pt = c_aDirPoint[dir];
				pt.x = (i+1) * pt.x * c_width;
				pt.y = (i+1) * pt.y * c_height;
				//与目标点的偏移
				pt -= pFather->GetDes();
				pt += pFather->GetPos();
				iMagic* p = GenerateAMagicDesOff(pFather,strGenerateType,pt,iStartSpeed);
				if (p)
					listGenerate.push_back(p);
			}
		}
		break;
	case Generate_33:
		{
			stPointI pt[9];
			pt[0] = stPointI(-c_width,-c_height);
			pt[1] = stPointI(-c_width,0);
			pt[2] = stPointI(-c_width,c_height);
			pt[3] = stPointI(0,-c_height);
			pt[4] = stPointI(0,0);
			pt[5] = stPointI(0,c_height);
			pt[6] = stPointI(c_width,-c_height);
			pt[7] = stPointI(c_width,0);
			pt[8] = stPointI(c_width,c_height);
			for (int i=0; i<9; i++)
			{
				pt[i].x += rand() % (c_nMagicTileWidth/2) - c_nMagicTileWidth/4;
				pt[i].y += rand() % (c_nMagicTileHeight/2) - c_nMagicTileHeight/4;
				iMagic* p = GenerateAMagicDesOff(pFather,strGenerateType,pt[i],iStartSpeed);
				if (p)
					listGenerate.push_back(p);
			}
		}
		break;
	case Generate_55:
		{
			int y = -c_height*2;
			for (int i=0; i<5; i++)
			{
				int x = -c_width * 2;
				for(int j=0;j<5;j++)
				{
					stPointI pt;
					pt.x = x + rand() % (c_nMagicTileWidth/2) - c_nMagicTileWidth/4;
					pt.y = y + rand() % (c_nMagicTileHeight/2) - c_nMagicTileHeight/4;
					iMagic* p = GenerateAMagicDesOff(pFather,strGenerateType,pt,iStartSpeed);
					if (p)
						listGenerate.push_back(p);
					x += c_width;
				}
				y +=  c_height;
			}
		}
		break;
	case Generate_FullScreenDirCharacter:
	case Generate_FullScreenCharacter:
		{
			SetFrameAllocator fa;
			tStackVectorICharacter listDes;
			listDes.reserve(512);
			RangeEnum eRange = Range_FullScreenDir;
			if (eGenerateGeometry == Generate_FullScreenCharacter)
				eRange =Range_FullScreen;
			GetMapScene()->GetCharacter(eRange,listDes,pFather->GetPos(),pFather->GetDir(),pFather->GetSender());
			//here must a call back function 
			for (tStackVectorICharacter::iterator it = listDes.begin(); it != listDes.end(); ++it)
			{
				POINT pt = {0,0};
				iCharacter* pCha = *it;
				if (pCha != pFather->GetSender())
				{
					iMagic* p = GenerateAMagicByPos(pFather,strGenerateType,pFather->GetPos(),pt,pCha,iStartSpeed);
					if (p)
						listGenerate.push_back(p);
				}
			}
		}
		break;
	case Generate_Dir_2_UnCircle:
	case Generate_Dir_1:
	case Generate_Round_Half:
	case Generate_Round:
		{
			bool aDir[8];
			for (size_t i = 0; i < 8; i ++)
				aDir[i] = false;
			if (eGenerateGeometry == Generate_Dir_1)
			{
				size_t dir = pFather->GetDir8();
				aDir[dir] = true;
			}
			else if (eGenerateGeometry == Generate_Dir_2_UnCircle)
			{
				size_t dir = pFather->GetDir8();
				dir += 2;
				if (dir >= 8)
					dir -= 8; 
				aDir[dir] = true;
			}
			else if (eGenerateGeometry == Generate_Round_Half)
			{
				size_t dir = pFather->GetDir8();
				for (int i = -1; i <= 1; i++)
				{
					int t = dir + i;
					if ( t < 0)
						t += 8;
					if (t >= 8)
						t -= 8; 
					aDir[t] = true;
				}
			}
			else if (eGenerateGeometry == Generate_Round)
			{
				for (size_t i = 0; i < 8; i ++)
					aDir[i] = true;
			}
			for (size_t i = 0; i < 8; i ++)
			{
				if (aDir[i])
				{
					POINT pt = c_aDirPoint[i];
					pt.x *= c_nMagicTileWidth;
					pt.y *= c_nMagicTileHeight;
					POINT pos = pFather->GetPos();
					POINT ptStart,ptDes;
					pFather->GetFly(ptStart,ptDes);
					iMagic* p = GenerateAMagicByPos(pFather,strGenerateType,ptStart,pos+pt,NULL,iStartSpeed);
					if (p)
						listGenerate.push_back(p);
				}
			}
		}
		break;
	}

}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pFather : 描述
 * \param n : 描述
 * \return 返回值的描述
 */
iMagic* stGenerateStruct::GenerateByBlock(cMagic* pFather,int n)
{
	iMagic* p = NULL;
	switch (n)
	{
	case stBlockStruct::Block_Character:
		p = GenerateAMagic(pFather,strGenerateBlockCharacter);
		break;
	case stBlockStruct::Block_Obstacle:
		pFather->GetProperty()->stMove.ptDes = pFather->GetPos();
		pFather->m_pDes = NULL;
		p = GenerateAMagic(pFather,strGenerateBlockObstacle);
		break;
	}
	if (p)
		p->SetBlockGenerate(true); 
	return p;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pt1 : 描述
 * \param pt2 : 描述
 * \return 返回值的描述
 */
inline float GetDistance(const POINT* pt1,const POINT* pt2)
{
	return sqrt((float)((pt1->x-pt2->x) * (pt1->x-pt2->x)) + (float)((pt1->y - pt2->y)*(pt1->y - pt2->y)*4));
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pFather : 描述
 * \param nLife : 描述
 * \param listGenerate : 描述
 * \return 返回值的描述
 */
void stGenerateStructA::GenerateByLife(cMagic* pFather,int nLife,tagListMagicGenerate& listGenerate)
{
	if(strGenerateType[0] == 0 || strcmp(strGenerateType,"null") == 0) return;

	if (nLife == 0 && eGenerateEvent != Generate_LifeEnd)
		return;

	//Test start
	//nLife = 0;
	//eGenerateEvent = Generate_LifeEnd;
	//Test end

	bool bGenerate = false;
	switch(eGenerateEvent)
	{
	case Generate_Circle:
		bGenerate = true;
		break;
	case Generate_LifeEnd:    // 生命结束生成
		if (nGenerated == 0)
			if (nLife == 0)
				bGenerate = true;
		break;
	case Generate_LifeStart: // 生命开始生成
		if (nGenerated == 0)
			if (nInitLife >= nLife)
				bGenerate = true;
		break;
	case Generate_LifeStartEnd:
		if (nGenerated == 0)
		{
			if (nInitLife >= nLife)
				bGenerate = true;
		}
		else if (nGenerated == 1)
		{
			if (nLife == 0)
				bGenerate = true;
		}
		break;
	case Generate_LifePassed:
		if (nGenerated == 0)
			if (nGenerateTime > nLife)
			{
				bGenerate = true;
			}
			break;
	case Generate_LifePass:
		if (nGenerated == 0)
			if (nInitLife - nLife > nGenerateTime)
			{
				bGenerate = true;
			}
			break;
	case Generate_LifeRandomEach:	// 随机持续生成
		if (nLife != 0 && nInitLife != nLife)
		{
			int rd = nGenerateTime / (nInitLife - nLife) + 2;
			if (rand() % rd == 0)
			{
				bGenerate = true;
				nInitLife = nLife+1;
			}
		}
		break;
	case Generate_LifePassRandomEach://每过时间随机生成
		if (nLife != 0 && nInitLife - nLife > nGenerateTime)
		{
			if(rand() % 2)
			{
				bGenerate = true;
			}
			nInitLife = nLife;
		}
		break;
	case Generate_LifePassEachDis:
		{
			float nDis = GetDistance(&pFather->GetPos(),&ptInitPos);
			if(nDis > (float)nGenerateTime)
			{
				ptInitPos = pFather->GetPos();
				bGenerate = true;
			}
		}
		break;
	case Generate_LifePassEach: //每过一段时间生成
		if (nLife != 0 && nInitLife - nLife > nGenerateTime)
		{
			bGenerate = true;
			nInitLife = nLife;
		}
		break;
	case Generate_LifeRandom:
		{
			eGenerateEvent = Generate_LifePass;
			nGenerateTime = rand() % (nInitLife-100);
		}
		break;
	case Generate_Life_MulTimeStamp: //  n连射
		{
			if(nTimer == 0)  // 第一次  
			{
				char * pfirst = szGenerateData;
				int totallen = strlen(szGenerateData);
				int nskip = 0 ;  

				char * pcur = strchr(pfirst,',');
				char szTemp[16] = {0};
				while(pcur && nskip < totallen)
				{
					nskip += pcur - pfirst + 1;
					strncpy(szTemp,pfirst,pcur-pfirst);
					int time = atoi(szTemp);
					vecTimer.push_back(time);
					pfirst = pcur+1;
					pcur = strchr(pfirst,',');					    
				}
				if(nskip < totallen) //  最后不是','
				{
					strncpy(szTemp,pfirst,totallen-nskip);
					int time = atoi(szTemp);
					vecTimer.push_back(time);
				}
				nTimer++;
			}			
			if( nTimer <= vecTimer.size())
			{
				int nElaspedTime = 0;
				for( int i = 0 ;i < nTimer ; i++)
					nElaspedTime += vecTimer[i];

				if( nLife !=0 && nInitLife - nLife > nElaspedTime)
				{
					nTimer ++ ;
					bGenerate = true;
				}
			}
		}
		break;
	}
	if (bGenerate)
	{
		//TRACE("%s time %d\n",__FUNCTION__,xtimeGetTime());
		Generate(pFather,listGenerate);
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pFather : 描述
 * \param strName : 描述
 * \param iStartSpeed : 描述
 * \return 返回值的描述
 */
//////////////////////////////////////////////////////////////////////////
//create a magic
//////////////////////////////////////////////////////////////////////////
iMagic* GenerateAMagic(cMagic* pFather,std::string strName,int iStartSpeed)
{
	POINT ptStart,ptDes;
	pFather->GetFly(ptStart,ptDes);
	return GenerateAMagicByPos(pFather,strName,ptStart,ptDes,NULL,iStartSpeed);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pFather : 描述
 * \param strName : 描述
 * \param ptDesOffset : 描述
 * \param iStartSpeed : 描述
 * \return 返回值的描述
 */
iMagic* GenerateAMagicDesOff(cMagic* pFather,std::string strName,POINT ptDesOffset,int iStartSpeed)
{
	POINT ptStart,ptDes;
	pFather->GetFly(ptStart,ptDes);
	return GenerateAMagicByPos(pFather,strName,ptStart,ptDes + ptDesOffset,NULL,iStartSpeed);	
}
inline bool is_eq(float f1,float f2)
{
	return fabs(f1-f2) < 0.0001;
}
void stMoveStruct::InitParabola()
{
	stParabola.total_t = this->nLife/1000.0f;

	stPointF pt1 = Pt2DTo3D_2(ptStart);// 屏幕到地面
	stPointF pt2 = Pt2DTo3D_2(ptDes);

	//x==offset z=offset(human height)
	// strParam3:魔法出发点距离人足点的x偏移,也就是魔法x从ptStart.x+atof(strParam3)开始;
	// strParam4:从屏幕上量出的人的高度，这个高度被转换到垂直于地面的人的真正的高度就是atof(strParam4)/cosf(c_fAngleA),
	//           同时人的高度也是魔法的z上的距离人足点的偏移
	stPoint3F pt_off(atof(strParam3),0.0f,atof(strParam4) / cosf(c_fAngleA));

	float yDis = -pt_off.z;// 魔法出发点y方向在 -pt_off.z处
	stPointF pt3 = pt2-pt1;
	float xDis = PtLength(pt3);

	// 以后的抛物线运动总在x-z平面上做好，然后旋转alpha后变到与x-z屏幕交角为alpha的相交平面上
	stParabola.alpha = PtToAngle(pt3);

	xDis -= pt_off.x;// 魔法出发点x方向已经在 pt_off.x了，所以真正要走的距离为xDis-=pt_off.x
	if(xDis < 0) xDis = 0;

	// 45度抛出
	float vx0 = xDis/stParabola.total_t;
	float vy0 = vx0;

	stParabola.magicOffX = pt_off.x;
	stParabola.magicOffZ = pt_off.z;

	stParabola.Init(xDis,yDis,vx0,vy0);

	dwTime = xtimeGetTime();
	RunParabola();
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void stMoveStruct::RunParabola()
{
	float time = (xtimeGetTime() - dwTime)/1000.f;
	

	stParabola.Run(time);
	stPoint3F pt(stParabola.x,0.0f,-stParabola.y);// y = 0.0f,可见在x-z平面上
	pt.x += stParabola.magicOffX;
	pt.z += stParabola.magicOffZ;

	stPointF pt1 = Pt3DTo2D(pt,stParabola.alpha);// 见 InitParabola()中的说明 

	xPos = pt1.x + ptStart.x;
	yPos = pt1.y + ptStart.y;

	if(time > stParabola.total_t)
	{
		xPos = ptDes.x;
		yPos = ptDes.y;
		this->SetLife(0);
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
POINT stMoveStruct::GetOvergroundPos()
{
	if(eMove == Move_Parabola)
	{
		stPoint3F pt(stParabola.x,0.0f,0.0f);
		pt.x += stParabola.magicOffX;
		//pt.z += stParabola.magicOffZ;
		
		stPointF pt1 = Pt3DTo2D(pt,stParabola.alpha);

		POINT pt3 ={pt1.x + ptStart.x,pt1.y + ptStart.y};
		return pt3;
	}
	else
	{
		POINT pt = {xPos,yPos};
		return pt;
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pFather : 描述
 * \param strName : 描述
 * \param ptStart : 描述
 * \param ptDes : 描述
 * \param pDesCharacter : 描述
 * \param iStartSpeed : 描述
 * \return 返回值的描述
 */
iMagic* GenerateAMagicByPos(cMagic* pFather,std::string strName,POINT ptStart,POINT ptDes,iCharacter* pDesCharacter,int iStartSpeed)
{
	stFly fly;
	fly.ptStart = ptStart;
	fly.ptDes = ptDes;
	fly.ptPos = pFather->GetPos();
	/*
	if(pFather->GetProperty()->stMove.eMove == stMoveDataStruct::Move_Parabola)
	{
		fly.ptStart = pFather->GetDes();
		fly.ptPos = pFather->GetDes();
	}
	//*/

	fly.pSender = pFather->m_pSender;
	fly.pDes = pFather->m_pDes;

	if (pDesCharacter != NULL)
	{
		fly.ptDes = pDesCharacter->GetPos();
		fly.pDes = pDesCharacter;
	}

	bool bLight = false;
	if (pFather)
	{
		if (pFather->GetLight() == NULL)
			bLight = false;
		else
			bLight = pFather->GetLight()->IsEnable();
	}

	if (strName != "null")
	{
		iMagic* p = Magic_CreateMagic(strName.c_str(),pFather->m_nTable);
		if (p == NULL)
			return NULL;

		p->CreateMagic(bLight);
		//p->TurnTo(pFather->GetDir());
		p->Fly(fly,iStartSpeed);
		if (fly.pSender != NULL)
			fly.pSender->OnMagicBegin(p);
		
		if (p->GetFollowType() == stMoveStruct::Move_Follow_Des )
		{
			if (fly.pDes != NULL)
			{
				//fly.pDes->AddMagic(p);
				if( !fly.pDes->FindAndAddMagic(p))
					p->Release();
				return NULL;
			}
		}

		return p;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
//move
//////////////////////////////////////////////////////////////////////////

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param f : 描述
 * \param fa : 描述
 * \return 返回值的描述
 */
void test(float f,float fa)
{
	if (f > 0 && fa < 0 || f < 0 && fa > 0)
	{
		if (fabs(f) < fabs(fa))
		{
			//Assert(0);
		}
	}
}
bool InSeam(POINT p1,POINT p2, POINT pt)
{
	if (p1 == pt)
		return false;
	if ( (p1.x - pt.x)*( p2.x - pt.x) > 0)
		return false;
	if ( (p1.y - pt.y)*( p2.y - pt.y) > 0)
		return false;
	POINT pt1 = pt - p1;
	POINT pt2 = p2 - pt;
	if (GetDir8(pt1.x,pt1.y) == GetDir8(pt2.x,pt2.y))
		return true;
	return false;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
float stMoveStruct::GetCurSpeed()
{
	return sqrtf((float)(xSpeed*xSpeed + ySpeed*ySpeed));
}

/**
 * \brief 更新(主要是累加)位移和速度
 * 
 * 详细描述
 * 
 * \param time_eslasped : 描述
 * \param pDes : 描述
 * \return 返回值的描述
 */
int stMoveStruct::Move(long time_eslasped,iCharacter* pDes)
{
	nLife -= time_eslasped;
	if (nLife < 0)
		nLife = 0;

	bool bMoved = true;
	switch (eMove)
	{
	case Move_Parabola:
		RunParabola();
		break;
	case Move_Circle:
		{
			// float alpha1000 = 1 * PI / 6;
			float addAlpha = (float)(nSpeed)/1000.f * (float)time_eslasped/1000.f;
			stCircle.fAlpha += addAlpha;
			float fx = sin(stCircle.fAlpha) * stCircle.fRadius;
			float fy = cos(stCircle.fAlpha) * stCircle.fRadius;
			SetPos(stPointI(fx,fy/2));
		}
		break;
	case Move_DropFollow:
	case Move_Drop:
	case Move_DropHere:
	case Move_Up:
	case Move_MorePower:   
		//nSpeed += Time2Speed(fAcceleration);
		xPos += Time2Speed(xSpeed);
		yPos += Time2Speed(ySpeed);
		
		xSpeed += Time2Speed(xAcceleration);
		ySpeed += Time2Speed(yAcceleration);
		break;
	case Move_Line:
	case Move_LineDes:
	case Move_LineFollow:
		//test(xSpeed,xAcceleration);
		//test(ySpeed,yAcceleration);
		//其中可能错过了目标点？
		if(eFollow == Move_Follow_Des_Fly && pDes) //跟随目的人飞行
		{
			stPointI ptDes = pDes->GetPos();
			stPointI ptCur = GetPos();
			stPointI ptDir = ptDes - ptCur;

			FlyTo(ptDes,nSpeed);
		}

		//nSpeed += Time2Speed(fAcceleration);
		if (pDes != NULL)
		{
			int oldx = xPos;
			int oldy = yPos;
			xPos += Time2Speed(xSpeed);
			yPos += Time2Speed(ySpeed);
			POINT ptDes = pDes->GetPos();
			if (InSeam(stPointI(oldx,oldy),stPointI(xPos,yPos),ptDes))
			{
				xPos = ptDes.x;
				yPos = ptDes.y;
			}
		}
		else
		{
			xPos += Time2Speed(xSpeed);
			yPos += Time2Speed(ySpeed);
		}
		
		xSpeed += Time2Speed(xAcceleration);
		ySpeed += Time2Speed(yAcceleration);

		break;
	case Move_LineCircle:
		stCircle.xCenter += Time2Speed(xSpeed);
		stCircle.yCenter += Time2Speed(ySpeed);
		xSpeed += Time2Speed(xAcceleration);
		ySpeed += Time2Speed(yAcceleration);
		stCircle.fAlpha += 7.f * PI / 16.f;
		xPos = stCircle.xCenter + stCircle.fRadius * cos(stCircle.fAlpha);
		yPos = stCircle.yCenter + stCircle.fRadius * sin(stCircle.fAlpha);
		break;
	default:
		bMoved = false;
	}
     //          直线运动到目的点        从空中掉下         从当前点掉下                空中下落跟踪                   快速移动　
	if (eMove == Move_LineDes || eMove == Move_Drop || eMove == Move_DropHere || eMove == Move_DropFollow || eMove == Move_MorePower)
	{
		bool bStop = false;
		if (fabs(xSpeed) >= fabs(ySpeed))
		{
			if (xSpeed > 0 && xPos > ptDes.x)
			{
				bStop = true;
			}
			else if (xSpeed < 0 && xPos < ptDes.x)
			{
				bStop = true;
			}
		}
		else
		{
			if (ySpeed > 0 && yPos > ptDes.y)
			{
				bStop = true;
			}
			else if (ySpeed < 0 && yPos < ptDes.y)
			{
				bStop = true;
			}
		}
		if (bStop)
		{
			xPos = ptDes.x;
			yPos = ptDes.y;
			if( eMove == Move_MorePower)
			{ 
				  POINT pt;
				  pt.x = xPos;
				  pt.y = yPos;
				  stMorePower.pSender->Stop(pt);
				//stMorePower.pSender->RestoreRunAniSpeed();
			}
			xSpeed = 0;
			ySpeed = 0;
			nSpeed = 0;
			xAcceleration = 0;
			yAcceleration = 0;
			if (nLife > 1)
				SetLife(1);
		}
	}
	return nLife;
}
