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

/* length of the files array (number of files to be processed) */
int nn_files = 0;
char files[50][50];
/* variável com a watermark */
gdImagePtr watermark_img;


void * water_function(void * arg){

    /* file name of the image created and to be saved on disk	 */
	char out_file_name[100];
    gdImagePtr in_img, out_watermark_img;
    char folder_paths[50];
    gdImagePtr corruption_check;

    for (int i = 0; i < nn_files; i++){

        strcpy(folder_paths, WATER_DIR);
        strcat(folder_paths, files[i]);

        corruption_check = read_png_file(folder_paths);

        if( corruption_check!= NULL ){
            printf("%s encontrado\n", files[i]);
            gdImageDestroy(corruption_check);
        }else{
            printf("%s nao encontrado\n", files[i]);
        
            printf("watermark  %s\n", files[i]);
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
                sprintf(out_file_name, "%s%s", WATER_DIR, files[i]);
                if(write_png_file(out_watermark_img, out_file_name) == 0){
                    fprintf(stderr, "Impossible to write %s image\n", out_file_name);
                }
                gdImageDestroy(out_watermark_img);
            }
            gdImageDestroy(in_img);
        }
	}    
    return (void*) 0;
}

void * resize_function(void * arg){

    /* file name of the image created and to be saved on disk	 */
	char out_file_name[100];
    gdImagePtr in_img, out_watermark_img, out_resized_img;
    char folder_paths[50];
    gdImagePtr corruption_check;

    for (int i = 0; i < nn_files; i++){

        strcpy(folder_paths, RESIZE_DIR);
        strcat(folder_paths, files[i]);

        corruption_check = read_png_file(folder_paths);

        if( corruption_check!= NULL ){
            printf("%s encontrado\n", files[i]);
            gdImageDestroy(corruption_check);
        }else{
            printf("%s nao encontrado\n", files[i]);

            printf("resize %s\n", files[i]);
            /* load of the input file */
            in_img = read_png_file(files[i]);
            if (in_img == NULL){
                fprintf(stderr, "Impossible to read %s image\n", files[i]);
                continue;
            }
            /* resizes of each image */
            out_watermark_img = add_watermark(in_img, watermark_img);
            if (out_watermark_img == NULL) {
                fprintf(stderr, "Impossible to add watermark to %s image\n", files[i]);
            }else{
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
                gdImageDestroy(out_watermark_img);
            }
            gdImageDestroy(in_img);
        }
	}
    return (void*) 0;
}

void * thumb_function(void * arg){

    /* file name of the image created and to be saved on disk	 */
	char out_file_name[100];
    gdImagePtr in_img, out_watermark_img, out_thumb_img;
    char folder_paths[50];
    gdImagePtr corruption_check;

    for (int i = 0; i < nn_files; i++){

        strcpy(folder_paths, THUMB_DIR);
        strcat(folder_paths, files[i]);

        corruption_check = read_png_file(folder_paths);

        if(corruption_check != NULL){
            printf("%s ficheiro encontrado\n", files[i]);
            gdImageDestroy(corruption_check);
        }else{
            printf("%s nao encontrado\n", files[i]);

            printf("thumbnail %s\n", files[i]);
            /* load of the input file */
            in_img = read_png_file(files[i]);
            if (in_img == NULL){
                fprintf(stderr, "Impossible to read %s image\n", files[i]);
                continue;
            }

            /* creation of thumbnail image */
            out_watermark_img = add_watermark(in_img, watermark_img);
            if (out_watermark_img == NULL) {
                fprintf(stderr, "Impossible to creat thumbnail of %s image\n", files[i]);
            }else{
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
                gdImageDestroy(out_watermark_img);
            }
            gdImageDestroy(in_img);
        }
	}
    return (void*) 0;
}

int main(int argc, char *argv[]){

    char *pasta = argv[1];

	pthread_t thread_id[50];

	/* ficheiro que contém o nome dos ficheiros das imagens */
	FILE *fpin;

    /* leitura da watermark */
    watermark_img = read_png_file("watermark.png");
	if(watermark_img == NULL){
		fprintf(stderr, "Impossible to read %s image\n", "watermark.png");
		exit(-1);
	}

    /* mudança de pasta */
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
	
    /* thread create (times 3) */
	pthread_create(&(thread_id[0]), NULL, water_function, NULL);
    pthread_create(&(thread_id[1]), NULL, resize_function, NULL);
    pthread_create(&(thread_id[2]), NULL, thumb_function, NULL);

    for(int j = 0; j < 3; j++){
        pthread_join(thread_id[j], NULL);
    };

    gdImageDestroy(watermark_img);
	
	exit(0);
}