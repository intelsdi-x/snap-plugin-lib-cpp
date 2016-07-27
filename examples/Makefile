.PHONY: build clean

build: snap-collector-rando

clean:
	rm snap-collector-rando

snap-collector-rando:
	c++ --std=c++11 -lsnap -lprotobuf -lgrpc++ -o snap-collector-rando rando/src/rando.cc
