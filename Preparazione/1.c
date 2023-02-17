
/*
*   TESTO:
*   Nei sistemi operativi UNIX, /dev/urandom è un dispositivo a caratteri (char device) 
*   virtuale in grado di generare numeri casuali. Nello specifico, l’operazione di 
*   lettura dal relativo file produce byte casuali. 
*   Scrivere un programma C che genera un file con contenuto interamente randomico. Il programma:
*       - prende come parametri da linea di comando: un numero N e una stringa 
*         S da usare come nome del file da creare;
*       - crea un file S contenente N byte randomici;
*       - utilizza il dispositivo /dev/random come sorgente di numeri pseudo-casuali.
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFERSIZE 1
#define RANDOM_GENERATOR  "/dev/urandom"

int main(int argc, char* argv[]){

    int input_file;
    int output_file;

    // check if parameters number is the expected one 
	if(argc != 3){
        printf("usage: program <number> <target file>");
        return 1;
    }

    printf("N = %s \n", argv[1]);
    printf("S = %s \n", argv[2]);

    // open input file in read_only mode
    input_file = open(RANDOM_GENERATOR, O_RDONLY);

    // check for errors in input file reading
    if (input_file == -1) {
        printf("error reading %s", RANDOM_GENERATOR);
        return 1;
    }

    /* 
        The first argument, "argv[2]", is a pointer to a string containing the name of 
        the file to be opened.

        The second argument, "O_WRONLY|O_CREAT|O_TRUNC", is a bitwise OR combination of three 
        flags that specify how the file should be opened:
            "O_WRONLY" specifies that the file should be opened for writing only.
            "O_CREAT" specifies that the file should be created if it does not already exist.
            "O_TRUNC" specifies that the file should be truncated to zero length if it already exists.

        The third argument, "0660", specifies the permissions that should be set for the file.
    */
    output_file = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 0660);

    // check for errors in output file reading
    if (output_file == -1) {
        printf("error reading %s", argv[2]);
        return 1;
    }

    int operation_ended = 0;

    // declaring an array of characters named "buffer" with a size of "BUFERSIZE"
    char buffer[BUFFERSIZE];

    long file_size = atoi(argv[1]);
    int readed_byte_size;
    int writed_byte_size;
    long written_byte = 0;

    if (!operation_ended){

        /*
            When the "read" function is called, it attempts to read up to "BUFFERSIZE" bytes of data 
            from the input file or device and stores them in the buffer pointed to by the 
            "buffer" argument. The function returns the number of bytes read, 
            or -1 if an error occurred.
        */
        readed_byte_size = read(input_file, buffer, BUFFERSIZE);
        if (readed_byte_size == -1){
            printf("error in reading from imput");
            return 1;
        }

        /*
            When the "write" function is called, it writes the specified number of bytes 
            from the buffer pointed to by "buffer" to the output file identified by the 
            file descriptor "output_file". The function returns the number of bytes 
            actually written, which may be less than the requested number of bytes 
            if an error occurs
        */
        writed_byte_size = write(output_file, buffer, readed_byte_size);
        if (writed_byte_size == -1){
            printf("error in writing in output");
            return 1;
        }

        written_byte++;
        operation_ended = (written_byte == file_size);
    }

    close(input_file);
    close(output_file);

}



