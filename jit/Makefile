COMMON = util.o parser.o
ALL = bf jitbf1 jitbf2 jitbf3
CPPFLAGS = -std=c++11 -Wno-c++11-extensions -Wall -Werror -O3 -g

%.o:

bf: $(COMMON)
jitbf1: $(COMMON)
jitbf2: $(COMMON)
jitbf3: $(COMMON)

main: $(COMMON) main.cpp

all: $(ALL)

clean:
	rm -rf *.o bf jitbf1 jitbf2 jitbf3 *.dSYM

format:
	clang-format -style=file -i *.cpp *.h

.PHONY: all clean format
