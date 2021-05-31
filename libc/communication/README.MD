Tools for communication with radar. These tools work on radar side (ARM).

# Communicator
Abstarct class for communication
Interface:
0. Constructor can take additional parameters (ip, port, etc)
1. int setup() - setup. Returns error code or 0 for success
2. int send(char * message). Returns error code or 0 for success
3. 

