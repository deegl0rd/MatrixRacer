#pragma once
#include "racer_types.h"

char*	   mrCopyFileToChar(char* filename);

Words*	   mrReadWordList(char* content, const char* delimiter, size_t* count);
void	   mrFreeWordList(Words* head);

Dictionary mrReadDictionary(char* filename);
void	   mrFreeDictionary(Dictionary d);
