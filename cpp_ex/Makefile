CC =	g++
CFLAGS =	-O2 -g -std=c++11 -Wall -fmessage-length=0

OBJS =		cpp_ex.o

LIBS =

TARGET =	cpp_ex

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	ch01	ch03	ch3_3.4.2	ch3_3.5	ex3_25 renamefile_test

clean:
	rm -f $(OBJS) $(TARGET)
ch01:	ch01/ch01.cpp
	$(CC)  ch01/ch01.cpp	-o	debug/ch01
ch03:	ch03/ex3_23.cpp
	$(CC)  ch03/ex3_23.cpp	-o	debug/ex3_23
ch3_3.4.2:	ch03/ch3_3.4.2.cpp
	$(CC)  ch03/ch3_3.4.2.cpp	-o	debug/ch3_3.4.2
ch3_3.5:	ch03/ch3_3.5.cpp
	$(CC)  ch03/ch3_3.5.cpp	-o	debug/ch3_3.5
ex3_25:	ch03/ex3_25.cpp
	$(CC)  ch03/ex3_25.cpp	-o	debug/ex3_25
#test	
renamefile_test:	test/renamefile_test.cpp
	$(CC)  test/renamefile_test.cpp	-o	debug/renamefile_test
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	