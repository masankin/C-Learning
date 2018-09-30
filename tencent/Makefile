CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		tencent.o

LIBS =

TARGET =	tencent

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
