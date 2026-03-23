#include "SceneUser.h"
#include "SceneNpcManager.h"
#include "NpcTrade.h"
#include "RebuildObject.h"
#include "CharBase.h"
#include <string>

bool SceneUser::doHorseTrainingCmd(const Cmd::stHorseUserCmd *ptCmd,unsigned int cmdLen)
{
	if (!horse.horse())
	{
		Channel::sendSys(this,Cmd::INFO_TYPE_FAIL,"你还没有马匹!");
		return true;
	}
	
	if (horse.data.lv < 100 )
	{
		Channel::sendSys(this,Cmd::INFO_TYPE_FAIL,"只有100级以上的马才可以接受训练");
		return true;
	}
					
	using namespace Cmd;
	switch(ptCmd->byParam)
	{
	case HORSETRAINING_USERCMD_TEY:
		{
			stHorseTrainingUserCmd * pCmd = (stHorseTrainingUserCmd*)ptCmd;

			switch(pCmd->CmdTey)
			{
			case 0:	//马匹训练1
				{
					if (!packs.checkMoney( 1000 ) || !packs.removeMoney(1000,"二级马匹训练"))
					{
						Channel::sendSys(this,Cmd::INFO_TYPE_FAIL,"小样,没那么多钱就别来找我！");
						return true;
					}

					if( horse.data.horseXLlevel > 1 )
					{
						Channel::sendSys(this,Cmd::INFO_TYPE_FAIL,"请先清除当前战马训练效果");
						return true;
					}
					else
					{
						if( horse.data.horseXLlevel == 1 )
						{
							horse.data.speed	-= 10;
						}
						else if( horse.data.horseXLlevel == 2 )
						{
							horse.data.speed	-= 20;
						}
						else if( horse.data.horseXLlevel == 3 )
						{
							horse.data.speed	-= 30;
						}
						horse.data.horseXLlevel = 0;
					}

					horse.data.horseXLlevel = 1;
					horse.data.horseXLtime	+= 86400;
					horse.data.speed		+= 10;
					horse.mount(false);
					horse.sendData();
					Channel::sendSys(this,Cmd::INFO_TYPE_GAME,"你的战马额外增加10移动速度");
				}
				break;
			case 1:	//马匹训练2
				{
					if (!packs.checkMoney( 2000 ) || !packs.removeMoney(2000,"二级马匹训练"))
					{
						Channel::sendSys(this,Cmd::INFO_TYPE_FAIL,"小样,没那么多钱就别来找我！");
						return true;
					}

					if( horse.data.horseXLlevel == 1 || (horse.data.horseXLlevel > 2 && horse.data.horseXLlevel < 5) )
					{
						Channel::sendSys(this,Cmd::INFO_TYPE_FAIL,"请先清除当前战马训练效果");
						return true;
					}
					else
					{
						if( horse.data.horseXLlevel == 1 )
						{
							horse.data.speed	-= 10;
						}
						else if( horse.data.horseXLlevel == 2 )
						{
							horse.data.speed	-= 20;
						}
						else if( horse.data.horseXLlevel == 3 )
						{
							horse.data.speed	-= 30;
						}
						horse.data.horseXLlevel = 0;
					}

					horse.data.horseXLlevel = 2;
					horse.data.horseXLtime	+= 86400;
					horse.data.speed		+= 20;
					horse.mount(false);
					horse.sendData();
					Channel::sendSys(this,Cmd::INFO_TYPE_GAME,"你的战马额外增加20移动速度");
				}
				break;
			case 2:	//马匹训练3
				{
					if (!packs.checkMoney( 5000 ) || !packs.removeMoney(5000,"三级马匹训练"))
					{
						Channel::sendSys(this,Cmd::INFO_TYPE_FAIL,"小样,没那么多钱就别来找我！");
						return true;
					}

					if( (horse.data.horseXLlevel > 0 && horse.data.horseXLlevel < 3) || horse.data.horseXLlevel == 4 )
					{
						Channel::sendSys(this,Cmd::INFO_TYPE_FAIL,"请先清除当前战马训练效果");
						return true;
					}
					else
					{
						if( horse.data.horseXLlevel == 1 )
						{
							horse.data.speed	-= 10;
						}
						else if( horse.data.horseXLlevel == 2 )
						{
							horse.data.speed	-= 20;
						}
						else if( horse.data.horseXLlevel == 3 )
						{
							horse.data.speed	-= 30;
						}
						horse.data.horseXLlevel = 0;
					}

					horse.data.horseXLlevel = 3;
					horse.data.horseXLtime	+= 86400;
					horse.data.speed		+= 30;
					horse.mount(false);
					horse.sendData();
					Channel::sendSys(this,Cmd::INFO_TYPE_GAME,"你的战马额外增加30移动速度");
				}
				break;
			case 4:	//清除训练
				{
					if( horse.data.horseXLlevel > 0 )
					{
					   horse.data.horseXLtime	= 0;
					   if( horse.data.horseXLlevel == 1 )
					   {
						   horse.data.speed	-= 10;
					   }
					   else if( horse.data.horseXLlevel == 2 )
					   {
						   horse.data.speed	-= 20;
					   }
					   else if( horse.data.horseXLlevel == 3 )
					   {
						   horse.data.speed	-= 30;
					   }
					   horse.data.horseXLlevel = 0;
					   horse.mount(false);
					   horse.sendData();
					   Channel::sendSys(this,Cmd::INFO_TYPE_GAME,"你的战马训练效果消失了");
					}
					else
					{
					   Channel::sendSys(this,Cmd::INFO_TYPE_GAME,"你的战马没有训练效果");
					   return true;
					}
				}
				break;
			default:
				{
					return true;
				}
				break;
			}
		}
	}
	return true;
}

