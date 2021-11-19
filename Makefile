TEST_DIR :=  ./test
TEST_CXX_FILE := $(wildcard $(TEST_DIR)/*.cc)
INC_DIR := ./include
SHAPE_DIR := shapes
PRIMITIVE_DIR := primitives
UTILS_DIR := utils
SRC_DIR := ./src
INC_CXX_FILE := $(wildcard $(INC_DIR)/*.hh $(INC_DIR)/*.inl $(INC_DIR)/$(SHAPE_DIR)/*.hh $(INC_DIR)/$(PRIMITIVE_DIR)/*.hh $(INC_DIR)/$(UTILS_DIR)/*.hh)
SCENE_DIR :=  ./scene
SRC_FILE := $(wildcard $(SCENE_DIR)/*.cc $(SRC_DIR)/$(SHAPE_DIR)/*.cc $(SRC_DIR)/*.cc)
DOCKER_DIR:= ./docker

export BUILDTYPE ?= Debug
buildtype := $(shell echo "$(BUILDTYPE)" | tr "[A-Z]" "[a-z]")
export BUILDDIR ?= build/default/$(buildtype)

ifeq ($(shell uname -s), Darwin)
  export JOBS ?= $(shell sysctl -n hw.ncpu)
else ifeq ($(shell uname -s), Linux)
  export JOBS ?= $(shell grep --count processor /proc/cpuinfo)
else
  $(error Cannot determine host platform)
endif

.PHONY: all
all: $(BUILDDIR)/Makefile
	cmake --build "$(BUILDDIR)" -j $(JOBS)

.PHONY: test
test: $(BUILDDIR)/Makefile
	cmake --build "$(BUILDDIR)" -j $(JOBS) -- raytracer_test

.PHONY: run-test
run-test: test
	"$(BUILDDIR)/test/raytracer_test" yellow

.PHONY: format
format:
	@echo "Format: "$(TEST_CXX_FILE) $(INC_CXX_FILE) $(SRC_FILE)
	clang-format -i $(TEST_CXX_FILE) $(INC_CXX_FILE) $(SRC_FILE)

.PHONY: clean
clean:
	-@rm -rvf $(BUILDDIR)

.PRECIOUS: $(BUILDDIR)/Makefile
$(BUILDDIR)/Makefile:
	mkdir -p $(BUILDDIR)
	cmake -H. -B$(BUILDDIR) -DCMAKE_BUILD_TYPE=$(BUILDTYPE)

.PHONY: docker-run
docker-run:
	-@sh $(DOCKER_DIR)/run.sh
