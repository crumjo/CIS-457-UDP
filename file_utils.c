/*****************************************************************
 Functions to read and write files.
 
 @author Joshua Crum
 @version 9/27/17
 *****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include "file_utils.h"



/*****************************************************************
 Read the contents of the file into the buffer and calculate the
 size of the file.
 
 @param filename the name of the file to be read.
 @param buffer the pointer to a pointer to read the file into.
 @return int the size of the file.
 *****************************************************************/
int read_file( char* filename, char **buffer )
{
    
    /** The number of elements in the file. */
    int size = 0;
    
    if (access(filename, F_OK) == -1) {
        fprintf(stderr, "\nThe file '%s' cannot be found "
                "or does not exist.\n\n", filename);
        return -1;
        
    } else {
        
        /** Calculate file size, provided by Professor Woodring */
        struct stat st;
        stat(filename, &st);
        size = (int)st.st_size;
        
        **buffer = (char)malloc(size * sizeof(char));
        if ( buffer == NULL ) {
            fprintf(stderr, "\nMemory allocation failed.\n");
            return -1;
            
        }
        
        /** Open the file in read mode. */
        FILE *in_file = fopen( filename, "r" );
        if (in_file == NULL) {
            fprintf(stderr, "File open failed.");
            fclose(in_file);
            return -1;
        }
        
        fread(*buffer, sizeof(char), size, in_file);
        fclose(in_file);
    }
    
    return size;
}


/*****************************************************************
 Write the contents of the buffer into a new or existing file.
 
 @param filename the name of the output file to write to.
 @param buffer the char array from which to write.
 @param size the number of elements to write to the output file.
 @return int 0 if the code executed correctly, 1 otherwise.
 *****************************************************************/
int write_file( char* filename, char *buffer, int size )
{
    
    if ( access( filename, F_OK) != -1 ) {
        
        /** Capture user input for choice to overwrite a file. */
        char x;
        printf("'%s' already exists, would you like to replace it?"
               " (y or n)\n", filename);
        scanf(" %c", &x);
        
        while ( x != 'n' && x != 'N' && x != 'y' && x != 'Y') {
            printf("Invalid input, enter y or n.\n");
            scanf(" %c", &x);
        }
        
        if (x == 'n' || x == 'N') {
            printf("\nNo changes to '%s' were made."
                   "\n\n", filename);
            return -1;
            
        } else if (x == 'y' || x == 'Y') {
            printf("'%s' has been overwritten.\n", filename);
        }
        
    }
    
    /** Create an output file in write mode. */
    FILE *out_file = fopen(filename, "w");
    if (out_file == NULL) {
        fprintf(stderr, "File open failed.");
        return -1;
    }
    
    buffer[size] = '\0';
    fwrite(buffer, sizeof(char), size, out_file);
    fclose(out_file);
    
    return 0;
}

