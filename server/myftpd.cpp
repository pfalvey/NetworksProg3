/*
 *    Computer Networks: Programming Assignment 3 - Server
 *  
 *    Team: Patrick Falvey, Matt Flanagan, and Michael Farren
 *  
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sstream>
#include <dirent.h>
#include <algorithm>
#include <fstream>
#include <time.h>
#include <sys/time.h>
#define MAX_PENDING 5
#define MAX_LINE 256
    
std::string get_permissions(std::string file);
std::string dir_list();
void deleteFile(int s, std::string buf);
void makeDir(int s, std::string buf);
void dwld(int s, std::string buf);
void upld(int s, std::string buf);
void rdir(int s, std::string buf);

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
            //printf("TCP Server Received: %s", buf);

			std::string temp = buf;
			temp.erase(std::remove(temp.begin(), temp.end(), '\n'), temp.end());
			//std::cout << temp.compare("LIST") << std::endl;
			//if(strcmp(buf, "LIST"))
      char * tok = strtok(buf, " ");
			//std::cout << "tok is " << tok << " and temp is " << temp << std::endl;
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
      else if (strcmp(tok, "MDIR") == 0){
          std::cout<<"\n";
          makeDir(new_s, temp);
      }
	else if(strcmp(tok, "DWLD") == 0)
	{
		dwld(new_s, temp);	
	}
      else if (strcmp(tok, "UPLD") == 0)
      {
          upld(new_s, temp);
      }	
      else if (strcmp(tok, "RDIR") == 0)
      {
          rdir(new_s, temp);
      }

            bzero((char *)&buf, sizeof(buf));
		}
        printf("Client closed connection! Waiting for new client...\n");
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

void dwld(int s, std::string cmd)
{    
	std::stringstream ss;
    ss << cmd;
    std::string command;
    std::string size;
    std::string fileName;
    ss >> command;
    ss >> size;
    ss >> fileName;

	char buf[MAX_LINE];
	std::ifstream file(fileName.c_str(), std::ifstream::in | std::ifstream::binary);
	
	//check if the file exists and open it
	//std::cout << "file name is >>" << fileName << "<<" << std::endl;
	int fd = open(fileName.c_str(), O_RDONLY);
	if(fd == -1 || !file.good())
	{
		std::string response = "Could not open file ";
		response += fileName;
		if(send(s, response.c_str(), strlen(response.c_str()), 0) == -1)
		{
			perror("Could not send to client");
			return;
		}
	}

	//send filesize to client
	file.seekg(0, std::ios::end);
	int fileSize = file.tellg();
	int bytesToRead = fileSize;

	if(send(s, std::to_string(fileSize).c_str(), sizeof(fileSize), 0) == -1)
	{
		perror("Error sending filesize");
	}

	//file is open, read a chunk and then write it to the socket
	//this is done in a loop until the file is sent in its entirety

	while(1)
	{
		int numRead = read(fd, buf, sizeof(buf));
		//if we don't read anything, we are done
		if(numRead == 0) 
		{
			break;
		}

		if(numRead < 0)
		{
			perror("Could not read from file");
			int t = send(s, "-1", sizeof("-1"), 0);
			return;
		}

		//make this a pointer so we can increment starting point
		//if we have to do multiple writes
		void *message = buf;
		while(numRead > 0)
		{
			int numWritten = write(s, message, numRead);
			if(numWritten == 0)
			{
				break;
			}
			if(numWritten <= 0)
			{
				perror("Could not write to socket");
				return;
			}
			numRead -= numWritten;
			message += numWritten;
		}


	}
			
	file.close();

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

void makeDir(int s, std::string buf){
    std::stringstream ss;
    ss << buf;
    std::string command;
    std::string size;
    std::string dirName;
    ss >> command;
    ss >> size;
    ss >> dirName;

    DIR *dir;
	struct dirent *ent;
	//open the directory and check for matching file
	if((dir = opendir(".")) != NULL)
	{
        int exists = 1;
		while((ent = readdir(dir)) != NULL)
		{
			if(dirName.compare(ent->d_name) == 0){
                exists = -2;
            }
		}
        std::string result = "-1";
        if (exists == 1){
            result = "1";
        }
        if (exists == -2){
            result = "-2";
        }
        if (exists == 1){
        //create directory
            if(mkdir(dirName.c_str(), 0755) == -1){
                result = "-1";
                perror("here");
            }
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

		closedir(dir);
	}
	else
	{
		perror("error openning directory");
	}
}

void upld(int s, std::string buf)
{
    /* Set Strings */
    std::stringstream ss;
    ss.str(buf);
    std::string command;
    std::string length;
    std::string filename;

   /* Determine Size of Filename and Filename */
    ss >> command;
    ss >> length;
    ss >> filename;

    /* Send Back Acknowledgement */
    char ackn[BUFSIZ] = "Length of filename and filename received\n\0";
    if (send(s, ackn, strlen(ackn) + 1, 0) == -1)
    {   
        perror("Server Send Error!\n");
        exit(1);
    }

    /* Receive Size of File */
    char filesize[BUFSIZ];
    bzero((char *)&filesize, sizeof(filesize));
    if (recv(s, filesize, sizeof(filesize), 0) == -1)
    {   
        perror("Error Receiving File Size From Client\n");
        exit(1);
    }

    long int iFileSize = atoi(filesize);
    if (iFileSize == -1) { return; }
    int bytesToRead = iFileSize;

    /* Create File */
    std::string delimeter = "/";
    size_t pos = 0;
    std::string token;
    while ((pos = filename.find(delimeter)) != std::string::npos)
    {   
        token = filename.substr(0, pos);
        filename.erase(0, pos + delimeter.length());
    }

    std::ofstream file;
    file.open(filename);

    /* Loop to Receive File and Time Process */
    struct timeval tv1;
    gettimeofday(&tv1, NULL);
    double time1 = tv1.tv_sec *(int)1e6 + tv1.tv_usec;

    char recvBuf[BUFSIZ];
    while (1)
    {
        bzero((char *)&recvBuf, sizeof(recvBuf));

        int numRead = read(s, recvBuf, sizeof(recvBuf));

        bytesToRead -= numRead;

        if (recvBuf == "-1") { break; }

        if (numRead < 0)
        {
            perror("Could not receive file");
            return;
        }

        file.write(recvBuf, numRead);
        if (bytesToRead <= 0) { break; }
    }

    file.close();

    struct timeval tv2;
    gettimeofday(&tv2, NULL);
    double time2 = tv2.tv_sec *(int)1e6 + tv2.tv_usec;

    /* Compute and Send Throughput to Servier */
    double elapsed_d = (time2 - time1);
    elapsed_d = elapsed_d / (int)1e6;
    std::string elapsed = std::to_string(elapsed_d);

    double mbps_d = ((double)iFileSize / (double)1e6)  / elapsed_d;
    std::string mbps = std::to_string(mbps_d);

    std::string FileSize_str = filesize;
    
    std::string throughput_str;
    throughput_str = FileSize_str + " bytes transferred in " + elapsed + " seconds : " + mbps + " Megabytes/sec\n";

    char throughput[BUFSIZ];
    bzero((char *)&throughput, sizeof(throughput));
    throughput_str.copy(throughput, BUFSIZ);
    int thru_len = strlen(throughput) + 1;
    throughput[thru_len-1] = '\0';

    if (send(s, throughput, thru_len, 0) == -1)
    {
        perror("Server Send Error\n");
        exit(1);
    }
}

