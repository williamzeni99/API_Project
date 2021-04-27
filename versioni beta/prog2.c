#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 1025
//#define ISTR 40

//int get_ind1(char comando[MAX]);
//int get_ind2(char comando[MAX]);  non più necessarie
char** check_lenght(int ind1, int ind2, int* fp, int* max, char** testo);
char*** check_lenght2(int* fp2, int* max2, int* fp_list, char*** lista_testo); //funzione che aumenta la dimensione della lista dei testi
void change(int ind1, int ind2, int* fp, char** testo);
void print(int ind1, int ind2, int* fp, char** testo);
void deletes(int ind1, int ind2, int* fp, char** testo);
char** save_testo(int* fp, int* max, char** testo); //funzione che copia il testo in un nuovo testo e ne ritorna l'indirizzo

char** undo(int ind, int* fp2, char*** lista_testo);
//char** redo(int ind1, int *fp, char** testo);

void main() {
    int ind1 = 0, ind2 = 0, check=0; //check servirà per funzione free_testo() da fare dopo che undo e redo c'è un c/d command
    int max = 1, max2 = 2, fp = 1, fp2 = 1; //max2 memoria allocata al vettore triplo (lista dei testi), fp2 indice dell'ultimo testo ancora non scritto, max e fp stesso di prog1
    char operazione; //c, d, p, u, r, q
    int* fp_list=NULL; // vettore di interi indicizzati a fp2 che salva gli fp di ogni testo
    char** testo=NULL; // testo
    char*** lista_testo=NULL; //lista dei testi (vettore contenente i puntatori al testo)

    lista_testo = (char***)malloc(max2 * sizeof(char**));
    testo = (char**)malloc(max * sizeof(char*));
    fp_list = (int*)malloc(max2 * sizeof(int));
    fp_list[0] = 0; // il testo 0 ha fp=0

    do {
        scanf("%d", &ind1);
        operazione = getchar();
        if (operazione == ',') {
            scanf("%d", &ind2);
            operazione = getchar();
        } // inserimento dei dati più veloce e comprensivo dei casi tipo 'VAlu' o 'q'
        /*get_ind1 e get_ind2 erano state pensate perchè inizialmente pensavo di salvare la lista dei comandi per l'undo e il redo
        ma con questa implementazione non è necessario*/

        lista_testo = check_lenght2(&fp2, &max2, fp_list, lista_testo);// cotrollo di poter lavorare con i testi

        switch (operazione) {
        case 'c': testo = check_lenght(ind1, ind2, &fp, &max, testo); //unico caso in cui può aumentare lo spazio da allocare è in change
            change(ind1, ind2, &fp, testo);
            lista_testo[fp2] = save_testo(&fp, &max, testo); //dopo aver fatto una modifica nel testo salvo l'intero testo e lo linko al vettore di testi
            fp_list[fp2] = fp; //salvo il fp del testo modificato
            fp2++; //passo al testo dopo
            break;
        case 'p': print(ind1, ind2, &fp, testo);
            break;
        case 'd': deletes(ind1, ind2, &fp, testo);
            lista_testo[fp2] = save_testo(&fp, &max, testo);
            fp_list[fp2] = fp;
            fp2++;// stesso ragionamento di change
            break;
        case 'u': testo = undo(ind1, &fp2, lista_testo);
            fp = fp_list[fp2];//ricarico il fp corretto per il testo giusto (fp2 viene aggiornato in undo)
            fp2++;
            break;
        case 'r': //testo=redo(ind1, &fp, testo);
            break;

        default: break;
        }

    } while (operazione != 'q');
}

void change(int ind1, int ind2, int* fp, char** testo) {
    char stringa[MAX];
    int i;

    if (*fp <= ind2) {
        *fp = ind2 + 1;
    }
   getchar(); //rispetto a prima mi sono accorto che la variabile dot era inutile per getchar
    for (i = ind1; i <= ind2; i++) {
        fgets(stringa, MAX, stdin);
        testo[i - 1] = (char*)malloc(strlen(stringa) + 1);
        strcpy(testo[i - 1], stringa);
    }
   getchar();
}//stessa change nessuna modifica rilevante (vedi prog1)

