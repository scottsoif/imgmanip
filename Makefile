# change the default compiler with the one
# that supports C++20 modules feature
CC='g++-11'
CFLAGS = -g -Wall -std=c++20 -c
LDFLAGS = -g

# optional library if we want to include
LDLIBS = -larmadillo -ljpeg -lpng

export CPATH=lib/include
export LIBRARY_PATH=lib/lib


main: main.o
	$(CC) main.o -o main $(LDFLAGS) $(LDLIBS)

main.o: main.cpp imgmanip/imgio/pch.h.gch imgmanip/imgio/imgio.h imgmanip/homography.h imgmanip/mosaic.h
	$(CC) $(CFLAGS) main.cpp -include imgmanip/imgio/pch.h 
#	$(CC) $(CFLAGS) -x c++-header imgmanip/imgio/pch.h -I lib/boost_1_77_0

imgmanip/imgio/pch.h.gch: imgmanip/imgio/pch.h
	$(CC) $(CFLAGS) -x c++-header imgmanip/imgio/pch.h -I lib/boost_1_77_0



.PHONY: clean
clean:
	rm -rf *.o main *.out *.pcm
