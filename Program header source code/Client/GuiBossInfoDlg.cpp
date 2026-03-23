
#include "public.h"
#include "miniusercommand.h"
#include "./GameGuiManager.h"
#include "./GuiBossInfoDlg.h"
#include "../gui/include/GuiManager.h"
#include "./Game.h"
#include "LordStrikeManager.h"
#include "LordStrikeLobby.h"
#include "SafetyUser.h"
#include "GameScene.h"
#include "./MainCharacter.h"
#include "./character.h"
#include <iostream>
#include <ctime>
#include <string>

///////////////////////////////////////////////////////////////////////////////
#define ID_BUTTON_CLOSE    1  //关闭按钮
#define ID_BUTTON_REFRESH   99  //刷新按钮

#define ID_BUTTON_PUAGE_UP     801  //上一页
#define ID_BUTTON_PUAGE_DOWN     802  //下一页
#define ID_STATIC_PUAGE_TXT      803  //当前页数
///////////////////////////////////////////////////////////////////////////////

CGuiBossInfoDlg::CGuiBossInfoDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}
 
CGuiBossInfoDlg::~CGuiBossInfoDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

void CGuiBossInfoDlg::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	Puange = 1;
	GotoOK = 0;
	NaviGationBar = 0;
    MaxPuange=1;
	for (int i = 0; i < 3; i++)
	{
		BgImgae[i] = GetImage(101+i);      //背景框
		IconImage[i] = GetImage(111+i);    //已击杀位置
        IconFace[i] = GetImage(121+i);     //头像框
		StrName[i] = GetStatic(131+i);     //怪物名字
        StrMapName[i] = GetStatic(171+i);  //地图名字
        BossState[i] = GetStatic(181+i);   //怪物名字
        BossTexts[i] = GetStatic(161+i);   //BOSS说明

	}
	GetButton(ID_BUTTON_PUAGE_UP);
	GetButton(ID_BUTTON_PUAGE_DOWN);
	GetStatic(ID_STATIC_PUAGE_TXT);
    GetButton(ID_BUTTON_REFRESH);

	m_pTab = GetTab(5);
	m_pTab->AddItem("",NULL); //0  中立区
	m_pTab->AddItem("",NULL); //1  本国
    m_pTab->AddItem("",NULL); //2  天机陵 
    m_pTab->AddItem("",NULL); //3  海岛
	m_pTab->AddItem("",NULL); //4  寻仙之地    
    m_pTab->AddItem("",NULL); //5  龙之谷
    m_pTab->AddItem("",NULL); //6  异次元空间

	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			m_pTab->AddControl(i,(CGuiControl*)GetImage(101+j));
			m_pTab->AddControl(i,(CGuiControl*)GetImage(111+j)); //已击杀位置
			m_pTab->AddControl(i,(CGuiControl*)GetImage(121+j));
			m_pTab->AddControl(i,(CGuiControl*)GetStatic(131+j));
            m_pTab->AddControl(i,(CGuiControl*)GetStatic(141+j));
            m_pTab->AddControl(i,(CGuiControl*)GetStatic(151+j));
            m_pTab->AddControl(i,(CGuiControl*)GetStatic(161+j));
			m_pTab->AddControl(i,(CGuiControl*)GetStatic(171+j));
            m_pTab->AddControl(i,(CGuiControl*)GetStatic(181+j));
		}
	}
	m_pTab->SetVisible(false);
    OnGuiEvent(EVENT_BUTTON_CLICKED,20,GetButton(20));

	FUNCTION_END;
}

bool CGuiBossInfoDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
		    case ID_BUTTON_CLOSE:
			{
				Close();
			}
			break;
			case 20://中立区
			{
				m_pTab->SetCurItem(0);
				Puange = Puange_Info1;
				UpData();
			}	
			break;
			case 21://本国
			{
				m_pTab->SetCurItem(1);
				Puange = Puange_Info1;
				UpData();
			}	
			break;
            case 22://中立区·天机陵
			{
				m_pTab->SetCurItem(2);
				Puange = Puange_Info1;
				UpData();
			}	
			break;
            case 23://中立区·海岛
			{
				m_pTab->SetCurItem(3);
				Puange = Puange_Info1;
				UpData();
			}	
			break;
			case 24://中立区·寻仙之岛
			{
				m_pTab->SetCurItem(4);
				Puange = Puange_Info1;
				UpData();
			}	
			break;
            case 25://中立区·龙之谷
			{
				m_pTab->SetCurItem(5);
				Puange = Puange_Info1;
				UpData();
			}	
			break;
            case 26://中立区·异次元空间
			{
				m_pTab->SetCurItem(6);
	            Puange = Puange_Info1;
				UpData();
			}	
			break;
			case ID_BUTTON_PUAGE_UP:
			{
				if(Puange == Puange_Info1 ) return false;

				if(Puange > Puange_Info1)
				{
					Puange--;
					UpData();
				}
			}
			break;
			case ID_BUTTON_PUAGE_DOWN:
			{
				if(Puange == MaxPuange ) return false;
				
				if(Puange < MaxPuange)
				{
					Puange++;
					UpData();
				}
			}
			break;
            case ID_BUTTON_REFRESH:
			{
                //刷新BOSS存活信息
				stReqKillerBossInfoUserCmd cmd; //sky 请求指令
                SEND_USER_CMD(cmd);	
			}
			break;	
		}
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}
const char* StateName[] = {"存活","死亡"};

