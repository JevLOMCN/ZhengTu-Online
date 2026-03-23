#include "SceneUser.h"
#include "SceneNpcManager.h"
#include "NpcTrade.h"
#include "RebuildObject.h"
#include "CharBase.h"
#include "Command.h"
#include <string>
// 功勋竞猜 Sc功勋竞猜 功勋竞猜系统 by:醉梦
bool SceneUser::doJingcaiCmd(const Cmd::stJingcaiUserCmd *ptCmd, unsigned int cmdLen)
{
	using namespace Cmd;
	switch (ptCmd->byParam) //这里就是判断子命令了byParam
	{
		case JINGCAI_XIAZHU:
		{
			stJingcaiXiazhuCmd *cmd = (stJingcaiXiazhuCmd *)ptCmd;
			zObject *itemobj = this->packs.uom.getObjectByThisID(cmd->itemUID);
			int index = cmd->index;
			if (itemobj)
			{
				if(itemobj->data.dwNum>500)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"最多下注500个物品");
					return true;
				}
				switch (index)
				{
				case 0:
					{
						this->charbase.jingcai0_objectid = itemobj->data.dwObjectID;
						this->charbase.jingcai0_objectnum = itemobj->data.dwNum;
						this->charbase.jingcai0_objectlevel = itemobj->data.upgrade;
						this->packs.removeObject(itemobj);
						Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"下注成功");		
					}
					break;
				case 1:
					{
						this->charbase.jingcai1_objectid = itemobj->data.dwObjectID;
						this->charbase.jingcai1_objectnum = itemobj->data.dwNum;
						this->charbase.jingcai1_objectlevel = itemobj->data.upgrade;
						this->packs.removeObject(itemobj);
						Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"下注成功");		
					}
					break;
				case 2:
					{
						this->charbase.jingcai2_objectid = itemobj->data.dwObjectID;
						this->charbase.jingcai2_objectnum = itemobj->data.dwNum;
						this->charbase.jingcai2_objectlevel = itemobj->data.upgrade;
						this->packs.removeObject(itemobj);
						Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"下注成功");		
					}
					break;	
				case 3:
					{
						this->charbase.jingcai3_objectid = itemobj->data.dwObjectID;
						this->charbase.jingcai3_objectnum = itemobj->data.dwNum;
						this->charbase.jingcai3_objectlevel = itemobj->data.upgrade;
						this->packs.removeObject(itemobj);
						Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"下注成功");		
					}
					break;
				case 4:
					{
						this->charbase.jingcai4_objectid = itemobj->data.dwObjectID;
						this->charbase.jingcai4_objectnum = itemobj->data.dwNum;
						this->charbase.jingcai4_objectlevel = itemobj->data.upgrade;
						this->packs.removeObject(itemobj);
						Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"下注成功");		
					}
					break;
				case 5:
					{
						this->charbase.jingcai5_objectid = itemobj->data.dwObjectID;
						this->charbase.jingcai5_objectnum = itemobj->data.dwNum;
						this->charbase.jingcai5_objectlevel = itemobj->data.upgrade;
						this->packs.removeObject(itemobj);
						Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"下注成功");		
					}
					break;	
				case 6:
					{
						this->charbase.jingcai6_objectid = itemobj->data.dwObjectID;
						this->charbase.jingcai6_objectnum = itemobj->data.dwNum;
						this->charbase.jingcai6_objectlevel = itemobj->data.upgrade;
						this->packs.removeObject(itemobj);
						Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"下注成功");		
					}
					break;	
				case 7:
					{
						this->charbase.jingcai7_objectid = itemobj->data.dwObjectID;
						this->charbase.jingcai7_objectnum = itemobj->data.dwNum;
						this->charbase.jingcai7_objectlevel = itemobj->data.upgrade;
						this->packs.removeObject(itemobj);
						Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"下注成功");		
					}
					break;	
				case 8:
					{
						this->charbase.jingcai8_objectid = itemobj->data.dwObjectID;
						this->charbase.jingcai8_objectnum = itemobj->data.dwNum;
						this->charbase.jingcai8_objectlevel = itemobj->data.upgrade;
						this->packs.removeObject(itemobj);
						Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"下注成功");		
					}
					break;
				case 9:
					{
						this->charbase.jingcai9_objectid = itemobj->data.dwObjectID;
						this->charbase.jingcai9_objectnum = itemobj->data.dwNum;
						this->charbase.jingcai9_objectlevel = itemobj->data.upgrade;
						this->packs.removeObject(itemobj);
						Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"下注成功");		
					}
					break;
				case 10:
					{
						this->charbase.jingcai10_objectid = itemobj->data.dwObjectID;
						this->charbase.jingcai10_objectnum = itemobj->data.dwNum;
						this->charbase.jingcai10_objectlevel = itemobj->data.upgrade;
						this->packs.removeObject(itemobj);
						Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"下注成功");		
					}
					break;
				case 11:
					{
						this->charbase.jingcai11_objectid = itemobj->data.dwObjectID;
						this->charbase.jingcai11_objectnum = itemobj->data.dwNum;
						this->charbase.jingcai11_objectlevel = itemobj->data.upgrade;
						this->packs.removeObject(itemobj);
						Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"下注成功");		
					}
					break;
				default:
					break;
				}	
				this->save(Cmd::Record::OPERATION_WRITEBACK); //立刻存档
			}
		}
		break;
		case JINGCAI_INFO:
		{
			Cmd::stJingcaiReturnInfoCmd Send;
			Send.jingcai[0].index=0;
			Send.jingcai[0].object_id = this->charbase.jingcai0_objectid;
			Send.jingcai[0].object_num = this->charbase.jingcai0_objectnum;
			Send.jingcai[0].object_level =this->charbase.jingcai0_objectlevel;

			Send.jingcai[1].index=1;
			Send.jingcai[1].object_id = this->charbase.jingcai1_objectid;
			Send.jingcai[1].object_num = this->charbase.jingcai1_objectnum;
			Send.jingcai[1].object_level =this->charbase.jingcai1_objectlevel;

			Send.jingcai[2].index=2;
			Send.jingcai[2].object_id = this->charbase.jingcai2_objectid;
			Send.jingcai[2].object_num = this->charbase.jingcai2_objectnum;
			Send.jingcai[2].object_level =this->charbase.jingcai2_objectlevel;

			Send.jingcai[3].index=3;
			Send.jingcai[3].object_id = this->charbase.jingcai3_objectid;
			Send.jingcai[3].object_num = this->charbase.jingcai3_objectnum;
			Send.jingcai[3].object_level =this->charbase.jingcai3_objectlevel;

			Send.jingcai[4].index=4;
			Send.jingcai[4].object_id = this->charbase.jingcai4_objectid;
			Send.jingcai[4].object_num = this->charbase.jingcai4_objectnum;
			Send.jingcai[4].object_level =this->charbase.jingcai4_objectlevel;

			Send.jingcai[5].index=5;
			Send.jingcai[5].object_id = this->charbase.jingcai5_objectid;
			Send.jingcai[5].object_num = this->charbase.jingcai5_objectnum;
			Send.jingcai[5].object_level =this->charbase.jingcai5_objectlevel;

			Send.jingcai[6].index=6;
			Send.jingcai[6].object_id = this->charbase.jingcai6_objectid;
			Send.jingcai[6].object_num = this->charbase.jingcai6_objectnum;
			Send.jingcai[6].object_level =this->charbase.jingcai6_objectlevel;

			Send.jingcai[7].index=7;
			Send.jingcai[7].object_id = this->charbase.jingcai7_objectid;
			Send.jingcai[7].object_num = this->charbase.jingcai7_objectnum;
			Send.jingcai[7].object_level =this->charbase.jingcai7_objectlevel;

			Send.jingcai[8].index=8;
			Send.jingcai[8].object_id = this->charbase.jingcai8_objectid;
			Send.jingcai[8].object_num = this->charbase.jingcai8_objectnum;
			Send.jingcai[8].object_level =this->charbase.jingcai8_objectlevel;

			Send.jingcai[9].index=9;
			Send.jingcai[9].object_id = this->charbase.jingcai9_objectid;
			Send.jingcai[9].object_num = this->charbase.jingcai9_objectnum;
			Send.jingcai[9].object_level =this->charbase.jingcai9_objectlevel;

			Send.jingcai[10].index=10;
			Send.jingcai[10].object_id = this->charbase.jingcai10_objectid;
			Send.jingcai[10].object_num = this->charbase.jingcai10_objectnum;
			Send.jingcai[10].object_level =this->charbase.jingcai10_objectlevel;

			Send.jingcai[11].index=11;
			Send.jingcai[11].object_id = this->charbase.jingcai11_objectid;
			Send.jingcai[11].object_num = this->charbase.jingcai11_objectnum;
			Send.jingcai[11].object_level =this->charbase.jingcai11_objectlevel;
			sendCmdToMe(&Send, sizeof(Send));
		}
		break;
	}
	return true;
}
