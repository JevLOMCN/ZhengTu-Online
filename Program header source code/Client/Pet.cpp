#include "public.h"
#include ".\Pet.h"
#include "./GameScene.h"
#include ".\gameguimanager.h"
#include ".\GuiPetCtrlDialog.h"
#include "./GuiPetDialog.h"

const DWORD plusColor = COLOR_ARGB(255,0,255,0);

CPet::CPet( petType type )
{
	bDeleted = true;
	m_type = type ;
}

CPet::CPet()
{
	bDeleted = false;
	m_type = PET_TYPE_NOTPET ;		//宠物的类型
}

CPet::~CPet(void)
{
	bDeleted = true;
}


void CPet::SetAI(petAIMode ai)
{
	stSetAIPetCmd cmd;
	cmd.type = m_type;
	cmd.mode = ai;
	SEND_USER_CMD(cmd);
}

void CPet::RequestData()
{
	stRequestDataPetCmd cmd;
	cmd.type = m_type;
	SEND_USER_CMD(cmd);
}

void CPet::Rename(const char * szName)
{
	stChangeNamePetCmd cmd;
	cmd.type = m_type;
	strcpy(cmd.name,szName);

	SEND_USER_CMD(cmd);
}

void CPet::Update(stHpExpPetCmd* cmd)
{
	m_data.hp = cmd->hp;
	m_data.exp = cmd->exp;
	if(GetGameGuiManager()->m_guiPetDlg)
	{
		GetGameGuiManager()->m_guiPetDlg->UpdateContents();
	}
#ifdef _DEBUG
	char szTemp[64];
	_snprintf(szTemp,64,"stHpExpPetCmd:[hp:%d][exp:%d]",cmd->hp,cmd->exp);
	szTemp[63] = 0;
	TRACE(szTemp);
	GetGameGuiManager()->AddClientSystemMessage(szTemp,COLOR_ARGB(255,0,255,0));
#endif
}

void CPet::DelSkill(int index)
{
	if(index >= 0 && index <= 4)
	{
		stDelSkillPetCmd cmd;
		cmd.type = m_type;
		cmd.id = m_data.skills[index];
		SEND_USER_CMD(cmd);
	}	
}

void CPet::SetAttrib(t_PetData data)
{
	bDeleted = false;
	memcpy(&m_data ,&data,sizeof(t_PetData));
#if defined _DEBUG || defined RELEASET_TEST
	Engine_WriteLogF("PetName[%s],type[%d] AI[%x] HP[%d/%d]\n",data.name,m_type,data.ai,data.hp,data.maxhp);
	for(int i = 0;i < 4; ++i)
	{
		Engine_WriteLogF("Skill[%d]:%d\n",i,data.skills[i]);
	}
#endif
	if( m_type == PET_TYPE_PET || m_type == PET_TYPE_SUMMON )
		GetGameGuiManager()->AddPetFloatDialog(m_type);
	if(GetGameGuiManager()->m_guiPetDlg)
	{
		GetGameGuiManager()->m_guiPetDlg->UpdateContents();
	}
}


CNpc* CPet::GetNpc()
{
	return GetScene()->FindNpcByID(m_dwID);
}

NpcBase_t* CPet::GetNpcBase()
{
	if(bDeleted) return NULL;
	return ::GetNpcBase( m_data.id );
}

DWORD CPet::GetNameTextColor(petType type,DWORD level)
{
	switch(type)
	{
	case PET_TYPE_SUMMON:
		{
			switch(level)
			{
			case 1:
			case 2:
				return COLOR_ARGB(255,255,255,255);
				break;
			case 3:
			case 4:
				return COLOR_ARGB(255,36,112,255);
				break;
			case 5:
			case 6:
				return COLOR_ARGB(255,206,182,97);
				break;
			case 7:
			case 8:
				return COLOR_ARGB(255,0,255,137);
				break;
			case 9:
			case 10:
				return COLOR_ARGB(255,87,37,142);
				break;
			}
		}
	}
	return -1;
}

DWORD CPet::GetNameTextColor()
{
	return GetNameTextColor(m_type,GetPetLevel());
}

void CPet::AutoChangeAI()
{
	petAIMode ai = GetPetAIMode();
	if(IsSetAi(ai,PETAI_ATK_ACTIVE))	//自动攻击
	{
		SetAI(PETAI_ATK_PASSIVE);		//切换为防御攻击
		RequestData();
		return ;
	}
	if(IsSetAi(ai,PETAI_ATK_PASSIVE))	//防御攻击
	{
		SetAI(PETAI_ATK_NOATK);		//切换为休息
		RequestData();
		return ;
	}
	if(IsSetAi(ai,PETAI_ATK_NOATK))	//休息
	{
		SetAI(PETAI_ATK_ACTIVE);		//切换为自动攻击
		RequestData();
		return ;
	}
}