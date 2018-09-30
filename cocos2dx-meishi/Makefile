CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		cocos2dx-meishi.o

LIBS =

TARGET =	cocos2dx-meishi

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
