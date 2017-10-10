#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#define MAX_LINE 256


void delf(std::string command, int s);
void makeDir(std::string command, int s);

int main(int argc, char * argv[])
{
    int SERVER_PORT;
    FILE *fp;
    struct hostent *hp;
    struct sockaddr_in sin;
    char *host;
    char buf[MAX_LINE];
    int s;
    int len;
    std::string response;
    if (argc==3) 
    {
        host = argv[1];
        SERVER_PORT = atoi(argv[2]);
    }
    else 
    {
        fprintf(stderr, "usage: ./myftp [SERVER NAME] [PORT]\n");
        exit(1);
    }

    /* translate host name into peer's IP address */
    hp = gethostbyname(host);
    if (!hp) 
    {
        fprintf(stderr, "simplex-talk: unknown host: %s\n", host);
        exit(1);
    }

    /* build address data structure */
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
    sin.sin_port = htons(SERVER_PORT);
    /* active open */
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("simplex-talk: socket"); exit(1);
    }
    
    printf("Welcome to the TCP client! To quit, type \'Exit\'\n");
    //add menu funtionality here

    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
      perror("simplex-talk: connect");
      close(s); exit(1);
    }

    /* main loop: get and send lines of text */
    while (fgets(buf, sizeof(buf), stdin)) 
    {
        //display menu
        buf[MAX_LINE-1] = '\0';
        if (!strncmp(buf, "Exit",4))
	    {
            printf("Good Bye!\n");
            break;
	    }
        std::string temp(buf);
        char * tok = strtok(buf, " ");
        if (!strncmp(tok, "DELF", 4))
        {
            delf(temp, s);
            continue;
        }
        if (!strncmp(tok, "MDIR", 4)){
            makeDir(temp, s);
            continue;
        }

        len = strlen(buf) + 1;
        if(send(s, buf, len, 0)==-1)
	    {
            perror("client send error!"); exit(1);
        }
        if(!strncmp(tok, "LIST", 4)){
            //wait for server response
            if(recv(s, buf, sizeof(buf), 0) == -1)
            {
                perror("Error receiving data from server");
            }
            else
            {
                std::cout << buf << std::endl;
            }
        }

	

        bzero((char *)&buf, sizeof(buf));
	//wait for server response
	/*if(recv(s, buf, sizeof(buf), 0) == -1)
	{
		perror("Error receiving data from server");
	}
	else
	{
		std::cout << buf << std::endl;
	}*/

	
        bzero((char *)&buf, sizeof(buf));

    }
    close(s);
}


void delf(std::string command, int s){
    
    std::stringstream ss;
    ss.str(command);
    std::string delf;
    std::string length;
    std::string fileName;
    
    ss >> delf;
    ss >> fileName;
    length = std::to_string(fileName.size());
    std::string message = delf + " " + length + " " +fileName;

    char temp[BUFSIZ];
    bzero((char *)&temp, sizeof(temp));
    message.copy(temp, BUFSIZ);

    //const char * temp = message.c_str();
    int tempLen = strlen(temp) + 1;
    temp[tempLen-1] = '\0';
    if(send(s, temp, tempLen, 0) == -1)
        {
            perror ("Client Send Error!\n");
            exit(1);
        }
    char buf[BUFSIZ];
    bzero((char *)&buf, sizeof(buf));
    if(recv(s, buf, sizeof(buf), 0) == -1)
	{
		perror("Error receiving data from server\n");
	}
    if (strcmp(buf, "1") == 0){
        std::cout<<"Are you sure you want to delete the file " <<fileName<<"?\nEnter \"Yes\" to delete or \"No\" to ignore:";
        std::string result;
        std::cin >> result;
        if (result.compare("Yes") == 0){
            char confirm[BUFSIZ] = "Yes\0";
            int conLen = 4;
            if(send(s, confirm, conLen, 0) == -1){
                perror("client send error!\n");
                exit(1);
            }
            bzero((char *)&confirm, sizeof(confirm));
            if (recv(s, confirm, sizeof(confirm), 0) == -1){
                perror("Error receiving data from server\n");
            }
            else {
                std::cout<<confirm<<std::endl;
            }
        }
        else {
            std::cout<<"Delete abandoned by the user!\n";
            char confirm[BUFSIZ] = "No\0";
            int conLen = 3;
            if(send(s, confirm, conLen, 0) == -1){
                perror("client send error!\n");
                exit(1);
            }
        }
    }
    else {
        std::cout<<"The file does not exist on server\n";
    }
    
}

void makeDir(std::string command, int s){
    std::stringstream ss;
    ss.str(command);
    std::string mdir;
    std::string length;
    std::string dirName;
    
    ss >> mdir;
    ss >> dirName;
    length = std::to_string(dirName.size());
    std::string message = mdir + " " + length + " " +dirName;

    char temp[BUFSIZ];
    bzero((char *)&temp, sizeof(temp));
    message.copy(temp, BUFSIZ);

    //const char * temp = message.c_str();
    int tempLen = strlen(temp) + 1;
    temp[tempLen-1] = '\0';
    if(send(s, temp, tempLen, 0) == -1)
        {
            perror ("Client Send Error!\n");
            exit(1);
        }
    char buf[BUFSIZ];
    bzero((char *)&buf, sizeof(buf));
    if(recv(s, buf, sizeof(buf), 0) == -1)
	{
		perror("Error receiving data from server\n");
	}
    if (strcmp(buf, "1") == 0){
        std::cout<<"The directory was successfully made\n";
    }
    else if (strcmp(buf, "-2") == 0){
        std::cout<<"The directory already exists on server\n";
    }
    else {
        std::cout<<"Error in making directory\n";
    }

}
