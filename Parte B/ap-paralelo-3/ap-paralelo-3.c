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

/* pipes declaration */
int pipefd1[2];     // pipe main-watermark
int pipefd2[2];     // pipe watermark-resize
int pipefd3[2];     // pipe resize-thumbnail

/* variável com a watermark */
gdImagePtr watermark_img;


void * water_function(void * arg){

    /* original filename */
    char og_file[50];
    /* output filename */
    char folder_paths[50];
    gdImagePtr in_img, out_watermark_img;
    gdImagePtr corruption_check;

    read(pipefd1[0], &og_file, 50*sizeof(char));

    while(strcmp(og_file, "0") != 0){

        strcpy(folder_paths, WATER_DIR);
        strcat(folder_paths, og_file);

        corruption_check = read_png_file(folder_paths);

        if( corruption_check!= NULL ){
            printf("%s encontrado\n", folder_paths);
            gdImageDestroy(corruption_check);
        }else{
            printf("%s nao encontrado\n", folder_paths);
        
            printf("watermark  %s\n", og_file);
            /* load of the input file */
            in_img = read_png_file(og_file);
            if (in_img == NULL){
                fprintf(stderr, "Impossible to read %s image\n", og_file);
                continue;
            }

            /* add watermark */
            out_watermark_img = add_watermark(in_img, watermark_img);
            if (out_watermark_img == NULL) {
                fprintf(stderr, "Impossible to creat thumbnail of %s image\n", og_file);
            }else{
                /* save watermark */
                if(write_png_file(out_watermark_img, folder_paths) == 0){
                    fprintf(stderr, "Impossible to write %s image\n", folder_paths);
                }
                gdImageDestroy(out_watermark_img);
            }
            gdImageDestroy(in_img);
        }
        write(pipefd2[1], &og_file, 50*sizeof(char));
        read(pipefd1[0], &og_file, 50*sizeof(char));
	}    
    return (void*) 0;
}

void * water_thread_create(void * arg){
    pthread_t water_thread_id[50];
    int n_threads = *(int*)arg;

    for(int i = 0; i < n_threads; i++){
        pthread_create(&(water_thread_id[i]), NULL, water_function, NULL);
    }
    
    for(int j = 0; j < n_threads; j++){
        pthread_join(water_thread_id[j], NULL);
    };

    /* a way to ensure that the resize threads come to a stop*/
    for(int stop_thread = 0; stop_thread < n_threads; stop_thread++){
        char stop[50];
        strcpy(stop, "0");
        write(pipefd2[1], &stop, 50*sizeof(char));
    }
    return (void*) 0;
}

void * resize_function(void * arg){

    /* original filename and watermark filename*/
	char og_file[50], watermark_file[50];
    /* output filename */
    char folder_paths[50];
    gdImagePtr in_img, out_resized_img;
    gdImagePtr corruption_check;

    read(pipefd2[0], &og_file, 50*sizeof(char));

    while(strcmp(og_file, "0") != 0){

        strcpy(folder_paths, RESIZE_DIR);
        strcat(folder_paths, og_file);

        strcpy(watermark_file, WATER_DIR);
        strcat(watermark_file, og_file);

        corruption_check = read_png_file(folder_paths);

        if( corruption_check != NULL ){
            printf("%s encontrado\n", folder_paths);
            gdImageDestroy(corruption_check);
        }else{
            printf("%s nao encontrado\n", folder_paths);

            printf("resize %s\n", og_file);
            /* load of the input file */
            in_img = read_png_file(watermark_file);
            if (in_img == NULL){
                fprintf(stderr, "Impossible to read %s image\n", watermark_file);
                continue;
            }
            /* resizes of each image */
            out_resized_img = resize_image(in_img, 800);
            if (out_resized_img == NULL) {
                fprintf(stderr, "Impossible to resize %s image\n", og_file);
            }else{
                /* save resized */
                if(write_png_file(out_resized_img, folder_paths) == 0){
                    fprintf(stderr, "Impossible to write %s image\n", folder_paths);
                }
                gdImageDestroy(out_resized_img);
            }
            gdImageDestroy(in_img);
        }
        write(pipefd3[1], &og_file, 50*sizeof(char));
        read(pipefd2[0], &og_file, 50*sizeof(char));
	}
    return (void*) 0;
}

