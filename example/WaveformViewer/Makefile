# example Makefile
# For example file builder

CC		= g++
CFLAGS		= -I./../../src $(shell root-config --cflags) -Wall -g -DLINUX -fPIC
LDFLAGS         = -L./../../ $(shell root-config --libs) $(shell root-config --glibs) -lpbf -lsnappy -lprotobuf -lpthread -lssl -lcrypto
SOURCES         = $(shell echo ./*cc)
OBJECTS         = $(SOURCES: .cc=.o)
CPP             = viewer

all: $(SOURCES) $(CPP)

$(CPP) : $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) $(LDFLAGS) -o $(CPP)

clean:	
	rm $(CPP)


