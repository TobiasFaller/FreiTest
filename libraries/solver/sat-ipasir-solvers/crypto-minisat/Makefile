all: libipasircryptominisat5.a

cmake-3.12.0/bin/cmake:
	rm -rf cmake-3.12.0
	tar xzvf cmake-3.12.0.tar.gz
	cd cmake-3.12.0 && \
	./configure && \
	make -j4


m4ri-20200125/myinstall/lib/libm4ri.a:
	rm -rf m4ri-20200125
	tar xzvf m4ri-20200125.tar.gz
	cd m4ri-20200125 && \
	rm -rf myinstall && \
	mkdir -p myinstall && \
	./configure --prefix=$(shell pwd)/m4ri-20200125/myinstall --with-pic && \
	make -j4 VERBOSE=1 && \
	make install

breakid/build/lib/libbreakid.a:
	rm -rf breakid/build
	mkdir -p breakid/build
	cd breakid/build && \
	../../cmake-3.12.0/bin/cmake -DSTATICCOMPILE=ON -DCMAKE_BUILD_TYPE=Release .. && \
	make -j4 VERBOSE=1

cms/build/lib/libipasircryptominisat5.a: cmake-3.12.0/bin/cmake m4ri-20200125/myinstall/lib/libm4ri.a breakid/build/lib/libbreakid.a
	rm -rf cms/build
	mkdir cms/build
	cd cms/build && \
	M4RI_ROOT_DIR=$(shell pwd)/m4ri-20200125/myinstall ../../cmake-3.12.0/bin/cmake -DENABLE_PYTHON_INTERFACE=OFF -DNOVALGRIND=ON -DNOZLIB=ON -DONLY_SIMPLE=ON -DSTATICCOMPILE=ON -DCMAKE_BUILD_TYPE=Release -DENABLE_TESTING=OFF -DIPASIR=ON -DMANPAGE=OFF .. && \
	make -j4 VERBOSE=1

libipasircryptominisat5.a: cms/build/lib/libipasircryptominisat5.a
	rm -rf abc*
	mkdir abc;  cd abc;  ar -x ../cms/build/lib/libipasircryptominisat5.a;
	mkdir abc3; cd abc3; ar -x ../m4ri-20200125/.libs/libm4ri.a; 
	mkdir abc4; cd abc4; ar -x ../breakid/build/lib/libbreakid.a;
	rm -f libipasircryptominisat5.a
	ar -qc libipasircryptominisat5.a abc/* abc3/* abc4/*

clean:
	rm -rf cmake-3.12.0
	rm -rf m4ri-20200125
	rm -rf breakid/build
	rm -rf cms/build
	rm -rf libipasircryptominisat5.a
	rm -f libipasircryptominisat5.so
	rm -rf abc*
