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
	g++ -std=c++11 -g -D_USE_OMP -msse4.2 -O3 -fomit-frame-pointer -funroll-loops -o csm$(EXT) CircularStringMatching.cpp main.cpp -lm -I.

clean:
	$(RM) main.o csm.o csm$(EXT)

