CC_FLAGS = -ggdb -fvisibility=hidden -D_GNU_SOURCE
VERSION = 0.1.0

ARCH ?= $(shell uname -m)

ifeq ($(ARCH),x86_64)
  ARCH_NAME = x86_64
else ifeq ($(ARCH),amd64)
  ARCH_NAME = x86_64
else ifeq ($(ARCH),aarch64)
  ARCH_NAME = aarch64
else ifeq ($(ARCH),arm64)
  ARCH_NAME = aarch64
else ifeq ($(findstring arm,$(ARCH)),arm)
  ARCH_NAME = arm
else
  $(error Unsupported architecture: $(ARCH))
endif

ARCH_SRC = src/arch/$(ARCH_NAME).s
ARCH_OBJ = $(ARCH_NAME).o

all: libtandem.so libtandem.a examples clean

libtandem.a: libtandem.so
	cd build && ar rc libtandem.a coro.o scheduler.o queue.o io.o $(ARCH_OBJ)

libtandem.so: include/tandem/queue.h include/tandem/coro.h include/tandem/io.h src/coro.c src/scheduler.c src/queue.c src/io.c $(ARCH_SRC)
	mkdir -p build/ build/include

	cd build && cc -fPIC $(CC_FLAGS) -I../include -c ../src/coro.c -o coro.o
	cd build && cc -fPIC $(CC_FLAGS) -I../include -c ../src/scheduler.c -o scheduler.o
	cd build && cc -fPIC $(CC_FLAGS) -I../include -c ../src/queue.c -o queue.o
	cd build && cc -fPIC $(CC_FLAGS) -I../include -c ../src/io.c -o io.o
	cc $(CC_FLAGS) -c $(ARCH_SRC) -o build/$(ARCH_OBJ)
	cd build && cc $(CC_FLAGS) -shared -luring coro.o scheduler.o queue.o io.o $(ARCH_OBJ) -o libtandem.so

	cp -r include/ build/

examples: libtandem.a
	mkdir -p build/examples

	cc $(CC_FLAGS) -Wall \
	examples/basic.c -o build/examples/basic \
	-Ibuild/include/ -Lbuild -l:libtandem.a -luring -Wl,-rpath=build

	cc $(CC_FLAGS) -Wall \
	examples/async.c -o build/examples/async \
	-Ibuild/include/ -Lbuild -l:libtandem.a -luring -Wl,-rpath=build

	cc $(CC_FLAGS) -Wall \
	examples/echoserver.c -o build/examples/echoserver \
	-Ibuild/include/ -Lbuild -l:libtandem.a -luring -Wl,-rpath=build

bench: libtandem.a
	mkdir -p build/benchmarks

	cc $(CC_FLAGS) -O3 benchmarks/bench_switch.c -o build/benchmarks/bench_switch \
	-Ibuild/include/ -Lbuild -l:libtandem.a -luring -Wl,-rpath=build

	./build/benchmarks/bench_switch

clean:
	cd build && rm -f coro.o scheduler.o queue.o io.o x86_64.o aarch64.o arm.o

cleanall:
	rm -rf build/
