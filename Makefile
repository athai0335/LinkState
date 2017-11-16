all: 	manager.cpp manager.h router.cpp router.h
	g++ -Wall -g -I. -O2 -pthread manager.cpp -o manager
	g++ -Wall -g -I. -O2 -pthread router.cpp -o router

manager: 	manager.cpp manager.h
	g++ -Wall -g -I. -O2 -pthread manager.cpp -o manager 

router:	router.cpp router.h
	g++ -Wall -g -I. -O2 -pthread router.cpp -o router

clean:
	rm -f manager
	rm -f router

