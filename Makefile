TEST_FILE := $(wildcard ./test/*.cc)
INC_FILE := $(wildcard ./include/primitives/*.hh  ./include/utils/*.hh ./include/*.hh)
SCENE_FILE := $(wildcard ./scene/*.cc)
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
	@echo "Format: "$(TEST_FILE) $(INC_FILE) $(SCENE_FILE)
	clang-format -i $(TEST_FILE) $(INC_FILE) $(SCENE_FILE)

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