std::string QWordToDateTime(uint64_t timeInMillis) 
{
	std::string result;
    char temp[512];
    // 将纳秒转换为秒
    uint64_t timeInSeconds = timeInMillis / 1000;
    // 使用 time_t 转换为可读的时间
    time_t rawTime = static_cast<time_t>(timeInSeconds);
    struct tm *timeInfo = localtime(&rawTime);
	//复活时间显示
	sprintf(temp,"%d:%d:%d:",timeInfo->tm_hour,timeInfo->tm_min,timeInfo->tm_sec);
    result = temp;
	return result;
}
 

void CGuiBossInfoDlg::UpData()
{
    for (int i = 0; i < 3; i++)
    {
        BgImgae[i]->SetVisible(false);
        IconImage[i]->SetVisible(false);
        IconFace[i]->SetVisible(false);
        StrName[i]->SetVisible(false);
		GetStatic(141+i)->SetVisible(false);
		GetStatic(151+i)->SetVisible(false);
        StrMapName[i]->SetVisible(false);
        BossState[i]->SetVisible(false);
        BossTexts[i]->SetVisible(false);
    }
	int m_pTabCurItem = m_pTab->GetCurItem();
    int Page = 0 ,MaxYe=0;
   
    char temp[512];
    for (int j = 0; j < 64; j++)
    {	
        if(m_pTabCurItem +1 == m_NpcBossInfo[j].TypeID) 
        {
            if(m_NpcBossInfo[j].Pagenum > 0)
            {
                Page++; 
            }

            if(Puange == m_NpcBossInfo[j].Pagenum) 
            {
                for (int i = 0; i < 3; i++)
                { 
                    if(i == m_NpcBossInfo[j].Number) 
                    {
                        BgImgae[i]->SetVisible(true);
                        //IconImage[i]->SetVisible(true);
                        IconFace[i]->SetVisible(true);
                        StrName[i]->SetVisible(true);
						GetStatic(141+i)->SetVisible(true);
		                GetStatic(151+i)->SetVisible(true);
                        StrMapName[i]->SetVisible(true);
                        BossState[i]->SetVisible(true);
                        BossTexts[i]->SetVisible(true);
                        //输出本国BOSS和中立区BOSS
                        if((m_NpcBossInfo[j].CountryID == GetScene()->GetMainCharacter()->GetProperty()->country) || (m_NpcBossInfo[j].CountryID == 6)) //比对国家和角色国家是否一致
                        {  
                            DWORD NpcID = m_NpcBossInfo[j].NpcID; 
                            NpcBase_t *pNpc = GetNpcBase(NpcID);
                            if(pNpc != NULL)
                            {
                                IconFace[i]->SetImage(stResourceLocation("data\\icons.gl",4,pNpc->dwPic));
								//BOSS名字
                                sprintf(temp,"%s",pNpc->strName);
                                StrName[i]->SetText(temp);
                                //BOSS出现地图
                                sprintf(temp,"%s",m_NpcBossInfo[j].MapName);
                                StrMapName[i]->SetText(temp);
								StrMapName[i]->SetTextColor(COLOR_ARGB(255,0,255,0));
                                int RevState = m_NpcBossInfo[j].NpcState;
                                sprintf(temp,"%s",StateName[RevState]);//{"存活","死亡"};
                                BossState[i]->SetText(temp);
								if(RevState == 0)
								{
                                   BossState[i]->SetTextColor(COLOR_ARGB(255,0,255,0));
								}
								else
								{
								   BossState[i]->SetTextColor(COLOR_ARGB(255,159,159,159));
								}
								if(m_NpcBossInfo[j].ReviveTime != 0 )
								{
                                    sprintf(temp,"下次复活时间:%s",QWordToDateTime(m_NpcBossInfo[j].ReviveTime).c_str());
									IconImage[i]->SetVisible(true);
								}
								else
								{
                                    sprintf(temp,"复活时间:已复活");
									IconImage[i]->SetVisible(false);
								}
								BossTexts[i]->SetText(temp);	

                            }
                            else
                            {
                                char ShuoM[256];
                                IconFace[i]->SetImage(stResourceLocation("data\\icons.gl",4,2925));
                                sprintf(ShuoM,"尽情期待");
                                StrName[i]->SetText(ShuoM);
                            }
                        }
                    }
                }    
            }
        }
    }
    //char atemsp[255];
    //sprintf(atemsp,"m_pTabCurItem=%d,Page=%d",m_pTabCurItem,Page);
    //GetGameGuiManager()->AddClientSystemMessage(atemsp);
    //循环页号
    for (int x = 1; x <= Page; x++) 
    {
        if(x % 3 == 0)
        {
            MaxYe++;
        }  
    }
    MaxPuange = 1+MaxYe;
    //soke 输出页数
    char temps1[128];
    sprintf(temps1,"%d/%d",Puange,MaxPuange);
	GetStatic(ID_STATIC_PUAGE_TXT)->SetText(temps1);     
}

HRESULT CGuiBossInfoDlg::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
	CMainCharacter* pMainChar = GetScene()->GetMainCharacter();
	if(pMainChar)
	{
		int nLevel = pMainChar->GetLevel();
		if(nLevel >=60) //等级
		{
			GetButton(25)->SetVisible(true);
	        GetButton(26)->SetVisible(true);
		}
		else
		{
			GetButton(25)->SetVisible(false);
	        GetButton(26)->SetVisible(false);
		}
	}
	return hr;

	FUNCTION_END;
}

void CGuiBossInfoDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiBossInfoDlg = NULL;

	FUNCTION_END;
}
