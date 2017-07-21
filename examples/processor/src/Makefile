# http://www.apache.org/licenses/LICENSE-2.0.txt
#
#
# Copyright 2017 Intel Corporation
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
PLUGINTYPE=processor
NAME=graffiti
TARGET=test-$(PLUGINTYPE)-$(NAME)

SNAPLIBS=/usr/local/lib
ICCROOT=
EROOT=../..
BDIR=$(EROOT)/bin

CC=g++
IDIRS=-I.
LDIRS=-L.
CFLAGS=-std=c++1y
LIBS=-Wl,--start-group $(SNAPLIBS)/libsnap.a $(SNAPLIBS)/libgrpc.a $(SNAPLIBS)/libgrpc++.a $(SNAPLIBS)/libprotobuf.a -Wl,--end-group -lboost_program_options -lboost_system -lboost_thread -lcppnetlib-uri -lcppnetlib-client-connections -lcppnetlib-server-parsers -lz -lpthread -lssl -lcrypto
FILES := $(wildcard *.cc)

all: $(TARGET)				# Build all targets

vars:					# Show Makefile variable values
	@echo "  PLUGINTYPE "$(PLUGINTYPE)
	@echo "        NAME "$(NAME)
	@echo "      TARGET "$(TARGET)
	@echo "" 
	@echo "    SNAPLIBS "$(SNAPLIBS) 
	@echo "     ICCROOT "$(ICCROOT) 
	@echo "       EROOT "$(EROOT)
	@echo "        BDIR "$(BDIR)
	@echo "" 
	@echo "          CC "$(CC)
	@echo "       IDIRS "$(IDIRS)
	@echo "       LDIRS "$(LDIRS)
	@echo "      CFLAGS "$(FLAGS)
	@echo "        LIBS "$(LIBS)
	@echo "       FILES "$(FILES)

help:					# Show Makefile targets
	@egrep "[a-z]\w+:" Makefile | sort

clean:
	rm -f $(TARGET)

.PHONY:	all clean vars help install uninstall rebuild $(TARGET)

install:
	cp $(TARGET) $(BDIR)

uninstall:
	rm -f $(BDIR)/$(TARGET)

rebuild: clean $(TARGET)		# Clean and build target

$(TARGET):
	$(CC) $(CFLAGS) $(IDIRS) $(LDIRS) -o $(TARGET) $(FILES) $(LIBS)
