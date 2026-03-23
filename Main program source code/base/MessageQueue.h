
#ifndef _MESSAGEQUEUE_H_
#define _MESSAGEQUEUE_H_
#include "zNullCmd.h"
#include "Zebra.h"
#include "zTime.h"
#include "zMisc.h"
#include "zSocket.h"
class MessageQueue
{
	protected:
		virtual ~MessageQueue(){};
	public:
		bool msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen)
		{
			return cmdQueue.put((void*)ptNullCmd , cmdLen);
		}
		virtual bool cmdMsgParse(const Cmd::t_NullCmd *, const unsigned int)=0;
		bool doCmd()
		{
			CmdPair *cmd = cmdQueue.get();
			while(cmd)
			{
				cmdMsgParse((const Cmd::t_NullCmd *)cmd->second , cmd->first);
				cmdQueue.erase();
				cmd = cmdQueue.get();
			}
			if(cmd)
			{
				cmdQueue.erase();
			}
			return true;
		}

	private:
		MsgQueue<> cmdQueue;
};
#endif
