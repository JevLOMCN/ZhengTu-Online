#include "zIncludeXML.h"
#include <iostream>

int main(int argc , char *argv[])
{
	std::string s;
	zIncludeXML::expand("root.xml",s);
	zIncludeXML::expand(s);
	std::cout<<s<<std::endl;
	return 0;
}