void print(int ind1, int ind2, int* fp, char** testo) {
    int i, j, check = 1;

    if (ind1 < 1) {
        printf(".\n");
        ind1++;
        if (ind2 < 1) {
            check = 0;
        }
    }

    for (i = ind1; i <= ind2 && i < *fp && check == 1; i++) {
        printf("%s", testo[i - 1]);
    }
    for (j = 0; j < ind2 - i + 1 && check == 1; j++) {
        printf(".\n");
    }
}//stessa print nessuna modifica (vedi prog1)

char** check_lenght(int ind1, int ind2, int* fp, int* max, char** testo) {
    char** new_testo;
    int i;

    if (ind2 >= *max) {
        do {
            *max = (*max) * 2;
        } while (*max <= ind2);
        new_testo = (char**)malloc(sizeof(char*) * (*max));
        for (i = 0; i < *fp -1; i++) {
            new_testo[i] = testo[i];
        }
    }
    else {
        new_testo = testo;
    }

    return new_testo;
} //stessa check_lenght nessuna modifica (vedi prog1)

char** save_testo(int* fp, int* max, char** testo) {
    char** new_testo;
    int i,x;

    new_testo = (char**)malloc((*max) * sizeof(char*)); //alloco memoria per un nuovo testo
    for (i = 0; i < *fp-1; i++) { // fp-1 è posto perchè fp è concorde al testo. Riguardare nota bene in fondo a prog1
        x = strlen(testo[i]);
        new_testo[i] = (char*)malloc((x+1)*sizeof(char)); //alloco memoria alla riga
        strcpy(new_testo[i], testo[i]); //copio la riga
    }

    return new_testo;
}

void deletes(int ind1, int ind2, int* fp, char** testo) {
    int new_fp, i, j;

    if (ind1<1) {
        ind1++;
    }

    if (ind2 > 0 && ind1< *fp) {
        if (*fp > ind2) {
            new_fp = *fp - (ind2 - ind1 + 1);
            for (i = ind1, j = 1; j < (*fp) - ind2; i++, j++) {
                testo[i - 1] = testo[ind2 - ind1 + i];
            }

        }
        else {
            new_fp = ind1;
        }
        *fp = new_fp;

    }
} // stessa delete nessuna modifica (vedi prog1)

char*** check_lenght2(int* fp2, int* max2, int* fp_list, char*** lista_testo) {
    char*** new_lista_testo;
    int i;
    int* new_fp_list;

    //se sono arrivato al penultimo testo salvabile duplico i numeri di testi salvabili
    if(*fp2==*max2-1){
      *max2 = (*max2) * 2;
      new_lista_testo = (char***)malloc((*max2) * sizeof(char**));
      new_fp_list = (int*)malloc((*max2) * sizeof(int));
      for (i = 1; i < *fp2; i++) {
        new_lista_testo[i] = lista_testo[i];
        new_fp_list[i] = fp_list[i];
      }//copio la lista dei testi e la lista dei fp nei due nuovi vettori
      new_fp_list[0] = 0; //pongo il primo fp della lista degli fp a 0
      fp_list = new_fp_list;//copio il nuovo indirizzo in quello vecchio
      //BAD! non libero la memoria del vecchio fp_list
    }
    else{
      new_lista_testo=lista_testo; //se non sono al penultimo non alloco memoria
    }

    return new_lista_testo;
}

char** undo(int ind, int* fp2, char*** lista_testo){

    if (*fp2 - ind - 1> 0) { //se il numero di undo non arriva alla zero_page
        *fp2 = *fp2-ind-1; //aggiorno il puntatore al testo corretto
        return lista_testo[*fp2];//ritorno l'indirizzo del testo corrente
    }
    else {//se il numero di undo elimina tutte le mie operazioni
        *fp2 = 0;
        return lista_testo[*fp2+1]; //lista_testo[0] è la mia zero_page è vuota
    }
}
/*
char** redo(int ind1, int* fp, char** testo) {

}
*/

/*nota bene: fp2 è sempre l'indice del primo testo libero sul quale stiamo apportando modifiche.
Nel mio programma lista_testo[0] non viene riempitan e fa da zero_page. Non c'è un momento all'interno
del programma che venga usata. Anche nel caso che le undo mandino a 0 tutto il testo usato sarà quello di
lista_testo[1] ma con fp a 0. Questa soluzione non mi piace ma è la più comoda che mi è venuta in mente
fin'ora per la gestione degli indici di fp2.
*/
