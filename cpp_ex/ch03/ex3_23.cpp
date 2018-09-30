///
/// Write a program to create a vector with ten int elements.
/// Using an iterator, assign each element a value that is twice its current
/// value.
/// Test your program by printing the vector.
///
#include <iostream>
#include <vector>
using std::vector;

int main() {
}

/**
 
 
 CC = g++
 CFLAGS =  -O2 -g -std=c++11 -Wall -fmessage-length=0

 OBJS =    cpp_ex.o

 LIBS =

 TARGET =  cpp_ex

 $(TARGET):  $(OBJS)
 $(CXX) -o $(TARGET) $(OBJS) $(LIBS)

 all:  ch01  ch03

 clean:
 rm -f $(OBJS) $(TARGET)
 ch01: ch01/ch01.cpp
 $(CC)  ch01/ch01.cpp  -o  debug/ch01
 ch03: ch03/ex3_23.cpp
 $(CC)  ch03/ex3_23.cpp  -o  debug/ex3_23 
 
 
 * 
 * */