// memóriaszivárgás ellenõrzõ könyvtár
#include "debugmalloc.h"

#include "racer_logic.h"
#include "racer_io.h"

#include <math.h>
#include <time.h>

#include <sys/timeb.h>

void print_vline()
{
	printf("--------------------------------------------------------------------------------------\n");
}

int mrInitialize(int argc, char* argv[], char* mr_lang, int* mr_diff)
{
	if (argc == 1 || (argc == 2 && !strcmp(argv[1], "help"))) // help command implementáció
	{
		printf("\tHasznalat: <program_neve> <e/h> [1/2/3]\n");
		return 0;
	}
	srand((unsigned int) time(NULL)); // Random szám generátor inicializálása

	switch (argc) // Argumentumok alapján játék beállítása
	{
		case 3:
			*mr_lang = argv[1][0];
			*mr_diff = atoi(argv[2]);
			break;
		case 2:
			*mr_lang = argv[1][0];
			break;
		default: break;
	}

	return 1;
}

Dictionary mrRandDictionary(Dictionary dic[], int difficulty)
{
	// Random szám [1...100]
	size_t RNG = (rand() % 100) + 1;

	if (difficulty < 1 || difficulty > 3) //Ha a választott nehézség nem volt az [1...3] sávban, akkor automatikusan 3-as lesz.
		difficulty = 3;

	if (difficulty == 2) // 2-es nehézséghez tartozó arányok
	{
		if (RNG > 0 && RNG <= 70) //70% eséllyel rövid szavak közül
			return dic[0];
		else return dic[1]; //30% eséllyel közepes hosszúságú szavak közül
	}
	else if (difficulty == 3) // 3-as nehézséghez tartozó arányok
	{
		if (RNG > 0 && RNG <= 70) //70% eséllyel rövid szavak közül
			return dic[0];
		else if (RNG > 70 && RNG <= 90) //20% eséllyel közepes hosszúságú szavak közül
			return dic[1];
		else return dic[2]; //10% eséllyel hosszú szavak közül
	}
	else
		return dic[0];
}

// VÉLETLENSZERÛ SZÓ GENERÁTOR. 
char* mrRandWord(Dictionary dic)
{
	// Hibás szótár esetén térjen vissza null mutatóval
	if (!dic.content || !dic.head)
		return NULL;

	// Random index kisorsolása, modulo a szavak számával. 
	// Eredmény: [0 ... dic.count-1] halmazból egy véletlenszerû szám
	size_t RNG = rand() % dic.count;

	// Iterálás RNG-ig
	Words* head = dic.head;
	for (unsigned int i = 0; i < RNG && head; i++)
		head = head->next;
	
	if (!head)
		return NULL;
	return head->word;
}

// KIHÍVÁS GENERÁLÓ ALGORITMUS.
char* mrGenerateChallenge(char* mr_lang, int difficulty)
{
	Dictionary dic[3];	
	if (*mr_lang == 'h') // Magyar nyelvû fájlok betöltése
	{
		dic[0] = mrReadDictionary("hun1.dict");
		dic[1] = mrReadDictionary("hun2.dict");
		dic[2] = mrReadDictionary("hun3.dict");
	}
	else // Angol nyelvû fájlok betöltése
	{
		dic[0] = mrReadDictionary("eng1.dict");
		dic[1] = mrReadDictionary("eng2.dict");
		dic[2] = mrReadDictionary("eng3.dict");
	};

	if (!dic[0].content || !dic[1].content || !dic[2].content) // Ha bármelyik fájlt nem sikerült beolvasni
		return NULL;

	// Kezdeti nullázott tömb, amibe a generált szöveg fog kerülni
	char* challenge = calloc(500, sizeof(char));
	if (!challenge) return NULL; // malloc hiba

	int words_in_challenge = MR_CHALLENGE_WORD_COUNT; // mennyi szót tartalmazzon a kihívás
	for (int i = 0; i < words_in_challenge; i++)
	{
		Dictionary rand_dic = mrRandDictionary(dic, difficulty); // Véletlenszerû szótár kiválasztása
		char* rand_word = mrRandWord(rand_dic); // Véletlenszerû szó kiválasztása
		if (!rand_word) break;

		strcat(challenge, rand_word); // A string appendálása
		
		if (i != words_in_challenge - 1) // Szóközök hozzáadása a stringhez, ha nem az utolsó szónál vagyunk.
			strcat(challenge, " ");
	}

	for (int i = 0; i < 3; i++) // A kihívás létrehozása után már felszabadíthatjuk a szótárakat
		mrFreeDictionary(dic[i]);

	challenge = mrResizeChallenge(challenge);

	return challenge;
}

char* mrResizeChallenge(char* challenge)
{
	int cnt;
	// üres for loop ami addig számol, amíg lezáró nullát nem talál a stringben.
	for (cnt = 0; cnt < 500 && challenge[cnt] != '\0'; cnt++);
	return realloc(challenge, cnt + 1); // Az 500 byte-os char tömb átméretezése megfelelõ hosszúságúra.
}

void mrPrintChallenge(char* challenge)
{
	char* copy = malloc((strlen(challenge) + 1) * sizeof(char)); // ideiglenes másolat

	if (!copy) // malloc hiba
		return;

	strcpy(copy, challenge); // másolás

	char* challenge_tok = strtok(copy, " "); // teljes kihívás szavakra bontása
	for (int i = 0; challenge_tok; i++)
	{
		printf("%s ", challenge_tok); // kiíratás
		challenge_tok = strtok(NULL, " "); // token léptetés

		if ((i + 1) % 10 == 0 && challenge_tok) // Sortörés minden 10. szó után (kivéve az utolsónál)
			printf("\n");
	}
	printf("\n");

	free(copy); // memória felszabadítás

	print_vline();
	printf("\t[ Kezdeshez nyomj ENTER-t! ]\n");
}

