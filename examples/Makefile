.PHONY: build clean

build: clean snap-collector-rando snap-processor-graf snap-publisher-log

clean:
	rm -f snap-collector-rando
	rm -f snap-processor-graf
	rm -f snap-publisher-log

snap-collector-rando:
	c++ --std=c++11 -o snap-collector-rando rando/src/rando.cc -lsnap -lprotobuf -lgrpc++

snap-processor-graf:
	c++ --std=c++11 -o snap-processor-graf graffiti/src/graffiti.cc -lsnap -lprotobuf -lgrpc++

snap-publisher-log:
	c++ --std=c++11 -o snap-publisher-log log/src/log.cc -lsnap -lprotobuf -lgrpc++
