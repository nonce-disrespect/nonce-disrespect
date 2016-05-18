CXXFLAGS += -Ofast
LDLIBS += -lgmp -lntl

all : recover forge

recover : recover.o gcm.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(LDLIBS) $^

forge : forge.o gcm.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(LDLIBS) $^

%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $^

clean :
	$(RM) *.o recover forge

.PHONY : all clean
