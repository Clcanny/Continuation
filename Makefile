compile :
	g++ -c jump_x86_64_sysv_elf_gas.S -o jump_fcontext.o
	g++ -c make_x86_64_sysv_elf_gas.S -o make_fcontext.o
	g++ -c ontop_x86_64_sysv_elf_gas.S -o ontop_fcontext.o
	ar crf libjumpfcontext.a jump_fcontext.o
	ar crf libmakefcontext.a make_fcontext.o
	ar crf libontopfcontext.a ontop_fcontext.o
	g++ -g -Wall -std=c++11 main.cpp libjumpfcontext.a libmakefcontext.a libontopfcontext.a -o main.o -static -static-libgcc -static-libstdc++

objdump : compile
	objdump -D main.o > main.S

clean :
	- rm *.o
	- rm *.a
	- rm core
	- rm main.S
