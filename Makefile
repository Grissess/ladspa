plugins.so: plugins.c
	gcc -shared -fPIC -o $@ $^

README.md: README.md.head
	perl -n docs.pl plugins.c | cat $^ - > $@

.PHONY: install
install: plugins.so
	cp $^ /usr/ladspa/grissess.so
