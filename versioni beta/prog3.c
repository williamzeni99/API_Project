#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 300

typedef struct {
    int ind1;
    int ind2;
    int fp_x;
}memory; //struttura per il salvataggio dei dati

char** check_lenght(int ind1, int ind2, int* fp, int* max, char** testo);//funzione che aumenta la dimensione del testo principale
char*** check_lenght2(int fp2, int max2, char*** lista_testo); //funzione che aumenta la dimensione della lista dei testi
memory* check_lenght3(int fp2, int max2,memory *dati);//funzione che aumenta la dimensione del vettore di struct
void change(int ind1, int ind2, int* fp, int fp_old, char** testo);
void print(int ind1, int ind2, int fp, char** testo);
void deletes(int ind1, int ind2, int* fp, char** testo);
char** save_testo_c(char** testo, int ind1, int ind2);
char** save_testo_d(char** testo, int ind1, int ind2, int fp); //funzione che copia le modifiche effettuate in un nuovo testo e ne ritorna l'indirizzo

char** undo(int ind, int* fp2, char*** lista_testo); //non implementata ancora
//char** redo(int ind1, int *fp, char** testo);

int main() {
    int ind1 = 0, ind2 = 0;
    int max = 1, max2 = 2, fp = 1, fp2 = 1; //stesse variabili prog2
    char operazione; //c, d, p, u, r, q
    char** testo = NULL; // testo
    char*** lista_testo=NULL; //lista dei testi (vettore contenente i puntatori al testo)
    memory* dati;

    dati = (memory*)malloc(sizeof(memory) * max2);
    testo = (char**)malloc(max * sizeof(char*));
    dati[0].ind1 = 0;
    dati[0].ind2 = 0;
    dati[0].fp_x = 0;

    do {
        scanf("%d", &ind1);
        operazione = getchar();
        if (operazione == ',') {
            scanf("%d", &ind2);
            operazione = getchar();
        }

        if (fp2==max2-1) {//controllo sulla dimensione della lista testi
          max2=max2+(max2/2);
          lista_testo = check_lenght2(fp2, max2, lista_testo);
          dati=check_lenght3(fp2, max2, dati);
        }

        switch (operazione) {
        case 'c': testo = check_lenght(ind1, ind2, &fp, &max, testo); //unico caso in cui può aumentare lo spazio da allocare è in change
            change(ind1, ind2, &fp, dati[fp2 - 1].fp_x, testo);
            dati[fp2].ind1 = ind1;
            dati[fp2].ind2 = ind2;
            dati[fp2].fp_x = fp;
            lista_testo[fp2]=save_testo_c(testo, ind1, ind2); //salvataggio dei dati necessari
            fp2++; //passo al testo dopo
            break;
        case 'p': print(ind1, ind2, fp, testo);
            break;
        case 'd': lista_testo[fp2] = save_testo_d(testo, ind1, ind2, fp);
            deletes(ind1, ind2, &fp, testo);
            dati[fp2].ind1 = ind1;
            dati[fp2].ind2 = ind2;
            dati[fp2].fp_x = fp;
            fp2++;// stesso ragionamento di change
            break;
        case 'u':// testo = undo(ind1, &fp2, lista_testo);
          //  fp = fp_list[fp2];//ricarico il fp corretto per il testo giusto (fp2 viene aggiornato in undo)
            //fp2++;
            break;
        case 'r': //testo=redo(ind1, &fp, testo);
            break;

        default: break;
        }

    } while (operazione != 'q');

/*
    for (i = 0; i < fp-1; i++) {
      free(testo[i]);
    }
    for (i = 1; i < fp2; i++) {
      x=dati[i].ind2-dati[i].ind1+1;
      for (j = 0; j < x; j++) {
        free(lista_testo[i][j]);
      }
      free(lista_testo[i]);
    }
    free(testo);
    free(lista_testo);
    free(dati);

    for (i = 1; i < fp2; i++) {
      printf("dati[%d]: %d %d %d\n",i,dati[i].ind1, dati[i].ind2, dati[i].fp_x );
      x=dati[i].ind2-dati[i].ind1+1;
      printf("testo\n");
      for (j = 0; j < x; j++) {
        printf("%s",lista_testo[i][j]);
      }
    }
*/
}

void change(int ind1, int ind2, int* fp, int fp_old, char** testo) {
    char stringa[MAX];
    int i;

    if (*fp <= ind2) {
        *fp = ind2 + 1;
    }
    getchar(); //rispetto a prima mi sono accorto che la variabile dot era inutile per getchar
    for (i = ind1; i <= ind2; i++) {
        fgets(stringa, MAX, stdin);
        if (i >= fp_old) {
            testo[i - 1] = (char*)malloc(sizeof(char) * MAX);
        }
        strcpy(testo[i - 1], stringa);
    }
    getchar();
}//stessa change nessuna modifica rilevante (vedi prog1)

void print(int ind1, int ind2, int fp, char** testo) {
    int i, j, check = 1;

    if (ind1 < 1) {
        printf(".\n");
        ind1++;
        if (ind2 < 1) {
            check = 0;
        }
    }

    for (i = ind1; i <= ind2 && i < fp && check == 1; i++) {
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
        for (i = 0; i < *fp - 1; i++) {
            new_testo[i] = testo[i];
        }
        free(testo);
    }
    else {
        new_testo = testo;
    }
    return new_testo;
} //stessa check_lenght nessuna modifica (vedi prog1)

