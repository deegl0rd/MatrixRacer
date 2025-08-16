// mem�riasziv�rg�s ellen�rz� k�nyvt�r
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
	if (argc == 1 || (argc == 2 && !strcmp(argv[1], "help"))) // help command implement�ci�
	{
		printf("\tHasznalat: <program_neve> <e/h> [1/2/3]\n");
		return 0;
	}
	srand((unsigned int) time(NULL)); // Random sz�m gener�tor inicializ�l�sa

	switch (argc) // Argumentumok alapj�n j�t�k be�ll�t�sa
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
	// Random sz�m [1...100]
	size_t RNG = (rand() % 100) + 1;

	if (difficulty < 1 || difficulty > 3) //Ha a v�lasztott neh�zs�g nem volt az [1...3] s�vban, akkor automatikusan 3-as lesz.
		difficulty = 3;

	if (difficulty == 2) // 2-es neh�zs�ghez tartoz� ar�nyok
	{
		if (RNG > 0 && RNG <= 70) //70% es�llyel r�vid szavak k�z�l
			return dic[0];
		else return dic[1]; //30% es�llyel k�zepes hossz�s�g� szavak k�z�l
	}
	else if (difficulty == 3) // 3-as neh�zs�ghez tartoz� ar�nyok
	{
		if (RNG > 0 && RNG <= 70) //70% es�llyel r�vid szavak k�z�l
			return dic[0];
		else if (RNG > 70 && RNG <= 90) //20% es�llyel k�zepes hossz�s�g� szavak k�z�l
			return dic[1];
		else return dic[2]; //10% es�llyel hossz� szavak k�z�l
	}
	else
		return dic[0];
}

// V�LETLENSZER� SZ� GENER�TOR. 
char* mrRandWord(Dictionary dic)
{
	// Hib�s sz�t�r eset�n t�rjen vissza null mutat�val
	if (!dic.content || !dic.head)
		return NULL;

	// Random index kisorsol�sa, modulo a szavak sz�m�val. 
	// Eredm�ny: [0 ... dic.count-1] halmazb�l egy v�letlenszer� sz�m
	size_t RNG = rand() % dic.count;

	// Iter�l�s RNG-ig
	Words* head = dic.head;
	for (unsigned int i = 0; i < RNG && head; i++)
		head = head->next;
	
	if (!head)
		return NULL;
	return head->word;
}

// KIH�V�S GENER�L� ALGORITMUS.
char* mrGenerateChallenge(char* mr_lang, int difficulty)
{
	Dictionary dic[3];	
	if (*mr_lang == 'h') // Magyar nyelv� f�jlok bet�lt�se
	{
		dic[0] = mrReadDictionary("hun1.dict");
		dic[1] = mrReadDictionary("hun2.dict");
		dic[2] = mrReadDictionary("hun3.dict");
	}
	else // Angol nyelv� f�jlok bet�lt�se
	{
		dic[0] = mrReadDictionary("eng1.dict");
		dic[1] = mrReadDictionary("eng2.dict");
		dic[2] = mrReadDictionary("eng3.dict");
	};

	if (!dic[0].content || !dic[1].content || !dic[2].content) // Ha b�rmelyik f�jlt nem siker�lt beolvasni
		return NULL;

	// Kezdeti null�zott t�mb, amibe a gener�lt sz�veg fog ker�lni
	char* challenge = calloc(500, sizeof(char));
	if (!challenge) return NULL; // malloc hiba

	int words_in_challenge = MR_CHALLENGE_WORD_COUNT; // mennyi sz�t tartalmazzon a kih�v�s
	for (int i = 0; i < words_in_challenge; i++)
	{
		Dictionary rand_dic = mrRandDictionary(dic, difficulty); // V�letlenszer� sz�t�r kiv�laszt�sa
		char* rand_word = mrRandWord(rand_dic); // V�letlenszer� sz� kiv�laszt�sa
		if (!rand_word) break;

		strcat(challenge, rand_word); // A string append�l�sa
		
		if (i != words_in_challenge - 1) // Sz�k�z�k hozz�ad�sa a stringhez, ha nem az utols� sz�n�l vagyunk.
			strcat(challenge, " ");
	}

	for (int i = 0; i < 3; i++) // A kih�v�s l�trehoz�sa ut�n m�r felszabad�thatjuk a sz�t�rakat
		mrFreeDictionary(dic[i]);

	challenge = mrResizeChallenge(challenge);

	return challenge;
}

char* mrResizeChallenge(char* challenge)
{
	int cnt;
	// �res for loop ami addig sz�mol, am�g lez�r� null�t nem tal�l a stringben.
	for (cnt = 0; cnt < 500 && challenge[cnt] != '\0'; cnt++);
	return realloc(challenge, cnt + 1); // Az 500 byte-os char t�mb �tm�retez�se megfelel� hossz�s�g�ra.
}

