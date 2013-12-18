
CC = g++
PARAMS = -c -g
OUTPUT = ntscan
INSTALL_DIR = /opt/NetworkScanner

main:main.o
	$(CC) -o $(OUTPUT) main.o -lpthread -lrt
	
main.o:main.cpp
	$(CC) $(PARAMS) main.cpp

clean:
	rm -f main.o
	rm -f $(OUTPUT)
	rm -rf log

install:
	mkdir -p $(INSTALL_DIR)
	cp -f ./$(OUTPUT) $(INSTALL_DIR)
	cp -f ./ntscan.conf $(INSTALL_DIR)
	cp -f ./ttl.prn $(INSTALL_DIR)
	cp -rf ./html $(INSTALL_DIR)
	ln -sf $(INSTALL_DIR)/$(OUTPUT) /usr/bin/$(OUTPUT)
	ln -sf $(INSTALL_DIR)/ntscan.conf /etc/ntscan.conf
	