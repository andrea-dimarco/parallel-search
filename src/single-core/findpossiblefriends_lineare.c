/**
 Questo codice prende in input un file di testo .txt
  e ritorna in output una lista di amici in comune per ogni coppia di utenti (che non sono già amici)
 */

// il main sta alla fine :)

//cd /Users/Ikarux/Desktop/Multicore/Progetto
//gcc findpossiblefriends_lineare.c -o findpossiblefriends_lineare
//./findpossiblefriends_lineare

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORDS   100 // massimo coppie di utenti analizzate
#define WORD_LENGTH 20  // lunghezza massima di DUE nomi
#define LINE_LENGTH 1000
#define SEPARATORE  '|'


/**
 Stampa il contenudo dell'array di interi
 */
void stampa_array_int(int* array, int size)
{
    if (size > 0)
    {
        printf(" [%d", array[0]);
        for (int i = 1; i < size; i++)
        {
            printf(", %d", array[i]);
        }
        printf("]\n");
    }
}
/**
 Stampa  matrice di int
 */
void stampa_matrice_int(int* matrice, int X, int Y)
{
    if ((X*Y) > 0)
    {
        for (int y = 0; y < Y; y++)
        {
            printf("| %d", matrice[(y*X)]);
            for(int x = 1; x < X; x++)
            {
                printf(", %d", matrice[(y*X) + x]);
            }
            printf(" |\n");
        }
    }
}
/**
 Stampa un array di stringhe
 */
void stampa_array_str(char* array, int n_parole)
{
    if (n_parole > 0)
    {
        printf("[%s", &array[0]);
        for (int i = 1; i < n_parole; i++)
        {
            printf(", %s", &array[i*WORD_LENGTH]);
        }
        printf("]\n");
    }
}
void stampa_array_str_nomi(char* array, int n_parole)
{
    if (n_parole > 0)
    {
        printf("[%s", &array[0]);
        for (int i = 1; i < n_parole; i++)
        {
            printf(", %s", &array[i*(WORD_LENGTH/2)]);
        }
        printf("]\n");
    }
}
/**
 Stampa una matrice di stringhe
 */
void stampa_matrice_str(char* matrice, int X, int Y)
{
    if ((X*Y) > 0)
    {
        for (int y = 0; y < Y; y++)
        {
            printf("| %s", &matrice[(y*X*WORD_LENGTH)]);
            for(int x = 1; x < X; x++)
            {
                printf(", %s", &matrice[(y*X*WORD_LENGTH) + (x*WORD_LENGTH)]);
            }
            printf(" |\n");
        }
    }
}


/**
 Data una stringa, genera un numero
 */
int ascii(char* word)
{
    int len = strlen(word);
    int sum = 0;
        // somma tutti i caratteri
    for (int i = 0; i < len; i++)
        sum += (int) word[i];
    
    return sum;
} /* ASCII */


/**
 Aggiunge il carattere separatore alla fine della stringa.
   É necessario che la zona di memoria della stringa sia abbastanza grandeda poter sostenere un carattere in più.
 */
void aggiungi_separatore(char* stringa, char separatore)
{
    int len = strlen(stringa);
    
    stringa[len] = separatore;
    stringa[len+1] = '\0';
} /* Aggiungi Separatore */


    // MAIN
