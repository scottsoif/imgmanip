# change the default compiler with the one
# that supports C++20 modules feature
CC=g++-11
CFLAGS = -g -Wall -std=c++20 -c
LDFLAGS = -g

# optional library if we want to include
LDLIBS = -larmadillo

export CPATH=lib/include
export LIBRARY_PATH=lib/lib

main: main.o imgio.o
	$(CC) main.o imgio.o -o main $(LDFLAGS) $(LDLIBS)

main.o: main.cpp imgmanip/imgio/imgio.h
	$(CC) $(CFLAGS) main.cpp

# compiling the imgio module
imgio.o: imgmanip/imgio/imgio.cpp imgmanip/imgio/imgio.h
	$(CC) $(CFLAGS) imgmanip/imgio/imgio.cpp


# main: main.o imgio.o
# 	$(CC) main.o imgio.o -o main $(LDFLAGS) $(LDLIBS)

# main.o: main.cpp imgio.pcm
# 	$(CC) $(CFLAGS) -fmodule-file=imgio.pcm -c main.cpp -o main.o

# # compiling the imgio module
# imgio.o: imgio.pcm
# 	$(CC) $(CFLAGS) -c imgio.pcm -o imgio.o

# imgio.pcm: imgmanip/imgio/imgio.cpp
# 	$(CC) $(CFLAGS) --compile imgmanip/imgio/imgio.cpp -o imgio.pcm

.PHONY: clean
clean:
	rm -rf *.o main *.out *.pcm