.PHONY: obj_file
.PHONY: old_makefile
.PHONY: new_makefile
.PHONY: mbr
.PHONY: disk_image

FLAGS = -fsanitize=leak -std=c++20 -Wall

obj_file:
	g++ $(FLAGS) -I ../include/ main.cpp -o ../bin/config.o

new_makefile:
	../bin/config.o nm

old_makefile:
	../bin/config.o om
	rm -rf ../boot/mbr.s
	rm -rf ../bin/*
	rm -rf ../bin/config.o

mbr:
	../bin/config.o mbr

disk_image:
	../bin/config.o dimg