#pragma once

#include <stddef.h>

typedef struct Words 
{ 
	char* word; //sz� mem�riac�me  
	struct Words* next; //k�vetkez� listaelem 
} Words; 

typedef struct Dictionary
{
	char* content; // a beolvasott f�jl nyers tartalma
	size_t count;  //a beolvasott f�jlban tal�lhat� szavak sz�ma
	struct Words* head; // a 'content'-b�l k�pzett lista feje
} Dictionary;

typedef struct Score
{
	size_t total_letters_typed;
	size_t total_good_letters;
	size_t total_good_words;

	double difSecs; double difMins; // id� differencia
	double words; double avgwords;  // szavak sz�ma
	double accuracy, WPM; // pontoss�g �s gyorsas�g
	double w_compl; // be�rt szavak �s a teljes kih�v�s szavainak ar�nya
	int    finalScore; // teljes pontsz�m
} Score;

#define CRED   "\x1b[31m"
#define CGREEN "\x1b[32m"
#define CRESET "\x1b[0m"

#define MR_CHALLENGE_WORD_COUNT 30