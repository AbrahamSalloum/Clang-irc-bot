#include <stdio.h> 
#include <stdlib.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h> 
#include <time.h> 
#include <sys/stat.h>
#include <sys/types.h>
void* prntmsg(void *);
void sayraw(char*, int);
void mesg(char*, char* , int);
int hostip(void *ircs); //Super UGLY!!!
int getlast(void *ircs); 
void* chkmsg(void *sock);

typedef struct ircdata {
        int irc_sock;
        char chan[50];
	char ip[100];
	char network[1024];
	int port;
	char last[512];
	char read[512]; 
	FILE *log; 
	int ret_tprint; pthread_t tprint;

}ircdata;
ircdata irc[3];

int main(int argc, char *argv[]){
daemon(1,1); 
if(argc==2){
chdir(argv[1]);
}
strcpy(irc[0].chan,"#sall");
strcpy(irc[0].network,"irc.efnet.org");
irc[0].port=6667;

strcpy(irc[1].chan,"#sall");
strcpy(irc[1].network,"irc.foonetic.net");
irc[1].port=6667;

char authnick[100]; char authuser[100]; 
struct sockaddr_in server;

int n; 
if((mkdir("logs", S_IRWXU|S_IRWXG|S_IRWXO)) <0){
  if((chdir("logs") <0)){
	printf("could not write to folder, permissions?\n");
	fflush(stdout); return 0; 
  }
chdir("logs"); 
} 
for(n=0;n<2;n++){
	if((irc[n].irc_sock = socket(AF_INET,SOCK_STREAM,0)) <0){
	printf("Could not create socket\n"); fflush(stdout); return 0;
	}
char filepath[100];

snprintf(filepath, 100, "log%s.%s.%ld",irc[n].network,irc[n].chan,time(0)); 
irc[n].log= fopen(filepath,"w+"); 

if((hostip((void *)&irc[n])==1)) { return 0; };
server.sin_addr.s_addr = inet_addr(irc[n].ip);
server.sin_family = AF_INET;
server.sin_port = htons(irc[n].port);

	if(connect(irc[n].irc_sock,(struct sockaddr *)&server,sizeof(server))){
	printf("connect error\n"); fflush(stdout); return 0; 
	}
irc[n].ret_tprint = pthread_create(&(irc[n].tprint),NULL,prntmsg,(void*)&irc[n]);
time_t timer = time(0);  
snprintf(authnick, sizeof(authnick), "NICK %s%ld\n\r", getenv("USER"),timer); 
snprintf(authuser, sizeof(authuser), "USER %s%ld 8 * Abe S. Salloum\n\r", getenv("USER"),timer); 
	sleep(3);
	sayraw(authnick,irc[n].irc_sock);
	sleep(3); 
	sayraw(authuser,irc[n].irc_sock);
	char chanjoin[57]; 
	snprintf(chanjoin, sizeof(chanjoin), "JOIN %s\r\n", irc[n].chan); 
	sleep(5); 
	sayraw(chanjoin, irc[n].irc_sock);

}
pthread_exit(0);
return 0;
}


void* prntmsg(void *ircs){
struct ircdata *irc = (struct ircdata*)(ircs);
 while(1){
	recv(irc->irc_sock,irc->read,sizeof(irc->read),0); //ALL data recv happens here
	fwrite(&irc->read,1, strlen(irc->read), irc->log);
getlast((void*)irc);
chkmsg((void*)irc); 
		if(!strncmp(irc->read,"PING",4)){ //repy to PING with PONG -ASAP
			irc->read[1] = 'O';
			sayraw(irc->read,irc->irc_sock);
		}
	memset(irc->read,'\0', 512);  
}
	
}

void sayraw(char write[512], int irc_sock){
	if ((send(irc_sock, write, strlen(write),0))<0){ //ALL data sending happens here
	printf("error writing\n"); fflush(stdout);  
	}
}

void mesg(char write[512], char *chan, int irc_sock){ //public chan msg formatting
char privmsg[512]; 
snprintf(privmsg, sizeof(privmsg), "PRIVMSG %s%s%s%s", chan, " :", write,"\r\n");
sayraw(privmsg,irc_sock); 
}

int hostip(void *ircs){ //crap error checking!!
struct ircdata *irc = (struct ircdata*)(ircs);
struct hostent *he;     
struct in_addr **addr_list;   
he = gethostbyname( irc->network );
if(he == NULL){ printf("No IP\n"); return 1; } 
addr_list = (struct in_addr **) he->h_addr_list;
strcpy(irc->ip,inet_ntoa(*addr_list[0]));
return 0; 
}

int getlast(void *ircs){ //Not very good
struct ircdata *irc = (struct ircdata*)(ircs);
	if(!(strncmp(irc->read,"PING",4))){ return 0; }
if((strchr(irc->read,'!'))==NULL){
return 0;
}
char *msg = (char *)malloc(512);
msg = strchr(irc->read+1, ':');
strncpy(irc->last,msg,strlen(msg)); 	
return 0;
}

void* chkmsg(void *ircs){
struct ircdata *irc = (struct ircdata*)(ircs);
		if(!strncmp(irc->last+1,"hi",2)){
		mesg("sup",irc->chan,irc->irc_sock);
		irc->last[1]='\0';
		}
}
