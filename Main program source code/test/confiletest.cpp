#include "zConfile.h"
#include "Zebra.h"

class myConfile:public zConfile
{
	protected:
		bool parseYour(const xmlNodePtr node)
		{
			return true;
		}
};

int main(int argc, char *argv[])
{
	myConfile c;
	const char *name="";
	Zebra::logger=new zLogger();
	c.parse(name);
	Zebra::global.dump(std::cout);
	return 0;
}
