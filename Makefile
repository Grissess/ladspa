plugins.so: plugins.c
	gcc -shared -fPIC -o $@ $^

README.md: README.md.head plugins.c
	perl -n docs.pl plugins.c | cat README.md.head - > $@

.PHONY: install
install: plugins.so
	cp $^ /usr/ladspa/grissess.so