char*** check_lenght2(int fp2, int max2, char*** lista_testo) {
    char*** new_lista_testo;
    int i;

    //se sono arrivato al penultimo testo salvabile duplico i numeri di testi salvabili
    new_lista_testo = (char***)malloc((max2) * sizeof(char**));
    for (i = 1; i < fp2; i++) {
      new_lista_testo[i] = lista_testo[i];
    }//copio la lista dei testi e la lista dei fp nei due nuovi vettori
    free(lista_testo);

    return new_lista_testo;
}

memory* check_lenght3(int fp2, int max2,memory *dati){
  memory *new_dati;
  int i;

  new_dati=(memory*)malloc((max2)*sizeof(memory));
  for (i = 0; i < fp2; i++) {
    new_dati[i].ind1=dati[i].ind1;
    new_dati[i].ind2=dati[i].ind2;
    new_dati[i].fp_x=dati[i].fp_x;
  }
  free(dati);

  return new_dati;
}

char** save_testo_c(char** testo, int ind1, int ind2){
  char** new_testo;
  int i,j;
  int x=ind2-ind1+1;

  new_testo = (char**)malloc(x * sizeof(char*)); //alloco memoria per un nuovo testo
  for (i =0, j=ind1 ; i < x ; i++,j++) { // fp-1 è posto perchè fp è concorde al testo. Riguardare nota bene in fondo a prog1
      new_testo[i] = (char*)malloc(MAX * sizeof(char)); //alloco memoria alla riga
      strcpy(new_testo[i], testo[j-1]); //copio la riga
  }

  return new_testo;
}

char** save_testo_d(char** testo, int ind1, int ind2, int fp){
  char** new_testo;
  int i,j;
  int x;

  if(ind1<fp){
    if (ind2<fp) {
      x=ind2-ind1+1;
    }
    else{
      x=fp-ind1;
    }

    new_testo = (char**)malloc(x * sizeof(char*)); //alloco memoria per un nuovo testo
    for (i =0, j=ind1 ; i < x ; i++,j++) { // fp-1 è posto perchè fp è concorde al testo. Riguardare nota bene in fondo a prog1
      new_testo[i] = (char*)malloc(MAX * sizeof(char)); //alloco memoria alla riga
      strcpy(new_testo[i], testo[j-1]); //copio la riga
    }
  }

  return new_testo;
}

void deletes(int ind1, int ind2, int* fp, char** testo) {
    int new_fp, i, j;

    if (ind1 < 1) {
        ind1++;
    }

    if (ind2 > 0 && ind1 < *fp) {
        if (*fp > ind2) {
            new_fp = *fp - (ind2 - ind1 + 1);
            for (i = ind1; i <= ind2; i++) {
              free(testo[i-1]);
            }
            for (i = ind1, j = 1; j < (*fp) - ind2; i++, j++) {
                testo[i - 1] = testo[ind2 - ind1 + i];
            }
        }
        else {
            new_fp = ind1;
            for (i = new_fp; i < *fp; i++) {
              free(testo[i-1]);
            }
        }
        *fp = new_fp;

    }
} // stessa delete nessuna modifica (vedi prog1)

/*
char** save_testo(int* fp, int* max, char** testo) {
    char** new_testo;
    int i, x;

    new_testo = (char**)malloc((*max) * sizeof(char*)); //alloco memoria per un nuovo testo
    for (i = 0; i < *fp - 1; i++) { // fp-1 è posto perchè fp è concorde al testo. Riguardare nota bene in fondo a prog1
        x = strlen(testo[i]);
        new_testo[i] = (char*)malloc((x + 1) * sizeof(char)); //alloco memoria alla riga
        strcpy(new_testo[i], testo[i]); //copio la riga
    }

    return new_testo;
}

void deletes(int ind1, int ind2, int* fp, char** testo) {
    int new_fp, i, j;

    if (ind1 < 1) {
        ind1++;
    }

    if (ind2 > 0 && ind1 < *fp) {
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

char*** check_lenght2(int* fp2, int* max2, char*** lista_testo) {
    char*** new_lista_testo;
    int i;

    //se sono arrivato al penultimo testo salvabile duplico i numeri di testi salvabili
    if (*fp2 == *max2 - 1) {
        *max2 = (*max2) * 2;
        new_lista_testo = (char***)malloc((*max2) * sizeof(char**));
        for (i = 1; i < *fp2; i++) {
            new_lista_testo[i] = lista_testo[i];
        }//copio la lista dei testi e la lista dei fp nei due nuovi vettori
        free(lista_testo);
    }
    else {
        new_lista_testo = lista_testo; //se non sono al penultimo non alloco memoria
    }

    return new_lista_testo;
}

char** undo(int ind, int* fp2, char*** lista_testo) {

    if (*fp2 - ind - 1 > 0) { //se il numero di undo non arriva alla zero_page
        *fp2 = *fp2 - ind - 1; //aggiorno il puntatore al testo corretto
        return lista_testo[*fp2];//ritorno l'indirizzo del testo corrente
    }
    else {//se il numero di undo elimina tutte le mie operazioni
        *fp2 = 0;
        return lista_testo[*fp2 + 1]; //lista_testo[0] è la mia zero_page è vuota
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
