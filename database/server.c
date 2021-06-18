#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#define PORT 8080

static  const  char *dirpath = "/home/kelvin/server";

int check(char string1[], char string2[]){
    for(int i=0; i<strlen(string2); i++){
        if(string1[i]!=string2[i]) return 0;
    }
    return 1;
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
    while (1) {
        if (listen(server_fd, 3) < 0) {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        valread = read( new_socket , buffer, 1024);
        char string1[2000],string2[100],path[200];
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
            for(int i=0;i<sizeof buffer; i++){
                buffer[i]='\0';
            }
            for(int i=0;i<sizeof buffer; i++){
                path[i]='\0';
            }
            sprintf(path,"%s",dirpath);
        }

        sprintf(string2,"USE ");
        if(check(string1,string2)==1){
            char fpath[100];
            for(int i=0;buffer[4+i]!='\0';i++){
                fpath[i]=buffer[4+i];
            }
            char oripath[50];
            sprintf(oripath,"%s",dirpath);
            sprintf(path,"%s/%s",oripath,fpath);
        }
        printf("%s\n",buffer );
        send(new_socket , path , strlen(path) , 0 );
        printf("Hello message sent\n");
        sleep(5);
    }
    return 0;
}
