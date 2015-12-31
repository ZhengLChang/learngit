TAGS=$(basename $(wildcard *.c))


all: clean $(TAGS)
	mv $(TAGS) atcom_tools

clean:
	@-touch $(wildcard *.c)
	@rm -rf $(basename $(wildcard *.c))

%.: %.c 
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -g -c -o $@ $<




install:
	cp -rf  atcom_tools/* $(INSTALL_PATH) -rf
