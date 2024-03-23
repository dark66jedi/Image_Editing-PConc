#include <stdio.h>
#include <pthread.h>  
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include "gd.h"
#include "image-lib.h"

/* the directories wher output files will be placed */
#define RESIZE_DIR "./Resize-dir/"
#define THUMB_DIR "./Thumbnail-dir/"
#define WATER_DIR "./Watermark-dir/"

/* variable of the threads */
int n_threads = 0;
/* length of the files array (number of files to be processed	 */
int nn_files = 0;
/* array containg the names of files to be processed */
char files[50][50];
/* variável com a watermark */
gdImagePtr watermark_img;


void * thread_function(void * arg){

	/* input images */
	gdImagePtr in_img;
	/* output images */
	gdImagePtr out_thumb_img, out_resized_img, out_watermark_img;

	char folder_paths[3][50];
	gdImagePtr corruption_check1, corruption_check2, corruption_check3;

	/* file name of the image created and to be saved on disk	 */
	char out_file_name[100];

	int n = (int)(intptr_t) arg;
    
	for (int i = 0; i < nn_files; i++){

		if(i%n_threads != n){
			continue;
		}

		strcpy(folder_paths[0], WATER_DIR);
        strcat(folder_paths[0], files[i]);

        corruption_check1 = read_png_file(folder_paths[0]);

		strcpy(folder_paths[1], RESIZE_DIR);
        strcat(folder_paths[1], files[i]);

        corruption_check2 = read_png_file(folder_paths[1]);

		strcpy(folder_paths[2], THUMB_DIR);
        strcat(folder_paths[2], files[i]);

        corruption_check3 = read_png_file(folder_paths[2]);

	    printf("%d watermark  %s\n", n, files[i]);
		/* load of the input file */
	    in_img = read_png_file(files[i]);
		if (in_img == NULL){
			fprintf(stderr, "Impossible to read %s image\n", files[i]);
			continue;
		}

		/* add watermark */
		out_watermark_img = add_watermark(in_img, watermark_img);
  		if (out_watermark_img == NULL) {
            fprintf(stderr, "Impossible to creat thumbnail of %s image\n", files[i]);
        }else{
			/* save watermark */
			if(corruption_check1 == NULL){
				sprintf(out_file_name, "%s%s", WATER_DIR, files[i]);
				if(write_png_file(out_watermark_img, out_file_name) == 0){
					fprintf(stderr, "Impossible to write %s image\n", out_file_name);
				}
			}else{
				printf("%s encontrado\n", files[i]);
				gdImageDestroy(corruption_check1);
			}
			/* resizes of each image */
			if(corruption_check2 == NULL){
				out_resized_img = resize_image(out_watermark_img, 800);
				if (out_resized_img == NULL) {
					fprintf(stderr, "Impossible to resize %s image\n", files[i]);
				}else{
					/* save resized */
					sprintf(out_file_name, "%s%s", RESIZE_DIR, files[i]);
					if(write_png_file(out_resized_img, out_file_name) == 0){
						fprintf(stderr, "Impossible to write %s image\n", out_file_name);
					}
					gdImageDestroy(out_resized_img);
				}
			}else{
				printf("%s encontrado\n", files[i]);
				gdImageDestroy(corruption_check2);
			}
			/* creation of thumbnail image */
			if(corruption_check3 == NULL){
				out_thumb_img = make_thumb(out_watermark_img, 150);
				if (out_thumb_img == NULL) {
					fprintf(stderr, "Impossible to creat thumbnail of %s image\n", files[i]);
				}else{
					/* save thumbnail image */
					sprintf(out_file_name, "%s%s", THUMB_DIR, files[i]);
					if(write_png_file(out_thumb_img, out_file_name) == 0){
						fprintf(stderr, "Impossible to write %s image\n", out_file_name);
					}
					gdImageDestroy(out_thumb_img);
				}
			}else{
				printf("%s encontrado\n", files[i]);
				gdImageDestroy(corruption_check3);
			}
			gdImageDestroy(out_watermark_img);
		}
		gdImageDestroy(in_img);

	}
    return (void *) 0;
}

int main(int argc, char *argv[]){

	/* localização das imagens */
	char *pasta = argv[1];

	/* número de threads */
	n_threads = atoi(argv[2]);

	pthread_t thread_id[100];

	/* ficheiro que contém o nome dos ficheiros das imagens */
	FILE *fpin;

	watermark_img = read_png_file("watermark.png");
	if(watermark_img == NULL){
		fprintf(stderr, "Impossible to read %s image\n", "watermark.png");
		exit(-1);
	}

	chdir(pasta);

	fpin = fopen("image-list.txt", "r");

	while(fscanf(fpin, "%s", files[nn_files]) != EOF){
		nn_files++;
	}

	fclose(fpin);

	/* creation of output directories */
	if (create_directory(RESIZE_DIR) == 0){
		fprintf(stderr, "Impossible to create %s directory\n", RESIZE_DIR);
		exit(-1);
	}
	if (create_directory(THUMB_DIR) == 0){
		fprintf(stderr, "Impossible to create %s directory\n", THUMB_DIR);
		exit(-1);
	}
	if (create_directory(WATER_DIR) == 0){
		fprintf(stderr, "Impossible to create %s directory\n", WATER_DIR);
		exit(-1);
	}

	/* thread create (times n_threads) */
	for(int i = 0; i < n_threads; i++){
		pthread_create(&(thread_id[i]), NULL, thread_function, (void*)(intptr_t) i);
	}
	for(int i = 0; i < n_threads; i++){
		pthread_join(thread_id[i], NULL);
	}


	gdImageDestroy(watermark_img);
	exit(0);
}