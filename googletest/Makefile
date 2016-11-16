CC		= g++
AR		= ar
GIT		= git
GTEST_VER	= release-1.8.0
GTEST_REPO	= https://github.com/google/googletest.git
GTEST_DIR	= $(CURDIR)/testing/googletest
GMOCK_DIR	= $(CURDIR)/testing/googlemock
PREFIX		= $(CURDIR)/lib
OBJ		= $(CURDIR)/obj
CPPFLAGS	= --std=c++11
ARFLAGS		= -rv

define run-cc =
	$(CC) $(RUN_CPPFLAGS) -c $^ -o $@
endef

define run-ar = 
	$(AR) $(ARFLAGS) $@ $^
endef

define run-clone =
	$(GIT) clone --branch $(GTEST_VER) -c advice.detachedHead=false $(GTEST_REPO) $@
endef

.PHONY : clean init gmock gtest all


all : init gtest gmock

sources : $(dir $(GTEST_DIR))

$(dir $(GTEST_DIR)):
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

