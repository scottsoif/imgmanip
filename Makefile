# change the default compiler with the one
# that supports C++20 modules feature
CC=/opt/homebrew/bin/g++-11
CFLAGS = -g -Wall -c -std=c++20 -fmodules-ts
LDFLAGS = -g

# optional library if we want to include
LDLIBS =

main: main.o imgio.o
	$(CC) $(LDFLAGS) main.o imgio.o -o main

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

# compiling the imgio module
imgio.o: imgmanip/imgio/imgio.cpp
	$(CC) $(CFLAGS) imgmanip/imgio/imgio.cpp

.PHONY: clean
clean:
	rm -rf *.o main *.out