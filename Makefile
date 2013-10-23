dupchk: dupchk.c digger.h hasher.h
	clang -O3 -w dupchk.c -o dupchk -lcrypto; ./setup.sh

exec: clean dupchk 
	./dupchk test_dir; ls -la dups

clean:
	rm -rf dupchk dupchk.dSYM dups test_dir
