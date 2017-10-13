# NetworksProg3

Patrick Falvey
Mikey Farren
Matthew Flanagan

Tasks:

Matt: DWLD (throughput), CDIR, LIST

Mikey: UPLD (throughput), RDIR

Patrick: DELF, MDIR, command line arguments, QUIT


Included Files: /client
   Makefile			Compiles C++ Code to create executable ./myftp
   myftp.cpp			C++ code for the client

Included Files: /server
   Makefile			Compiles C++ Code to create executable ./myftpd
   myftpd.cpp			C++ code for server


Example Command Line for Server (run from student01):
$ ./myftpd 41038


Example Command Line for Client (run from student02):
$ ./myftp student01.cse.nd.edu 41038


Example Commands From Client Side of Network
1) DWLD testDWLD.txt
2) UPLD testUPLD.txt
3) LIST
4) DELF testUPLD.txt
5) MDIR dir1
6) CDIR dir1
7) CDIR ../
8) RDIR dir1
9) QUIT
