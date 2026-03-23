#include "zRegex.h"
#include <iostream>

int main(int argc, char *argv[])
{
	zRegex reg=zRegex();
	if(reg.compile("^(b.*f)(.*b)$"))
	{
		if(reg.match("bsfsfadfsfbb\nsfdsb"))
		{
			std::string s;
			std::cout<<reg.getSub(s,0)<<std::endl;
			std::cout<<reg.getSub(s,1)<<std::endl;
			std::cout<<reg.getSub(s,2)<<std::endl;
			std::cout<<reg.getSub(s,3)<<std::endl;
			std::cout<<reg.getSub(s,4)<<std::endl;
		}
		else
			std::cout<< reg.getError()<<std::endl;
	}
	else
		std::cout<< reg.getError()<<std::endl;
	return 0;
}
