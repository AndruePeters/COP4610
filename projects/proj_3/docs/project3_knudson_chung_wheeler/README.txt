Project Members and Division of labor:



Steven Knudson
	

-Info, ls, size, and cd	

-README



Meng-Han Chung
	

-Open, close, read, and write


Hunter Wheeler


-Create and mkdir

Tar Contents: main.c is our interface, cmds.c is our source code, and cmds.h is our header file



Compilation:


Use make.


Extra Credit:
Rm and rmdir implemented.



Bugs: 
create and mkdir: do not function properly, when either a file or 
directory are inserted into the FAT Image, it does not display properly and when ls is called, it shows extra spaces meaning something is being inserted into the image, but it is unclear what is happening, 
also neither checks to see if a preexisting entry is in the FAT Table, mkdir struct does not initialize the current and parent directories

.