all: ap-paralelo-1


ap-paralelo-1: ap-paralelo-1.c image-lib.c image-lib.h
	gcc -Wall -pedantic -lpthread ap-paralelo-1.c image-lib.c -g -o ap-paralelo-1 -lgd 

clean:
	rm ap-paralelo-1

clean-data:
	rm -fry ./Resize-dir ./Thumbnail-dir ./Watermark-dir