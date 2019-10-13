#Makefile


CXX=gcc
ELF=basicShell
SOURCE_FILE=basicShell.c


basicShell:
	$(CXX) -o $(ELF) $(SOURCE_FILE)

basicShell-debug:
	$(CXX) -g -o $(ELF) $(SOURCE_FILE)

clean:
	rm $(ELF)
