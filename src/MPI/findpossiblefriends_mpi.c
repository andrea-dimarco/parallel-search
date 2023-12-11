/**
 Questo codice prende in input un file di testo .txt
  e ritorna in output una lista di amici in comune per ogni coppia di utenti (che non sono già amici)
 */

// il main sta alla fine :)

//cd /Users/Ikarux/Desktop/Multicore/Progetto

//mpicc findpossiblefriends_mpi.c -o findpossiblefriends_mpi

//mpiexec -n 4 ./findpossiblefriends_mpi

#include <mpi.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORDS    1000
#define WORD_LENGTH  40
#define PATH_LENGTH  100
#define LINE_LENGTH  1000
#define SEPARATORE   '|'

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
 * Stampa tutte le occorrenze trovate per le entry
 */
void stampa_conteggio(int numero_risultati, char* words, int counts[])
{
    printf("\nHo trovato le seguenti nuove amicizie possibili: \n");
    for (int i = 0; i < numero_risultati; i++)
    {
            // stampo solo le nuove amicizie
        if (counts[i] > 0)
        {
                // %-15s stampa la stringa in 15 caratteri.
            printf("%-15s : %d\n", &words[i*WORD_LENGTH], counts[i]);
        }
    }
} /* Stampa Conteggio */


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


/**
 Data una stringa, genera un numero compreso in [0,limit)
 */
int funzione_hash(char* word, int limit)
{
    int len = strlen(word);
    int sum = 0;
        // somma tutti i caratteri
    for (int i = 0; i < len; i++)
        sum += (int) word[i];
    
    return (sum%limit);
} /* Funzione Hash */


/**
 Ritorna la prossima parola da dover analizzare.
 */
char* ottieni_parola(FILE* fp, char* word, int my_rank, int comm_sz)
{
    if(fscanf(fp, "%s", word) != EOF)
        return word;
    
    return NULL;
} /* Ottieni Parola */


/**
 Conta le occorrenze delle parole nel file assegnato
 */
void find_possible_friends(char* words, int* counts, int* word_count, FILE* fp, int my_rank, int comm_sz)
{
    int i, j, isUnique;
    char *word;//word[WORD_LENGTH];
    char riga[LINE_LENGTH];
    int n_riga = 0;
    int index = (*word_count);
    
        // servono per la prima analisi
    char main_user[(WORD_LENGTH/2)];
    char current_user[(WORD_LENGTH/2)];
    char next_user[(WORD_LENGTH/2)];

    char friends[(MAX_WORDS/2)*(WORD_LENGTH/2)];
    int friends_index;
    char entry[WORD_LENGTH];
    
    // per ogni utente
    while (fgets(riga, LINE_LENGTH, fp) != NULL)
    {
        if (n_riga%comm_sz == my_rank)
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
        n_riga++;
    }
    (*word_count) = index;
} /* Word Count */


/**
 Scorre le entry trovate, decide a quale processo inviarle, e le invia.
 */
