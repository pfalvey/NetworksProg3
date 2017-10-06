CC = g++ -std=c++11
MAIN1 = ./myftp
EXEC1 = ./myftp

all: $(EXEC1) 

$(EXEC1): $(MAIN1).o
	$(CC) $(MAIN1).o -o $(EXEC1)

$(MAIN1).o: $(MAIN1).cpp
	$(CC) -c $(MAIN1).cpp -o $(MAIN1).o

clean:
	@echo "Cleaning..."
	@rm -f *.o
	@rm -f $(EXEC1) 