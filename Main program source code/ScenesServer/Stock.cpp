#include "SceneUser.h"
#include "BillCommand.h"





bool SceneUser::doStockCmd(const Cmd::stStockSceneUserCmd *ptCmd,unsigned int cmdLen)
{
	using namespace Cmd;
	using namespace Cmd::Bill;
	switch(ptCmd->byParam)
	{
		case TRANSFER_FUND_SAVE_STOCKPARA:
			{
				stTransferFundStockSaveUserCmd *rev = (stTransferFundStockSaveUserCmd *)ptCmd;
				if(!this->packs.checkGold(rev->dwGold) || !this->packs.checkMoney(rev->dwMoney))
				{
					Zebra::logger->debug("%s(%d)股票冲值时发现金额不够,可能是外挂",this->name,this->id);
				}
				else
				{
					this->stockSave(rev->dwMoney,rev->dwGold);
				}
				return true;
			}
			break;
		default:
			break;
	}
	return false;
}
bool SceneUser::stockSave(DWORD mon, DWORD go)
{
	DWORD realmon=0,realgo=0;
	bool bret=false;
	if(mon)
	{
		if(this->packs.removeMoney(mon,"股票冲值"))
		{
			realmon=mon;
			bret=true;
		}
	}
	if((!mon && !bret && go) ||(mon && bret && go))
	{
		if(this->packs.removeGold(go,"股票冲值",false))
		{
			realgo=go;
			bret=true;
		}
	}
	if(bret)
	{
		Cmd::Bill::t_Stock_Save save;
		save.dwMoney=realmon;
		save.dwGold=realgo;
		this->sendSceneCmdToBill(&save,sizeof(save));
		Zebra::logger->debug("%s(%d)股票冲值成功,金币:%d,银币:%d",this->name,this->id,go,mon);
	}
	else
	{
		Zebra::logger->debug("%s(%d)股票冲值失败,金币:%d,银币:%d",this->name,this->id,go,mon);
	}
	return bret;
}
