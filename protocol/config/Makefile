.PHONY: obj_file
.PHONY: old_makefile
.PHONY: new_makefile
.PHONY: mbr
.PHONY: filesystem
.PHONY: disk_image
.PHONY: config_kernel_size
.PHONY: ffs

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

filesystem:
	../bin/config.o fs

disk_image:
	../bin/config.o dimg

config_kernel_size:
	../bin/config.o ksize

ffs:
	../bin/config.o ffs