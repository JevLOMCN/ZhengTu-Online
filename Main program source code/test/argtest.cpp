#include "zArg.h"
#include "Zebra.h"

static struct argp_option myoptions[] =
{
	{"log",	'l',	"Level",	0,	"log level",	0},
	{0,		0,		0,			0,	0,				0}
};

static error_t myparse_opt(int key, char *arg, struct argp_state *state)
{
	switch (key)
	{
		case 'l':
			{
				std::string s;
				s=std::string(arg);
				//std::cout << "Log level = " << s << std::endl ;
				Zebra::global["level"]=arg;
			}
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

int main(int argc,char *argv[])
{
	zArg::getArg()->add(myoptions,myparse_opt);
	zArg::getArg()->parse(argc,argv);
	Zebra::global.dump(std::cout);
	return 0;
}
