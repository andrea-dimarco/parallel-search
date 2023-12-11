/**
 Questo codice prende in input un file di testo .txt
  e ritorna in output il conteggio dell'occorreenza di ogni parola all'interno
  del file
 */

// il main sta alla fine :)

//cd /Users/Ikarux/Desktop/Multicore/Progetto
//gcc wordcount_lineare.c -o wordcount_lineare
//./wordcount_lineare

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORDS   1000
#define WORD_LENGTH 50
#define PATH_LENGTH 100


/**
 Converte la stringa in minuscolo
 */
void minuscolo(char *word)
{
    int len = strlen(word);
    
    for (int i = 0; i < len; i++)
    {
        switch(word[i]) {
            case 'A' :
                word[i] = 'a';
                break;
            case 'B' :
                word[i] = 'b';
                break;
            case 'C' :
                word[i] = 'c';
                break;
            case 'D' :
                word[i] = 'd';
                break;
            case 'E' :
                word[i] = 'e';
                break;
            case 'F' :
                word[i] = 'f';
                break;
            case 'G' :
                word[i] = 'g';
                break;
            case 'H' :
                word[i] = 'h';
                break;
            case 'I' :
                word[i] = 'i';
                break;
            case 'J' :
                word[i] = 'j';
                break;
            case 'K' :
                word[i] = 'k';
                break;
            case 'L' :
                word[i] = 'l';
                break;
            case 'M' :
                word[i] = 'm';
                break;
            case 'N' :
                word[i] = 'n';
                break;
            case 'O' :
                word[i] = 'o';
                break;
            case 'P' :
                word[i] = 'p';
                break;
            case 'Q' :
                word[i] = 'q';
                break;
            case 'R' :
                word[i] = 'r';
                break;
            case 'S' :
                word[i] = 's';
                break;
            case 'T' :
                word[i] = 't';
                break;
            case 'U' :
                word[i] = 'u';
                break;
            case 'V' :
                word[i] = 'v';
                break;
            case 'X' :
                word[i] = 'x';
                break;
            case 'Y' :
                word[i] = 'y';
                break;
            case 'Z' :
                word[i] = 'z';
                break;
            case 'W' :
                word[i] = 'w';
                break;
           }
    }
} /* Minuscolo */


/**
 * Stama tutte le occorrenze trovate per le parole
 */
void stampa_conteggio(int index, char* words, int* count)
{
    printf("\nHo trovato le seguenti parole: \n");
    for (int i = 0; i < index; i++)
    {
            // %-15s stampa la stringa in 15 caratteri.
        printf("%-15s : %d\n", &words[i*WORD_LENGTH], count[i]);
    }
}


/**
 MAIN
 */
int main()
{
    FILE *fptr;
    char path[PATH_LENGTH];
    int i, len, index, isUnique;

    // Liste di parole distinte
    char words[MAX_WORDS*WORD_LENGTH];
    char word[WORD_LENGTH];

    // Conteggio parole distinte
    int  count[MAX_WORDS]; // già inizializzati a 0


    /* Chiedo path del file */
    strcpy(path, "/Users/Ikarux/Desktop/Multicore/FileTesto/StarWars/Star_Wars_A_New_Hope.txt");


    /* Apro il file */
    fptr = fopen(path, "r");

        /* Errore */
    if (fptr == NULL)
    {
        printf("Impossibile aprire file.\n");
        printf("Controlla i privilegi.\n");

        exit(EXIT_FAILURE);
    }

        // Inizializzo il contatore della parola a 0
    for (i=0; i < MAX_WORDS; i++)
        count[i] = 0;


        /*
            CONTO LE PAROLE
         */
    index = 0;
    while (fscanf(fptr, "%s", word) != EOF)
    {
            // Converto la parola in minuscolo
        minuscolo(word);

            // Rimuovo l'ultimo carattere di punteggiatura
        len = strlen(word);
        if (ispunct(word[len-1]))
            word[len - 1] = '\0';

        
            // Controllo se la parola già esiste nella lista
        isUnique = 1;
        for (i = 0; (i < index) && (isUnique); i++)
        {
            if (strcmp(&words[i*WORD_LENGTH], word) == 0)
                isUnique = 0;
        }

        /* Se la parola è unica, la devo aggiungere alla lista,
         * altimenti devo solo modificarne il contatore.
         */
        if (isUnique
            && index < (MAX_WORDS-1))
        {
            strcpy(&words[index*WORD_LENGTH], word);
            count[index]++;

            index++;
        }
        else if (!isUnique)
        {
            count[i - 1]++;
        }
    }

        // Chiudo il file
    fclose(fptr);


    stampa_conteggio(index, words, count);
    
    return 0;
}
