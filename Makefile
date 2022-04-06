# change the default compiler with the one
# that supports C++20 modules feature
CC='g++-11'
CFLAGS = -g -Wall -std=c++20 -c
LDFLAGS = -g

# optional library if we want to include
LDLIBS = -larmadillo -ljpeg

export CPATH=lib/include
export LIBRARY_PATH=lib/lib

main: main.o
	$(CC) main.o -o main $(LDFLAGS) $(LDLIBS)

main.o: main.cpp imgmanip/imgio/imgio.h
	$(CC) $(CFLAGS) main.cpp -I lib/boost_1_77_0

# compiling the imgio "module"
# imgio.o: imgmanip/imgio/imgio.cpp imgmanip/imgio/imgio.h
# 	$(CC) $(CFLAGS) imgmanip/imgio/imgio.cpp


.PHONY: clean
clean:
	rm -rf *.o main *.out *.pcm
