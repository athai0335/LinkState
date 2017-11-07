all: 	manager.cpp manager.h router.cpp router.h
	g++ -Wall -g -I. manager.cpp -o manager
	g++ -Wall -g -I. router.cpp -o router

manager: 	manager.cpp manager.h
	g++ -Wall -g -I. manager.cpp -o manager 

router:	router.cpp router.h
	g++ -Wall -g -I. router.cpp -o router

clean:
	rm -f manager
	rm -f router

