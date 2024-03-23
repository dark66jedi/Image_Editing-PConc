all: ap-paralelo-2


ap-paralelo-2: ap-paralelo-2.c image-lib.c image-lib.h
	gcc -Wall -pedantic -lpthread ap-paralelo-2.c image-lib.c -g -o ap-paralelo-2 -lgd 

clean:
	rm ap-paralelo-2

clean-data:
	rm -fry ./Resize-dir ./Thumbnail-dir ./Watermark-dir