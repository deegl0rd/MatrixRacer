#pragma once

#include <stddef.h>

typedef struct Words 
{ 
	char* word; //szó memóriacíme  
	struct Words* next; //következõ listaelem 
} Words; 

typedef struct Dictionary
{
	char* content; // a beolvasott fájl nyers tartalma
	size_t count;  //a beolvasott fájlban található szavak száma
	struct Words* head; // a 'content'-bõl képzett lista feje
} Dictionary;

typedef struct Score
{
	size_t total_letters_typed;
	size_t total_good_letters;
	size_t total_good_words;

	double difSecs; double difMins; // idõ differencia
	double words; double avgwords;  // szavak száma
	double accuracy, WPM; // pontosság és gyorsaság
	double w_compl; // beírt szavak és a teljes kihívás szavainak aránya
	int    finalScore; // teljes pontszám
} Score;

#define CRED   "\x1b[31m"
#define CGREEN "\x1b[32m"
#define CRESET "\x1b[0m"

#define MR_CHALLENGE_WORD_COUNT 30