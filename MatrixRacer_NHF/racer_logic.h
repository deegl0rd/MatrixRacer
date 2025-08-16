#pragma once

#include "racer_types.h"

void   print_vline();

int    mrInitialize(int argc, char* argv[], char* mr_lang, int* mr_diff);

Dictionary mrRandDictionary(Dictionary dic[], int difficulty);
char*  mrRandWord(Dictionary dic);

char*  mrGenerateChallenge(char* mr_lang, int difficulty);
char*  mrResizeChallenge(char* challenge);
void   mrPrintChallenge(char* challenge);

double mrCurrentEpoch();

size_t mrCalculateGoodLetters(char* ref, char* actual);
void   mrDisplayWordsEntered(size_t good_letters, char* ref, char* actual);

Score  mrCreateStatistics();
void   mrCalculateStatistics(Score* res);
void   mrPrintStatistics(Score res);

int    mrEvaluate(char* challenge, char* attempt, double stime);
void   mrRunGame(char* challenge, int* score);