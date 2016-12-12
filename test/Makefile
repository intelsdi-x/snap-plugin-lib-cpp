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
CXX		= g++
LD		= g++
GTEST_DIR	= $(CURDIR)/../googletest/testing/googletest
GMOCK_DIR	= $(CURDIR)/../googletest/testing/googlemock
GTESTLIB_DIR	= $(CURDIR)/../googletest
SNAPLIB_DIR	= $(CURDIR)/../lib
PREFIX		= $(CURDIR)
COV_ARGS	= -g -fprofile-arcs -ftest-coverage --coverage -fPIC -lgcov -O0
CPPFLAGS	= --std=c++0x
LDFLAGS		= --std=c++0x -fPIC -DPIC
SRCDIR		:= $(CURDIR)
OBJDIR		:= $(CURDIR)
SRC       	:= $(wildcard $(SRCDIR)/*.cc)
OBJ       	:= $(patsubst %.cc,%.o,$(SRC))
EXE		:= test_main

define run-cc =
	$(CXX) $(RUN_CPPFLAGS) -c $^ -o $@
endef

define run-ld = 
	$(LD) $^ $(RUN_LDFLAGS) -o $@
endef

.PHONY : clean init build test run


all : test

test : init build run coverage

coverage :
	../scripts/gather-coverage.sh 2>/dev/null

run : 
	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$(GTESTLIB_DIR):$(SNAPLIB_DIR)/lib $(PREFIX)/$(EXE)

build : $(PREFIX)/$(EXE)

clean :
	for dir in $(OBJDIR) $(PREFIX); do find $${dir} \( -type f -and \( -name '*.o' -or -name '*.a' -or -name '*.gc*' -or -name $(EXE) \) \) -exec rm {} + ; done

init :	;
	

$(PREFIX)/$(EXE) : RUN_LDFLAGS = $(LDFLAGS) -L$(SNAPLIB_DIR)/lib -L$(GTESTLIB_DIR)/lib -pthread -lpthread -lgmock -lgtest -lsnap -lprotobuf -lgrpc++ -lgcov $(COV_ARGS)
$(PREFIX)/$(EXE) : $(OBJ)
	$(run-ld)

%.o : RUN_CPPFLAGS = $(CPPFLAGS) -isystem $(GTEST_DIR)/include -isystem $(GMOCK_DIR)/include -I$(SNAPLIB_DIR)/include -pthread $(COV_ARGS)
%.o : %.cc
	$(run-cc)

