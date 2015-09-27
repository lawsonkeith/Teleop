all: teleop

clean:
	rm -f teleop libgamepad.so libgamepad.so.1 telop.o


libgamepad.so: libgamepad.so.1
	ln -sf libgamepad.so.1 libgamepad.so

teleop: teleop.c libgamepad.so
	$(CC) -o $@ $< -Wl,-rpath,. -L.  -lcurses -ludev

install: libgamepad.so

.PHONY: all clean install
