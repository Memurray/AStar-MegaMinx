Provided in this directory are 2 files relating to the coding implementation.
main.cpp and makefile. No additional libraries should be required.
Intended to be run on a 64 bit machine.

These files were designed to be run in a linux command line environment and 
were tested to ensure compatibility on my <linkblue>.netlab.uky.edu VM. 

1.	In linux console, navigate to working directory containing main.cpp and makefile
2.	Type make
3.	Execute created file 	./AStarMegaMinx

Once running, it will allow the user the ability to type commands, there are 3 options.

1.	‘q’ to quit
2. 	k<randomize steps> ex. k10 will solve a megaMinx with 10 random rotations applied
3. 	f<randomize steps> ex.f10 will solve 5 different megaMinxs with for each k value between 3 and 10 rotations applied.

OF NOTE:
I have included a 64 bit flag in the construction of the executable (makefile).
My personal work in creating the cpp was done on Windows using Visual Studio 2015.
I have provided a fully formed Windows .exe file "Megaminx.exe" this is a 64 bit exe of the program 
and depending on your methods of testing may prove to be preferable after reviewing the source code provided in main.cpp
Student Linux VM memory limitations definitely result in a preferable running on a local machine.

For some unknown reason, as memory usage climbs I have occasionally encountered the error of corrupted heap,
I have pinpointed the error being caused by deleting the old top of the stack after popping it, I make a new 
pointer to the old top before popping and deleting old top, so the new top should not be impacted. Therefore this
should only result in an error in the event that either the pop didnt work and thus head of the queue is still
old top or if there are two elements at the top of the stack that both point to the same memory address. 
Neither of these should be an issue as each element added to my queue is added to the heap using new.
This was a point of much confusion, however due to the infrequencies of the issue and my inability to solve it using
methods I consider to be acceptable, it does persist and some runs in testing could exhibit this issue.