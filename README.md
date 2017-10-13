# NetworksProg3


Tasks:

Matt: DWLD (throughput), CDIR, LIST

Mikey: UPLD (throughput), RDIR, QUIT

Patrick: DELF, MDIR, command line arguments


Included Files: /client
   Makefile			Compiles C++ Code to create executable ./myftp
   myftp.cpp			C++ code for the client
   testUPLD.txt			Text file that can be sent from client to server using the UPLD command

Included Files: /server
   Makefile			Compiles C++ Code to create executable ./myftpd
   myftpd.cpp			C++ code for server
   testDWLD.txt			Text file that can be sent from server to client using the DWLD command


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
