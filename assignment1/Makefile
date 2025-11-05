CXX := g++
CXXFLAGS := -O2 -Wall -Wextra -pedantic -std=c++17

all: spellcheck

spellcheck: hash.o spellcheck.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

hash.o: hash.cpp hash.h
	$(CXX) $(CXXFLAGS) -c hash.cpp

spellcheck.o: spellcheck.cpp hash.h
	$(CXX) $(CXXFLAGS) -c spellcheck.cpp

clean:
	rm -f *.o spellcheck

