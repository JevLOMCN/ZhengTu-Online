# $Id: Makefile 6743 2006-06-05 10:14:54Z zjw $

CXX = /usr/bin/g++
CXXFLAGS = $(ZEBRA_COMPILE_OPT) -g3 -I../base -Wall -D_GNU_SOURCE -D_REENTRANT `mysql_config --cflags` `xml2-config --cflags`
CXXLFLAGS = $(ZEBRA_COMPILE_OPT) -Wall -L../base -lzebra -lencdec  -lpthread -lz `mysql_config --libs_r` `xml2-config --libs` -llog4cxx -lgd -ljpeg 
LIB_ZEBRA = ../base/libzebra.a

ALL_TARGETS = fsmtest TCPClient TCPServer log ThreadTest timetest regextest ServiceTest MySqlTest argtest includefiletest entrytest confiletest NewMySqlTest logger

all: $(ALL_TARGETS)

$(OBJS): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

include .TCPClient.d
.TCPClient.d: .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$
TCPClient: TCPClient.o $(LIB_ZEBRA)
	$(CXX) TCPClient.o -o $@ $(CXXLFLAGS)

include .TCPServer.d
.TCPServer.d: .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$
TCPServer: TCPServer.o $(LIB_ZEBRA)
	$(CXX) TCPServer.o -o $@ $(CXXLFLAGS)


include .timetest.d
.timetest.d: .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$
timetest: timetest.o $(LIB_ZEBRA)
	$(CXX) timetest.o -o $@ $(CXXLFLAGS)

include .log.d
.log.d: .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$
log: log.o $(LIB_ZEBRA)
	$(CXX) log.o -o $@ $(CXXLFLAGS)

include .ThreadTest.d
.ThreadTest.d: .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$
ThreadTest: ThreadTest.o $(LIB_ZEBRA)
	$(CXX) ThreadTest.o -o $@ $(CXXLFLAGS)

include .ThreadPoolTest.d
.ThreadPoolTest.d: .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$
ThreadPoolTest: ThreadPoolTest.o $(LIB_ZEBRA)
	$(CXX) ThreadPoolTest.o -o $@ $(CXXLFLAGS)

include .regextest.d
.regextest.d: .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$
regextest: regextest.o $(LIB_ZEBRA)
	$(CXX) regextest.o -o $@ $(CXXLFLAGS)

include .ServiceTest.d
.ServiceTest.d: .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$
ServiceTest: ServiceTest.o $(LIB_ZEBRA)
	$(CXX) ServiceTest.o -o $@ $(CXXLFLAGS)

include .MySqlTest.d
.MySqlTest.d: .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$
MySqlTest: MySqlTest.o $(LIB_ZEBRA)
	$(CXX) MySqlTest.o -o $@ $(CXXLFLAGS)
	
include .NewMySqlTest.d
.NewMySqlTest.d: .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$
NewMySqlTest: NewMySqlTest.o $(LIB_ZEBRA)
	$(CXX) NewMySqlTest.o -o $@ $(CXXLFLAGS)

include .argtest.d
.argtest.d: .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$
argtest: argtest.o $(LIB_ZEBRA)
	$(CXX) argtest.o -o $@ $(CXXLFLAGS)

include .includefiletest.d
.includefiletest.d: .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$
includefiletest: includefiletest.o $(LIB_ZEBRA)
	$(CXX) includefiletest.o -o $@ $(CXXLFLAGS)

include .entrytest.d
.entrytest.d: .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$
entrytest: entrytest.o $(LIB_ZEBRA)
	$(CXX) entrytest.o -o $@ $(CXXLFLAGS)

include .confiletest.d
.confiletest.d: .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$
confiletest: confiletest.o $(LIB_ZEBRA)
	$(CXX) confiletest.o -o $@ $(CXXLFLAGS)

include .logger.d
.logger.d: .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$
logger: logger.o $(LIB_ZEBRA)
	$(CXX) logger.o  -o $@ $(CXXLFLAGS)
clean:
	@rm -rf *.o .*.d $(ALL_TARGETS) core *~

