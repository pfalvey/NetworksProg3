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
#include <algorithm>
#define MAX_PENDING 5
#define MAX_LINE 256
    
std::string get_permissions(std::string file);
std::string dir_list();
void deleteFile(int s, std::string buf);

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
		std::string current_dir = ".";
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
            bzero((char *)&buf, sizeof(buf));
            if((len=recv(new_s, buf, sizeof(buf), 0))==-1)
	    			{
                perror("Server Received Error!");
                exit(1);
	    			}
            if (len==0) break;
            printf("TCP Server Received: %s", buf);

			std::string temp = buf;
			temp.erase(std::remove(temp.begin(), temp.end(), '\n'), temp.end());
			//std::cout << temp.compare("LIST") << std::endl;
			//if(strcmp(buf, "LIST"))
      char * tok = strtok(buf, " ");
			std::cout << "tok is " << tok << " and temp is " << temp << std::endl;
			if(strcmp(tok, "CDIR") == 0)
			{
				//scrub first 5 characters, i.e. "CDIR "
				//all that's left is command
				temp.erase(0, 5);
				chdir(temp.c_str());
				std::string response = "Working directory changed to ";
				response += temp;
				if(send(new_s, response.c_str(), strlen(response.c_str()), 0) == -1)
				{
					perror("Error sending response to client");
					exit(1);
				}
			}
			else if(temp == "LIST")			
			{
				std::string response = dir_list();
				if(send(new_s, response.c_str(), strlen(response.c_str()), 0) == -1)
				{
					perror("Error sending data to client");
					exit(1);
				}
			}
            else if (strcmp(tok, "DELF") == 0){
                std::cout<<"\n";
                deleteFile(new_s, temp);

            }
            bzero((char *)&buf, sizeof(buf));
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

void deleteFile(int s, std::string buf){
    std::stringstream ss;
    ss << buf;
    std::string command;
    std::string size;
    std::string file;
    ss >> command;
    ss >> size;
    ss >> file;

    DIR *dir;
	struct dirent *ent;
	//open the directory and check for matching file
	if((dir = opendir(".")) != NULL)
	{
        int exists = -1;
		while((ent = readdir(dir)) != NULL)
		{
			if(file.compare(ent->d_name) == 0){
                exists = 1;
            }
		}
        std::string result = "-1";
        if (exists == 1){
            result = "1";
        }
        char buffer[BUFSIZ];
        bzero((char *)&buffer, sizeof(buffer));
        result.copy(buffer, BUFSIZ);
        int bufferLen = strlen(buffer) + 1;
        buffer[bufferLen - 1] = '\0';
        if (send(s, buffer, bufferLen, 0) == -1){
            perror ("Server send error!\n");
            exit(1);
        }
        if (result.compare("1") == 0){
            char mesg[BUFSIZ];
            bzero((char *)&mesg, sizeof(mesg));
            if (recv(s, mesg, sizeof(mesg), 0) == -1){
                perror("Error receiving message from client\n");
                exit(1);
            }
            else {
                if (strcmp(mesg, "Yes") == 0){
                //delete file
                    if (remove(file.c_str()) != 0){
                        char last[BUFSIZ] = "Unable to remove file\n\0";
                        int lastLen = strlen(last) + 1;
                        if (send(s, last, lastLen, 0) == -1){
                            perror ("Server send error!\n");
                            exit(1);
                        } 
                    } else {
                        char last[BUFSIZ] = "File successfully removed\n\0";
                        int lastLen = strlen(last) + 1;
                        if (send(s, last, lastLen, 0) == -1){
                            perror ("Server send error!\n");
                            exit(1);
                        }
                    }
                }
            }
        }
		closedir(dir);
        /*char last[BUFSIZ] = "\0";
        int lastLen = 1;
        if (send(s, last, lastLen, 0) == -1){
            perror("Server send error!\n");
            exit(1);
        }
        std::cout<<"Truuuuuuu\n";*/
	}
	else
	{
		perror("error openning directory");
	}

}
