# http://www.apache.org/licenses/LICENSE-2.0.txt
#
#
# Copyright 2016 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
.PHONY: build clean

build: clean test-collector-rando test-processor-graffiti test-publisher-log

clean:
	rm -f test-collector-rando
	rm -f test-processor-graffiti
	rm -f test-publisher-log

test-collector-rando:
	c++ --std=c++0x ${CPP_FLAGS} -o test-collector-rando collector/src/rando.cc ${CPP_FLAGS_POST} -lsnap -lprotobuf -lgrpc++

test-processor-graffiti:
	c++ --std=c++0x ${CPP_FLAGS} -o test-processor-graffiti processor/src/graffiti.cc ${CPP_FLAGS_POST} -lsnap -lprotobuf -lgrpc++

test-publisher-log:
	c++ --std=c++0x ${CPP_FLAGS} -o test-publisher-log publisher/src/log.cc ${CPP_FLAGS_POST} -lsnap -lprotobuf -lgrpc++

