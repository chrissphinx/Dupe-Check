dupchk: dupchk.c digger.h hasher.h
	clang -O0 -w dupchk.c -o dupchk -lcrypto -ggdb

exec: clean dupchk 
	./setup.sh; ./dupchk test_dir

memo: clean dupchk 
	valgrind --track-origins=yes ./dupchk test_dir

clean:
	rm -rf dupchk dupchk.dSYM dups test_dir
