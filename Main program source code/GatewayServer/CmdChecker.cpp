/**
 * \file
 * \version	$Id: CmdChecker.cpp  $
 * \author	
 * \date	
 * \brief	Ö¸Áî¼ì²â
 * 
 */

#include "CmdChecker.h"

#include "Command.h"

/*
CheckerTable* CheckerTable::_instance = NULL;

CheckerTable& CheckerTable::instance()
{
	if (!_instance) {
		static CheckerTable new_instance;
		new_instance.init();
		_instance = &new_instance;
	}	
	
	return *_instance;
}
*/

CheckerTable::CheckerTable()
{
	init();	
}

CheckerTable::~CheckerTable()
{
	std::for_each(_checkers.begin(), _checkers.end(), FreeMemory());
}

bool CheckerTable::init()
{

	Checker* magic_checker = new Checker; 
	magic_checker->add(Cmd::MAGIC_USERCMD);
	
	_checkers[Cmd::MAGIC_USERCMD] = magic_checker;
	
	
	MoveChecker* move_checker = new MoveChecker; 
	move_checker->add(Cmd::MOVE_USERCMD);
	
	_checkers[Cmd::MOVE_USERCMD] = move_checker;
		
	return true;
}

bool CheckerTable::check(int cmd, const zRTime& current) const
{
	const_iterator it = _checkers.find(cmd);
	if ( it!= _checkers.end()) {
		return it->second->check(cmd, current);
	}
	
	return true;	
}
