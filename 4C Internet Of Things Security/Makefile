default: 
	gcc -o lab4c_tcp -Wall -Wextra -lm -lmraa -g lab4c_tcp.c
	gcc -o lab4c_tls -Wall -Wextra -lm -lmraa -g -lssl -lcrypto lab4c_tls.c

clean:
	rm -f lab4c-105180929.tar.gz lab4c_tcp lab4c_tls

dist:
	tar -czvf lab4c-105180929.tar.gz lab4c_tls.c lab4c_tcp.c README Makefile passed.PNG