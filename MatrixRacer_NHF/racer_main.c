// memóriaszivárgás ellenőrző könyvtár
#include "debugmalloc.h"

#include "racer_types.h"
#include "racer_logic.h"
#include "racer_io.h"

// Globális változók
static char* init_ascii[] =
{
	CGREEN,"010    100", CRESET,"  00010 ",  CGREEN," 01000001", CRESET," 100000 ", CGREEN," 10", CRESET," 01   10", CGREEN," 000100 ", CRESET,"  01001 ", CGREEN,"  000010", CRESET," 1000001", CGREEN," 000100 \n",
	CGREEN,"1000  0001", CRESET," 01   10",  CGREEN,"    01   ", CRESET," 10   01", CGREEN," 00", CRESET,"  10 01 ", CGREEN," 10   01", CRESET," 00   10", CGREEN," 01     ", CRESET," 10     ", CGREEN," 01   10\n",
	CGREEN,"01 1000 10", CRESET," 0000001",  CGREEN,"    10   ", CRESET," 000010 ", CGREEN," 10", CRESET,"   000  ", CGREEN," 000100 ", CRESET," 0000001", CGREEN," 01     ", CRESET," 00010  ", CGREEN," 000100 \n",
	CGREEN,"10  01  10", CRESET," 00   01",  CGREEN,"    10   ", CRESET," 10   01", CGREEN," 01", CRESET,"  00 10 ", CGREEN," 10   01", CRESET," 00   10", CGREEN," 01     ", CRESET," 10     ", CGREEN," 01   10\n",
	CGREEN,"01      10", CRESET," 00   01",  CGREEN,"    10   ", CRESET," 10   01", CGREEN," 10", CRESET," 00   10", CGREEN," 00   01", CRESET," 00   10", CGREEN,"  000010", CRESET," 1000001", CGREEN," 01   10\n",
	CRESET,"\0"
};
static char  mr_lang = 'e';
static int   mr_diff = 3;

void print_intro()
{
	for (int i = 0; strcmp(init_ascii[i], "\0"); i++)
	{
		printf("%s", init_ascii[i]);
	}
	printf("Nehezseg: %d, Nyelv: ", mr_diff);
	if (mr_lang == 'h')
		printf("hungarian\n");
	else
		printf("english\n");

	print_vline();
}

int main(int argc, char* argv[])
{
	if (!mrInitialize(argc, argv, &mr_lang, &mr_diff)) // Játék beállítása
		return 1;
	print_intro(); // ASCII betűk (nagyon menő 😎)

	char* challenge = mrGenerateChallenge(&mr_lang, mr_diff); // Kihívás generálása
	if (!challenge)
	{
		printf("Hiba! Nem sikerult valamelyik fajlt beolvasni!");
		return 1;
	}

	int final_score;
	mrPrintChallenge(challenge);
	mrRunGame(challenge, &final_score);

	free(challenge); // Kihívás string felszabadítása
	return 0;
}