void shuffle(char *words, int* count, int words_number, int my_rank, int comm_sz,
             int* receive_buffer_index, char* receive_buffer, int* receive_counts)
{
    /*
     INIZIALIZZO
     */
        // per comodità
    int         receiver;
    char*       parola;
    int         i, j, k;
    
        // per la ricezione
    MPI_Status  status;
    MPI_Request* requests[comm_sz];
    
        // per l'invio
    int         send_buffer_index[comm_sz];
    char        send_buffer[comm_sz*MAX_WORDS*WORD_LENGTH];
    int         send_counts[comm_sz*MAX_WORDS];
    
        // inizializzo a 0
    for (i = 0; i < comm_sz; i++)
    {
        send_buffer_index[i] = 0;
        receive_buffer_index[i] = 0;
    }
    for (i = 0; i < (comm_sz*MAX_WORDS); i++)
    {
        receive_counts[i] = 0;
        send_counts[i] = 0;
    }
    
    
    /*
     SMISTAMENTO PAROLE
     */
    // per ogni parola che ho trovato
    for (i = 0; i < words_number; i++)
    {
        parola = &words[i*WORD_LENGTH];
        
        // trova il destinatario appropriato in base alla chiave
        receiver = funzione_hash(parola, comm_sz);
        
        // inserisci la parola nel buffer del destinatario
        strcpy(
               &send_buffer[(receiver*MAX_WORDS*WORD_LENGTH)
                            + (send_buffer_index[receiver]*WORD_LENGTH)
                            + 0] // prendo l'inizio della parola
               , parola);
        send_counts[(receiver*MAX_WORDS)
                    + send_buffer_index[receiver]] = count[i];
        send_buffer_index[receiver]++;
    }
    //stampa_array_int(send_buffer_index, comm_sz);
    
    /*
     INVIO PAROLE
     */
    // ci sincronizziamo per accordarci
    MPI_Barrier(MPI_COMM_WORLD);
    
    // Comunico ad ogni processo quante parole gli devo mandare
    // e ogni processore mi dice quanti parole deve mandare lui a me
    /*
     Per questa fase forzo un ordine di invio. I processori si mettono in fila in base al loro rank, fanno Recv del processo di turno e quando tocca a loro, inviano messaggi a tutti gli altri.
     
     Penserò ad ottimizzarla dopo.
     */
    // COMUNICO il numero di parole trovate
    for (i = 0; i < comm_sz; i++)
    {
            // Tocca a me inviare
        if (i == my_rank)
        {
            // invio a tutti
            for(j = 0; j < comm_sz; j++)
            {
                if (j == my_rank)
                {
                    // senza che invio a me stesso, me li posiziono da solo
                    for(k = 0; k < send_buffer_index[j]; k++)
                    {
                        // inserisci la parola nel buffer del destinatario
                        parola = &send_buffer[(my_rank*MAX_WORDS*WORD_LENGTH)
                                              + (k*WORD_LENGTH)
                                              + 0]; // la parola va presa dall'inizio
                        
                        strcpy(&receive_buffer[(my_rank*MAX_WORDS*WORD_LENGTH)
                                           + (receive_buffer_index[my_rank]*WORD_LENGTH)]
                               , parola);
                        
                        receive_counts[(my_rank*MAX_WORDS)
                                       + receive_buffer_index[my_rank]] = send_counts[(my_rank*MAX_WORDS)+k];
                        
                        receive_buffer_index[my_rank]++;
                    }
                } else {
                    MPI_Send(&send_buffer_index[j], // da dove
                             1, // quanti
                             MPI_INT, // di che tipo
                             j, // a chi
                             j, // a chi (tag)
                             MPI_COMM_WORLD); // luogo
                             //requests[j]);
                }
            }
        } else {
            // Devo ricevere il messaggio di i
            MPI_Recv(&receive_buffer_index[i],
                     1,
                     MPI_INT,
                     i,
                     my_rank,
                     MPI_COMM_WORLD,
                     &status);
        }
    }
    //stampa_array_int(receive_buffer_index, comm_sz);
    
    
    // ci sincronizziamo per INVIARE i dati
    MPI_Barrier(MPI_COMM_WORLD);
    if (my_rank == 0)
    {
        //stampa_matrice_str(send_buffer, MAX_WORDS, comm_sz);
        //stampa_array_int(send_buffer_index, comm_sz);
    }
    
    // invio ad ogni processo e ricevo da ogni processo le sue parole
    for (i = 0; i < comm_sz; i++)
    {
            // tocca a me inviare
        if (i == my_rank)
        {
            for (j = 0; j < comm_sz; j++)
            {
                    // devo inviare dati a j
                if (send_buffer_index[j] != 0
                    && j != my_rank) // non a me stesso
                {
                        //  invio le parole
                    MPI_Send(&send_buffer[j*MAX_WORDS*WORD_LENGTH], // da dove
                             send_buffer_index[j]*WORD_LENGTH, // quanti
                             MPI_CHAR, // di che tipo
                             j, // a chi
                             (j+(comm_sz*2)), // a chi (tag)
                             MPI_COMM_WORLD); // luogo
                    
                        // invio i conteggi
                    MPI_Send(&send_counts[j*MAX_WORDS], // da dove
                             send_buffer_index[j], // quanti
                             MPI_INT, // cosa
                             j, // a chi
                             (j+(comm_sz*3)), // a chi (tag)
                             MPI_COMM_WORLD); // luogo di scambio
                }
            }
            
        } else {
                // devo (effettivamente) ricevere dati da i
            if (receive_buffer_index[i] != 0)
            {
                    // ricevo le parole
                MPI_Recv(&receive_buffer[i*MAX_WORDS*WORD_LENGTH], // dove
                         receive_buffer_index[i]*WORD_LENGTH, // quanti
                         MPI_CHAR, // cosa
                         i, // da chi
                         (my_rank+(comm_sz*2)), // per chi (tag)
                         MPI_COMM_WORLD, // luogo di scambio
                         &status); // controlli
                 
                
                    // ricevo i conteggi
                MPI_Recv(&receive_counts[i*MAX_WORDS], // dove
                         receive_buffer_index[i], // quanti
                         MPI_INT,
                         i, // da chi
                         (my_rank+(comm_sz*3)), // per chi (tag)
                         MPI_COMM_WORLD, // luogo di scambio
                         &status); // controlli
            }
        }
    }
    /*
     FINE SHUFFLE
     */
    if (my_rank == 0)
    {
        //printf(" Sono [%d]\n", my_rank);
        //stampa_array_int(send_buffer_index, comm_sz);
        //stampa_array_int(receive_buffer_index, comm_sz);
        //stampa_matrice_str(receive_buffer, MAX_WORDS, comm_sz);
    }
    
} /* Shuffle */


