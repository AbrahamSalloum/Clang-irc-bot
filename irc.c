//quick IRC bot -2014 - ASalloum.
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
int hostip(char hostname[1024], char *ip); //Super UGLY!!!
void getchan(char join[], char *chan, int irc_sock); 
int printt(char read[512]); 
char last[512]; 
void* chkmsg(void *sock);

typedef struct ircdata {
        int irc_sock;
        char chan[50];
	char ip[100];

}ircdata;
ircdata irc;

int main(int argc, char *argv[]){
if (argc != 3) {
        printf("Specify host and port: ./a.out <hostname> <port>\n"); 
        return 1;
    }

char message[512]; char authnick[15]; 
char authuser[35]; int ret_tprntmsg; int ret_tchkmsg; 
struct sockaddr_in server;

	if((irc.irc_sock = socket(AF_INET,SOCK_STREAM,0)) <0){
	printf("Could not create socket\n");
	}

if ((hostip(argv[1],irc.ip)) == 1) { return 0; };  
server.sin_addr.s_addr = inet_addr(irc.ip);
server.sin_family = AF_INET;
server.sin_port = htons(atoi(argv[2]));

	if(connect(irc.irc_sock, (struct sockaddr *)&server, sizeof(server))){
	printf("connect error\n");
	}
pthread_t tprntmsg; pthread_t tchkmsg;
ret_tprntmsg = pthread_create(&tprntmsg,NULL,prntmsg,(void*)irc.irc_sock);
ret_tchkmsg  = pthread_create(&tchkmsg,NULL,chkmsg,(void*)&irc); 
snprintf(authnick, sizeof(authnick), "NICK %s\n\r", getenv("USER")); 
snprintf(authuser, sizeof(authuser), "USER %s 8 * Jack U. Lemmon\n\r", getenv("USER")); 
	sayraw(authnick,irc.irc_sock);
	sayraw(authuser,irc.irc_sock);

while(1){ 
fgets(message, sizeof(message), stdin);

	if(!(strncmp(message,"JOIN",4))){
	sayraw(message, irc.irc_sock);
	getchan(message, irc.chan, irc.irc_sock);
	}	
		else if(!(strncmp(message,"QUIT",4))){
		sayraw(message, irc.irc_sock);
		break; 
		}
			else if(!(strncmp(message, "PRIVATE", 7))){
			getchan(message, irc.chan, irc.irc_sock); 
			}	
				else if(!(strncmp(message, "LIST", 4))){
				sayraw(message, irc.irc_sock);
				}	
					else if(!(strncmp(message, "WHOIS", 5))){
					sayraw(message, irc.irc_sock); 
					}	
						else if(!(strncmp(message, "NICK", 5))){
						sayraw(message, irc.irc_sock);
						}		
							else{ 
							mesg(message,irc.chan,irc.irc_sock);
							}					
}
close(irc.irc_sock);
return 0;
}
void* prntmsg(void *sock){
char read[512];
int irc_sock = (int)sock;
	while(1){
	memset(read, 0, sizeof(read)); 
	recv(irc_sock, read, sizeof(read), 0); //ALL data recv happens here
	printt(read);
		if(!strncmp(read,"PING",4)){ //repy to PING with PONG -ASAP
			read[1] = 'O';
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
char privmsg[512]; 
snprintf(privmsg, sizeof(privmsg), "PRIVMSG %s%s%s%s", chan, " :", write,"\r\n");
sayraw(privmsg,irc_sock); 
}

void getchan(char join[], char *chan, int irc_sock ){
char *hash = (char *)malloc(50);
hash = strchr(join, '#'); 
snprintf(chan,strlen(hash),hash); 
}

int hostip(char hostname[1024], char *ip){ //crap error checking!!
struct hostent *he;     
struct in_addr **addr_list;   
he = gethostbyname( hostname );
if(he == NULL){ printf("No IP\n"); return 1; } 
addr_list = (struct in_addr **) he->h_addr_list;
strcpy(ip, inet_ntoa(*addr_list[0]));
return 0; 
}

int printt(char read[512]){ //Not very good: part/join messages fail
	if(!(strncmp(read,"PING",4))){ return 0; }

if((strchr(read,'!'))==NULL){
printf("%s", read);
return 0;
}
printf("\x1B[36m"); //cyan
int n = strcspn(read, "!"); int x;
for(x = 1; x<n; x++){
printf("%c", read[x]);
}
printf("\x1B[0m"); //normal
char *msg = (char *)malloc(512);
msg = strchr(read+1, ':');
strncpy(last, msg, sizeof(last)); 	
printf("%s",last);
return 0;
}

void* chkmsg(void  *ircs){
struct ircdata *irc = (struct ircdata*)(ircs);
	while(1){
		if(!strncmp(last+1,"hi",2)){
		mesg("sup",irc->chan,irc->irc_sock);
		memset(last,'\0',512);
		}
	}
}
