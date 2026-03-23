#include "public.h"
#include ".\Pet.h"
#include "./GameScene.h"
#include ".\horse.h"
#include "./GameGuiManager.h"
#include "./GuiPetDialog.h"
#include "GameScene.h"
#include "MainCharacter.h"

CHorse::CHorse(void)
:CPet(PET_TYPE_RIDE)
{
} 

CHorse::~CHorse(void)
{

}

void CHorse::SetAttrib(const t_HorseData& data)
{
	bDeleted = false;
	memcpy(&m_horsedata ,&data,sizeof(t_HorseData));

	strcpy(m_data.name,m_horsedata.name);
	m_data.lv = m_horsedata.lv;
	m_data.id = m_horsedata.id;

	if(GetGameGuiManager()->m_guiPetDlg)
		GetGameGuiManager()->m_guiPetDlg->UpdateContents();
}

void CHorse::Follow()
{
	if ( GetScene()->GetMainCharacter() && 
		!(GetScene()->GetMainCharacter()->IsMoving()) )
	{
		stCallHorsePetCmd cmd;
		SEND_USER_CMD(cmd);
	}	
}

void CHorse::Putup()
{
	if ( GetScene()->GetMainCharacter() && 
		!(GetScene()->GetMainCharacter()->IsMoving()) )
	{
		stPutHorsePetCmd cmd;
		SEND_USER_CMD(cmd);
	}
}