void * resize_thread_create(void * arg){
    pthread_t resize_thread_id[50];
    int n_threads = *(int*)arg;

    for(int i = 0; i< n_threads; i++){
        pthread_create(&(resize_thread_id[i]), NULL, resize_function, NULL);
    }
    
    for(int j = 0; j < n_threads; j++){
        pthread_join(resize_thread_id[j], NULL);
    };

    /* a way to ensure that the resize threads come to a stop*/
    for(int stop_thread = 0; stop_thread < n_threads; stop_thread++){
        char stop[50];
        strcpy(stop, "0");
        write(pipefd3[1], &stop, 50*sizeof(char));
    }
    return (void*) 0;
}

void * thumb_function(void * arg){

    /* original filename and watermark filename*/
	char og_file[50], watermark_file[50];
    /* output filename */
    char folder_paths[50];
    gdImagePtr in_img, out_thumb_img;
    gdImagePtr corruption_check;

    read(pipefd3[0], &og_file, 50*sizeof(char));

    while(strcmp(og_file, "0") != 0){

        strcpy(folder_paths, THUMB_DIR);
        strcat(folder_paths, og_file);

        strcpy(watermark_file, WATER_DIR);
        strcat(watermark_file, og_file);

        corruption_check = read_png_file(folder_paths);

        if(corruption_check != NULL){
            printf("%s ficheiro encontrado\n", folder_paths);
            gdImageDestroy(corruption_check);
        }else{
            printf("%s nao encontrado\n", folder_paths);

            printf("thumbnail %s\n", og_file);
            /* load of the input file */
            in_img = read_png_file(watermark_file);
            if (in_img == NULL){
                fprintf(stderr, "Impossible to read %s image\n", watermark_file);
                continue;
            }

            /* creation of thumbnail image */
            out_thumb_img = make_thumb(in_img, 150);
            if (out_thumb_img == NULL) {
                    fprintf(stderr, "Impossible to create thumbnail of %s image\n", og_file);
            }else{
                /* save thumbnail image */
                if(write_png_file(out_thumb_img, folder_paths) == 0){
                    fprintf(stderr, "Impossible to write %s image\n", folder_paths);
                }
                gdImageDestroy(out_thumb_img);
            }
            gdImageDestroy(in_img);
        }
        read(pipefd3[0], &og_file, 50*sizeof(char));
	}
    return (void*) 0;
}

void * thumb_thread_create(void * arg){
    pthread_t thumb_thread_id[50];
    int n_threads = *(int*)arg;

    for(int i = 0; i< n_threads; i++){
        pthread_create(&(thumb_thread_id[i]), NULL, thumb_function, NULL);
    }
    
    for(int j = 0; j < n_threads; j++){
        pthread_join(thumb_thread_id[j], NULL);
    };
    return (void*) 0;
}

int main(int argc, char *argv[]){

    char *pasta = argv[1];
    char filename[50];

	pthread_t thread_id[50];

	/* ficheiro que contém o nome dos ficheiros das imagens */
	FILE *fpin;

    /* pipes creation */
    pipe(pipefd1);
    pipe(pipefd2);
    pipe(pipefd3);

    /* leitura da watermark */
    watermark_img = read_png_file("watermark.png");
	if(watermark_img == NULL){
		fprintf(stderr, "Impossible to read %s image\n", "watermark.png");
		exit(-1);
	}

    /* argumento do número de threads a criar em cada estágio */
    
    int *n_threads = malloc(sizeof(int));
    *n_threads = atoi(argv[2]);

    /* mudança de pasta */
    chdir(pasta);

	fpin = fopen("image-list.txt", "r");

	while(fscanf(fpin, "%s", filename) != EOF){
        write(pipefd1[1], &filename, 50*sizeof(char));
        // sleep(2);
        // read(pipefd1[0], &response, 50*sizeof(char));
        // printf("%s\n", response);
	}

    /* a way to ensure that the watermark threads come to a stop*/
    for(int stop_thread = 0; stop_thread < *n_threads; stop_thread++){
        char stop[50];
        strcpy(stop, "0");
        write(pipefd1[1], &stop, 50*sizeof(char));
    }

    // char response[50];
    // read(pipefd1[0], &response, 50*sizeof(char));
    // while(strcmp(response, "0") != 0){        
    //     printf("%s\n", response);
    //     read(pipefd1[0], &response, 50*sizeof(char));
    // }

    // exit(0);

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
	pthread_create(&(thread_id[0]), NULL, water_thread_create, n_threads);
    pthread_create(&(thread_id[1]), NULL, resize_thread_create, n_threads);
    pthread_create(&(thread_id[2]), NULL, thumb_thread_create, n_threads);

    for(int j = 0; j < 3; j++){
        pthread_join(thread_id[j], NULL);
    };

    gdImageDestroy(watermark_img);
	
	exit(0);
}