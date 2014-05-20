=======================================
Protobuf File Format
=======================================

0. Intro
--------------------------------

File format for based on Google protocol buffers.

Author(s): Daniel Coderre, LHEP, University of Bern           
daniel(dot)coderre(at)lhep(dot)unibe(dot)ch

1. Brief 
----------------------------------

This library provides a user frontend for writing and reading .pbf
files. These files store binary data organized into three
sub-levels: files, events, and channels. Each file has many events.
Each event can have many channels. At each sub-level there is certain
meta-data stored in the container. Events, for example, have time
stamps, channels have id numbers, files store general data properties.

2. Installation 
-----------------------------------------
    
Fetch code from github::

    git clone https://github.com/coderdj/libpbf

Install dependencies. On ubuntu this looks like::

    apt-get install build-essential libsnappy-dev libprotobuf-dev

Other operating systems probably have the required libraries available
but they will be named differently. Installation on windows or mac
probably works but isn't supported.

In the top directory type::

    make
    make install

The installation will put the libraries in /usr/local/lib and the
includes in /usr/local/include. This is optional. If you'd rather you
can just link the libraries in headers in place.

The makefile automatically compiles the protocol buffer class with
your version of libprotobuf. If you want access to the class or if you
want to compile a python class, go to the protoc directory and read
the instructions there.

More Information
----------------

The docs are hosted by github pages. Please navigate to
http://coderdj.github.io/libpbf/ for a complete user's guide.

Contact
---------

Please contact the author with any questions or praise.

