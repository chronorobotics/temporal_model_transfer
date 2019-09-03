OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))

include ../Mk/local.Mk
CXXINCLUDE+=-I/usr/local/include -I/usr/include/python2.7/ -I./ -I../common
CXXINCLUDE+=-I/usr/local/include

CXXDEFINE=-D"NUM_LOCATIONS=${NUM_LOCATIONS}" -D"PLAN_GRANULARITY=${PLAN_GRANULARITY}"

all: $(OBJS) 

.cpp.o:
	$(CXX)  $(CXXFLAGS) $(CXXDEFINE) -c  $(CXXINCLUDE) $< 

.c.o:
	$(CXX)  $(FLAGS) $(CXXDEFINE) -c  $(CXXFLAGS) $(CXXINCLUDE) $< 

clean:
	$(RM) $(OBJS) *.moc $(UI_HEAD) $(UI_CPP)
