#include "../engine/include/engine.h"
#include "public.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//////////////////////////////////////////////////////////////////////////
//event
//////////////////////////////////////////////////////////////////////////
bool stEvent::Run0() const
{
	return false;  // so nothing was done!
	if (eEvent == Event_Null || eGenerate == Event_Generate_Null)
		return false;
	if (eGenerate == Event_Generate_FrameStart && nEventFrame == 0)
		return true;
	return false;
}

bool stEvent::Run(long time_eslasped,int preFrame,int curFrame,int allframe) const
{
	if (eEvent == Event_Null || eGenerate == Event_Generate_Null)
		return false;
	if (nEventFrame < 0)
		return false;

	switch(eGenerate)
	{
	case Event_Generate_FrameEach://每隔帧数，例如每隔4帧就是0,4,8,12……
		{
			if (nEventFrame > 0)
				for(int frame=0; frame<=curFrame; frame+=nEventFrame)
				{
					if (frame!= preFrame && frame == curFrame)
						return true;				
				}
		}
		break;
	case Event_Generate_EndContinue:
		{
			if (preFrame != curFrame && curFrame >= nEventFrame)
				return true;
		}
		break;
	case Event_Generate_EndContinue2:
		{
			if (curFrame >= nEventFrame)
				return true;
		}
		break;
	case Event_Generate_EndRandom:
		{
			if (preFrame != curFrame && curFrame >= nEventFrame)
			{
				if (nEventTime <= 0 && (rand() % nEventTime) == 0)
					return true;
			}
		}
		break;
	case Event_Generate_FrameEnd:
		{
			int frame = allframe - nEventFrame;
			if (preFrame <= frame && frame < curFrame)
				return true;
		}
		break;
	case Event_Generate_FrameStart:
		{
			if (preFrame <= nEventFrame && nEventFrame < curFrame){
				//TRACE("event: preFrame %d , EventFrame %d, curFrame %d\n",preFrame,nEventFrame,curFrame);
				return true;
			}

		}
		break;
	}
	return false;
}

