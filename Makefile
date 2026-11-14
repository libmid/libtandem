CC_FLAGS = -ggdb
VERSION = 0.1.0

all: libtandem.so libtandem.a examples clean

libtandem.a: libtandem.so
	cd build && ar rc libtandem.a coro.o scheduler.o queue.o x86_64.o

libtandem.so: include/tandem/queue.h include/tandem/coro.h src/coro.c src/scheduler.c src/queue.c
	mkdir -p build/ build/include

	cd build && cc -fPIC $(CC_FLAGS) -I../include -c ../src/coro.c -o coro.o
	cd build && cc -fPIC $(CC_FLAGS) -I../include -c ../src/scheduler.c -o scheduler.o
	cd build && cc -fPIC $(CC_FLAGS) -I../include -c ../src/queue.c -o queue.o
	cc $(CC_FLAGS) -c src/arch/x86_64.s -o build/x86_64.o
	cd build && cc $(CC_FLAGS) -shared -luring coro.o scheduler.o queue.o x86_64.o -o libtandem.so

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
	examples/echoserver.c -o build/examples/eschoserver \
	-Ibuild/include/ -Lbuild -l:libtandem.a -luring -Wl,-rpath=build

clean:
	cd build && rm -f coro.o scheduler.o queue.o x86_64.o

cleanall:
	rm -rf build/
