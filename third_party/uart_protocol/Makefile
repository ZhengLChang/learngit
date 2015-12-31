
all: app/app_proto.o link/link_proto.o tty/tty_proto.o
	$(AR) cr libprotocol.a $^

clean:
	- @rm *.a
	- @rm app/*.o
	- @rm link/*.o
	- @rm tty/*.o
