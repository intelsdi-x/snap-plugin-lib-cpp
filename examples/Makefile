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

# Collector build
A_PREFIX=test
A_TYPE=collector
A_NAME=rando
A_TARGET=$(A_PREFIX)-$(A_TYPE)-$(A_NAME)
A_DIR=$(A_TYPE)/src

# Processor build
B_PREFIX=test
B_TYPE=processor
B_NAME=graffiti
B_TARGET=$(B_PREFIX)-$(B_TYPE)-$(B_NAME)
B_DIR=$(B_TYPE)/src

# Publisher build
C_PREFIX=test
C_TYPE=publisher
C_NAME=log
C_TARGET=$(C_PREFIX)-$(C_TYPE)-$(C_NAME)
C_DIR=$(C_TYPE)/src

# Streaming collector build
D_PREFIX=test
D_TYPE=streaming-collector
D_NAME=rando
D_TARGET=$(D_PREFIX)-$(D_TYPE)-$(D_NAME)
D_DIR=$(D_TYPE)/src

A_PATH=$(PWD)

all: $(A_TARGET) $(B_TARGET) $(C_TARGET) $(D_TARGET)  # Build all targets

.PHONY: all help clean rebuild install uninstall $(A_TARGET) $(A_TARGET)-clean $(A_TARGET)-install $(A_TARGET)-uninstall $(B_TARGET) $(B_TARGET)-clean $(B_TARGET)-install $(B_TARGET)-uninstall $(C_TARGET) $(C_TARGET)-clean $(C_TARGET)-install $(C_TARGET)-uninstall $(D_TARGET) $(D_TARGET)-clean $(D_TARGET)-install $(D_TARGET)-uninstall

help:                                      # Show Makefile targets
        @egrep "[a-z]\w+:" Makefile | sort

clean: $(A_TARGET)-clean $(B_TARGET)-clean $(C_TARGET)-clean $(D_TARGET)-clean

rebuild: clean all                      # Clean and build targets

install: $(A_TARGET)-install $(B_TARGET)-install $(C_TARGET)-install $(D_TARGET)-install

uninstall: $(A_TARGET)-uninstall $(B_TARGET)-uninstall $(C_TARGET)-uninstall $(D_TARGET)-uninstall


$(A_TARGET):
	@(cd $(A_DIR); make; echo " ") 

$(A_TARGET)-clean:
	@(cd $(A_DIR); make clean; echo " ") 

$(A_TARGET)-install:
	@(cd $(A_DIR); make install; echo " ")

$(A_TARGET)-uninstall:
	@(cd $(A_DIR); make uninstall; echo " ")


$(B_TARGET):
	@(cd $(B_DIR); make; echo " ")

$(B_TARGET)-clean:
	@(cd $(B_DIR); make clean; echo " ") 

$(B_TARGET)-install:
	@(cd $(B_DIR); make install; echo " ")

$(B_TARGET)-uninstall:
	@(cd $(B_DIR); make uninstall; echo " ")


$(C_TARGET):
	@(cd $(C_DIR); make; echo " ") 

$(C_TARGET)-clean:
	@(cd $(C_DIR); make clean; echo " ")

$(C_TARGET)-install:
	@(cd $(C_DIR); make install; echo " ")

$(C_TARGET)-uninstall:
	@(cd $(C_DIR); make uninstall; echo " ")


$(D_TARGET):
	@(cd $(D_DIR); make; echo " ") 

$(D_TARGET)-clean:
	@(cd $(D_DIR); make clean; echo " ")

$(D_TARGET)-install:
	@(cd $(D_DIR); make install; echo " ")

$(D_TARGET)-uninstall:
	@(cd $(D_DIR); make uninstall; echo " ")

