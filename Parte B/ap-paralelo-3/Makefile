all: ap-paralelo-3


ap-paralelo-3: ap-paralelo-3.c image-lib.c image-lib.h
	gcc -Wall -pedantic -lpthread ap-paralelo-3.c image-lib.c -g -o ap-paralelo-3 -lgd 

clean:
	rm ap-paralelo-3

clean-data:
	rm -fry ./Resize-dir ./Thumbnail-dir ./Watermark-dir