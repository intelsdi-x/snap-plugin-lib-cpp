.PHONY: build clean

build: clean snap-collector-rando snap-processor-graf snap-publisher-log

clean:
	rm -f snap-collector-rando
	rm -f snap-processor-graf
	rm -f snap-publisher-log

snap-collector-rando:
	c++ --std=c++11 ${CPP_FLAGS} -o snap-collector-rando rando/src/rando.cc ${CPP_FLAGS_POST} -lsnap -lprotobuf -lgrpc++

snap-processor-graf:
	c++ --std=c++11 ${CPP_FLAGS} -o snap-processor-graf graffiti/src/graffiti.cc ${CPP_FLAGS_POST} -lsnap -lprotobuf -lgrpc++

snap-publisher-log:
	c++ --std=c++11 ${CPP_FLAGS} -o snap-publisher-log log/src/log.cc ${CPP_FLAGS_POST} -lsnap -lprotobuf -lgrpc++

