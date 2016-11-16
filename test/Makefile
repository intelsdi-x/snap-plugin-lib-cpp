CC		= g++
LD		= g++
GTEST_DIR	= $(CURDIR)/../googletest/testing/googletest
GMOCK_DIR	= $(CURDIR)/../googletest/testing/googlemock
GTESTLIB_DIR	= $(CURDIR)/../googletest
SNAPLIB_DIR	= $(CURDIR)/../lib
PREFIX		= $(CURDIR)
CPPFLAGS	= --std=c++11
LDFLAGS		= --std=c++11
SRCDIR		:= $(CURDIR)
OBJDIR		:= $(CURDIR)
SRC       	:= $(wildcard $(SRCDIR)/*.cc)
OBJ       	:= $(patsubst %.cc,%.o,$(SRC))
EXE		:= test_main

define run-cc =
	$(CC) $(RUN_CPPFLAGS) -c $^ -o $@
endef

define run-ld = 
	$(LD) $^ $(RUN_LDFLAGS) -o $@
endef

.PHONY : clean init build test run


all : test

test : init build run

run : 
	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$(GTESTLIB_DIR):$(SNAPLIB_DIR)/lib $(PREFIX)/$(EXE)

build : $(PREFIX)/$(EXE)

clean :
	for dir in $(OBJDIR) $(PREFIX); do find $${dir} -type f -name '*.o' -or -name '*.a' -or -name '*.gc*' -or -name $(EXE) -exec rm {} + ; done

init :	;
	

$(PREFIX)/$(EXE) : RUN_LDFLAGS = $(LDFLAGS) -L${SNAPLIB_DIR}/lib -L${GTESTLIB_DIR}/lib -pthread -lpthread -lgmock -lgtest -lsnap -lprotobuf -lgrpc++ -lgcov
$(PREFIX)/$(EXE) : $(OBJ)
	$(run-ld)

%.o : RUN_CPPFLAGS = $(CPPFLAGS) -isystem ${GTEST_DIR}/include -isystem ${GMOCK_DIR}/include -I${SNAPLIB_DIR}/include -pthread
%.o : %.cc
	$(run-cc)