void rdir(int s, std::string buf)
{
    /* Set Strings */
    std::stringstream ss;
    ss << buf;
    std::string command;
    std::string size;
    std::string dirName;
    
    ss >> command;
    ss >> size;
    ss >> dirName;

    /* Check if Directory Exists */
    DIR *dir = opendir(dirName.c_str());

    if (dir) 
    {
        if (send(s, "1", sizeof("1"), 0) == -1)
        {
            perror("Server Send Error");
            return;
        }
    }
    else if (ENOENT == errno) 
    {
       if (send(s, "-1", sizeof("-1"), 0) == -1)
       {
           perror("Server Send Error");
       }
       return;
    }
    else
    {
        perror("opendir() error");
        return;
    }

    /* Client Confirmation to Delete */
    char confirm[BUFSIZ];
    bzero((char *)&confirm, sizeof(confirm));

    if (recv(s, confirm, sizeof(confirm), 0) == -1)
    {
        perror("Error receiving data from client");
        return;
    }

    /* Client does not want to delete directory */
    if (confirm[0] == 'N') { std::cout << "wow\n"; return; }

    /* Client wants to delete directory */
    std::string response; 
    if (rmdir(dirName.c_str()) == 0) { response = "Yes"; }
    else { response = "No"; }

    if (send(s, response.c_str(), sizeof(response.c_str()), 0) == -1)
    {
        perror("Error sending data to client");
        return;
    }
}