void conteggio_finale(char *my_words, int* my_count, int* my_words_number, int comm_sz,
    int* receive_buffer_index, char* receive_buffer, int* receive_counts)
{
        // per comodità
    char*       parola;
    int         word_count;
    int         i, j, k, isUnique;
    
        // Resetto il mio dizionario
    for (i = 0; i < (*my_words_number); i++)
    {
        my_count[i] = 0;
    }
    (*my_words_number) = 0;
    
    /*
     Passo a rassegna tutte le parole che mi sono state inviate e le aggiungo al mio dizionario.
     Se la parola già è nel dizionario, ne aggiorno il conteggio
     */
        // per ogni processo
    for (i = 0; i < comm_sz; i++)
    {
            // leggo le parole che ha
        for (j = 0; j < receive_buffer_index[i]; j++)
        {
                // prendo la parola
            parola = &receive_buffer[(i*MAX_WORDS*WORD_LENGTH) // salto gli alti processi
                                     + (j*WORD_LENGTH)]; // salto le parole che ho già letto
                // prendo il suo valore
            word_count = receive_counts[(i*MAX_WORDS) // salto gli altri processi
                                        + j]; // salto i conteggi che ho già letto
            
                // controllo se la parola già c'è nel mio dizionario
            isUnique = 1;
            for (k = 0; (k < (*my_words_number)) && (isUnique); k++)
            {
                if (strcmp(&my_words[k*WORD_LENGTH], parola) == 0)
                    isUnique = 0;
            }

            /* Se la parola è unica, la devo aggiungere alla lista,
             * altimenti devo solo modificarne il contatore.
             */
            if (isUnique
                && (*my_words_number) < (MAX_WORDS))
            {
                strcpy(&my_words[(*my_words_number)*WORD_LENGTH], parola);
                my_count[(*my_words_number)] = word_count;

                (*my_words_number)++;
            } else if (!isUnique)
            {
                my_count[k-1] += word_count;
            }
        }
    }
} /* Conteggio Finale */


/**
 Come conteggio finale ma non resetta le parole che il nodo ha nel suo dizionario
 */
