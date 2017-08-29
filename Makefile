plugins.so: plugins.c
	gcc -shared -fPIC -o $@ $^

.PHONY: install
install: plugins.so
	cp $^ /usr/ladspa/grissess.so
