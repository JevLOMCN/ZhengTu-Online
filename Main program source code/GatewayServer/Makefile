# $Id: Makefile 6515 2006-05-14 06:03:00Z xwl $

CXX = /usr/bin/g++
CXXFLAGS = $(ZEBRA_COMPILE_OPT) -D_USE_EPOLL_ -D_USE_CMD_NAMESPACE -D_ENCDEC_MSG -D_WQH_DEBUG -D_ALL_SUPER_GM -I../base -I../zise -Wall -D_GNU_SOURCE -D_REENTRANT `mysql_config --cflags` `xml2-config --cflags`
CXXLFLAGS = $(ZEBRA_COMPILE_OPT) -Wall -L../base -lzebra -lencdec -lpthread -lz `mysql_config --libs_r` `xml2-config --libs` -llog4cxx -L/usr/local/lib -lgd -ljpeg
LIB_ZEBRA = ../base/libzebra.a

OBJS = MiniClient.o SceneClientManager.o ScreenIndex.o CountryInfo.o GatewayServer.o GatewayTask.o GateUser.o GateUserManager.o SessionClient.o SceneClient.o LoginSessionManager.o RecordClient.o BillClient.o GatewayTaskManager.o TimeTick.o CmdChecker.o
SRCS = $(OBJS:%.o=%.cpp)
DEPS = $(OBJS:%.o=.%.d)

ALL_TARGETS = GatewayServer

all: $(ALL_TARGETS)

include $(DEPS)
$(DEPS): .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$

$(OBJS): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(ALL_TARGETS): $(OBJS) $(LIB_ZEBRA)
	$(CXX) $(OBJS) -o $@ $(CXXLFLAGS)

clean:
	@rm -rf *.o .*.d $(ALL_TARGETS) core *~

