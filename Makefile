#
# Copyright (C) 2022 2666680 Ontario Inc.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#
NAME := GVM User Suite
VERSION := 0.1.0.0

RM_VERSION := "510.47.03"

# Recursive wildcard function
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

BUILD := build

EXT_SRC := $(call rwildcard,extern/src,*.c)
LIB_SRC := $(call rwildcard,src/lib,*.c) $(call rwildcard,src/lib,*.cpp)
LIB_OBJ := $(LIB_SRC:src/lib/%=$(BUILD)/lib/%.o) $(EXT_SRC:extern/src/%=$(BUILD)/extern/%.o)

EXE_SRC := $(call rwildcard,src/exe,*.cpp)
BINARIES := $(EXE_SRC:src/exe/%.cpp=%)

TEST_SRC := $(call rwildcard,tests,*.cpp)
TEST_BIN := $(TEST_SRC:tests/%.cpp=test-%)

ifndef VERBOSE
.SILENT:
endif

ASM := gcc
CC := gcc
CXX := g++
LD := g++

DEFS := -DRM_VERSION="\"$(RM_VERSION)\""
GENFLAGS := -c -g -Og -I inc -I extern/inc -Wall -Wextra $(DEFS)

ASMFLAGS := $(GENFLAGS)
CFLAGS := $(GENFLAGS)
CXXFLAGS := $(CFLAGS)
LDFLAGS :=

all: lib
	$(MAKE) execs
	$(info Made all executables)

lib: $(LIB_OBJ)
	$(info Made $(NAME) (lib) version: $(VERSION))

tests: lib $(TEST_BIN)
	$(info Made all tests)

execs: $(BINARIES)

docs:
	doxygen

clean:
	rm -rf build bin docs
	$(info Made clean)

$(BUILD)/extern/%.c.o: extern/src/%.c
	$(info [CC] compiling $?)
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $? -o $@

$(BUILD)/lib/%.S.o: src/lib/%.S
	$(info [ASM] assembling $?)
	mkdir -p $(@D)
	$(ASM) $(ASMFLAGS) $? -o $@

$(BUILD)/lib/%.c.o: src/lib/%.c
	$(info [CC] compiling $?)
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $? -o $@

$(BUILD)/lib/%.cpp.o: src/lib/%.cpp
	$(info [CXX] compiling $?)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $? -o $@

$(BUILD)/exe/%.c.o: src/exe/%.c
	$(info [CC] compiling $?)
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $? -o $@

$(BUILD)/exe/%.cpp.o: src/exe/%.cpp
	$(info [CXX] compiling $?)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $? -o $@

$(BUILD)/tests/%.cpp.o: tests/%.cpp
	$(info [CXX] compiling $?)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $? -o $@

test-%: $(BUILD)/tests/%.cpp.o
	$(info [LD] Linking $@)
	mkdir -p bin
	$(LD) $(LIB_OBJ) $^ $(LDFLAGS) -o bin/$@

%: $(BUILD)/exe/%.cpp.o
	$(info [LD] Linking $@)
	mkdir -p bin
	$(LD) $(LIB_OBJ) $^ $(LDFLAGS) -o bin/$@
