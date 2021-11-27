##
# General Complier for Single File
#
# @file
# @version 0.1
CC=g++
CFLAGS=-std=c++20 -O2 -g -fprofile-arcs -ftest-coverage -lprofiler -lpthread
DESP=*.cpp
OUT=a.out

all: $(DESP)
	@echo "=====================start complier======================="
	$(CC) -o $(OUT) $^ $(CFLAGS)
	@echo "=====================complier finish======================"

gcov:
	gcov $(DESP)
	lcov -d . -t 'Analyse Coverage' -o 'analyse.info' -b . -c
	genhtml -o analyse analyse.info
	ln -s analyse/index.html ./analyse.html
	rm *.gcda *.gcno *.info

gprof:
	pprof --pdf $(OUT) *.prof > analyse.pdf
	rm *.prof

clean:
	rm -rf $(OUT) analyse analyse.* *.prof *.gcno output

# end
