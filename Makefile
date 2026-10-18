default:
	g++ main.cpp -o awPass
set_key:
	g++ setKey.cpp -o setKey
install:
	g++ main.cpp -o awPass
	sudo mv awPass /usr/bin/awPass
clean:
	rm awPass
clean-all:
	rm awPass setKey
uninstall:
	sudo rm /usr/bin/awPass
