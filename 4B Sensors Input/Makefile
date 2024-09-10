FLAGS = -Wall -Wextra -lm -lmraa 
SRCS = lab4b.c smoketest.sh Makefile README smoketests.PNG period_and_scale_tests.PNG

default:
	gcc -o lab4b -g $(FLAGS) lab4b.c

check:
	./smoketest.sh
	rm -f log.txt


clean: 
	rm -f lab4b
	rm -f lab4b-105180929.tar.gz


dist: 
	tar -czvf lab4b-105180929.tar.gz $(SRCS) 
