.PHONY: build clean

build: clean snap-collector-rando snap-processor-graf snap-publisher-log

clean:
	rm -f snap-collector-rando
	rm -f snap-processor-graf
	rm -f snap-publisher-log

snap-collector-rando:
	c++ --std=c++11 -lsnap -lprotobuf -lgrpc++ -o snap-collector-rando rando/src/rando.cc

snap-processor-graf:
	c++ --std=c++11 -lsnap -lprotobuf -lgrpc++ -o snap-processor-graf graffiti/src/graffiti.cc

snap-publisher-log:
	c++ --std=c++11 -lsnap -lprotobuf -lgrpc++ -o snap-publisher-log log/src/log.cc
