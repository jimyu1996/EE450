/*Use some code from Beej's Guide to Network Programming which
is Copyright © 2019 Brian “Beej Jorgensen” Hall.

url: https://beej.us/guide/bgnet
license: https://creativecommons.org/licenses/by-nc-nd/3.0/

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <time.h>


#include <signal.h>


#define LOCALHOST "127.0.0.1"

#define UDPPORT "24713"
#define TCPAPORT "25713"  
#define TCPBPORT "26713"

#define SEVERAPORT "21713"
#define SEVERBPORT "22713"
#define SEVERCPORT "23713"

#define BACKLOG 1000
#define MAXDATASIZE 10000
#define MAXBUFLEN 10000	 

#define MAXTRANLEN 1280
#define MAXROW 1000

#define TXLISTFILENAME "alichain.txt"




typedef struct StatsItem {
    char username[512];
    int trans_number;
    int trans_amount;
}StatsItem;


//Use function sigchld_handler(int s) in sever.c from Beej's Guide to Network Programming
void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}


//Use function get_in_addr(struct sockaddr *sa) in sever.c from Beej's Guide to Network Programming
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


// void sort_by_serial(char* stringlist[], int len) {
//     int i;
//     int j;
//     char temp[MAXTRANLEN];
//     for (i = 0; i < len - 1; i = i + 1){
//     	for (j = 0; j < len - 1 - i; j = j + 1){
//     		printf("start %s and %s\n", stringlist[j], stringlist[j + 1]);
//         	if (strcmp(stringlist[j], stringlist[j + 1]) > 0) {
//                 strcpy(temp, stringlist[j]);
//                 strcpy(stringlist[j], stringlist[j+1]);
//                 strcpy(stringlist[j+1], temp);
//             }
//             printf("end %s and %s\n", stringlist[j], stringlist[j + 1]);
//         }
//     }

            
// }

//Use function int main() in listener.c from Beej's Guide to Network Programming
char *receive_udp_message(char *return_value)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[MAXBUFLEN];
    socklen_t addr_len;
    //char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(LOCALHOST, UDPPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return "1";
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return "2";
    }

    freeaddrinfo(servinfo);



    addr_len = sizeof their_addr;
    if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
        (struct sockaddr *)&their_addr, &addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
    }

    // printf("listener: got packet from %s\n",
    //     inet_ntop(their_addr.ss_family,
    //         get_in_addr((struct sockaddr *)&their_addr),
    //         s, sizeof s));

    buf[numbytes] = '\0';
    strcpy(return_value,buf);

    close(sockfd);

    return return_value;
}



//Use function int main() in talker.c from Beej's Guide to Network Programming
int run_udp_connection(char **recv, char *severport, char *send_message)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
	char recv_buf[MAXBUFLEN];

	if(strcmp(send_message,"GETDATA") == 0){
		*recv = (char*) malloc(MAXTRANLEN * MAXROW * sizeof(char));
	}else{

		*recv = (char*) malloc(MAXDATASIZE * sizeof(char));
	}
	


	strcpy(*recv,"");

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(LOCALHOST, severport, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");
        return 2;
    }


    if ((numbytes = sendto(sockfd, send_message, strlen(send_message), 0,
             p->ai_addr, p->ai_addrlen)) == -1) {
        perror("talker: sendto");
        exit(1);
    }

    strcpy(recv_buf,"");


    while(strlen(recv_buf) ==0 || recv_buf[strlen(recv_buf) - 1] != '@'){

    	strcat(*recv,recv_buf);
    	//printf("222%s\n", *recv);
    	//printf("333%s\n", recv_buf);
    	strcpy(recv_buf,"");

	    if ((numbytes = recvfrom(sockfd, recv_buf, MAXBUFLEN - 2, 0,
	             NULL, NULL)) == -1) {
	        perror("talker: recvfrom");
	        exit(1);
	    }
	    recv_buf[numbytes] = '\0';


    }

    if(strlen(recv_buf) > 0){
    	recv_buf[strlen(recv_buf) - 1] = '\0';
    }

    strcat(*recv,recv_buf);


    

    // if ((numbytes = recvfrom(sockfd, recv_buf, MAXBUFLEN, 0,
    //          NULL, NULL)) == -1) {
    //     perror("talker: recvfrom");
    //     exit(1);
    // }

    freeaddrinfo(servinfo);

    close(sockfd);

    return 0;
}

//Use function int main() in sever.c from Beej's Guide to Network Programming
int run_the_connection(char *port, char* client_name)
{
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    //char s[INET6_ADDRSTRLEN];
    int rv;
    char buf[MAXDATASIZE];
    int numbytes;
    char *recv_list[3];
    char *recv_item;
    int recv_num = 0;
    //char buf_list[MAXROW*3][MAXTRANLEN];
    char *buf_item;
    int buf_num = 0;
    char recv_message[MAXDATASIZE];

    char *serial;
    char *username_a;
    char *username_b;
    char *transfer_amount;

    int user_exist = 0;
    int user_receiver_exist = 0;
    int max_serial_number = 0;
    char send_to_client_message[MAXDATASIZE];

    //time_t t;
   
   	

    FILE *fp = NULL;


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(LOCALHOST, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }



    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        /*inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);*/

        if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener


            //Receive message from client
			if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
			    perror("recv");
			    exit(1);
			}


			buf[numbytes] = '\0';
			strcpy(recv_message,buf);
			//split message
			recv_num = 0;
			recv_item = strtok(buf,":");

			while(recv_item != NULL){
				recv_list[recv_num] = recv_item;
				recv_item = strtok(NULL,":");
				recv_num = recv_num + 1;

			}

			char* recv_buf;

			//Check balance
			if (recv_num == 1 && strcmp(recv_list[0],"TXLIST") != 0 ){
				



				printf("The main server received input=%s from the client using TCP over port %s.\n", recv_list[0],port);
				
				buf_num = 0;
				char** buf_list = (char**)malloc(MAXROW * 3  * sizeof(char*));
				for (int k = 0; k < MAXROW * 3; k = k + 1){
					buf_list[k] = malloc(MAXTRANLEN * sizeof(char));
				}

				printf("The main server sent a request to server A.\n");

				run_udp_connection(&recv_buf, SEVERAPORT,"GETDATA");

				printf("The main server received the transactions from sever %s using UDP over port %s.\n","A",SEVERAPORT);
				
				buf_item = strtok(recv_buf,"\n");
				while(buf_item != NULL){

					strcpy(buf_list[buf_num],buf_item);
					buf_item = strtok(NULL,"\n");
					buf_num = buf_num + 1;
					
				}

				free(recv_buf);

				printf("The main server sent a request to server B.\n");

				run_udp_connection(&recv_buf, SEVERBPORT,"GETDATA");

				printf("The main server received the transactions from sever %s using UDP over port %s.\n","B",SEVERBPORT);
				
				buf_item = strtok(recv_buf,"\n");
				while(buf_item != NULL){

					strcpy(buf_list[buf_num],buf_item);
					buf_item = strtok(NULL,"\n");
					buf_num = buf_num + 1;
					
				}

				free(recv_buf);

				printf("The main server sent a request to server C.\n");

				run_udp_connection(&recv_buf, SEVERCPORT,"GETDATA");

				printf("The main server received the transactions from sever %s using UDP over port %s.\n","C",SEVERCPORT);
				
				buf_item = strtok(recv_buf,"\n");
				while(buf_item != NULL){

					strcpy(buf_list[buf_num],buf_item);
					buf_item = strtok(NULL,"\n");
					buf_num = buf_num + 1;
					
				}

				free(recv_buf);



				//char rece_m[MAXBUFLEN];
    			//receive_udp_message(rece_m);
    			user_exist = 0;
				int balance_amount = 1000;

    			for(int i=0; i < buf_num; i = i+1){
    				char temp[strlen(buf_list[i])];
					strcpy(temp, buf_list[i]);
					strtok(temp," ");
					username_a = strtok(NULL," ");
					username_b = strtok(NULL," ");
					transfer_amount = strtok(NULL," ");

					if(strcmp(username_a,recv_list[0]) ==0){
						user_exist = 1;
						balance_amount = balance_amount - atoi(transfer_amount);
					}
					if(strcmp(username_b,recv_list[0]) ==0){
						user_exist = 1;
						balance_amount = balance_amount + atoi(transfer_amount);
					}


    			}
    			free(buf_list);

    			if (user_exist == 0){
    				sprintf(send_to_client_message, "Unable to proceed with the transaction as \"%s\" is not part of the network.", recv_list[0]);
    			}else{
    				sprintf(send_to_client_message, "The current balance of \"%s\" is : %d alicoins.", recv_list[0], balance_amount);
    			}

    			
    			if (send(new_fd, send_to_client_message, strlen(send_to_client_message), 0) == -1){
                	perror("send");
				}else{
					printf("The main server sent the current balance to client %s.\n",client_name);
				}

			}
			
			//Transfer money
			if (recv_num == 3){
				printf("The main server received from %s to transfer %s coins to %s using TCP over port %s.\n",recv_list[0],recv_list[2],recv_list[1],port);
				
				
				char** buf_list = (char**)malloc(MAXROW * 3  * sizeof(char*));
				for (int k = 0; k < MAXROW * 3; k = k + 1){
					buf_list[k] = malloc(MAXTRANLEN * sizeof(char));
				}

				printf("The main server sent a request to server A.\n");

				run_udp_connection(&recv_buf, SEVERAPORT,"GETDATA");

				printf("The main server received the transactions from sever %s using UDP over port %s.\n","A",SEVERAPORT);
				buf_num = 0;
				buf_item = strtok(recv_buf,"\n");
				while(buf_item != NULL){

					strcpy(buf_list[buf_num], buf_item);
					buf_item = strtok(NULL,"\n");
					buf_num = buf_num + 1;
				}
				free(recv_buf);
				printf("The main server sent a request to server B.\n");
				run_udp_connection(&recv_buf, SEVERBPORT,"GETDATA");

				printf("The main server received the transactions from sever %s using UDP over port %s.\n","B",SEVERBPORT);
				buf_item = strtok(recv_buf,"\n");
				while(buf_item != NULL){

					strcpy(buf_list[buf_num], buf_item);
					buf_item = strtok(NULL,"\n");
					buf_num = buf_num + 1;
				}
				
				free(recv_buf);
				printf("The main server sent a request to server C.\n");

				run_udp_connection(&recv_buf, SEVERCPORT,"GETDATA");
				printf("The main server received the transactions from sever %s using UDP over port %s.\n","C",SEVERCPORT);
				buf_item = strtok(recv_buf,"\n");
				while(buf_item != NULL){

					strcpy(buf_list[buf_num], buf_item);
					buf_item = strtok(NULL,"\n");
					buf_num = buf_num + 1;
				}
				free(recv_buf);	

    			user_exist = 0;
    			user_receiver_exist = 0;
    			max_serial_number = 0;
    			int balance_amount = 1000;

    			

    			for(int i=0; i < buf_num; i = i+1){
    				char temp[strlen(buf_list[i])];
					strcpy(temp, buf_list[i]);
					serial = strtok(temp," ");
					if(atoi(serial) > max_serial_number){
						max_serial_number = atoi(serial);
					}
					username_a = strtok(NULL," ");
					username_b = strtok(NULL," ");
					transfer_amount = strtok(NULL," ");

					if(strcmp(username_a,recv_list[0]) ==0){
						user_exist = 1;
						balance_amount = balance_amount - atoi(transfer_amount);
					}
					if(strcmp(username_b,recv_list[0]) ==0){
						user_exist = 1;
						balance_amount = balance_amount + atoi(transfer_amount);
					}
					if(strcmp(username_a,recv_list[1]) ==0 || strcmp(username_b,recv_list[1]) ==0){
						user_receiver_exist = 1;
					}


    			}
    			free(buf_list);
    			max_serial_number = max_serial_number + 1;
    			if (user_exist == 0 && user_receiver_exist == 0){
    				sprintf(send_to_client_message, "Unable to proceed with the transaction as \"%s\" and \"%s\" are not part of the network.", recv_list[0], recv_list[1]);
    			}else if(user_exist == 0){
    				sprintf(send_to_client_message, "Unable to proceed with the transaction as \"%s\" is not part of the network.", recv_list[0]);
    			}else if(user_receiver_exist == 0){
    				sprintf(send_to_client_message, "Unable to proceed with the transaction as \"%s\" is not part of the network.", recv_list[1]);
    			}else if(balance_amount < atoi(recv_list[2])){
    				sprintf(send_to_client_message,"\"%s\" was unable to transfer %s alicoins to \"%s\" because of insufficient balance.\nThe current balance of \"%s\" is : %d alicoins.",recv_list[0],recv_list[2],recv_list[1],recv_list[0],balance_amount);
    			}else{
    				//sent write in block request to sever
    				//Ramdom select a sever

    				srand((unsigned)time(NULL));

    				char *sever_recv_buf;
    				char send_to_sever_message[MAXDATASIZE];
    				sprintf(send_to_sever_message,"%d %s %s %s",max_serial_number, recv_list[0], recv_list[1], recv_list[2]);
    				
    				char severport_save[5];
    				char severport_name[1];
    				int random_number = rand();
    				random_number = random_number % 3;
    				if(random_number == 0){
    					strcpy(severport_save, SEVERAPORT);
    					strcpy(severport_name,"A");
    				}else if(random_number == 1){
						strcpy(severport_save, SEVERBPORT);
    					strcpy(severport_name,"B");

    				}else{
						strcpy(severport_save, SEVERCPORT);
    					strcpy(severport_name,"C");

    				}

    				run_udp_connection(&sever_recv_buf, severport_save,send_to_sever_message);	
    				printf("The main server sent a request to server %s.\n",severport_name);
    				if(strcmp(sever_recv_buf,"SUCCESS") == 0){
    					printf("The main server received the feedback from sever %s using UDP over port %s.\n",severport_name,severport_save);
    					sprintf(send_to_client_message,"\"%s\" successfully transferred %s alicoins to \"%s\".\nThe current balance of \"%s\" is : %d alicoins.",recv_list[0],recv_list[2],recv_list[1],recv_list[0],balance_amount-atoi(recv_list[2]));
    				
    				}else{
    					sprintf(send_to_client_message,"transferred failed because of network ERROR");
    				}
    				free(sever_recv_buf);

    				

    			}
			
    			if (send(new_fd, send_to_client_message, strlen(send_to_client_message), 0) == -1){
                	perror("send");
				}else{
					printf("The main server sent the result of the transaction to client %s.\n",client_name);
				}
				

			}

			//txlist
			if (recv_num == 1 && strcmp(recv_list[0],"TXLIST") == 0 ){

				printf("A TXLIST/STATS request has been received\n");
				buf_num = 0;
				
				char** buf_list = (char**)malloc(MAXROW * 3  * sizeof(char*));
				for (int k = 0; k < MAXROW * 3; k = k + 1){
					buf_list[k] = malloc(MAXTRANLEN * sizeof(char));
				}
				printf("The main server sent a request to server A.\n");
				run_udp_connection(&recv_buf, SEVERAPORT,"GETDATA");
				printf("The main server received the feedback from sever %s using UDP over port %s.\n","A",SEVERAPORT);
				
				buf_item = strtok(recv_buf,"\n");
				while(buf_item != NULL){

					strcpy(buf_list[buf_num] ,buf_item);
					buf_item = strtok(NULL,"\n");
					buf_num = buf_num + 1;
				}
				free(recv_buf);

				printf("The main server sent a request to server B.\n");
				run_udp_connection(&recv_buf, SEVERBPORT,"GETDATA");
				printf("The main server received the feedback from sever %s using UDP over port %s.\n","B",SEVERBPORT);
				
				buf_item = strtok(recv_buf,"\n");
				while(buf_item != NULL){

					strcpy(buf_list[buf_num] ,buf_item);
					buf_item = strtok(NULL,"\n");
					buf_num = buf_num + 1;
				}
				free(recv_buf);

				printf("The main server sent a request to server C.\n");
				run_udp_connection(&recv_buf, SEVERCPORT,"GETDATA");
				printf("The main server received the feedback from sever %s using UDP over port %s.\n","C",SEVERCPORT);
				
				buf_item = strtok(recv_buf,"\n");
				while(buf_item != NULL){

					strcpy(buf_list[buf_num] ,buf_item);
					buf_item = strtok(NULL,"\n");
					buf_num = buf_num + 1;
				}
				free(recv_buf);



				//sort and save

				//sort_by_serial(buf_list, buf_num);
				int ii;
				int j;
				char temp[MAXTRANLEN];
				char temp_a[MAXTRANLEN];
				char temp_b[MAXTRANLEN];
				for (ii = 0; ii < buf_num - 1; ii = ii + 1){
					for (j = 0; j < buf_num - 1 - ii; j = j + 1){
						strcpy(temp_a, buf_list[j]);
						strcpy(temp_b, buf_list[j+1]);
				    	if (atoi(strtok(temp_a," "))  > atoi(strtok(temp_b," "))) {
				            
				            strcpy(temp, buf_list[j]);
				            strcpy(buf_list[j], buf_list[j+1]);
				            strcpy(buf_list[j+1], temp);
				        }
				    }
				}


				fp = fopen(TXLISTFILENAME, "w");
            	

				for(int i =0; i < buf_num; i = i + 1){

					//printf("%s\n", buf_list[i]);
					fprintf(fp, "%s\n", buf_list[i]);
				}

				fclose(fp);
				free(buf_list);
				printf("The sorted file is up and ready\n");
				//printf("The main server received TXLIST request from the client using TCP over port %s.\n", port);
				if (send(new_fd, "The sorted file is up and ready", strlen("The sorted file is up and ready"), 0) == -1){
                	perror("send");
				}

			}

			//stats
			if (recv_num == 2 && strcmp(recv_list[1],"stats") == 0 ){
				printf("The main server received %s stats request from the client using TCP over port %s.\n", recv_list[0],port);
				//GET DATA FROM UDP
				buf_num = 0;
				char** buf_list = (char**)malloc(MAXROW * 3  * sizeof(char*));
				for (int k = 0; k < MAXROW * 3; k = k + 1){
					buf_list[k] = malloc(MAXTRANLEN * sizeof(char));
				}

				printf("The main server sent a request to server A.\n");
				run_udp_connection(&recv_buf, SEVERAPORT,"GETDATA");
				printf("The main server received the feedback from sever %s using UDP over port %s.\n","A",SEVERAPORT);
				
				buf_item = strtok(recv_buf,"\n");
				while(buf_item != NULL){

					strcpy(buf_list[buf_num], buf_item);
					buf_item = strtok(NULL,"\n");
					buf_num = buf_num + 1;
				}
				free(recv_buf);

				printf("The main server sent a request to server B.\n");
				run_udp_connection(&recv_buf, SEVERBPORT,"GETDATA");
				printf("The main server received the feedback from sever %s using UDP over port %s.\n","B",SEVERBPORT);
				
				buf_item = strtok(recv_buf,"\n");
				while(buf_item != NULL){

					strcpy(buf_list[buf_num], buf_item);
					buf_item = strtok(NULL,"\n");
					buf_num = buf_num + 1;
				}
				free(recv_buf);

				printf("The main server sent a request to server C.\n");
				run_udp_connection(&recv_buf, SEVERCPORT,"GETDATA");
				printf("The main server received the feedback from sever %s using UDP over port %s.\n","C",SEVERCPORT);
				
				buf_item = strtok(recv_buf,"\n");
				while(buf_item != NULL){

					strcpy(buf_list[buf_num], buf_item);
					buf_item = strtok(NULL,"\n");
					buf_num = buf_num + 1;
				}
				free(recv_buf);

    			user_exist = -1;
    			int statslist_length = 0;
    			StatsItem* statslist = (StatsItem*)malloc(MAXROW * 3 * sizeof(StatsItem));

    			for(int i=0; i < buf_num; i = i+1){

    				char temp[strlen(buf_list[i])];
    				memset(temp,'\0',strlen(buf_list[i]));
					strcpy(temp, buf_list[i]);
					strtok(temp," ");

					
					username_a = strtok(NULL," ");
					username_b = strtok(NULL," ");
					transfer_amount = strtok(NULL," ");

					if(strcmp(username_a,recv_list[0]) ==0){
						user_exist = -1;
						for(int j = 0; j < statslist_length; j = j + 1){
							if(strcmp(username_b, statslist[j].username) == 0){
								user_exist = j;
								break;
							}
							
						}
						if(user_exist == -1){
							strcpy(statslist[statslist_length].username,username_b);
							statslist[statslist_length].trans_number = 1;
							statslist[statslist_length].trans_amount = atoi(transfer_amount);
							statslist_length = statslist_length + 1;
						}else{
							statslist[user_exist].trans_number = statslist[user_exist].trans_number + 1;
							statslist[user_exist].trans_amount = statslist[user_exist].trans_amount + atoi(transfer_amount);
						}

		
					}
					if(strcmp(username_b,recv_list[0]) ==0){

						user_exist = -1;
						for(int j = 0; j < statslist_length; j = j + 1){
							if(strcmp(username_a, statslist[j].username) == 0){
								user_exist = j;
								break;
							}
							
						}
						if(user_exist == -1){
							strcpy(statslist[statslist_length].username,username_a);
							statslist[statslist_length].trans_number = 1;
							statslist[statslist_length].trans_amount = atoi(transfer_amount) * -1;
							statslist_length = statslist_length + 1;
						}else{
							statslist[user_exist].trans_number = statslist[user_exist].trans_number + 1;
							statslist[user_exist].trans_amount = statslist[user_exist].trans_amount - atoi(transfer_amount);
						}
						
					
					}


    			}

    			//sort item by number of trans

				char temp_username[512];
				int temp_trans_amount;
				int temp_trans_number;
				for (int ii = 0; ii < statslist_length - 1; ii = ii + 1){
					for (int jj = 0; jj < statslist_length - 1 - ii; jj = jj + 1){
				    	if (statslist[jj].trans_number < statslist[jj+1].trans_number) {

				            strcpy(temp_username, statslist[jj].username);
				            strcpy(statslist[jj].username, statslist[jj+1].username);
				            strcpy(statslist[jj+1].username, temp_username);
				            temp_trans_amount = statslist[jj].trans_amount;
				            statslist[jj].trans_amount = statslist[jj+1].trans_amount;
				            statslist[jj+1].trans_amount = temp_trans_amount ;
				            temp_trans_number = statslist[jj].trans_number;
				            statslist[jj].trans_number = statslist[jj+1].trans_number;
				            statslist[jj+1].trans_number = temp_trans_number ;
				        }
				    }
				}
				if(statslist_length != 0){
					memset(send_to_client_message,'\0',MAXDATASIZE) ;
					strcpy(send_to_client_message, recv_list[0]);
					strcat(send_to_client_message," statistics are the following.:\n");
					strcat(send_to_client_message,"Rank\tUsername\tNumofTranscions\tTotal\n");
					char mess[640];
	    			for(int i=0; i < statslist_length; i++){
	    				memset(mess,'\0',640);
	    				sprintf(mess,"%d\t%s\t%d\t%d\n", i + 1, statslist[i].username, statslist[i].trans_number,statslist[i].trans_amount);
	    				strcat(send_to_client_message,mess);
	    			}
	    			if(send_to_client_message[strlen(send_to_client_message)-1] == '\n'){

	    				send_to_client_message[strlen(send_to_client_message)-1] = '\0';
	    			}
				}else{
					sprintf(send_to_client_message, "Unable to proceed with the transaction as \"%s\" is not part of the network.", recv_list[0]);
				}



    			free(statslist);
    			free(buf_list);



				if (send(new_fd, send_to_client_message, strlen(send_to_client_message), 0) == -1){
                	perror("send");
				}else{
					printf("The stats data sent to client %s\n", client_name);
				}

			}

			//free(recv_buf);


            close(new_fd);
            exit(0);
        }
        close(new_fd);  // parent doesn't need this
    }

    return 0;
}




int main(){

	printf("The main sever is up and running.\n");
	
	if(fork()==0){
		run_the_connection(TCPAPORT, "A");

	}else{

		run_the_connection(TCPBPORT, "B");
	}
	


	//run_the_connection(TCPAPORT, "A");
	return 0;

}

