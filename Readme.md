c. What I done: check balance, transfer coins, TXLIST, stats (I did optional part)
d. Files description:

Readme.md: a simply description for this project.
block1.txt: The transfer data saved in sever A.
block2.txt: The transfer data saved in sever B.
block3.txt: The transfer data saved in sever C.

Makefile: use to make file, "make all" to compile all files from .c to a file can run in terminal by gcc, "make clean" to delete all files have created

severM.c: the code of sever M, use Makefile to complie it and create sever M
severA.c: the code of sever A, use Makefile to complie it and create sever A
severB.c: the code of sever B, use Makefile to complie it and create sever B
severC.c: the code of sever C, use Makefile to complie it and create sever C
clientA.c: the code of client A, use Makefile to complie it and create client A
clientB.c: the code of client B, use Makefile to complie it and create client B

severM: a sever use to receive TCP connection from client and send request to get data from sever A, B, C, then send back to client
severA: a sever use to send block coins data over UDP to main sever
severB: a sever use to send block coins data over UDP to main sever
severC: a sever use to send block coins data over UDP to main sever
clientA: a client use to send request TCP connection to main sever 
clientB: a client use to send request TCP connection to main sever

For severM, severA, severB, severC,
use below command in terminal to run it, and use ctrl+c to stop it
./severM
./severA
./severB
./severC


client A,B can run using below command in terminal
check <username>'s balance
./clientA <username>
./clientB <username>

transfer <n> coins from <usernameA> to <usernameB>
./clientA <usernameA> <usernameB> <n>
./clientB <usernameA> <usernameB> <n>

TXLIST function (save all block coins transfer data into alichain.txt)
./clientA TXLIST
./clientB TXLIST

get <username>'s' stats data
./clientA <username> stats
./clientB <username> stats

e. How data exchange

check <username>'s balance

step 1: clientA/B gets the command in terminal and then it sends the request to the sever M.
		clientA/B shows "user sent a statistics enquiry request to the main server." message on the screen.
step 2: sever M received the request.
		sever M shows "The main server received input=<user> from the client using TCP over port <port number>."
step 3: sever M send getdata request to severA, severB and severC.
		sever M shows "The main server sent a request to server <sever name>."
step 4: severA, B and C got the request, they read data from files and send it to sever M.
		severA, B and C shows "The sever <sever name> received a request from the Main Server." when it received request
		severA, B and C shows "The sever <sever name> finished sending the response to the Main Server." when it send the data to sever M.
step 4: sever M got the data from sever A, B, C.
		sever M shows "The main server received the feedback from <sever name> using UDP over port <portnumber>."
step 5: sever M start to process the analyze of data, after that it will send the result to cilent.
		sever M shows "The main server sent the current balance to client <client name>."
step 6: clientA/B gets the result from sever and print the result on screen.
		clientA/B shows "The current balance of <user> is : <n> alicoins."
		or if the <user> not exist, it will shows "Unable to proceed with the transaction as <user> is not part of the network."

transfer <n> coins from <usernameA> to <usernameB>

step 1: clientA/B gets the command in terminal and then it sends the request to the sever M.
		clientA/B shows "<usernameA> has requested to transfer <n> coins to <usernameB>." message on the screen.
step 2: sever M received the request.
		sever M shows "The main server received from <usernameA> to transfer <n> coins to <usernameB> using TCP over port <port number>."
step 3: sever M send getdata request to severA, severB and severC.
		sever M shows "The main server sent a request to server <sever name>."
step 4: severA, B and C got the request, they read data from files and send it to sever M.
		severA, B and C shows "The sever <sever name> received a request from the Main Server." when it received request
		severA, B and C shows "The sever <sever name> finished sending the response to the Main Server." when it send the data to sever M.
step 4: sever M got the data from sever A, B, C.
		sever M shows "The main server received the feedback from <sever name> using UDP over port <portnumber>."
step 5: sever M start to process the analyze of data. If it is able to add the record transation file, it randomly chooses one of severA, B or C and send reqeust to let severA, B or C to add that record in its file.
		sever M shows "The main server sent a request to server <sever name>."
