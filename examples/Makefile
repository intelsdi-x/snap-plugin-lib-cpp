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

build: clean snap-collector-rando snap-processor-graf snap-publisher-log

clean:
	rm -f snap-collector-rando
	rm -f snap-processor-graf
	rm -f snap-publisher-log

snap-collector-rando:
	c++ --std=c++0x ${CPP_FLAGS} -o snap-collector-rando rando/src/rando.cc ${CPP_FLAGS_POST} -lsnap -lprotobuf -lgrpc++

snap-processor-graf:
	c++ --std=c++0x ${CPP_FLAGS} -o snap-processor-graf graffiti/src/graffiti.cc ${CPP_FLAGS_POST} -lsnap -lprotobuf -lgrpc++

snap-publisher-log:
	c++ --std=c++0x ${CPP_FLAGS} -o snap-publisher-log log/src/log.cc ${CPP_FLAGS_POST} -lsnap -lprotobuf -lgrpc++

