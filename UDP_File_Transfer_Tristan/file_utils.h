/*****************************************************************
 File open contents to buffer, write from buffer to a new or
 existing file, and reverse the contents of a buffer.
 
 @author Joshua Crum
 @version 9/27/17
 *****************************************************************/

#ifndef file_utils_h
#define file_utils_h

int read_file( char* filename, char **buffer );
int write_file( char* filename, char *buffer, int size);

#endif /** file_utils_h */
