build:
	mkdir build
	gcc main.c -o build/toggleKeyboardBacklight

clean:
	rm -rf build

install:
	sudo cp build/toggleKeyboardBacklight /usr/bin/toggleKeyboardBacklight

uninstall:
	rm /usr/bin/toggleKeyboardBacklight