// Jelenlegi idõ lekérése <timeb.h>
// Unix epoch (1970.01.01 óta eltelt másodpercek száma)
double mrCurrentEpoch()
{
	struct timeb cur;
	ftime(&cur);
	return cur.time + (0.001 * cur.millitm);
}

// Megszámolja és visszaadja hogy az egyes szavakban mennyi betût sikerült eltalálni.
// Ha a játékos véletlen elütött egy betût, ha még a többi jó is, kilép a for ciklusból.
size_t mrCalculateGoodLetters(char* ref, char* actual) 
{
	size_t count = 0;

	if (!ref || !actual)
		return 0;
	
	for (int i = 0; i < strlen(ref); i++)
	{
		if (ref[i] == actual[i]) count++;
		else break;
	}
	return count;
}

void mrDisplayWordsEntered(size_t good_letters, char* ref, char* actual)
{
	printf(CRESET"%s -> ", ref);

	for (int i = 0; i < strlen(ref); i++)
	{
		if (i >= strlen(actual))
		{
			printf(CRED"%c", ref[i]);
			continue;
		}

		if (i >= good_letters)
			printf(CRED"%c", actual[i]);
		else
			printf(CGREEN"%c", actual[i]);
	}
	printf(CRESET"\n");
}

// Létrehoz egy üres statisztika struktúrát
Score mrCreateStatistics()
{
	Score ret;
	ret.total_letters_typed = 0;
	ret.total_good_letters = 0;
	ret.total_good_words = 0;
	return ret;
}

// Statisztikát számol
void mrCalculateStatistics(Score* res)
{
	res->avgwords = (double) res->total_letters_typed / res->total_good_words; // Átlag betûszám
	res->accuracy = (double) res->total_good_letters / res->total_letters_typed; // Pontosság
	res->w_compl  = (double) res->total_good_words / MR_CHALLENGE_WORD_COUNT; // Beírt szavak és a kihívás szavainak aránya
	res->difMins  = res->difSecs / 60.0; // Eltelt idõ percben
	res->words    = res->total_good_letters / res->avgwords; // Sikeresen beírt szavak aránya
	res->WPM      = res->words / res->difMins; // Gyorsaság (szó/perc)

	// Pontszám képlet
	// (JÖA * pontosság * WPM)^1.5
	// ahol 'JÖA' a beírt teljesen Jó szavak és a kihívás Összes szavának Aránya 
	int score = (int) pow(res->w_compl * res->accuracy * res->WPM, 1.5); // nem akarunk tört pontszámot
	res->finalScore = score;
}

void mrPrintStatistics(Score res)
{
	print_vline();
	printf("Idotartam: %.1lf mp. (%.3lf perc) Befejezve %lu/%lu\n", 
		res.difSecs, 
		res.difMins, 
		res.total_good_words, 
		MR_CHALLENGE_WORD_COUNT);
	printf("WPM: %.1lf\n", res.WPM);
	printf("Pontossag: %.1lf%%\n", res.accuracy * 100.0);
	printf("Pontszam: %u\n", res.finalScore);
}

// Leállítja a stoppert és statisztikát készít
int mrEvaluate(char* challenge, char* attempt, double stime)
{
	Score res = mrCreateStatistics();

	// Idõ differencia kiszámítása
	res.difSecs = mrCurrentEpoch() - stime;

	// A kihívás és a próbálkozás stringek listába alakítása
	Words* wc = mrReadWordList(challenge, " ", NULL), *wc_head = wc;
	Words* wa = mrReadWordList(attempt, " ", NULL),   *wa_head = wa;

	printf("\n");
	while (wc && wa)
	{
		// Amíg a játékos írta a szavakat, különbözõ adatokat rögzít a Score típusú adatszerkezetbe.

		size_t good_letters = mrCalculateGoodLetters(wc->word, wa->word);
		res.total_letters_typed += strlen(wc->word);  // Betûk száma a szóban
		res.total_good_letters += good_letters; // Helyes betûk száma

		if (strlen(wc->word) == good_letters)
			res.total_good_words++; // Beírt szavak száma (amik teljesen jók)

		mrDisplayWordsEntered(good_letters, wc->word, wa->word); // Kiíratja formázva

		wc = wc->next; // Listák léptetése
		wa = wa->next;
	}

	mrFreeWordList(wc_head); // Listák felszabadítása
	mrFreeWordList(wa_head);

	mrCalculateStatistics(&res); // Statisztikát készít az eddigi adatokból
	mrPrintStatistics(res);

	return res.finalScore;
}

void mrRunGame(char* challenge, int* score)
{
	char c;

	//Enter-re várás (játék eleje)
	while (scanf("%c", &c) && c != '\n');
	double timer = mrCurrentEpoch();
	printf("> ");

	char* attempt = malloc(500 * sizeof(char));

	//Enter-re várás (játék vége)
	int i = 0;
	for (i = 0; scanf("%c", &c) && c != '\n' && i < 500; i++)
		attempt[i] = c;
	attempt[i] = '\0';

	// Param listán visszaadja az eredményt
	*score = mrEvaluate(challenge, attempt, timer);

	free(attempt);
}
