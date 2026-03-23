#pragma once
 
typedef std::map<Card, DWORD> CardList;
typedef std::map<Card, DWORD>::iterator card_iter;

class CardPattern
{
	public:
		static bool match_pattern(const DWORD &packNum, const Card *cards, const DWORD &num, CardPattern &pat);

		CardList list;
		DWORD serialNum;//连续几个
		DWORD unitNum;//一组几个
		Card value;//值，按序列最大的算
		BYTE bomb;
		BYTE missile;
		BYTE add;//有几张附带牌(3顺)

		CardPattern():serialNum(1),unitNum(1),bomb(0),missile(0),add(0),_valid(false){}
		CardPattern &operator=(const CardPattern &c);
		bool operator>(const CardPattern &c);

		void clear(){_valid = false;}
		bool valid()const{return _valid;}
	private:
		bool _valid;
};

