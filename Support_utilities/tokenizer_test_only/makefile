#g++ or direct path to it
CC = g++
#Mingw or Unix
CompilerVersion = Mingw
	
tokenizer.exe : 	tokenizer.o 
	$(CC) -O2 -std=c++11 -Wall -o tokenizer.exe tokenizer.o 

#Wl,option Pass option as an option to the linker
# -s  Remove all symbol table and relocation information from the executable	

tokenizer.o	:	tokenizer.cpp 
	$(CC) -c -Wall -O2 -fconserve-space tokenizer.cpp

clean:
	del *.o
	del *.exe