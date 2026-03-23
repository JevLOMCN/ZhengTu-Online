/**
 * \file
 * \version  $Id: MagicRange.h  $
 * \author  
 * \date 
 * \brief 群攻技能范围定义
 *
 * 
 */


#ifndef _MagicRange_h_
#define _MagicRange_h_

#include <vector>
#include <ext/hash_map>
#include "zType.h"
#include "zRWLock.h"
#include "zSceneEntry.h"

#define magicrange_hash(type , dir) ((type * 2) + dir)
#define MAX_RELATIVE_POS 20
/**
 * \brief 相对坐标数据结构
 *
 */
struct RelativePos : public zPos
{
	DWORD w; /// 伤害损耗
	RelativePos()
	{
	}
	zPos getAbsolutePos(const zPos &center , DWORD dir);
};
typedef std::vector<RelativePos> MagicRange;
typedef struct {
	MagicRange lib;
	DWORD num;
} SMagicRange;
/**
 * \brief 技能格字回调
 *
 */
struct MagicPosExec
{
	virtual ~MagicPosExec(){};
	virtual bool exec(zPos &pos) = 0;
};
/**
 * \brief 技能作用点
 *
 */
struct MagicPos
{
	MagicPos(const DWORD type , const DWORD dir);
	void execEvery(MagicPosExec &pos);
	private:
		SMagicRange range;
};

/**
 * \brief 初始化技能群攻类型范围
 *
 */
class MagicRangeInit
{

	public:

		~MagicRangeInit()
		{
			final();
		}

		static MagicRangeInit &getInstance()
		{
			if (NULL == instance)
				instance = new MagicRangeInit();

			return *instance;
		}

		static void delInstance()
		{
			SAFE_DELETE(instance);
		}

		bool init();
		void get(const DWORD type , const DWORD dir , SMagicRange &range);
		void final();

	private:

		static MagicRangeInit *instance;

		MagicRangeInit() {};

		typedef __gnu_cxx::hash_map<DWORD, SMagicRange> MagicRangeContainer;
		zRWLock rwlock;
		MagicRangeContainer ranges;

};

#endif

