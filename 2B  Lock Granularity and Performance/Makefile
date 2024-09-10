PNG = lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png
FILES = SortedList.h SortedList.c Makefile lab2b_list.csv lab2_list.c profile.out README lab2b_list.gp

#remove add pngs
build: 
	gcc -Wall -Wextra -g -o lab2_list -lpthread lab2_list.c SortedList.c

tests:
	./lab2_list --threads=1  --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=m	>> lab2b_list.csv
	./lab2_list --threads=12  --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16  --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=24  --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12  --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16  --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=24  --iterations=1000 --sync=s >> lab2b_list.csv

	./lab2_list --threads=1 --yield=id --lists=4 --iterations=10 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --yield=id --lists=4 --iterations=20 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --yield=id --lists=4 --iterations=40 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --yield=id --lists=4 --iterations=80 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --yield=id --lists=4 --iterations=10 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --yield=id --lists=4 --iterations=20 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --yield=id --lists=4 --iterations=40 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --yield=id --lists=4 --iterations=80 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --yield=id --lists=4 --iterations=10 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --yield=id --lists=4 --iterations=20 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --yield=id --lists=4 --iterations=40 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --yield=id --lists=4 --iterations=80 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --yield=id --lists=4 --iterations=10 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --yield=id --lists=4 --iterations=20 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --yield=id --lists=4 --iterations=40 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --yield=id --lists=4 --iterations=80 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --yield=id --lists=4 --iterations=10 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --yield=id --lists=4 --iterations=20 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --yield=id --lists=4 --iterations=40 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --yield=id --lists=4 --iterations=80 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --yield=id --lists=4 --iterations=10 --sync=s >> lab2b_list.csv
	./lab2_list --threads=1 --yield=id --lists=4 --iterations=20 --sync=s >> lab2b_list.csv
	./lab2_list --threads=1 --yield=id --lists=4 --iterations=40 --sync=s >> lab2b_list.csv
	./lab2_list --threads=1 --yield=id --lists=4 --iterations=80 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --yield=id --lists=4 --iterations=10 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --yield=id --lists=4 --iterations=20 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --yield=id --lists=4 --iterations=40 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --yield=id --lists=4 --iterations=80 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --yield=id --lists=4 --iterations=10 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --yield=id --lists=4 --iterations=20 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --yield=id --lists=4 --iterations=40 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --yield=id --lists=4 --iterations=80 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --yield=id --lists=4 --iterations=10 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --yield=id --lists=4 --iterations=20 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --yield=id --lists=4 --iterations=40 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --yield=id --lists=4 --iterations=80 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --yield=id --lists=4 --iterations=10 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --yield=id --lists=4 --iterations=20 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --yield=id --lists=4 --iterations=40 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --yield=id --lists=4 --iterations=80 --sync=s >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=m	--lists=4 >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=m	--lists=4 >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=m	--lists=4 >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=m	--lists=4 >> lab2b_list.csv
	./lab2_list --threads=12  --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=s	--lists=4 >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12  --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=m	--lists=8 >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	./lab2_list --threads=12  --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=s	--lists=8 >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	./lab2_list --threads=12  --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=m	--lists=16 >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=m	--lists=16 >> lab2b_list.csv
	./lab2_list --threads=12  --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=s	 --lists=16 >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=s	 --lists=16 >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=s  --lists=16 >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=s  --lists=16 >> lab2b_list.csv
	./lab2_list --threads=12  --iterations=1000 --sync=s  --lists=16 >> lab2b_list.csv

	-./lab2_list --threads=1 --yield=id --lists=4 --iterations=1 >> lab2b_list.csv
	-./lab2_list --threads=1 --yield=id --lists=4 --iterations=2 >> lab2b_list.csv
	-./lab2_list --threads=1 --yield=id --lists=4 --iterations=4 >> lab2b_list.csv
	-./lab2_list --threads=1 --yield=id --lists=4 --iterations=8 >> lab2b_list.csv
	-./lab2_list --threads=1 --yield=id --lists=4 --iterations=16 >> lab2b_list.csv
	-./lab2_list --threads=12 --yield=id --lists=4 --iterations=1 >> lab2b_list.csv
	-./lab2_list --threads=12 --yield=id --lists=4 --iterations=2 >> lab2b_list.csv
	-./lab2_list --threads=12 --yield=id --lists=4 --iterations=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --yield=id --lists=4 --iterations=8 >> lab2b_list.csv
	-./lab2_list --threads=12 --yield=id --lists=4 --iterations=16 >> lab2b_list.csv
	-./lab2_list --threads=4 --yield=id --lists=4 --iterations=1 >> lab2b_list.csv
	-./lab2_list --threads=4 --yield=id --lists=4 --iterations=2 >> lab2b_list.csv
	-./lab2_list --threads=4 --yield=id --lists=4 --iterations=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --yield=id --lists=4 --iterations=8 >> lab2b_list.csv
	-./lab2_list --threads=4 --yield=id --lists=4 --iterations=16 >> lab2b_list.csv
	-./lab2_list --threads=8 --yield=id --lists=4 --iterations=1 >> lab2b_list.csv
	-./lab2_list --threads=8 --yield=id --lists=4 --iterations=2 >> lab2b_list.csv
	-./lab2_list --threads=8 --yield=id --lists=4 --iterations=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --yield=id --lists=4 --iterations=8 >> lab2b_list.csv
	-./lab2_list --threads=8 --yield=id --lists=4 --iterations=16 >> lab2b_list.csv
	-./lab2_list --threads=16 --yield=id --lists=4 --iterations=1 >> lab2b_list.csv
	-./lab2_list --threads=16 --yield=id --lists=4 --iterations=2 >> lab2b_list.csv
	-./lab2_list --threads=16 --yield=id --lists=4 --iterations=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --yield=id --lists=4 --iterations=8 >> lab2b_list.csv
	-./lab2_list --threads=16 --yield=id --lists=4 --iterations=16 >> lab2b_list.csv

profile:
	rm -f ./raw.gperf profile.out
	LD_PRELOAD=/u/cs/ugrad/glyn/cs111/lab2/newdir/lib/libprofiler.so.0 CPUPROFILE=./raw.gperf ./lab2_list --iterations=1000 --threads=12 --sync=s
	../newdir/bin/pprof --text ./lab2_list ./raw.gperf > profile.out
#	../newdir/bin/pprof --list=threadHandler ./lab2_list ./raw.gperf >> profile.out
	rm -f ./raw.gperf profile.gperf lab2_list
#remove add
graphs: tests
	./lab2b_list.gp

clean:
	rm -f lab2_list lab2b-105180929.tar.gz lab2b_list.csv

dist:
	tar -czvf lab2b-105180929.tar.gz $(FILES) $(PNG)
