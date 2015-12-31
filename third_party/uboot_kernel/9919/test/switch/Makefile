TAGS=$(basename $(wildcard *.c))


all: clean $(TAGS)
	mv $(TAGS) ../atcom_tools

clean:
	@-touch $(wildcard *.c)
	@rm -rf $(basename $(wildcard *.c))
	@rm -rf ../atcom_tools/test_switch 

%.: %.c 
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -g -c -o $@ $<



