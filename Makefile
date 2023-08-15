.PHONY: run
.PHONY: build
.PHONY: clean

build:
	cd protocol && make

run:
	cd protocol && make run

clean:
	cd protocol && make clean