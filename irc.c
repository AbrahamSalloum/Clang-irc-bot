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
int hostip(char hostname[1024], char *ip);
void getchan(char join[], char *chan, int irc_sock);  
int printt( char read[512]); 
pthread_mutex_t mutex ; 
int main(int argc, char *argv[]){
if (argc != 3) {
        printf("Specify host and port: ./a.out <hostname> <port>\n"); 
        return 1;
    }
char chan[50]; char ip[100]; char message[512]; char authnick[15]; 
char authuser[35]; int irc_sock; int ret_tprntmsg;
struct sockaddr_in server;

	if((irc_sock = socket(AF_INET,SOCK_STREAM,0)) <0){
	printf("Could not create socket\n");
	}

if ((hostip(argv[1],ip)) == 1) { return 0; };  
server.sin_addr.s_addr = inet_addr(ip);
server.sin_family = AF_INET;
server.sin_port = htons(atoi(argv[2]));

	if(connect(irc_sock, (struct sockaddr *)&server, sizeof(server))){
	printf("connect error\n");
	}

pthread_t tprntmsg;
ret_tprntmsg = pthread_create(&tprntmsg,NULL,prntmsg,(void*)irc_sock);
sleep(1); 
snprintf(authnick, sizeof(authnick), "NICK %s\n\r", getenv("USER")); 
snprintf(authuser, sizeof(authuser), "USER %s 8 * nota not\n\r", getenv("USER")); 
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
				else if(!(strncmp(message, "LIST", 4))){
				sayraw(message, irc_sock);
				}	
					else if(!(strncmp(message, "WHOIS", 5))){
					sayraw(message, irc_sock); 
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
	printt(read);
		if(!strncmp(read,"PING",4)){
			read[1] = 'O';
			sayraw(read,irc_sock);
			memset(read, 0, sizeof(read));
		}	
	memset(read, 0, sizeof(read));
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

int hostip(char hostname[1024], char *ip){ //crap error checking!!
struct hostent *he;     
struct in_addr **addr_list;   
he = gethostbyname( hostname );
if(he == NULL){ printf("No IP\n"); return 1; } 
addr_list = (struct in_addr **) he->h_addr_list;
strcpy(ip, inet_ntoa(*addr_list[0]));
return 0; 
}

int printt(char read[512]){ //Not very good-terribe. Make better soon. Breaks login srvmsgs. avoid regex
	if(!(strncmp(read,"PING",4))){ 
	return 0; }
	int y = 1; int x=1;
        while(read[x] != '!') { 
       		printf("%c", read[x++]); 
		if(x == strlen(read)){break;} 
	}	
	while(read[y] != ':'){ 
	y++; 
	}
        while(y<strlen(read)){ 
	printf("%c", read[y++]); 
	}
return 0; 
}

