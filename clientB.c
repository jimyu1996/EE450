#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/*Use some code from Beej's Guide to Network Programming which
is Copyright © 2019 Brian “Beej Jorgensen” Hall.

url: https://beej.us/guide/bgnet
license: https://creativecommons.org/licenses/by-nc-nd/3.0/

*/


#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define LOCALHOST "127.0.0.1"

#define PORT "26713" // the port client will be connecting to 

#define MAXDATASIZE 10000 // max number of bytes we can get at once 
#define CLIENTNAME "client B"

//Use function get_in_addr(struct sockaddr *sa) in client.c from Beej's Guide to Network Programming
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//Use function int main() in client.c from Beej's Guide to Network Programming
int main(int argc, char *argv[])
{
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	//char s[INET6_ADDRSTRLEN];
	int argv_length;

	printf("The %s is up and running.\n",CLIENTNAME);

	

	//confrim input and ready to send message.
	if(argc == 2 && strcmp(argv[1],"TXLIST") != 0){
		printf("%s sent a balance enquiry request to the main server.\n", argv[1]);
	}else if(argc == 4){
		printf("%s has requested to transfer %s coins to %s.\n",argv[1],argv[3],argv[2]);
	}else if(argc == 2 && strcmp(argv[1],"TXLIST") == 0){
		printf("%s sent a sorted list request to the main server.\n", CLIENTNAME);
	}else if(argc == 3 && strcmp(argv[2],"stats") == 0){
		printf("%s sent a statistics enquiry request to the main server.\n", argv[1]);
	}else {
	    fprintf(stderr,"usage: clientB <username>\n");
		exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(LOCALHOST, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	// inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
	// 		s, sizeof s);
	// printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	
	argv_length = 0;
	for(int i = 1; i < argc; i = i + 1){
		argv_length = argv_length + strlen(argv[i]) + 1;

	}

	char temp_argv[argv_length];
	strcpy(temp_argv,"");
	
	for(int i = 1; i < argc; i = i + 1){
		
		strcat(temp_argv,argv[i]);
		strcat(temp_argv,":");
		

	} 
			
	if (send(sockfd, temp_argv, strlen(temp_argv), 0) == -1) {
	    	perror("send");
	    	exit(1);
	}/*else{
		//checckwallet username
		if(argc == 2 && strcmp(argv[1],"TXLIST") != 0){
			printf("%s sent a balance enquiry request to the main server.\n", argv[1]);
		}
		//transfer money
		if(argc == 4){
			printf("%s has requested to transfer %s coins to %s.\n",argv[1],argv[3],argv[2]);
		}
		//TXLIST
		if(argc == 2 && strcmp(argv[1],"TXLIST") == 0){
			printf("%s sent a sorted list request to the main server.\n", CLIENTNAME);
		}
		//stats
		if(argc == 3 && strcmp(argv[2],"stats") == 0){
			printf("%s sent a statistics enquiry request to the main server.\n", argv[1]);
		}
	}
	*/
	


	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}

	buf[numbytes] = '\0';

	printf("%s\n",buf);

	close(sockfd);

	return 0;
}
