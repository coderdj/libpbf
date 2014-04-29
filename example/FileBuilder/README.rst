Example File Builder
====================

Prerequisites
-------------

- Build the libpff library using the makefile in the top-level directory.
- You need libmongoclient

Usage
-----

make
./builder --dbaddr={mongodb address} --dbname={db name} --fout={outfile} --options={options string}

