compile = g++ \
	  -g -Wall -std=c++11 \
	  -static -static-libgcc \
	  -static-libstdc++ \
	  -I../ \
	  $1.cpp \
	  ../libjumpfcontext.a ../libmakefcontext.a ../libontopfcontext.a \
	  -o $1.o

all :
	$(call compile, SimpleContinuation)
	$(call compile, NestedContinuation)

clean :
	rm *.o
