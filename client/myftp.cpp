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


void delf(std::string command, int s, struct sockaddr_in sin);

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
        if (!strncmp(buf, "DELF test.txt", 13))
        {
            delf(temp, s, sin);
            continue;
        }

        len = strlen(buf) + 1;
        if(send(s, buf, len, 0)==-1)
	    {
            perror("client send error!"); exit(1);

	}

	bzero(buf, sizeof(buf));
	
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
    close(s);
}


void delf(std::string command, int s, struct sockaddr_in sin){
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
    message.copy(temp, BUFSIZ);

    //const char * temp = message.c_str();
    int tempLen = strlen(temp) + 1;
    temp[tempLen-1] = '\0';
    if(send(s, temp, tempLen, 0) == -1)
        {
            perror ("Client Send Error!\n");
            exit(1);
        }
    /*int len = strlen(command.c_str());
    std::string length = std::to_string(len);
    length.append("\n");
    length.append("\0");
    const char *temp = length.c_str();
    int tempLen = strlen(temp) + 1;
    if(send(s, temp, tempLen, 0) == -1)
        {
            perror ("Client Send Error!\n");
            exit(1);
        }
    command.append("\n");
    length.append("\0");
    const char *buf = command.c_str();
    len = strlen(buf) + 1;
    //std::cout<<len<<" "<<buf<<std::endl;
    if(send(s, buf, len, 0) == -1)
        {
            perror ("Client Send Error!\n");
            exit(1);
        }
    
    */

}
