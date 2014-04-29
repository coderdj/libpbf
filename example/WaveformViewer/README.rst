Waveform Viewer for .pff Files
##############################

Intro
-----

Example waveform grapher.

Requirements
------------

You need ROOT (root.cern.ch). You should be able to type "root" into a
console and get dropped into the shell.

Usage
-----

Change script parameters if wanted. Then run 

>make
>./viewer --file={path} --event={eventID}

The file parameter is necessary and can either be an absolute path or
a stub. The event ID is optional and can specify a certain event.

Credit
------

Written by Daniel Coderre, LHEP, Uni Bern. ROOT is an open source
package developed and distributed by CERN (see root.cern.ch). 