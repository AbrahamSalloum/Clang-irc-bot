#include <stdio.h> 
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h> 
void* prntmsg(void *);
void sayraw(char*, int);
void mesg(char*, char* , int);
void hostip(char hostname[1024], char *ip);
void getchan(char join[], char *chan, int irc_sock);  

int main(int argc, char *argv[]){
char chan[50]; char ip[100]; char message[512]; int irc_sock; int ret_tprntmsg;
struct sockaddr_in server;

	if((irc_sock = socket(AF_INET,SOCK_STREAM,0)) <0){
	printf("Could not create socket\n");
	}

hostip(argv[1],ip);  
server.sin_addr.s_addr = inet_addr(ip);
server.sin_family = AF_INET;
server.sin_port = htons(atoi(argv[2]));

	if(connect(irc_sock, (struct sockaddr *)&server, sizeof(server))){
	printf("connect error\n");
	}

pthread_t tprntmsg;
ret_tprntmsg = pthread_create(&tprntmsg,NULL,prntmsg,(void*)irc_sock);
sleep(1); 
char authnick[]="NICK uiyonzhu\n\r";
char authuser[]="USER notabot 8 * nota bot\n\r";
	sayraw(authnick,irc_sock);
sleep(1);
	sayraw(authuser,irc_sock);

while(1){ 
fgets(message, sizeof(message), stdin);

	if(!(strncmp(message,"JOIN",4))){
	sayraw(message, irc_sock);
	getchan(message, chan, irc_sock);
	}	
		else if(!(strncmp(message,"QUIT",4))){
		sayraw(message, irc_sock);
		close(irc_sock);
		return 0; //only exit
		}
			else if(!(strncmp(message, "PRIVATE", 7))){
			getchan(message, chan, irc_sock); 
			}	
				else if(!(strncmp(message, "NICK", 4))){
				sayraw(message, irc_sock);
				}		
					else{ 
					mesg(message,chan,irc_sock);
					}					
}
}
void* prntmsg(void *sock){
char read[512];
int irc_sock = (int)sock;
	while(1){
	memset(read, 0, sizeof(read)); 
		recv(irc_sock, read, sizeof(read), 0); //ALL data recv happens here
		printf("%s",read);
		if(!strncmp(read,"PING",4)){
			read[1] = 'O';
			printf("%s", read);
			sayraw(read,irc_sock);
		}	
	}
}

void sayraw(char write[512], int irc_sock){
	if ((send(irc_sock, write, strlen(write),0))<0){ //ALL data sending happens here
	printf("error writing\n"); 
	}
}

void mesg(char write[512], char *chan, int irc_sock){ //public chan msg formatting
char privmsg[512] ; 
snprintf(privmsg, sizeof(privmsg), "PRIVMSG %s%s%s%s", chan, " :", write,"\r\n");
sayraw(privmsg,irc_sock); 
}

void getchan(char join[], char *chan, int irc_sock ){
char *hash;
hash=malloc(50*sizeof(char));
printf("%s\n", join); 
hash=strtok(join," \n");
hash=strtok(NULL, " \n");
strcpy(chan,hash); 
}

void hostip(char hostname[1024], char *ip){ //no error checking!!
struct hostent *he;     
struct in_addr **addr_list;   
he = gethostbyname( hostname );
addr_list = (struct in_addr **) he->h_addr_list;
strcpy(ip, inet_ntoa(*addr_list[0]));
}
