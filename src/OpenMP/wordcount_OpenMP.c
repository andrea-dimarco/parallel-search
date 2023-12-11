/**
 Questo codice prende in input un file di testo .txt
  e ritorna in output il conteggio dell'occorreenza di ogni parola all'interno
  del file
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORDS    1000
#define WORD_LENGTH  50
#ifdef _OPENMP	//Non tutti i compilatori riconoscono OpenMP
#include <omp.h>
#endif

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

/**
 * Stampa tutte le occorrenze trovate per le parole
 */
void stampa_conteggio(int numero_risultati, char* words, int count[])
{
    printf("\nHo trovato le seguenti parole: \n");
    for (int i = 0; i < numero_risultati; i++)
    {
        // %-15s stampa la stringa in 15 caratteri.
        printf("%-15s : %d\n", &words[i*WORD_LENGTH], count[i]);
    }
} /* Stampa Conteggio */

/**
 Conta le occorrenze delle parole nel file assegnato
 */
void wordcount(char* words, int* count, int* index, int thread_count)
{
	// Ogni thread conosce il suo rank
	int my_rank= omp_get_thread_num();
		
    // Prendo il file e lo apro con un path assegnatogli
    FILE *fp;
	char path[]="wordcount_demo1.txt";
    fp = fopen(path, "r");
    
    // Creo variabili private a ciascun thread
    int i, len, isUnique;
    char *word;	//word[WORD_LENGTH];
    char riga[1000];
    int n_riga = 0;

    //demo
	printf("Ciao sono il thread %d di %d\n", my_rank, thread_count);
	#pragma omp barrier
	if (my_rank == 0){
		printf("\n\n--- Inizio conteggio parallelo OpenMP con %d thread --- \n\n ", thread_count);
		
	}


	// Ogni thread prende una riga del file    
    while (fgets(riga, 1000, fp) != NULL)
	{
		// Se è la sua riga entra nell'if e processa,
		// altrimenti passa alla prossima (in caso la riga sia assegnata ad un altro thread)
        if (n_riga%thread_count == my_rank)
        {
            //demo
            printf("Sono il thread %d e prendo questa riga (riga n*%d):\n %s \n", my_rank, n_riga, riga);
        	// preleva la parola dalla riga
            word = strtok(riga, " .,!?()-\n\t");

            while (word != NULL)
            {
            	
                // Converto la parola in minuscolo
                strlwr(word);

                // Rimuovo l'ultimo carattere di punteggiatura (se c'è)
                len = strlen(word);
                if (ispunct(word[len - 1]))
                    word[len - 1] = '\0';

                // Controllo se la parola già esiste nella lista
                isUnique = 1;
                for (i = 0; (i < (*index)) && (isUnique); i++)
                {
                    if (strcmp(&words[i*WORD_LENGTH], word) == 0)
                        isUnique = 0;
                }

                /* Se la parola è unica, la devo aggiungere alla lista,
                 * altimenti devo solo modificarne il contatore.
                 */
                printf(" * %s *  esiste nella matrice globale? \n", word);
                
                if (isUnique
                    && (*index) < (MAX_WORDS))
                {
                
                	// fase seriale: i thread scrivo sulle variabili globali my_words e my_count
               		#pragma omp critical(nome1)
	                    strcpy(&words[(*index)*WORD_LENGTH], word);
	                    count[(*index)]++;
	                    (*index)++;
	                	// demo
   				        printf("Parola nuova, la aggiungo alla matrice globale: [%s][%d]\n\n", word, count[*index]+1);
	                    
                } else if (!isUnique)
                {
                	// fase seriale: i thread scrive sulla variabile globale my_count
                	#pragma omp critical(nome2)
                    	count[i-1]++;
                    	printf("Parola trovata nella matrice globale, aumento il contatore globale: [%s][%d]\n\n", word, count[(i - 1)]);
                }
                // il thread passa alla prossima parola della frase da computare
                word = strtok(NULL, " .,!?()-\n\t");
            }
            printf("\n-----------------------------------------------------------------\n\n");
        }
        //per passare alla riga successiva che spetta al thread
        n_riga++;
    }
    // Chiudo il file
    fclose(fp);
} /* Word Count */

/**
    IL MAIN
 */
int main(int argc, char* argv[])
{
	//numero di thread da command line
	int thread_count = strtol(argv[1], NULL, 10);
	
	//o stabilire  il numero di thread 
	// omp_set_num_threads();
   
    /*
     CREO CONTATORI
     */
        // Liste di parole distinte
    char my_words[MAX_WORDS*WORD_LENGTH];

        // Conteggio parole distinte
    int  my_count[MAX_WORDS];
        // inizializzo a 0
    for (int i = 0; i < MAX_WORDS; i++)
        my_count[i] = 0;
    
        //indice "dizionario"
    int index = 0;
    	// demo
	printf("\n --- Esecuzione di wordcount parallelo OpenMP --- \n\n");
		// demo
	printf("--- Fase di parallelizzazione, creazione %d thread --- \n\n ", thread_count);
	// Fase di parallelizzazione, creazione thread
	#pragma omp parallel num_threads(thread_count)
	
    /*
     CONTO LE PAROLE
    //  */

    wordcount(my_words, my_count, &index, thread_count);
    
    //barrier sincronizza i thread e si assicura che tutti abbiano salvato i risultati su var. globali
    #pragma omp barrier
    
    printf("--- Fase di sincronizzazione con barrier --- \n Tutti i %d thread hanno concluso \n", thread_count);
    
    	// demo
    printf("\n Uso funzione stampa_conteggio\n");
    // stampa output del nostro programma
	stampa_conteggio(index, my_words, my_count);
	
    return 0;
    
} /* Main */