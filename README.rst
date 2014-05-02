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
    
Fetch code from github:

``
git clone https://github.com/coderdj/pff
``

Install dependencies. On ubuntu this looks like:

``
apt-get install build-essential libsnappy-dev libprotobuf-dev
``

Other operating systems probably have the required libraries available
but they will be named differently. Installation on windows or mac
probably works but isn't supported.

Right now there is not installation script (not yet at least). So you
have to regenerate the protocol buffer class with your version of
libprotobuf. Don't worry, it's easy. Assuming you start in the top
level directory:

``
cd protoc
protoc -I=./ --cpp_out=../src protoDef.proto
``

See the readme in the protoc directory if you want to do some other
fancy stuff, like compile a python class.

Now that everything is done go to the top level directory and type:

``
make
make install
``

The installation will put the libraries in /usr/local/lib and the
includes in /usr/local/include. This is optional. If you'd rather you
can just link the libraries in headers in place.

More Information
----------------

The docs are hosted by github pages. Please navigate to
http://coderdj.github.io/libpbf/ for a complete user's guide.

Contact
---------

Please contact the author with any questions or praise.

