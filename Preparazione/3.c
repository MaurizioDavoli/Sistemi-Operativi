
/*
*   Scrivere un programma C invert che dato un file *A* ne inverte il contenuto e 
*   lo memorizza in nuovo file B. Il programma deve:
*       - riportare il contenuto di A in memoria;
*       - invertire la posizione di ciascun byte utilizzando un 
          numero N di thread/processi concorrenti;
*       - scrivere il risultato in un nuovo file B.
*
*   A, B e N sono parametri che il programma deve acquisire da linea di comando.
*/


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>

// file offset size, off_t defined in the "unistd.h" library
off_t file_lenght;
char *input_buffer, *output_buffer;
int effective_thread_count;

void* thread_func(void* param){

    // idx-th thread created
    int idx = *(int*)param;

    // compute the common slice size
    off_t slice_size = file_lenght/effective_thread_count;
    off_t remainder  = file_lenght%effective_thread_count;
    // If I am the last created thread get the remaining bytes 
    off_t my_slice_size =  slice_size + (-(idx == (effective_thread_count-1)) & remainder); // AGAIN!!! but this try to understand it by yourself :) 
    
    printf("I am thread with index %d\n", *(int*)param);

    for(int i=0;i<my_slice_size;i++){
        size_t source_byte       = idx*slice_size;                      //\\    <-- skip the slice of other threads
        source_byte             += i;                                  //  \\   <-- this is the byte I want to copy into the output buffer     
        size_t destination_byte  = file_lenght-1-source_byte;         //====\\  <-- this is the position of the source byte in the destination memory buffer
        output_buffer[destination_byte] = input_buffer[source_byte]; //      \\ <-- write in the output buffer
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if(argc != 4){
        printf("Command usage: <input file> <output file> <num_threads>");
        exit(1);
    }

    char *in_file_path  = argv[1];        // parameter A
    char *out_file_path = argv[2];        // parameter B
    int num_threads     = atoi(argv[3]);  // parameter N
    if(num_threads < 1) num_threads = 1;  // min thread number set to 1

    int input_file  = open(in_file_path, O_RDONLY);                               
    int output_file = open(out_file_path, O_WRONLY|O_CREAT|O_TRUNC, 0660);

    if( input_file < 0) { printf("error while opening %s\n",  in_file_path); exit(1);}
    if(output_file < 0) { printf("error while opening %s\n", out_file_path); exit(1);}

    // return the length of the file in byte 
    file_lenght = lseek(input_file, 0, SEEK_END);
    // reset the pointer to the beginning of the file
    lseek(input_file, 0, SEEK_SET);

    // allocate memory for read and write
    input_buffer  = malloc(file_lenght+1);
    output_buffer = malloc(file_lenght+1);
    input_buffer[file_lenght] = 0;
    output_buffer[file_lenght] = 0;

    // sanity check 
    if(!input_buffer || !output_buffer) {printf("Cannot allocate memory\n");exit(1);}

    // s a data type in C that is used to represent the size of objects in memory
    size_t read_bytes = 0;

    // read all the file in memory 
    while(read_bytes < file_lenght){
        size_t currently_read_bytes;
        currently_read_bytes = read(input_file, input_buffer+read_bytes, file_lenght);
        
        if(currently_read_bytes < 0) {printf("Error while reading input file\n");exit(1);}
        
        read_bytes += currently_read_bytes;
    }

    /*
    *   Thread Inverter part
    */
   
    // The max number of parallel threads can not be bigger than the fength of the file
    if (file_lenght > num_threads) effective_thread_count = num_threads;
    else effective_thread_count = file_lenght;

    printf("Thread number = %d \n", effective_thread_count);

    // allocate memory for the threads 
    // pthread_t is a data type in C that represents a thread ID
    // threads_ids is a pointer to a block of memory that will store the thread IDs
    pthread_t *threads_ids = malloc(effective_thread_count*sizeof(pthread_t));
    int *indexes           = malloc(effective_thread_count*sizeof(pthread_t));

    for(int i = 0; i < effective_thread_count; i++){
        indexes[i] = i;

        pthread_create(threads_ids+i, NULL, thread_func, indexes+i);
        
    }

    // join created threads at work done
    for(int i =0; i< effective_thread_count; i++) pthread_join(threads_ids[i], NULL);
  
    // free the allocated memeory for the therads ids
    free(threads_ids);
    free(indexes);

    size_t write_bytes = 0;
    while(write_bytes < file_lenght){

        size_t cur_write_bytes = write(output_file, output_buffer+write_bytes, file_lenght);

        // sanity check 
        if(cur_write_bytes < 0) {printf("Error while writing output file\n");exit(1);}
        
        write_bytes += cur_write_bytes;
    }

    // free allocated memory
    free(input_buffer);
    free(output_buffer);  

    close(input_file);
    close(output_file);
}
