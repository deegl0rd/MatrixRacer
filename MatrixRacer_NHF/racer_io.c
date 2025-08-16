// mem�riasziv�rg�s ellen�rz� k�nyvt�r
#include "debugmalloc.h"

#include "racer_io.h"
#include <string.h>

// F�jl tartalm�nak �tm�sol�sa dinamikusan allok�lt char t�mbbe
char* mrCopyFileToChar(char* filename)
{
	// F�jl megnyit�s
	FILE* f = fopen(filename, "r");
	if (!f || ferror(f)) // F�jl nem l�tezik, vagy bels� I/O hiba
	{
		fprintf(stderr, "Hiba tortent %s olvasasa soran!\n", filename);
		return NULL;
	}

	// F�jlm�ret lek�rdez�se
	fseek(f, 0L, SEEK_END);
	int size = ftell(f);
	fseek(f, 0L, SEEK_SET);

	// M�sol�s
	int allocationSize = size * sizeof(char);
	char* contents = malloc(allocationSize);
	if (contents)
		fgets(contents, allocationSize, f);
	else // malloc hiba
	{
		fclose(f);
		return NULL;
	}

	// A f�jl �t lett m�solva, bez�rhatjuk
	fclose(f);

	return contents;
}

Words* mrReadWordList(char* content, const char* delimiter, size_t* count)
{
	// Tokenekre bont�s
	char* vals = strtok(content, delimiter);

	size_t word_count = 0;

	// L�ncolt lista feje �s iter�tor
	Words* head = malloc(sizeof(Words));
	Words* it = head;

	// V�gig iter�l a list�n
	while (it)
	{
		// Pointerek NULL�z�sa
		it->word = NULL;
		it->next = NULL;

		// Listaelem sz� be�ll�t�sa, majd k�vetkez� tokenre l�ptet�s
		it->word = vals;

		if (vals) //sz� sz�ml�l� inkrement�l�sa
			word_count++;

		vals = strtok(NULL, delimiter);

		// Ha m�g vannak szavak, mem�ri�t allok�l a k�vetkez� listaelemnek
		if (vals)
			it->next = malloc(sizeof(Words));

		// Lista l�ptet�s
		it = it->next;
	}

	if (count)
		*count = word_count;

	return head;
}

void mrFreeWordList(Words* head)
{
	// L�ncolt lista elemek felszabad�t�sa
	while (head)
	{
		Words* cur = head;
		head = head->next;
		free(cur);
	}
}

// SZ�T�R-F�JL OLVAS� ALGORITMUS. A f�ggv�ny az strtok (string.h) f�ggv�ny seg�ts�g�vel ';' karakterek �ltal 
// elv�lasztott tokenekre bontja a beolvasott f�jl tartalm�t, majd ezeket l�ncolt lista adatszerkezetbe t�lti.
Dictionary mrReadDictionary(char* filename)
{
	// Ezt a dictionary-t fogja visszaadni.
	Dictionary dic;
	dic.content = NULL;
	dic.count = 0;
	dic.head = NULL;

	// M�sol�s char* t�mbbe
	dic.content = mrCopyFileToChar(filename);
	if (!dic.content) // hibakezel�s
		return dic;

	// L�ncolt lista feje �s iter�tor
	dic.head = mrReadWordList(dic.content, ";", &dic.count);

	return dic;
}

// Mem�ria felszabad�t� f�ggv�ny
void mrFreeDictionary(Dictionary dic)
{
	// L�ncolt lista elemek felszabad�t�sa
	mrFreeWordList(dic.head);

	// A sz�t�rf�jl 'nyers' tartalm�nak felszabad�t�sa
	if (dic.content) free(dic.content);
}