void conteggio_finale_master(char *my_words, int* my_count, int* my_words_number, int comm_sz,
    int* receive_buffer_index, char* receive_buffer, int* receive_counts)
{
        // per comodità
    char*       parola;
    int         word_count;
    int         i, j, k, isUnique;
    
    /*
     Passo a rassegna tutte le parole che mi sono state inviate e le aggiungo al mio dizionario.
     Se la parola già è nel dizionario, ne aggiorno il conteggio
     
     Se ho fatto bene i calcoli ogni parola è nuova.
     */
        // per ogni processo oltre a me
    for (i = 1; i < comm_sz; i++)
    {
        //printf("# [%d] mi deve dare %d parole.\n", i, receive_buffer_index[i]);
            // leggo le parole che ha
        for (j = 0; j < receive_buffer_index[i]; j++)
        {
                // prendo la parola
            parola = &receive_buffer[(i*MAX_WORDS*WORD_LENGTH) // salto gli alti processi
                                     + (j*WORD_LENGTH)]; // salto le parole che ho già letto
                // prendo il suo valore
            word_count = receive_counts[(i*MAX_WORDS) // salto gli altri processi
                                        + j]; // salto i conteggi che ho già letto

            
            /* Se la parola è unica, la devo aggiungere alla lista,
             * altimenti devo solo modificarne il contatore.
             */
            if ((*my_words_number) < (MAX_WORDS)
                && (receive_buffer_index[i] != 0))
            {
                //printf("-> Sto scrivendo \"%s\" da parte di [%d]\n", parola, i);
                strcpy(&my_words[(*my_words_number)*WORD_LENGTH], parola);
                my_count[(*my_words_number)] = word_count;

                (*my_words_number)++;
            }
        }
    }
} /* Conteggio Finale Master */


/**
 Consegno i miei risultati finali al JobTracker
 */
void reduce(char *my_words, int* my_counts, int* my_words_number, int my_rank, int comm_sz)
{
        // ci sincronizziamo
    MPI_Barrier(MPI_COMM_WORLD);
    
        // comunico al JobTracker quante parole gli devo mandare
    MPI_Send(my_words_number, // da dove
             1, // quanti
             MPI_INT, // di che tipo
             0, // a chi
             0, // (tag)
             MPI_COMM_WORLD); // luogo
    //printf(" Sono [%d] ho detto al JobTracker che ho %d parole per lui\n", my_rank, (*my_words_number));
    
    // invio al JobTracker (rank = 0) le mie parole
    if (my_words_number != 0)
    {
            // invio le parole
        MPI_Send(my_words, // da dove
                 (*my_words_number)*WORD_LENGTH, // quanti
                 MPI_CHAR, // di che tipo
                 0, // a chi
                 0, // (tag)
                 MPI_COMM_WORLD); // luogo
        //printf(" Sono [%d] ho inviato le PAROLE al JobTracker\n", my_rank);
               
            // invio i conteggi
        MPI_Send(my_counts, // da dove
                 (*my_words_number), // quanti
                 MPI_INT, // di che tipo
                 0, // a chi
                 0, // (tag)
                 MPI_COMM_WORLD); // luogo
        //printf(" Sono [%d] ho inviato i CONTEGGI al JobTracker\n", my_rank);
    }
} /* Reduce */


/**
 Reduce del JobTracker
 */
void reduce_master(char *my_words, int* my_counts, int* my_words_number, int comm_sz,
    int* receive_buffer_index, char* receive_buffer, int* receive_counts)
{
    /*
     INIZIALIZZO VARIABIILI
     */
    MPI_Status status;
    int i;
    
    /*
     Vado ad aggiungere tutti i messaggi ricevuti, direttamente al buffer finale che poi stamperò
     */
    MPI_Barrier(MPI_COMM_WORLD);
        // devo ricevere messaggi da tutti i nodi
        // parto da 1 perché non mi devo mandare i messaggi da solo
    for (i = 1; i < comm_sz; i++)
    {
            // ricevo il numero di parole
        MPI_Recv(&receive_buffer_index[i], // dove
                 1, // quanti
                 MPI_INT, // cosa
                 i, // da chi
                 0, // (tag)
                 MPI_COMM_WORLD, // luogo di scambio
                 &status); // controlli
        
        if (receive_buffer_index[i] != 0)
        {
                // ricevo le parole
            MPI_Recv(&receive_buffer[i*MAX_WORDS*WORD_LENGTH], // dove
                     receive_buffer_index[i]*WORD_LENGTH, // quanti
                     MPI_CHAR, // cosa
                     i, // da chi
                     0, // (tag)
                     MPI_COMM_WORLD, // luogo di scambio
                     &status); // controlli
            
                // ricevo i conteggi
            MPI_Recv(&receive_counts[i*MAX_WORDS], // dove
                     receive_buffer_index[i], // quanti
                     MPI_INT, // cosa
                     i, // da chi
                     0, // (tag)
                     MPI_COMM_WORLD, // luogo di scambio
                     &status); // controlli
        }
    }
        // aggiungo i dati al mio dizionario
    /*
     Potrei ottimizzarlo facendo in modo che i dati vengano scritti direttamente sul dizionario. Rimuovendo il bisogno di questa funzione.
     */
    //stampa_conteggio((*my_words_number), my_words, my_counts);
    //stampa_matrice_str(receive_buffer, MAX_WORDS, comm_sz);
    //stampa_array_int(receive_buffer_index, comm_sz);
    //stampa_matrice_int(receive_counts, MAX_WORDS, comm_sz);
    
    conteggio_finale_master(my_words, my_counts, my_words_number, comm_sz, receive_buffer_index, receive_buffer, receive_counts);
}


