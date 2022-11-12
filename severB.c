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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define LOCALHOST "127.0.0.1"

#define MYSEVER "severB"
#define MYPORT "22713"    // the port users will be connecting to
#define FILENAME "block2.txt"
#define SERVERPORT "24713" //severM Port

#define MAXBUFLEN 10000
#define MAXLINELEN 1280
#define MAXDATASIZE 10001
#define MAXROW 1000

//Use function int main() in talker.c from Beej's Guide to Network Programming
int send_message_to_main(char *send_message)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(LOCALHOST, SERVERPORT, &hints, &servinfo)) != 0) {
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

    freeaddrinfo(servinfo);

    close(sockfd);

    return 0;
}
//Use function get_in_addr(struct sockaddr *sa) in listener.c from Beej's Guide to Network Programming
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
//Use function int main() in listener.c from Beej's Guide to Network Programming
int listen_to_main()
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[MAXBUFLEN];
    socklen_t addr_len;
    //char s[INET6_ADDRSTRLEN];
    //char *recv_list[3];
    //char *recv_item;
    //int recv_num = 0;
    FILE *fp = NULL;


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(LOCALHOST, MYPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
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
        return 2;
    }

    freeaddrinfo(servinfo);
    while(1){
   
        addr_len = sizeof their_addr;
        if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
            (struct sockaddr *)&their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(1);
        }

        /*inet_ntop(their_addr.ss_family,
                get_in_addr((struct sockaddr *)&their_addr),
                s, sizeof s);*/
        printf("The %s received a request from the Main Server.\n",MYSEVER);

        

        buf[numbytes] = '\0';
        //split message
        /*recv_num = 0;
        recv_item = strtok(buf,":");

        while(recv_item != NULL){
            recv_list[recv_num] = recv_item;
            recv_item = strtok(NULL,":");
            recv_num = recv_num + 1;

        }*/

        //open block file
        // fp = fopen(FILENAME, "r");
        // char file_lines[MAXBUFLEN];
        // strcpy(file_lines,"");
        // char file_line_buff[200];
        // while(fgets(file_line_buff, 200, (FILE*)fp) != NULL && strlen(file_line_buff) != 0 && file_line_buff[0] != '\n'){
            
        //     if(file_line_buff[strlen(file_line_buff) - 1] != '\n') {
        //         strcat(file_line_buff,"\n");
        //     }
        //     strcat(file_lines, file_line_buff);

        // }
        // fclose(fp);

/*
        fp = fopen(FILENAME, "r");
        char file_lines[MAXDATASIZE];
        strcpy(file_lines,"");
        char file_line_buff[MAXLINELEN];

        while(fgets(file_line_buff, MAXLINELEN, (FILE*)fp) != NULL && strlen(file_line_buff) != 0 && file_line_buff[0] != '\n'){

            if(file_line_buff[strlen(file_line_buff) - 1] != '\n') {
                strcat(file_line_buff,"\n");
            }
            strcat(file_lines, file_line_buff);

            if(strlen(file_lines) > MAXDATASIZE - MAXLINELEN){


            }

        }
        fclose(fp);
*/

        //GET DATA
        if (strcmp(buf,"GETDATA") == 0){

            fp = fopen(FILENAME, "r");
            char file_lines[MAXDATASIZE];
            strcpy(file_lines,"");
            char file_line_buff[MAXLINELEN];
            if(fp){
                while(fgets(file_line_buff, MAXLINELEN, (FILE*)fp) != NULL && strlen(file_line_buff) != 0 && file_line_buff[0] != '\n'){

                    if(file_line_buff[strlen(file_line_buff) - 1] != '\n') {
                        strcat(file_line_buff,"\n");
                    }
                    strcat(file_lines, file_line_buff);

                    if(strlen(file_lines) > MAXDATASIZE - MAXLINELEN - 1){
                        if ((numbytes = sendto(sockfd, file_lines, strlen(file_lines), 0,(struct sockaddr *)&their_addr, addr_len)) == -1) {
                            perror("talker: sendto");
                        }
                        strcpy(file_lines,"");

                    }
                    

                }
                fclose(fp);

            }
            strcat(file_lines,"@");
            
            //send_message_to_main(file_lines);
            if ((numbytes = sendto(sockfd, file_lines, strlen(file_lines), 0,
            (struct sockaddr *)&their_addr, addr_len)) == -1) {
                perror("talker: sendto");
            }
            

            printf("The %s finished sending the response to the Main Server.\n", MYSEVER);


        }else{

            //send_message_to_main(file_lines);
            //open block file
            fp = fopen(FILENAME, "r");
            char file_line_buff[MAXLINELEN];
            char* recv = (char*) malloc(MAXLINELEN * MAXROW * sizeof(char));
            strcpy(recv,"");
            while(fgets(file_line_buff, MAXLINELEN, (FILE*)fp) != NULL && strlen(file_line_buff) != 0 && file_line_buff[0] != '\n'){

                if(file_line_buff[strlen(file_line_buff) - 1] != '\n') {
                    strcat(file_line_buff,"\n");
                }
                strcat(recv,file_line_buff);
                memset(file_line_buff, '\0', sizeof(file_line_buff));

            }

            fclose(fp);


            fp = fopen(FILENAME, "w");
            fprintf(fp, "%s%s\n", recv, buf);
            fclose(fp);
            free(recv);

            if ((numbytes = sendto(sockfd, "SUCCESS@", strlen("SUCCESS@"), 0,
            (struct sockaddr *)&their_addr, addr_len)) == -1) {
                perror("talker: sendto");
                exit(1);
            }

            printf("The %s finished sending the response to the Main Server.\n", MYSEVER);



        }





        /*if (recv_num == 1 && strcmp(recv_list[0],"TXLIST") != 0 ){
            printf("Check balance\n");


        }
        
        //Transfer money
        if (recv_num == 3){
            printf("Transfer money\n");
        
        }

        //txlist
        if (recv_num == 1 && strcmp(recv_list[0],"TXLIST") == 0 ){
            printf("txlist\n");

        }

        //stats
        if (recv_num == 2 && strcmp(recv_list[1],"stats") == 0 ){
            printf("stats\n");

        }*/

        //close block file
        

    }
    close(sockfd);
    return 0;
}




int main(){

    printf("The %s is up and running using UDP on port %s.\n", MYSEVER, MYPORT);
    listen_to_main();

}