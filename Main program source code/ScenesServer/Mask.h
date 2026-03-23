/**
 * \file
 * \version	$Id: Mask.h  $
 * \author	
 * \date	2005-04-13
 * \brief	蒙面系统
 * 
 */
 
#ifndef __USER_MASK__
#define __USER_MASK__

#include "zTime.h"

class zObject;
class SceneUser;

/**
 * \brief 蒙面类
 *
 */
class Mask
{
public:
	enum {
		TIME_CONSUME_PERMANENCE = 10,
		ATTACKED_CONSUME_PERMANENCE = 1,
		DROP_ODDS = 720,
		ATTACK_USE_TIME = 10*1000,
	};
	
	Mask();
	~Mask();

	int on_use(SceneUser* user, zObject* ob);
	
	int on_attack(SceneUser* victim);
	int on_defence();
	
	int on_timer();
	
	int on_trade();

	int on_gem();
	
	bool is_masking() const;

	bool is_use(zObject* ob) const;
			
private:
	int drop_odds() const;
	
	int reduce_permanence();

	zRTime _last;
	bool _drop;
	
	/// 蒙面物品
	zObject* _mask;
	/// 蒙面用户
	SceneUser* _user;
};

#endif
