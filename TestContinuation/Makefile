compilen = g++ \
	  -g -Wall -std=c++11 \
	  -I../ \
	  -DWITHOUT_BOOST \
	  $1.cpp \
	  libjumpfcontext.a libmakefcontext.a libontopfcontext.a \
	  -static -static-libgcc \
	  -static-libstdc++ \
	  -o $1.o

compileb = g++ \
	   -g -Wall -std=c++11 \
	   $1.cpp \
	   -o $1.o \
	   -lboost_system \
	   -lboost_context \
	   -lboost_thread \
	   -lpthread \
	   -static \
	   -static-libgcc \
	   -static-libstdc++

test :
	g++ -c ../jump_x86_64_sysv_elf_gas.S -o jump_fcontext.o
	g++ -c ../make_x86_64_sysv_elf_gas.S -o make_fcontext.o
	g++ -c ../ontop_x86_64_sysv_elf_gas.S -o ontop_fcontext.o
	ar crf libjumpfcontext.a jump_fcontext.o
	ar crf libmakefcontext.a make_fcontext.o
	ar crf libontopfcontext.a ontop_fcontext.o
	$(call compilen, SimpleContinuation)
	$(call compilen, FibonacciContinuation)
	$(call compilen, NestedContinuation)
	$(call compilen, Parser)

stanard :
	$(call compileb, SimpleContinuation)
	$(call compileb, FibonacciContinuation)
	$(call compileb, NestedContinuation)
	$(call compileb, Parser)

clean :
	- rm *.o
	- rm *.a
