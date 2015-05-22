ifdef SystemRoot
   RM = del /Q
   EXT = .exe
else
   ifeq ($(shell uname), Linux)
      RM = rm -f
      EXT = 
   endif
endif

all:
	g++ -std=c++11 -o csm$(EXT) CircularStringMatching.cpp main.cpp -lm -I.

clean:
	$(RM) main.o csm.o csm$(EXT)

