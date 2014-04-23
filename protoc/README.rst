Google Protocol Buffers for File Output
=======================================

D. Coderre, LHEP, Uni Bern
daniel(dot)coderre(at)lhep(dot)unibe(dot)ch
23.04.2014

Compiling a Class
-----------------

Google has a nice tool for making c++ and python classes from a .proto
file. You need to have protocol buffers installed to do this (on
ubuntu use apt-get install protobuf-dev).

To compile:
protoc -I=./ --cpp_out=./ --python_out=./ FILE.proto

Assuming you are running the script in the same directory you want
your ouput classes. 

Rules
-----

We want to maintain .pff as a valid data format. Therefore any new
functionality should not break backwards compatibility with old code.
In general:
                - Please only add optional fields
		- Store all .proto files used in production (with the 
		  current date appended to them) in protoc/old
		- Updates to the source code should not require data to 
		  have an optional field (thus they are optional)

As a rule, the most recent class should always be copied to the src
directory exactly as returned by the protoc compilation call and built
into the software. 

Need Help?
----------

Probably best not to touch anything in this case. ;-)
The google documentation for protocol buffers can be found at
https://developers.google.com/protocol-buffers/ (as of April 2014). 

If you are insistent on breaking something but don't know the most
brutal way to do so, feel free to contact the devs.