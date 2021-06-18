#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <time.h>
#include <wait.h>
#include <dirent.h>
#include <stdbool.h> 
#define PORT 8080
#define REGISTER 0
#define LOGIN 1
#define ADD 0

typedef struct Acount {
	char user[101], pass[101];
}akun;

static  const  char *dirpath = "/home/kelvin/server/databases";
akun seseorang;

int check(char string1[], char string2[]){
    for(int i=0; i<strlen(string2); i++){
        if(string1[i]!=string2[i]) return 0;
    }
    return 1;
}

void create_account(char *query) {
	
	char command[111], username[111], password[111];
	char buffer[1024];
	sscanf(query,"%s %s %s %s %s %s", command, command, username, command, command, password);
	FILE *f;
	f = fopen("/home/kelvin/server/databases/acount.scsv","r");
	bool ada = false;
	while (fscanf(f,"%s",buffer) !=EOF) {
		const char pemisah[1]= ";";
		char *clean;
		
		clean = strtok(buffer, pemisah);
		char user[1001], pass[1001];
		strcpy(user,clean);
		
		while(clean !=NULL) {
			clean=strtok(NULL, pemisah);
			if (strlen(pass)==0) strcpy(pass,clean);
		
		}
		if (strcmp(username,user) == 0 ) {
			ada = true;
			break;
		}
	}
	fclose(f);
	if (ada) printf ("User Telah Ada!");
	else {
		printf ("User Registered!");
		f = fopen("/home/kelvin/server/databases/acount.scsv","a");
		fprintf(f,"%s;%s\n",username,password);
		fclose(f);
	}
} 

void write_log(char *command){
	time_t timer;
	char buffer[5010];
	struct tm* tm_info;

	timer = time(NULL);
	tm_info = localtime(&timer);
	strftime(buffer,5010,"%Y-%m-%d %X:",tm_info);
	strcat(buffer,seseorang.user);
	strcat(buffer,":");
	strcat(buffer,command);
	FILE* f;
	f = fopen("a.log","a");
	fprintf(f,"%s\n",buffer);
	fclose(f);
}

int main(int argc, char const *argv[]) {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }


    pid_t pid, sid;  
    pid = fork();  
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
    umask(0);
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }
    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    bool logged = false;
    
    while (1) {
    
    	if (!logged) {
    		if (recv(new_socket,(void*)&seseorang,sizeof(seseorang),0) < 0) {
				perror("Login Attempt Failed");
				exit(EXIT_FAILURE);
			}
    		
    		FILE *f;
    		f = fopen("/home/kelvin/server/databases/acount.scsv","a+");
    		bool ada = false, benar = false;
    		while (fscanf(f, "%s", buffer) != EOF) {
    			const char pembatas[1] = ";";
    			char user[1001], pass[1001];
    			char *clean;
    			clean = strtok (buffer,pembatas);
    			strcpy(user,clean);
    			strcpy(pass,"");
    			
    			if(strcmp(user,seseorang.user)==0) {
    				ada=true;
    				if (strcmp(pass,seseorang.pass)==0);
    				break;
    			}    		
    		}
    	fclose(f);	
    	}
    	
    	
        if (listen(server_fd, 3) < 0) {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        valread = read( new_socket , buffer, 1024);
        char string1[2000],string2[100],path[100];
        sprintf(string1,"%s",buffer);

        sprintf(string2,"CREATE DATABASE ");
        if(check(string1,string2)==1){
            char filename[50];
            for(int i=0;buffer[16+i]!='\0';i++){
                filename[i]=buffer[16+i];
            }
            char fpath[500];
            sprintf(fpath,"%s/%s",dirpath,filename);
            mkdir(fpath,S_IRWXU);
        }

        sprintf(string2,"USE ");
        if(check(string1,string2)==1){
            char fpath[50];
            for(int i=0;buffer[4+i]!='\0';i++){
                fpath[i]=buffer[4+i];
            }
            char oripath[50];
            sprintf(oripath,"%s",dirpath);
            sprintf(path,"%s/%s",oripath,fpath);
            
        }
        printf("%s\n",buffer );
        send(new_socket ,buffer , strlen(buffer) , 0 );
        memset(buffer,'\0',sizeof(buffer);
        printf("Hello message sent\n");
        sleep(5);
    }
    return 0;
}
