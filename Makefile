#libpbf Makefile
# use 'make' to compile the shared library

PREFIX          = /usr/local
CC		= g++
CFLAGS		= -Wall -g -DLINUX -fPIC
LDFLAGS         = -shared -lsnappy -lprotobuf
SOURCES         = $(shell echo ./src/*cc)
OBJECTS         = $(SOURCES: .cc=.o)
SO              = libpbf.so

all: $(SO)

$(SO) : $(OBJECTS)
	echo 'Regenerating protocol buffer classes'
	protoc -I=protoc/ --cpp_out=src/ protoc/protocDef.proto
	echo 'done.'
	$(CC) $(LDFLAGS) $(OBJECTS) $(CFLAGS) -o $(SO)

install:	
	install -m 0755 $(SO) $(PREFIX)/lib/$(SO)
	test -d $(PREFIX)/include/pbf || mkdir $(PREFIX)/include/pbf
	install -m 0644 $(shell echo ./src/*h) $(PREFIX)/include/pbf/

.PHONY: install

clean:
	rm $(SO)



