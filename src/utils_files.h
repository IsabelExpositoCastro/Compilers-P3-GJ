#ifndef UTILS_FILES_H
#define UTILS_FILES_H

// ------------------ LIBRARIES ------------------
#include <stdio.h>
#include <stdlib.h>     // To set env for time zone
#include <string.h>     // For filename manipulation    
#include <time.h>       // For time-stamp of output logs filenames


// ---------------- GLOBAL DEFINES ------------------
#define MAXFILENAME 256 // Maximum length of the filename for output logs
#define MAXFILEEXT 64   // Maximum length of the file extension

// Path to the logs directory: put your full path, the directory has to exist
//#define PATHDIRLOGS "I:/Mi unidad/UPFdrive/docencia/github/compilers/modules_template/logs/" 
#define PATHDIRLOGS "./logs/" // For running yml

// Function prototypes
void split_path(const char *fullpath, char *path, char *filename, char *extension);
void generate_timestamped_log_filename(const char* base_name, char* output, size_t maxlen);
FILE* set_output_test_file(const char* filename);

#endif // UTILS_FILES_H