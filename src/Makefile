CXX = g++
AR = ar
FLAGS = -Wall -O2 -std=c++11
LIBS = libshp.a timeit.a

all : main

main : main.cpp
	$(CXX) $(FLAGS) -o $@ $< $(LIBS)

clean :
	rm -rf main

.PHONY : all clean
