/**
 * \file
 * \version	$Id: CmdChecker.h  $
 * \author	
 * \date	
 * \brief	÷∏¡ÓºÏ≤‚
 * 
 */

#ifndef __CMD_CHECKER__
#define __CMD_CHECKER__

#include "zTime.h"
#include "Zebra.h"

#include <map>
#include <set>

class CheckInfo
{
public:
	CheckInfo() : _previous(0), _last(0), _packets(0)
	{ }
	
	zRTime& previous()
	{
		return _previous;
	}

	void previous(const zRTime& time)
	{
		_previous = time;	
	}

	
	zRTime& last()
	{
		return _last;
	}
	
	void last(const zRTime& time)
	{
		_last = time;	
	}
	
	int packets() const
	{
		return _packets;
	}
	
	void packets(int packets_)
	{
		_packets = packets_;	
	}
	
private:
	zRTime _previous;
	zRTime _last;
	int _packets;
};

template <int interval, int count, int percent>
class percent_up_off
{
public:
	enum {
		MIN_INTERVAL = static_cast<int>(interval*(1.0-percent/100.0)),
		MAX_INTERVAL = static_cast<int>(interval*(1.0+percent/100.0)),	
		COUNT_TIME = interval*count,
	};
	
	static bool check_interval(const zRTime& current, CheckInfo& check)
	{
		//Zebra::logger->debug("previous:%ld, current:%ld, interval:%d, must_interval:%d", check.previous().msecs(), current.msecs(), check.previous().elapse(current), MIN_INTERVAL);
		if ( check.previous().elapse(current) >= MIN_INTERVAL ) {
			check.previous(current);
			return true;
		}

		return false;
	}
	
	static bool check_count(const zRTime& current, CheckInfo& check)
	{
		//not reached 
		int packets = check.packets();
		if ( packets < count) {
			check.packets(++packets);
			return true;
		}
		//time ok
		//Zebra::logger->debug("last:%ld, current:%ld, elpase:%d, must_elpase:%d", check.last().msecs(), current.msecs(), check.last().elapse(current), COUNT_TIME);
		if ( check.last().elapse(current) >= COUNT_TIME ) {
			check.packets(0);
			check.last(current);
			return true;
		}
		
		return false;
	}
};

template<typename I = int>
class ICmdChecker
{
public:
	virtual bool check(I cmd, const zRTime& current) = 0;
//	virtual void add(I cmd) = 0;	
	virtual ~ICmdChecker() { }
};

template <typename O, typename I = int>
class CmdChecker : public ICmdChecker<I>
{
public:	
/*
	typedef CmdChecker<O, I> self_t;
	static self_t& instance()
	{
		if (!_instance) {
			static self_t new_instance;
			_instance = new_instance;
		}
		return *_instance;
	}
*/	
	bool check(I cmd, const zRTime& current)
	{
		iterator it = _cmds.find(cmd);
		if (it != _cmds.end()) {
			return do_check(current);
		}
		
		return true;
	}
	
	void add(I cmd)
	{
		_cmds.insert(cmd);		
	}
	
	CmdChecker()
	{
		
	}

	~CmdChecker()
	{
		
	}

private:
	bool do_check(const zRTime& current)
	{
		return O::check_interval(current, _check) && O::check_count(current, _check); 
	}

	//static self_t* _instance;

	typedef std::set<I> set;
	typedef typename set::iterator iterator;
	set _cmds; 

	CheckInfo _check;
	
};

/*
template <typename O, typename I>
self_t* CmdChecker<O, I>::_instance = NULL;
*/

class CheckerTable
{
public:	
//	static CheckerTable& instance();
	
	bool check(int cmd, const zRTime& current) const;

	CheckerTable();
	~CheckerTable();
	
private:
	
	bool init();

	class FreeMemory 
	{
	public:
		template <typename T>
		void operator () (T target)
		{
			SAFE_DELETE(target.second);
		}	
	};
	
//	static CheckerTable* _instance;	
	
	typedef CmdChecker< percent_up_off<512, 10, 5> > Checker;
	typedef CmdChecker< percent_up_off<512, 10, 5> > MoveChecker;

//	typedef std::vector< ICmdChecker<int>* > CHECK;

	typedef std::map< int, ICmdChecker<int>* > CHECK;
	typedef CHECK::const_iterator const_iterator;

	CHECK _checkers; 

};

#endif
