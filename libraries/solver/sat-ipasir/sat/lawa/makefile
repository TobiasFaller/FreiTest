CXX=g++
CFLAGS= -Wall -O3 -std=c++11

OBJS = lawa.o lawa-main.o lawa-ipasir.o
LIBS = 
TARGET = lawa

%.o: %.cpp
	$(CXX) -c -o $@ $(LIBS) $(CFLAGS) $<

all: lawa libipasirlawa.a

lawa: lawa.o lawa-main.o
	$(CXX) -o $@ $^

libipasirlawa.a: lawa.o lawa-ipasir.o
	ar rc $@ lawa.o lawa-ipasir.o
	ranlib $@

clean:
	rm -f $(OBJS) $(TARGET)
