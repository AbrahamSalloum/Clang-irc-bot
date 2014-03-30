//quick IRC bot -2014 - ASalloum.
#include <stdio.h> 
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h> 
#include <time.h> 
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
	char network[1024];
	int port;

}ircdata;
ircdata irc[3];

int main(int argc, char *argv[]){

strcpy(irc[0].chan,"#sall");
strcpy(irc[0].network,"irc.undernet.org");
irc[0].port=6667;

strcpy(irc[1].chan,"#jack");
strcpy(irc[1].network,"irc.foonetic.net");
irc[1].port=6667;

char message[512]; char authnick[100]; 
char authuser[100]; int ret_tprntmsg[3]; int ret_tchkmsg[3]; 
struct sockaddr_in server;
pthread_t tprntmsg[3]; pthread_t tchkmsg[3];

int n; 

for(n=0;n<2;n++){


	if((irc[n].irc_sock = socket(AF_INET,SOCK_STREAM,0)) <0){
	printf("Could not create socket\n");
	}

if ((hostip(irc[n].network,irc[n].ip)) == 1) { return 0; };  
server.sin_addr.s_addr = inet_addr(irc[n].ip);
server.sin_family = AF_INET;
server.sin_port = htons(irc[n].port);

	if(connect(irc[n].irc_sock, (struct sockaddr *)&server, sizeof(server))){
	printf("connect error\n");
	}
ret_tprntmsg[n] = pthread_create(&tprntmsg[n],NULL,prntmsg,(void*)irc[n].irc_sock);
ret_tchkmsg[n]  = pthread_create(&tchkmsg[n],NULL,chkmsg,(void*)&irc[n]);
time_t timer = time(0);  
snprintf(authnick, sizeof(authnick), "NICK %s%ld\n\r", getenv("USER"),timer); 
snprintf(authuser, sizeof(authuser), "USER %s%ld 8 * Jack U. Lemmon\n\r", getenv("USER"),timer); 
	sayraw(authnick,irc[n].irc_sock);
	sayraw(authuser,irc[n].irc_sock);
	char chanjoin[155]; 
	snprintf(chanjoin, sizeof(chanjoin), "JOIN %s\r\n", irc[n].chan); 
	sleep(20); 
	sayraw(chanjoin, irc[n].irc_sock);

}

while(1){ }

close(irc[0].irc_sock);
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
//printf("%s", read);
return 0;
}
//printf("\x1B[36m"); //cyan
int n = strcspn(read, "!"); int x;
for(x = 1; x<n; x++){
//printf("%c", read[x]);
}
//printf("\x1B[0m"); //normal
char *msg = (char *)malloc(512);
msg = strchr(read+1, ':');
//strncpy(last, msg, sizeof(last)); 	
//printf("%s",read);
return 0;
}

void* chkmsg(void  *ircs){
//struct ircdata *irc = (struct ircdata*)(ircs);
//	while(1){
//		if(!strncmp(last+1,"hi",2)){
//		mesg("sup",irc->chan,irc->irc_sock);
//		memset(last,'\0',512);
//		}
//	}
return 0;
}
