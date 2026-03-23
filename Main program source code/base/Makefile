# $Id: Makefile 6739 2006-06-05 07:52:38Z song $
#include des/Makefile

CXX = /usr/bin/g++
CXXFLAGS = $(ZEBRA_COMPILE_OPT) -I. -Wall -D_GNU_SOURCE -D_USE_CMD_NAMESPACE -D_REENTRANT `mysql_config --cflags` `xml2-config --cflags`
CXXLFLAGS = $(ZEBRA_COMPILE_OPT) -Wall -lpthread -lencdec -lz `mysql_config --libs_r` `xml2-config --libs` -llog4cxx -L/usr/local/lib -lgd -ljpeg

OBJS = zMysqlDBConnPool.o zXMLParser.o Zebra.o zArg.o zSocket.o zTCPClient.o zTCPClientTask.o zTCPClientTaskPool.o zTCPServer.o zMTCPServer.o zThread.o zTCPTask.o zTCPTaskPool.o zHttpTask.o zHttpTaskPool.o zLogger.o zTimer.o zTime.o zService.o zNetService.o zMNetService.o zSubNetService.o zRegex.o zProperties.o zIncludeXML.o zConfile.o zDatabaseManager.o zBase64.o zUrl.o zJpeg.o zMetaData.o zDatabase.o
LIB_ZEBRA = libzebra.a
SRCS = $(OBJS:%.o=%.cpp)
DEPS = $(OBJS:%.o=.%.d)

all: $(LIB_ZEBRA)

include $(DEPS)
$(DEPS): .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$

$(OBJS): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(LIB_ZEBRA): $(OBJS) 
	@rm -rf $(LIB_ZEBRA)
	@ar cru $(LIB_ZEBRA) $(OBJS)
	@ranlib $(LIB_ZEBRA)

clean:
	@rm -rf *.o .*.d $(ALL_TARGETS) core *~

