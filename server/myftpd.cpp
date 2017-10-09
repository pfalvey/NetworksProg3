#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>
#include <dirent.h>
#define MAX_PENDING 5
#define MAX_LINE 256
    
std::string get_permissions(std::string file);
std::string dir_list();
void deleteFile(int s, char * buf);

int main(int argc, char * argv[])
{
    int SERVER_PORT;
    /* get port number from cmd line arguments */
    if (argc == 2)
	{
        SERVER_PORT = atoi(argv[1]);
    }
    else 
	{
        std::cout<<"Usage: ./myftpd [PORT NUMBER]\n";
        exit(1);
    }
    struct sockaddr_in sin;
    char buf[MAX_LINE];
    socklen_t len;
    int s, new_s;
    /* build address data structure */
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(SERVER_PORT);
    /* setup passive open */
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("simplex-talk: socket");
        exit(1);
    }
    // set socket option
    int opt = 1;
    if ((setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)& opt, sizeof(int)))<0)
    {
        perror ("simplex-talk:setscokt");
        exit(1);
    }
    if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) 
    {
        perror("simplex-talk: bind"); exit(1);
    }
    if ((listen(s, MAX_PENDING))<0)
    {
        perror("simplex-talk: listen"); exit(1);
    } 
    printf("Welcome to the TCP Server!\n");
    /* wait for connection, then receive and print text */
    while(1) 
    {
        if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0) 
		{
            perror("simplex-talk: accept");
            exit(1);
		}

        while (1)
		{
            if((len=recv(new_s, buf, sizeof(buf), 0))==-1)
	    	{
                perror("Server Received Error!");
                exit(1);
	    	}
            if (len==0) break;
            printf("TCP Server Received: %s", buf);
            char * tok = strtok(buf, " ");
			if(strcmp(buf, "LIST"))
			{
				std::string response = dir_list();
				std::cout << response << std::endl;
				if(send(new_s, response.c_str(), strlen(response.c_str()), 0) == -1)
				{
					perror("Error sending data to client");
					exit(1);
				}
			}
            else if (strcmp(tok, "DELF") == 0){
                deleteFile(new_s, buf);

            }
		}
        printf("Client finishes, close the connection!\n");
        close(new_s);
    }
}


std::string dir_list()
{
	DIR *dir;
	struct dirent *ent;
	std::string permissions;
	std::string response = "";
	//open the directory and print all files/subdirs
	if((dir = opendir(".")) != NULL)
	{
		while((ent = readdir(dir)) != NULL)
		{
			//generate permissions + filenames into string
			//to send back to the client
			permissions = get_permissions(ent->d_name);
			response += permissions;
			response += " ";
			response += ent->d_name;
			response += "\n";			
			//std::cout << permissions << " ";
			//std::cout << ent->d_name << std::endl;
		}
		closedir(dir);
	}
	else
	{
		perror("");
	}
	//std::cout << response;
	return response;
}

std::string get_permissions(std::string file)
{
	struct stat st;
	std::string perms = "---------";
	if(stat(file.c_str(), &st) == 0)
	{
		//check read/write/exec permissions 
		//for user, group, and other
		//set appropriate letter, or - as necessary
		mode_t p = st.st_mode;
		perms[0] = (p & S_IRUSR) ? 'r' : '-';
		perms[1] = (p & S_IWUSR) ? 'w' : '-';		
		perms[2] = (p & S_IXUSR) ? 'x' : '-';
		perms[3] = (p & S_IRGRP) ? 'r' : '-';
		perms[4] = (p & S_IWGRP) ? 'w' : '-';		
		perms[5] = (p & S_IXGRP) ? 'x' : '-';
		perms[6] = (p & S_IROTH) ? 'r' : '-';
		perms[7] = (p & S_IWOTH) ? 'w' : '-';		
		perms[8] = (p & S_IXOTH) ? 'x' : '-';
		return perms;
	}		
	else
	{
		std::cout << "Error getting permissions for file " << file << std::endl;
	}		

}

void deleteFile(int s, char * buf){
    
}
