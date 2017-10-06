
CC = g++ -std=c++11
MAIN1 = ./client/myftp
EXEC1 = ./client/myftp
MAIN2 = ./server/myftpd
EXEC2 = ./server/myftpd

all:	$(EXEC1) $(EXEC2)

$(EXEC1): $(MAIN1).o
	$(CC) $(MAIN1).o -o $(EXEC1)

$(MAIN1).o: $(MAIN1).cpp
	$(CC) -c $(MAIN1).cpp -o $(MAIN1).o

$(EXEC2): $(MAIN2).o
	$(CC) $(MAIN2).o -o $(EXEC2)

$(MAIN2).o: $(MAIN2).cpp
	$(CC) -c $(MAIN2).cpp -o $(MAIN2).o

clean:
	@echo "Cleaning..."
	@rm -f *.o
	@rm -f ./server/*.o
	@rm -f ./client/*.o
	@rm -f $(EXEC1) $(EXEC2) 
