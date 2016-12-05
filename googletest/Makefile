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
AR		= ar
GIT		= git
GTEST_VER	= release-1.8.0
GTEST_REPO	= https://github.com/google/googletest.git
GTESTLIB_DIR	= $(CURDIR)
GTEST_DIR	= $(GTESTLIB_DIR)/testing/googletest
GMOCK_DIR	= $(GTESTLIB_DIR)/testing/googlemock
PREFIX		= $(CURDIR)/lib
OBJ		= $(CURDIR)/obj
CPPFLAGS	= --std=c++11
ARFLAGS		= -rv

define run-cc =
	$(CXX) $(RUN_CPPFLAGS) -c $^ -o $@
endef

define run-ar = 
	$(AR) $(ARFLAGS) $@ $^
endef

define run-clone =
	$(GIT) clone --branch $(GTEST_VER) -c advice.detachedHead=false $(GTEST_REPO) $@
endef

.PHONY : clean init gmock gtest all


all : init gtest gmock

sources : $(GTESTLIB_DIR)/testing

$(GTESTLIB_DIR)/testing :
	$(run-clone)

clean :
	for dir in $(OBJ) $(PREFIX) $(dir $(GTEST_DIR)) ; do \
		find $${dir} -type f -and -name '*.o' -or -name '*.a' -or -name '*.gc*' -exec rm {} + ; \
	done ;\
	rm -rf $(OBJ)

init :	sources
	mkdir -p $(PREFIX)
	mkdir -p $(OBJ)

gtest : RUN_CPPFLAGS = $(CPPFLAGS) -isystem $(GTEST_DIR)/include -I$(GTEST_DIR) -pthread
gtest : $(PREFIX)/libgtest.a

$(PREFIX)/libgtest.a: $(OBJ)/gtest-all.o
	$(run-ar)

$(OBJ)/gtest-all.o: $(GTEST_DIR)/src/gtest-all.cc 
	$(run-cc)
		
gmock : RUN_CPPFLAGS = $(CPPFLAGS) -isystem ${GTEST_DIR}/include -I${GTEST_DIR} -isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} -pthread
gmock :	$(PREFIX)/libgmock.a

$(PREFIX)/libgmock.a: $(OBJ)/gmock-all.o
	$(run-ar)

$(OBJ)/gmock-all.o: $(GMOCK_DIR)/src/gmock-all.cc
	$(run-cc)

