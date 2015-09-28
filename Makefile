all: teleop_client

clean:
	rm -f teleop_client libgamepad.so libgamepad.so.1 telop.o


libgamepad.so: libgamepad.so.1
	ln -sf libgamepad.so.1 libgamepad.so

teleop: teleop.c 
	$(CC) -o $@ $< -Wl,-rpath,. -L.  -lcurses -ludev

install: libgamepad.so

.PHONY: all clean install
