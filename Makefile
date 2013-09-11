
SOURCE_FILE = main.cpp features.cpp featuresratio.cpp tools.cpp naivebayesclassifier.cpp


nbc_clang:
	clang++ -std=c++11 -stdlib=libc++ -O3 ${SOURCE_FILE} -o ./bin/nbc.o

nbc_gcc:
	g++ -std=c++0x -O3 ${SOURCE_FILE} -o ./bin/nbc.o

nbc_gdb:
	g++ -g -std=c++0x ${SOURCE_FILE} -o ./bin/nbc_gdb.o

clean:
	rm ./bin/*.o ./*.o