step 6: severA, B or C got the request, it write data into file and send it "SUCCESS" message to sever M.
		severA, B or C shows "The sever <sever name> received a request from the Main Server." when it received request
		severA, B or C shows "The sever <sever name> finished sending the response to the Main Server." when it send the message to sever M.
step 7: sever M got the data from sever A, B, C.
		sever M shows "The main server received the feedback from <sever name> using UDP over port <portnumber>."

step 8:	After that it will send the result to cilent.
		sever M shows "he main server sent the result of the transaction to client <client name>."

step 9: clientA/B gets the result from sever and print the result on screen.
		clientA/B shows "<usernameA> successfully transferred <n> alicoins to <usernameA>. The current balance of <usernameA> is : <m> alicoins."
		if <usernameA> or <usernameB> not in the network. it shows "Unable to proceed with the transaction as <user> is not part of the network."
		if <usernameA> and <usernameB> not in the network. it shows "Unable to proceed with the transaction as <usernameA> and <usernameB> are not part of the network."
		If <usernameA> has no enough money, "<usernameA> is unable to transferred <n> alicoins to <usernameA> because of no enough money. The current balance of <usernameA> is : <m> alicoins.""
	

TXLIST function (save all block coins transfer data into alichain.txt)

step 1: clientA/B gets the command in terminal and then it sends the request to the sever M.
		clientA/B shows "user sent a sorted list request to the main server." message on the screen.
step 2: sever M received the request.
		sever M shows "A TXLIST/STATS request has been received"
step 3: sever M send getdata request to severA, severB and severC.
		sever M shows "The main server sent a request to server <sever name>."
step 4: severA, B and C got the request, they read data from files and send it to sever M.
		severA, B and C shows "The sever <sever name> received a request from the Main Server." when it received request
		severA, B and C shows "The sever <sever name> finished sending the response to the Main Server." when it send the data to sever M.
step 4: sever M got the data from sever A, B, C.
		sever M shows "The main server received the feedback from <sever name> using UDP over port <portnumber>."
step 5: sever M start to process the analyze of data and save data into alichain.txt. After that it will send the result to cilent.
		sever M shows "The sorted file is up and ready"
step 6: clientA/B gets the result from sever and print the result on screen.
		clientA/B shows "The sorted file is up and ready."

get <username>'s' stats data

step 1: clientA/B gets the command in terminal and then it sends the request to the sever M.
		clientA/B shows "<username> sent a statistics enquiry request to the main server"
step 2: sever M received the request.
		sever M shows "The main server received <username> stats request from the client using TCP over port <port number>"
step 3: sever M send getdata request to severA, severB and severC.
		sever M shows "The main server sent a request to server <sever name>."
step 4: severA, B and C got the request, they read data from files and send it to sever M.
		severA, B and C shows "The sever <sever name> received a request from the Main Server." when it received request
		severA, B and C shows "The sever <sever name> finished sending the response to the Main Server." when it send the data to sever M.
step 4: sever M got the data from sever A, B, C.
		sever M shows "The main server received the feedback from <sever name> using UDP over port <portnumber>."
step 5: sever M start to process the analyze of data. After that it will send the result to cilent.
		sever M shows "The stats data sent to client <client name>"
step 6: clientA/B gets the result from sever and print the result on screen.

g.
1) If you don't follow the command in the part d, the program may fail. (eg. input something like ./clientA A B C D)
2) Please make sure there are files 'block1.txt' for sever A, 'block2.txt' for sever B and 'block3.txt' for sever C otherwise you may get fail!


h.
Use some code from Beej's Guide to Network Programming which
is Copyright © 2019 Brian “Beej Jorgensen” Hall.

url: https://beej.us/guide/bgnet
license: https://creativecommons.org/licenses/by-nc-nd/3.0/

use get_in_addr, main in client.c
use sigchld_handler, get_in_addr, main in sever.c
use main, get_in_addr in listen.c
use main in talker.c