/**
    IL MAIN
 */
int main()
{
    /*
     INIZIALIZZO MPI
     */
    int my_rank, comm_sz;
    
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    
    
    /*
     PRENDO FILE
     */
    FILE *fp;
    char path[PATH_LENGTH];
    
        // JobTracker decide il file
    if (my_rank == 0)
    {
        strcpy(path, "/Users/Ikarux/Desktop/Multicore/SocialNetwork/friends_test_1.txt");
    }
    
        // Broadcast
    MPI_Bcast(path, // da dove
              PATH_LENGTH, // quanti
              MPI_CHAR, // cosa
              0, // da chi
              MPI_COMM_WORLD); // luogo di scambio
    
        // apro il file
    fp = fopen(path, "r");
    
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
    
        // per la ricezione
    int         receive_buffer_index[comm_sz];
    char        receive_buffer[comm_sz*MAX_WORDS*WORD_LENGTH];
    int         receive_counts[comm_sz*MAX_WORDS];
    
    
    /*
     CONTO LE PAROLE
     */
    //printf("------\n Sono [%d] e sto per contare le parole del file.\n------\n", my_rank);
    find_possible_friends(my_words, my_count, &index, fp, my_rank, comm_sz);
    
    /*
     SHUFFLE
     */
    //printf("------\n Sono [%d] e sto per iniziare lo shuffle.\n------\n", my_rank);
    shuffle(my_words, my_count, index, my_rank, comm_sz,
            receive_buffer_index, receive_buffer, receive_counts);
    
    
    /*
     CONTO DI NUOVO
     */
    //printf("------\n Sono [%d] e sto per iniziare il conteggio finale.\n------\n", my_rank);
    conteggio_finale(my_words, my_count, &index, comm_sz,
            receive_buffer_index, receive_buffer, receive_counts);
    //printf("[%d]\n", my_rank);
    //stampa_conteggio(index, my_words, my_count);
    
    
    /*
     INVIO RISULTATI FINALI AL JOBTRACKER
     */
    if (my_rank != 0)
    {
        //printf("------\n Sono [%d] e sto per iniziare il REDUCE.\n------\n", my_rank);
        reduce(my_words, my_count, &index, my_rank, comm_sz);
    }
    else
    {
        //printf("------\n Sono [JOBTRACKER] e sto per iniziare il reduce.\n------\n");
        reduce_master(my_words, my_count, &index, comm_sz, receive_buffer_index, receive_buffer, receive_counts);
    }
    
    
    /*
     STAMPO RISULTATI
     */
    // se sono il JobTracker
    if (my_rank == 0)
    {
        stampa_conteggio(index, my_words, my_count);
    } else {
        //printf("### Sono [%d] e ho concluso. ###\n", my_rank);
    }
    
    
    /*
     FINE
     */
        // Chiudo il file
    fclose(fp);
        // Chiudo le comunicazioni
    MPI_Finalize();
        // Esco
    return 0;
} /* Main */
