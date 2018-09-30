CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		hawkserver-master.o

LIBS =

TARGET =	hawkserver-master

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
