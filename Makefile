build:
	mkdir build
	cd build
	gcc main.c -o toggleKeyboardBacklight

install:
	ln -s $(pwd)/build/toggleKeyboardBacklight /usr/bin/toggleKeyboardBacklight

uninstall:
	rm /usr/bin/toggleKeyboardBacklight
