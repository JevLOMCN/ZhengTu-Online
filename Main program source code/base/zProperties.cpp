/**
 * \file
 * \version  $Id: zProperties.cpp  $
 * \author  
 * \date 
 * \brief 实现zProperties属性关联类
 *
 * 
 */


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ext/hash_map>

#include "zProperties.h"
#include "zString.h"

unsigned int zProperties::parseCmdLine(const std::string &cmdLine)
{
	std::vector<std::string> sv;
	Zebra::stringtok(sv, cmdLine);
	for(std::vector<std::string>::const_iterator it = sv.begin(); it != sv.end(); it++)
	{
		std::vector<std::string> ssv;
		Zebra::stringtok(ssv, *it, "=", 1);
		if (ssv.size() == 2)
		{
			properties[ssv[0]] = ssv[1];
		}
	}
	return properties.size();
}

unsigned int zProperties::parseCmdLine(const char *cmdLine)
{
	std::vector<std::string> sv;
	Zebra::stringtok(sv, cmdLine);
	for(std::vector<std::string>::const_iterator it = sv.begin(); it != sv.end(); it++)
	{
		std::vector<std::string> ssv;
		Zebra::stringtok(ssv, *it, "=", 1);
		if (ssv.size() == 2)
		{
			properties[ssv[0]] = ssv[1];
		}
	}
	return properties.size();
}

