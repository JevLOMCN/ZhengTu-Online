# $Id: Makefile 6498 2006-05-12 02:29:28Z zjw $

CXX = /usr/bin/g++
CXXFLAGS = $(ZEBRA_COMPILE_OPT) -D_USE_EPOLL_ -D_USE_CMD_NAMESPACE -D_ENCDEC_MSG -D_WQH_DEBUG -D_ALL_SUPER_GM -I../base -I../zise -Wall -D_GNU_SOURCE -D_REENTRANT `mysql_config --cflags` `xml2-config --cflags`
CXXLFLAGS = $(ZEBRA_COMPILE_OPT) -Wall -L../base -lzebra -lencdec -lpthread -lz `mysql_config --libs_r` `xml2-config --libs` -llog4cxx -L/usr/local/lib -lgd -ljpeg
LIB_ZEBRA = ../base/libzebra.a

OBJS = GameConfigMgrX.o RecommendManager.o EmperorForbid.o CDareEmperor.o Ally.o Gift.o CDareSept.o CDareSeptNpc.o CGem.o CArmy.o CVote.o CartoonPetService.o AuctionService.o MailService.o SessionChat.o SessionServer.o SessionTask.o Session.o SessionManager.o SessionTaskManager.o CUnion.o CRelationManager.o SchoolManager.o CSept.o OfflineMessage.o TempArchive.o Team.o RecordClient.o CDare.o CQuiz.o TimeTick.o CNpcDare.o CCityManager.o CCountryManager.o CDareRecordManager.o CSort.o CHero.o

SRCS = $(OBJS:%.o=%.cpp)
DEPS = $(OBJS:%.o=.%.d)

ALL_TARGETS = SessionServer

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

