#ifndef _TRADE_H_
#define _TRADE_H_

class SceneUser;

class TradeOrder
{
public:
	enum {
		WIDTH = 8,
		HEIGHT = 3
	};
	
	TradeOrder(SceneUser* owner);
	~TradeOrder();

	SceneUser* target() const;

	void ready(SceneUser* target);
	bool canRequest();
	bool canAnswer();
	void begin();
	bool hasBegin();
	bool commit();
	void rollback();
	bool hasCommit();
	void finish();
	void cancel();		
	void reset();

	bool can_trade();
	void trade();
	
	void add(zObject* ob);
	void add_money(DWORD money);
	
	void remove(DWORD id);
	void clear();
			
	bool in_trade(zObject* ob) const;

private:
	
	std::map<DWORD, zObject*> _items;

	bool begined;
	bool commited;
	
	SceneUser * _me;
	SceneUser * _target;
	DWORD _targetid;

	time_t lastmove;
	DWORD _money;
};

class PrivateStore
{
public:
	enum {
		WIDTH = 6,
		HEIGHT = 7,
	};

	enum STEP {
		NONE = 0,
		START = 1,
		BEGIN = 2,
	} ;
	
	class SellInfo
	{
	public:
		SellInfo(zObject* ob=NULL, DWORD money=0, BYTE x=0, BYTE y=0) : _ob(ob), _money(money),  _x(x), _y(y)
		{ }

		zObject* object() {return _ob;}
		DWORD money() {return _money;}
		BYTE x() { return _x;}
		BYTE y() {return _y;}

	private:
		zObject* _ob;
		DWORD _money;
		BYTE _x;
		BYTE _y;
	};
		
	PrivateStore();
	~PrivateStore();

	void step(STEP step_, SceneUser *pUser);
	STEP step();

	void add(zObject* ob, DWORD money, BYTE x, BYTE y);
	void remove(DWORD id);

	void show(SceneUser* target);

	SellInfo* sell_ob(DWORD id);
private:
	
	void clear();
	
	STEP _step;

	std::map<DWORD, SellInfo> _items;
};

class RepairCost : public PackageCallback
{
public:
	RepairCost() : _cost(0)
	{ }

	bool exec(zObject* ob);
	
	
	DWORD cost() const
	{
		return _cost;
	}
	
private:
	DWORD _cost;
};

class RepairEquip : public PackageCallback
{
public:
	RepairEquip(SceneUser* user)  : _user(user)
	{ }

	bool exec(zObject* ob);
	
private:
	SceneUser* _user;
	
};

class RepairEquipUseGold : public PackageCallback
{
public:
	RepairEquipUseGold(SceneUser* user)  : _user(user)
	{ }

	bool exec(zObject* ob);
private:
	SceneUser* _user;
	
};


#endif