int main()
{
    FILE *fp;
    char path[100];
    char riga[LINE_LENGTH];
    int i, j, index, isUnique;

        // Liste di parole distinte
    char words[MAX_WORDS*WORD_LENGTH];
    char* word;
    
        // Conteggio parole distinte
    int  counts[MAX_WORDS];
    
        // servono per la prima analisi
    char main_user[(WORD_LENGTH/2)];
    char current_user[(WORD_LENGTH/2)];
    char next_user[(WORD_LENGTH/2)];
    
    char friends[(MAX_WORDS/2)*(WORD_LENGTH/2)];
    int friends_index;
    char entry[WORD_LENGTH];


    /* Chiedo path del file */
    //printf("Inserisci percorso file: ");
    //scanf("%s", path);
    strcpy(path, "/Users/Ikarux/Desktop/Multicore/SocialNetwork/friends_test_1.txt");


        // Apro il file
    fp = fopen(path, "r");

        // Errore
    if (fp == NULL)
    {
        printf("Impossibile aprire file.\n");
        printf("Controlla i privilegi.\n");

        exit(EXIT_FAILURE);
    }

    
        // Inizializzo contatore a 0
    for (i = 0; i < MAX_WORDS; i++)
        counts[i] = 0;



    index = 0;
        // per ogni utente
    while (fgets(riga, LINE_LENGTH, fp) != NULL)
    {
            // mi salvo tutti i suoi amici su un'array
        friends_index = 0;
        word = strtok(riga, "\t,\n");
        while (word != NULL
               && friends_index < (MAX_WORDS/2))
        {
            strcpy(&friends[friends_index*(WORD_LENGTH/2)], word);
            friends_index++;
            word = strtok(NULL, "\t,\n");
        }
        
        //stampa_array_str_nomi(friends, friends_index);
        
        /*
         Formo le COPPIE DA EVITARE
         */
        strcpy(main_user, &friends[0*(WORD_LENGTH/2)]); // il primo utente della lista è quello principale
        
            // per ogni amico di main_user
        for (i = 1; i < friends_index; i++)
        {
                // resetto stringa main_user nel caso sia stata già usata con il separatore
            if (main_user[strlen(main_user)-1] == SEPARATORE)
            {
                main_user[strlen(main_user)-1] = '\0';
            }
                // prendo l'amico da analizzare nella lista
            strcpy(current_user, &friends[i*(WORD_LENGTH/2)]);
            
                // decido l'ordine in cui scrivere i nomi
            if (ascii(current_user) > ascii(main_user))
            {
                    // prima current_user
                aggiungi_separatore(current_user, SEPARATORE);
                strcpy(entry, current_user);
                strcat(entry, main_user);
            } else {
                    // prima main_user
                aggiungi_separatore(main_user, SEPARATORE);
                strcpy(entry, main_user);
                strcat(entry, current_user);
            }
            
                // <main_user|current_user> va messo negativo, perché sono già amici.
            isUnique = 1;
            for (i = 0; (i < index) && (isUnique); i++)
            {
                if (strcmp(&words[i*WORD_LENGTH], entry) == 0)
                    isUnique = 0;
            }

            /* Se la parola è unica, la devo aggiungere alla lista,
             * altimenti devo solo modificarne il contatore.
             */
            if (isUnique
                && index < (MAX_WORDS-1))
            {
                strcpy(&words[index*WORD_LENGTH], entry);
                counts[index] = -(MAX_WORDS+1);

                index++;
            }
        }
        
        
        /*
            FORMO le COPPIE VALIDE
         */
            // salto main_user
        for (i = 1; i < friends_index-1; i++)
        {
                // prendo il primo amico della coppia
            strcpy(current_user, &friends[i*(WORD_LENGTH/2)]);
            
                // ciclo tutti gli altri amici
                // escludendo le coppie che ho già visto
            for (j = i+1; j < friends_index; j++)
            {
                // resetto stringa current_user nel caso sia stata già usata con il separatore
                if (current_user[strlen(current_user)-1] == SEPARATORE)
                {
                    current_user[strlen(current_user)-1] = '\0';
                }
                
                    // prendo next_user
                strcpy(next_user, &friends[j*(WORD_LENGTH/2)]);
                
                    // resetto stringa current_user nel caso sia stata già usata con il separatore
                if (main_user[strlen(current_user)-1] == SEPARATORE)
                {
                    main_user[strlen(current_user)-1] = '\0';
                }
                    // prendo l'amico da analizzare nella lista
                strcpy(next_user, &friends[j*(WORD_LENGTH/2)]);
                
                    // decido l'ordine in cui scrivere i nomi
                if (ascii(current_user) > ascii(next_user))
                {
                        // prima current_user
                    aggiungi_separatore(current_user, SEPARATORE);
                    strcpy(entry, current_user);
                    strcat(entry, next_user);
                } else {
                        // prima next_user
                    aggiungi_separatore(next_user, SEPARATORE);
                    strcpy(entry, next_user);
                    strcat(entry, current_user);
                }
                
                    // Controllo se l'entry già esiste nella lista
                isUnique = 1;
                for (i = 0; (i < index) && (isUnique); i++)
                {
                    if (strcmp(&words[i*WORD_LENGTH], entry) == 0)
                        isUnique = 0;
                }

                /* Se l'entry è unica, la devo aggiungere alla lista,
                 * altimenti devo solo modificarne il contatore.
                 */
                if (isUnique
                    && index < (MAX_WORDS-1))
                {
                    strcpy(&words[index*WORD_LENGTH], entry);
                    counts[index]++;

                    index++;
                }
                else if (!isUnique)
                {
                    counts[i-1]++;
                }
            }
        }
    }

    // Chiudo il file
    fclose(fp);


    /*
     * Stama tutte le occorrenze trovate per le parole
     */
    printf("\nHo trovato le seguenti nuove amicizie possibili: \n");
    for (i=0; i < index; i++)
    {
            // stampo solo le nuove amicizie
        if (counts[i] > 0)
        {
                // %-15s stampa la stringa in 15 caratteri.
            printf("%-15s : %d\n", &words[i*WORD_LENGTH], counts[i]);
        }
    }    
    

    return 0;
} /* Main */
