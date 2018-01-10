# Kernel_Module_JProbes
Kernel Module to get the Page faults Using JProbes

Command to execute:

1)make- compiles the kernalcode and the user level code to generates .ko file file for kernal level code and user_code.out file for user level code

2) sudo insmod kernel_code.ko pid=X 		-Inserts the kernal module where X is the pid of a process running in the system

3)./user_code.out		-Prints the page fault and the time of occurance in the user space

4) dmesg			-Prints system log with pagefault and the time of occurance

5)sudo rmmod kernel_code - removes the kernal device

Project Report:

This Project contains a Proc Kernel Module Kernel_code.c and a User module user_code.c

-Kernel_code.c: Is a proc interface with JProbes to handle all the page faults that occure on the perticular process whose pid is passed as parameter.
-User_code.c: Is a User applicatiion which reads the data collect from kernal module and prints it on the terminal.
