#include "public.h"
#include "miniusercommand.h"
#include "CardPattern.h"

class m_sort
{
	public:
		int operator()(const std::pair<DWORD, Card> &p1, const std::pair<DWORD, Card> &p2)
		{
			if (p1.first==p2.first)
				return p1.second>p2.second;
			else
				return p1.first>p2.first;
		}
};
bool CardPattern::match_pattern(const DWORD &packNum, const Card *cards, const DWORD &num, CardPattern &pat)
{
	if (!packNum || !cards || !num) return false;

	for (DWORD i=0; i<num; i++)
		pat.list[cards[i]]++;

	if ((packNum==2 && num==4 && cards[0].number()>13 && cards[1].number()>13 && cards[2].number()>13 && cards[3].number()>13)
			|| (packNum==1 && num==2 && cards[0].number()>13 && cards[1].number()>13))
	{
		pat.missile = 1;
		pat._valid = true;
		return true;
	}

	//using namespace std;
	//std::sort(cards, cards+num-1);
	//CardList l;
	std::vector<std::pair<DWORD, Card> > m;
	for (card_iter it=pat.list.begin(); it!=pat.list.end(); it++)
		m.push_back(std::make_pair(it->second, it->first));
	sort(m.begin(), m.end(), m_sort());

	//for (vector<pair<DWORD, Card> >::iterator it=m.begin(); it!=m.end(); it++)
	//	Zebra::logger->debug("%u ¸ö %u", it->first, it->second.number()+2);

	pat.unitNum = m.begin()->first;
	pat.value = m.begin()->second;
	pat.serialNum = 1;

	for (DWORD i=1; i<m.size(); i++)
	{
		if (m[i].first!=m[i-1].first)
			if (pat.unitNum!=3)
				return false;
			else
				break;

		if (m[i-1].second.number()>12) return false;
		if (m[i-1].second.number()-m[i].second.number()!=1) return false;

		pat.serialNum++;
	}

	if (pat.unitNum>=4)
	{
		if (num!=pat.unitNum*pat.serialNum)
			return false;
		else
			pat.bomb = pat.serialNum==1?1:0;
	}

	if (pat.serialNum>1)
		if (5>pat.unitNum*pat.serialNum) return false;

	if (pat.unitNum==3 && pat.serialNum!=m.size())//3Ë³
	{
		if (num==pat.serialNum*4)
			pat.add = (BYTE)pat.unitNum;
		else
		{
			BYTE count = 0;
			for (DWORD i=pat.serialNum; i<m.size(); i++)
				if (m[i].first%2==0)
					count += (BYTE)(m[i].first/2);
				else
					return false;
			if (count==pat.serialNum)
				pat.add = count*2;
			else
				return false;
		}
	}

	pat._valid = true;
	return true;
}

CardPattern & CardPattern::operator=(const CardPattern &c)
{
	list = c.list;
	serialNum = c.serialNum;
	unitNum = c.unitNum;
	value = c.value;
	bomb = c.bomb;
	missile = c.missile;
	_valid = c._valid;
	add = c.add;
	return *this;
}

bool CardPattern::operator>(const CardPattern &c)
{
	if (!c.valid()) return true;

	if (c.missile) return false;
	if (missile) return true;

	if (bomb)
		if (c.bomb)
			if (unitNum==c.unitNum)
				return value>c.value;
			else
				return unitNum>c.unitNum;
		else
			return true;
	else
		if (c.bomb)
			return false;

	return unitNum==c.unitNum && serialNum==c.serialNum && add==c.add && value>c.value;
}
