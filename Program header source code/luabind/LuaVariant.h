/******************************************************************************
* Name: LuaWrapper for C++
*
* Author: Yong Lin
* Date: 2004-07-19 17:15
* E-Mail: ly4cn@21cn.com
*
* Copyright (C) 2004 Yong Lin.  All rights reserved.
* HomePage：第二人生 http://www.d2-life.com
*　　　http://www.d2-life.com/LBS/blogview.asp?logID=41
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

#pragma once
#include <string>
#include <map>

enum LuaVariantType
{
	LUAVARIANTTYPE_NIL = 0,
	LUAVARIANTTYPE_STR = 1,
	LUAVARIANTTYPE_NUM = 2,
	LUAVARIANTTYPE_BOOL = 3,
	LUAVARIANTTYPE_TABLE = 100
};

class LuaVariant;
typedef std::map<LuaVariant, LuaVariant> LuaTable;

class LuaVariant
{
	LuaVariantType type;
	std::string strvalue;
	double numvalue;
	bool boolvalue;
	LuaTable tablevalue;
public:
	LuaVariant()
	{
		Clear();
	}
	LuaVariant(bool value)
	{
		Set(value);
	}
	LuaVariant(int value)
	{
		Set(value);
	}
	LuaVariant(double value)
	{
		Set(value);
	}
	LuaVariant(const std::string& value)
	{
		Set(value);
	}
	LuaVariant(const char* value)
	{
		Set(value);
	}
	LuaVariant(const LuaTable& value)
	{
		Set(value);
	}

	void Clear()
	{
		type = LUAVARIANTTYPE_NIL;
		numvalue = 0;
		boolvalue = false;
		strvalue = "(null)";
		tablevalue.clear();
	}
	void Set(bool value)
	{
		type = LUAVARIANTTYPE_BOOL;
		numvalue = value;
		boolvalue = value;
		if(value)
			strvalue = "(true)";
		else
			strvalue = "(false)";
		tablevalue.clear();
	}
	inline void Set(int value)
	{
		Set(static_cast<double>(value));
	}
	void Set(double value)
	{
		boolvalue = true;
		numvalue = value;
		type = LUAVARIANTTYPE_NUM;
		static char buffer[_CVTBUFSIZE];
		strvalue = _gcvt(value, 12, buffer);
		tablevalue.clear();
	}

	void Set(const std::string& value)
	{
		boolvalue = true;
		strvalue = value;
		type = LUAVARIANTTYPE_STR;
		char* pos;
		numvalue = strtod(strvalue.c_str(), &pos);
		tablevalue.clear();
	}
	void Set(const char* value)
	{
		boolvalue = true;
		strvalue = value;
		type = LUAVARIANTTYPE_STR;
		char* pos;
		numvalue = strtod(value, &pos);
		tablevalue.clear();
	}
	void Set(const LuaTable& value)
	{
		boolvalue = true;
		tablevalue = value;
		type = LUAVARIANTTYPE_TABLE;
		numvalue = 0;
		strvalue = "(table)";
	}

	LuaVariantType GetType() const
	{
		return type;
	}

	bool GetValueAsBool() const
	{
		return boolvalue;
	}

	const char* GetValueAsCStr() const
	{
		return strvalue.c_str();
	}

	const std::string & GetValueAsStdStr() const
	{
		return strvalue;
	}

	double GetValueAsNum() const
	{
		return numvalue;
	}

	int GetValueAsInt() const
	{
		return static_cast<int>(numvalue);
	}

	const LuaTable& GetValueAsTable() const
	{
		return tablevalue;
	}

	LuaTable GetValueAsTable()
	{
		return tablevalue;
	}

	LuaTable& GetValueAsRefTable()
	{
		return tablevalue;
	}
};

inline bool operator < (const LuaVariant& l, const LuaVariant& r)
{
	return l.GetValueAsCStr() < r.GetValueAsCStr();
}