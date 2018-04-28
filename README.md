# Writing-a-Unix-Shell



## 1 Compile, Run, and Test program:

### 1.1 Compile and Run

	I have stored the folder testScripts inside this project so you can compare it with my code 
by typing ./osh -t < testScripts/eachtestcase.txt

	a/ To test each command:

make
./osh

	b/ To run with test scripts and seeing the comand prompt osh>:

make
./osh < testScripts

	c/ To run with test scripts but not seeing the comand prompt osh>:

make
./osh -t < testScripts


## 4 Problems when doing PA1:


    There are not so many problem when i am doing this programming assignments with the implemented folder osh-parser/ since the only problem for me is to read and understand each function like pipe(), fork() or dup2() carefully to utilize them correctly. However, before using osh-parser/, i have struggled a lot when using the folder PA1/ since this only help me to parser command. Everything else like creating an infinite loop, identify malfromed commands or doing MAkefile, i will have to do it on my own.
