TEST_FILE := $(wildcard ./test/*.cc)
INC_FILE := $(wildcard ./include/primitives/*.hh  ./include/utils/*.hh ./include/*.hh)
SCENE_FILE := $(wildcard ./scene/*.cc)
DOCKER_DIR:= ./docker

STATIC_RENDER_DEF := OFF
ifeq ($(STATIC),1)
	STATIC_RENDER_DEF = ON
endif

ifneq ($(CH),)
	_CH=$(CH)
endif

CHVAR := 
ifeq ($(_CH), 4)
	CHVAR := CHAPTER4
else ifeq ($(_CH), 5)
	CHVAR := CHAPTER5
else ifeq ($(_CH), 6)
	CHVAR := CHAPTER6
else ifeq ($(_CH), 7)
	CHVAR := CHAPTER7
else ifeq ($(_CH), 8)
	CHVAR := CHAPTER8
else ifeq ($(_CH), 9)
	CHVAR := CHAPTER9
else ifeq ($(_CH), 10)
	CHVAR := CHAPTER10
endif

TEST_DEF := OFF
ifeq (1, $(filter 1, $(TEST) $(test)))
	TEST_DEF = ON
endif

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

.PHONY: chapter
chapter: $(BUILDDIR)/Makefile
	cmake --build "$(BUILDDIR)" -j $(JOBS) -- $(CHVAR)

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
	cmake -H. -B$(BUILDDIR) \
	-DCMAKE_BUILD_TYPE=$(BUILDTYPE) \
	-DBUILD_TESTS=$(TEST_DEF) \
	-DRENDER_CHAPTER=$(_CH) \
	-DRENDER_STATIC=$(STATIC_RENDER_DEF)

.PHONY: docker-run
docker-run:
	-@bash $(DOCKER_DIR)/run.sh