void mrPrintChallenge(char* challenge)
{
	char* copy = malloc((strlen(challenge) + 1) * sizeof(char)); // ideiglenes m�solat

	if (!copy) // malloc hiba
		return;

	strcpy(copy, challenge); // m�sol�s

	char* challenge_tok = strtok(copy, " "); // teljes kih�v�s szavakra bont�sa
	for (int i = 0; challenge_tok; i++)
	{
		printf("%s ", challenge_tok); // ki�rat�s
		challenge_tok = strtok(NULL, " "); // token l�ptet�s

		if ((i + 1) % 10 == 0 && challenge_tok) // Sort�r�s minden 10. sz� ut�n (kiv�ve az utols�n�l)
			printf("\n");
	}
	printf("\n");

	free(copy); // mem�ria felszabad�t�s

	print_vline();
	printf("\t[ Kezdeshez nyomj ENTER-t! ]\n");
}

// Jelenlegi id� lek�r�se <timeb.h>
// Unix epoch (1970.01.01 �ta eltelt m�sodpercek sz�ma)
double mrCurrentEpoch()
{
	struct timeb cur;
	ftime(&cur);
	return cur.time + (0.001 * cur.millitm);
}

// Megsz�molja �s visszaadja hogy az egyes szavakban mennyi bet�t siker�lt eltal�lni.
// Ha a j�t�kos v�letlen el�t�tt egy bet�t, ha m�g a t�bbi j� is, kil�p a for ciklusb�l.
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

// L�trehoz egy �res statisztika strukt�r�t
Score mrCreateStatistics()
{
	Score ret;
	ret.total_letters_typed = 0;
	ret.total_good_letters = 0;
	ret.total_good_words = 0;
	return ret;
}

// Statisztik�t sz�mol
void mrCalculateStatistics(Score* res)
{
	res->avgwords = (double) res->total_letters_typed / res->total_good_words; // �tlag bet�sz�m
	res->accuracy = (double) res->total_good_letters / res->total_letters_typed; // Pontoss�g
	res->w_compl  = (double) res->total_good_words / MR_CHALLENGE_WORD_COUNT; // Be�rt szavak �s a kih�v�s szavainak ar�nya
	res->difMins  = res->difSecs / 60.0; // Eltelt id� percben
	res->words    = res->total_good_letters / res->avgwords; // Sikeresen be�rt szavak ar�nya
	res->WPM      = res->words / res->difMins; // Gyorsas�g (sz�/perc)

	// Pontsz�m k�plet
	// (J�A * pontoss�g * WPM)^1.5
	// ahol 'J�A' a be�rt teljesen J� szavak �s a kih�v�s �sszes szav�nak Ar�nya 
	int score = (int) pow(res->w_compl * res->accuracy * res->WPM, 1.5); // nem akarunk t�rt pontsz�mot
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

// Le�ll�tja a stoppert �s statisztik�t k�sz�t
int mrEvaluate(char* challenge, char* attempt, double stime)
{
	Score res = mrCreateStatistics();

	// Id� differencia kisz�m�t�sa
	res.difSecs = mrCurrentEpoch() - stime;

	// A kih�v�s �s a pr�b�lkoz�s stringek list�ba alak�t�sa
	Words* wc = mrReadWordList(challenge, " ", NULL), *wc_head = wc;
	Words* wa = mrReadWordList(attempt, " ", NULL),   *wa_head = wa;

	printf("\n");
	while (wc && wa)
	{
		// Am�g a j�t�kos �rta a szavakat, k�l�nb�z� adatokat r�gz�t a Score t�pus� adatszerkezetbe.

		size_t good_letters = mrCalculateGoodLetters(wc->word, wa->word);
		res.total_letters_typed += strlen(wc->word);  // Bet�k sz�ma a sz�ban
		res.total_good_letters += good_letters; // Helyes bet�k sz�ma

		if (strlen(wc->word) == good_letters)
			res.total_good_words++; // Be�rt szavak sz�ma (amik teljesen j�k)

		mrDisplayWordsEntered(good_letters, wc->word, wa->word); // Ki�ratja form�zva

		wc = wc->next; // List�k l�ptet�se
		wa = wa->next;
	}

	mrFreeWordList(wc_head); // List�k felszabad�t�sa
	mrFreeWordList(wa_head);

	mrCalculateStatistics(&res); // Statisztik�t k�sz�t az eddigi adatokb�l
	mrPrintStatistics(res);

	return res.finalScore;
}

void mrRunGame(char* challenge, int* score)
{
	char c;

	//Enter-re v�r�s (j�t�k eleje)
	while (scanf("%c", &c) && c != '\n');
	double timer = mrCurrentEpoch();
	printf("> ");

	char* attempt = malloc(500 * sizeof(char));

	//Enter-re v�r�s (j�t�k v�ge)
	int i = 0;
	for (i = 0; scanf("%c", &c) && c != '\n' && i < 500; i++)
		attempt[i] = c;
	attempt[i] = '\0';

	// Param list�n visszaadja az eredm�nyt
	*score = mrEvaluate(challenge, attempt, timer);

	free(attempt);
}
