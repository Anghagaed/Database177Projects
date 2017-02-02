CC = g++ -g -O0 -Wno-deprecated

# g++ DataStructure.h DataStructure.cc EfficientMap.h EfficientMap.cc Keyify.h Keyify.cc Config.h Swap.h Catalog.h Catalog.cc Schema.h Schema.cc main.cc -l sqlite3 -o a

tag = -i

ifdef linux
	tag = -n
endif

main.out:	DataStructure.o EfficientMap.o Keyify.o Schema.o Catalog.o main.o 
	$(CC) -o main.out main.o Catalog.o Schema.o -lsqlite3
	
DataStructure.o: DataStructure.h DataStructure.cc
	$(CC) -c DataStructure.cc

EfficientMap.o: EfficientMap.h Swap.h EfficientMap.cc
	$(CC) -c EfficientMap.cc

Keyify.o: Keyify.h Keyify.cc
	$(CC) -c Keyify.cc

Schema.o: Config.h Schema.h Schema.cc	
	$(CC) -c Schema.cc

Catalog.o: Catalog.h Catalog.cc	
	$(CC) -c Catalog.cc

main.o:	main.cc
	$(CC) -c main.cc

clean: 
	rm -f *.o
	rm -f *.out
