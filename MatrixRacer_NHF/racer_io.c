// memóriaszivárgás ellenõrzõ könyvtár
#include "debugmalloc.h"

#include "racer_io.h"
#include <string.h>

// Fájl tartalmának átmásolása dinamikusan allokált char tömbbe
char* mrCopyFileToChar(char* filename)
{
	// Fájl megnyitás
	FILE* f = fopen(filename, "r");
	if (!f || ferror(f)) // Fájl nem létezik, vagy belsõ I/O hiba
	{
		fprintf(stderr, "Hiba tortent %s olvasasa soran!\n", filename);
		return NULL;
	}

	// Fájlméret lekérdezése
	fseek(f, 0L, SEEK_END);
	int size = ftell(f);
	fseek(f, 0L, SEEK_SET);

	// Másolás
	int allocationSize = size * sizeof(char);
	char* contents = malloc(allocationSize);
	if (contents)
		fgets(contents, allocationSize, f);
	else // malloc hiba
	{
		fclose(f);
		return NULL;
	}

	// A fájl át lett másolva, bezárhatjuk
	fclose(f);

	return contents;
}

Words* mrReadWordList(char* content, const char* delimiter, size_t* count)
{
	// Tokenekre bontás
	char* vals = strtok(content, delimiter);

	size_t word_count = 0;

	// Láncolt lista feje és iterátor
	Words* head = malloc(sizeof(Words));
	Words* it = head;

	// Végig iterál a listán
	while (it)
	{
		// Pointerek NULLázása
		it->word = NULL;
		it->next = NULL;

		// Listaelem szó beállítása, majd következõ tokenre léptetés
		it->word = vals;

		if (vals) //szó számláló inkrementálása
			word_count++;

		vals = strtok(NULL, delimiter);

		// Ha még vannak szavak, memóriát allokál a következõ listaelemnek
		if (vals)
			it->next = malloc(sizeof(Words));

		// Lista léptetés
		it = it->next;
	}

	if (count)
		*count = word_count;

	return head;
}

void mrFreeWordList(Words* head)
{
	// Láncolt lista elemek felszabadítása
	while (head)
	{
		Words* cur = head;
		head = head->next;
		free(cur);
	}
}

// SZÓTÁR-FÁJL OLVASÓ ALGORITMUS. A függvény az strtok (string.h) függvény segítségével ';' karakterek által 
// elválasztott tokenekre bontja a beolvasott fájl tartalmát, majd ezeket láncolt lista adatszerkezetbe tölti.
Dictionary mrReadDictionary(char* filename)
{
	// Ezt a dictionary-t fogja visszaadni.
	Dictionary dic;
	dic.content = NULL;
	dic.count = 0;
	dic.head = NULL;

	// Másolás char* tömbbe
	dic.content = mrCopyFileToChar(filename);
	if (!dic.content) // hibakezelés
		return dic;

	// Láncolt lista feje és iterátor
	dic.head = mrReadWordList(dic.content, ";", &dic.count);

	return dic;
}

// Memória felszabadító függvény
void mrFreeDictionary(Dictionary dic)
{
	// Láncolt lista elemek felszabadítása
	mrFreeWordList(dic.head);

	// A szótárfájl 'nyers' tartalmának felszabadítása
	if (dic.content) free(dic.content);
}