# Project 2 – Group 5
Members: 
Andrue Peters: (60%)
- Part1
- Part2
- Part3
- Part 3 Structural Layout

Oscar Flores: (40%)
- makefiles
- system calls
- Part 3 structural planning
- Debugging
- test runs
- README

#How to compile using makefile
Use make when inside the directory for each specific part you want to compile.

#Tar Contents and File Description
We were told to include the pathname, due to the unusual layout of our project.
Part 1: 
1.	Makefile (./proj_2/part1/Makefile): compiles the two files inside part1 folder by calling  strace and it prints out 
    the number of system calls.
2.	part1.c (./proj_2/part1/part1.c): Source file for part 1. Makes 6 system calls and uses “Hello world!” for testing.
3.	empty.c ((./proj_2/part1/empty.c): empty main routine used to have the minimum number of system calls (6) that we want.

Part 2:
4.	Makefile (./proj_2/part2/src/Makefile): compiles the kernel module
5.	my_xtime.c (./proj_2/part2/src/my_xtime.c): Source file for Part 2. This kernel module keeps track of the time elapsed 
    since the unix epoch

Part 3:
6.	.gitignore (./proj_2/part3/.gitignore): Outlines which files and directories should be ignored when making a commit.
7.	Makefile (./proj_2/part3/Makefile): Compiles the kernel module. Based on example given in lecture slides.
8.	elevator.h (./proj_2/part3/elevator.h): hold the function declarations and structs used for the elevator scheduler in elevator.c.
9.	elevator.c (./proj_2/part3/elevator.c): This is the elevator scheduler
10.	floor.h (./proj_2/part3/): Declares the struct for each floor and the functions to add a passenger and offload a passenger.
11.	floor.c (./proj_2/part3/): Defines and initializes floor struct and functions
12.	my_elev.c (./proj_2/part3/): This module provides the proc entry and prints the current elevator information (state, floor, 
    next floor, load). This also prints the data for each floor (waiting passenger loads and the number of passengers that have    been serviced).
13.	my_elev_config.h (./proj_2/part3/): Initializes and stores the configuration of the elevator module.
14.	passenger.h (./proj_2/part3/): Declares the enumerator, struct, and functions used for the different types of elevator passengers.
15.	passenger.c (./proj_2/part3/): Defines the different types of passenger, their weight/unit values, and useful accessor functions.
16.	sys_my_elev.c (./proj_2/part3/sys_call/syst_my_elev.c): This is the system call module for the elevator scheduler.
17.	Makefile (./proj_2/part3/sys_call/Makefile): compiles system call module for the elevator scheduler
18.	__syscalls.h (./proj_2/part3/sys_call/kernel/__syscalls.h): header file declaring the start elevator, stop elevator, and issue 
    request functions needed for system calls
19.	__syscall_64.tbl (./proj_2/part3/sys_call/kernel/__syscall_64.tbl): these files need to be included in the kernel files. They 
    are the functions for the system calls.

docs:
This folder contains useful documents regarding the project, such as examples, lectures, and project description. The provided tests 
are also stored here in the Testing folder and are as follows and contents extracted in the corresponding test1, test2, test3, and test4 folders:
./proj_2/docs/Testing/elevator1_issue_one
./proj_2/docs/Testing/elevator2_issue_one_invalid
./proj_2/docs/Testing/elevator3_issue_several
./proj_2/docs/Testing/elevator4_stress_test
To run a test, go to the desired test folder and use make. Run the executables.

Computer_info.txt (./proj_2/computer_info.txt): This file holds the login password for the lab computer.

# Development Log and Bugs
Week 1 (starting Feb. 25): Worked on Parts 1 and 2. We had issues initially with each makefile, especially when we started Part 1. 
We finished Part 1 this week.

Week 2 (starting March 4): Worked primarily on Part 2. We finished Part 2 and then planned the structure for Part 3.

Week 3 (starting March 11): Worked exclusively on Part 3. We worked on VMs and found that code that had worked on our machines no 
longer worked on the CS computer kernel. This was due to using an incorrect version of the kernel. I, Oscar Flores, followed the 
instructions on the lecture slides for setting up the kernel. There were also deadlock safety mechanisms implemented in the VM kernels 
we used. The CS kernel did not have these safety mechanisms and we were getting errors, initially. We realized we had a deadlock 
because we did not release the lock before the lock was acquired again. Fixing this bug increased the speed of our code. With the 
deadlock and safety mechanism, there was a delay in the proc and printing out the individual floor information, but once we fixed the 
problem, there was almost no noticeable delay. We were getting compilation errors for Part 3 that all of the .h files we included were 
not found. Switching to a different version of the kernel did not fix this issue. Instead, we changed from using <> to using “ " for 
our #include statements in each of our files. The compiler was having difficulty finding the file location. We are not sure why the 
VM we worked on did not have this issue. This fixed our error. We also had issues starting the elevator, but this was because we typed 
in the wrong command. We would write “- start” rather than “—start”. Our code most likely worked before, but we were getting messages 
that seemed like we were dereferencing NULL pointers or had undefined functions. Once we input the correct command, there were no errors.

