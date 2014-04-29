# example Makefile
# For example file builder

CC		= g++
CFLAGS		= -I../../src -Wall -g -DLINUX -fPIC
LDFLAGS         = -L../../ -lmongoclient -lboost_thread -lboost_filesystem -lboost_program_options -lboost_system -lpff -lsnappy -lprotobuf -lpthread -lssl -lcrypto
SOURCES         = $(shell echo ./*cc)
OBJECTS         = $(SOURCES: .cc=.o)
CPP             = builder

all: $(SOURCES) $(CPP)

$(CPP) : $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) $(LDFLAGS) -o $(CPP)

clean:	
	rm $(CPP)


