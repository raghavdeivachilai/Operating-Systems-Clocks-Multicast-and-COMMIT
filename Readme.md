# Clocks, Multicast, and COMMIT:

## Part I: Berkeley Algorithm
The Berkeley.cpp file should be compiled for the first part of the project. It can be compiled as follows:
g++ -std=c++11 -o berkeley Berkeley.cpp -pthread
The format to compile the program is: program output, port number, process id.
./berkeley 9060 2
./berkeley 9063 3
./berkeley 9074 4
./berkeley 7001 0
All client processes in this case first three commands should be executed in three separated terminals
before running the master program 0. Those client processes would be waiting for the master server to
connect. Then, the master process 0 is executed by using the last command. All waiting processes would
start running and generate a random logical clock for themselves. Those clock values are sent to the master
process which calculates and sends the value by which each process should adjust its local clock. The final
updated clock is displayed as” My Updated Clock Value After Berkeley Algorithm” which is the
synchronized clock among all processes.
It also displays the current value in file and updated value in file as I have combined this part with the
distributed locking part of the project.

## Part II: Causal Ordering
The files for the second part of the project can be found in the files causalordering.cpp and
noncausal.cpp. . These files can be compiled as below:
g++ -std=c++11 -o causalordering causalordering.cpp -pthread
The format to compile the program is: output filename, portnumber, the process ID, message
./causalordering 9002 3 <message>
./causalordering 9004 1 <message>
./causalordering 9003 2 <message>
To test causal ordering, execute all the three commands in three separate terminals. Then, send a multicast
from process 1 to process 3 by pressing 1 and enter in terminal window of process 1(port number 9004).
Then, send a multicast from process 2 to process 3 by pressing 1 and enter in the terminal window of
process 2(port number 9003). Now the message from process 2 will get buffered as it does not satisfy the
causal ordering which is shown in process 3(port number 9002). I have used sleep in my program to delay
the sending of message from process 1 to process 3. After sleep period of 10, then, the message from
process 1 will be received by process 3 and it will get delivered in the terminal window of process 3(portRaghav Deivachilai
ZG70451
number 9002). I have also shown the queue contents to show that the message from process 2 to process
was buffered.
  
## Non-Causal Ordering:
g++ -std=c++11 -o noncausal noncausal.cpp -pthread
The format to compile the program is: output filename, portnumber, the process ID, message, delay
Here the values 0,10,5 are the delays .
./noncausal 9002 3 <message> 0
./noncausal 9004 1 <message> 10
./noncausal 9003 2 <message> 0
./noncausal 9005 1 <message> 5
To test non causal ordering, execute all the three commands in three terminals. Then, send a multicast
from process 1 to process 3 by pressing 1 and enter in terminal of process 1 I.e. terminal with port
number 9004. I have used sleep to delay the sending of the message. Now, we must send another
multicast from the process 1. To do that, we have to open another terminal and execute the following
command ./noncausalordering 9005 1 <message> 5 and send a multicast by pressing 1 and enter.
The delay should be ideally 10 in the first case and 5 in the second case. When all this is executed, the
second multicast from process 1 will get buffered and is shown in the buffer contents in process 3 I.e. in
the terminal window with port number 9002. Finally, when the first multicast reaches the process 3, it
will get delivered and the buffer contents are shown to be empty in process 3.

## Part III: Distributed Lock
This part is combined with the first part of the project. It is implemented using mutex locks. The
filecounter.txt file has a value which is updated by each of the processes. Each time the berkeley.cpp
program is run in multiple terminals; the locking ensures that only a single process enters the critical
section. This can be verified, as the output displays the current value in the file and the value updated
by the process as the updated value in file.
