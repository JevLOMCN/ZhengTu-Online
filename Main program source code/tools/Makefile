# $Id: Makefile 6739 2006-06-05 07:52:38Z song $

CXX = /usr/bin/g++
CXXFLAGS = $(ZEBRA_COMPILE_OPT) -I../base -I../zise -Wall -D_USE_CMD_NAMESPACE -D_GNU_SOURCE -D_REENTRANT `mysql_config --cflags` `xml2-config --cflags`
CXXLFLAGS = $(ZEBRA_COMPILE_OPT) -Wall -L../base -lzebra -lencdec -lpthread -lz `mysql_config --libs_r` `xml2-config --libs` -L/usr/local/lib -llog4cxx -lgd -ljpeg
LIB_ZEBRA = ../base/libzebra.a

ALL_TARGETS = zntop zebraclientService datetime 

all: $(ALL_TARGETS)

$(OBJS): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

include .zntop.d .CollectServerInfo.d
.zntop.d .CollectServerInfo.d: .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$
zntop: zntop.o CollectServerInfo.o $(LIB_ZEBRA)
	$(CXX) zntop.o CollectServerInfo.o -o $@ $(CXXLFLAGS) -lncurses

base64: base64.o $(LIB_ZEBRA)
	$(CXX) base64.o -o $@ $(CXXLFLAGS)

include .zebraclientService.d
.zebraclientService.d: .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$
zebraclientService: zebraclientService.o LoadMap.o loginClient.o zebraclient.o zebraclientManager.o TimeTick.o ArrayTable.o $(LIB_ZEBRA)
	$(CXX) loginClient.o zebraclientService.o LoadMap.o zebraclient.o zebraclientManager.o TimeTick.o ArrayTable.o -o $@ $(CXXLFLAGS)

clean:
	@rm -rf *.o .*.d $(ALL_TARGETS) core *~

