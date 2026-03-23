# $Id: Makefile 1654 2005-07-05 01:09:23Z song $

CXX = /usr/bin/g++
CXXFLAGS = $(ZEBRA_COMPILE_OPT) -g -I. -Wall -D_GNU_SOURCE -D_REENTRANT `mysql_config --cflags` `xml2-config --cflags`
CXXLFLAGS = $(ZEBRA_COMPILE_OPT) -Wall -lpthread -lz `mysql_config --libs_r` `xml2-config --libs` -llog4cxx -lgd -ljpeg

OBJS = blowfish.o md5ex.o mycast.o mydes.o myidea.o myrc5.o EncDec.o #md5c.o

#LIB_ZEBRA = ../libzebra.a
LIB_ENCDEC = ../libencdec.a
SRCS = $(OBJS:%.o=%.cpp)
DEPS = $(OBJS:%.o=.%.d)

all: $(LIB_ENCDEC)

#include $(DEPS)
#$(DEPS): .%.d: %.cpp
#	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$
	
$(OBJS): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@
	gcc -c $(CXXFLAGS) md5c.c -o md5c.o

$(LIB_ENCDEC): $(OBJS)
	@rm -rf $(LIB_ENCDEC)
	@ar cru $(LIB_ENCDEC) $(OBJS) md5c.o
	@ranlib $(LIB_ENCDEC)

clean:
	@rm -rf $(OBJS) $(LIB_ENCDEC) core *~
