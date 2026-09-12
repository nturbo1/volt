#ifndef BASE_FILES_H
#define BASE_FILES_H

#include "vlt_string.h"

// Returns a filepath that ends with the directory name.
// For instance:
//    if "path/to/dir/filename" was given,
//    then it returns "path/to/dir/"
//
//    if "path/to/dir/" was given,
//    then it returns "path/to/dir/"
//
//    if NULL was given,
//    then returns NULL
String* dirName(const String* const filepath);

#endif // BASE_FILES_H
