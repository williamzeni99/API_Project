#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 300

typedef struct {
    int ind1; //indice 1 del comando
    int ind2; //indice 2 del comando
    int pos1; //posizione 1 nello storico
    int pos2; //posizione 2 nello storico
    int fp_x; //fp del testo in quel momento
    char op; //operazione c,d
}memory; //struttura per il salvataggio dei dati

char** realloc_punt(int index, int dim, char** punt); //funzione che aumenta la dimensione di un puntatore salvandone il contenuto e ne ritorna l'indirizzo
memory* realloc_struct(int index, int dim, memory* punt); //funzione che aumenta la dimensione di un puntatore memory salvandone il contenuto
void save_info(int ind1, int ind2, int fp2, char operazione, memory* dati); //salva ind1 , ind2 e op
void save_frasi(int ind1, int ind2, int fp2, int* fp3, char** lista_frasi, memory* dati);// salva le frasi raccolte con change, aggiorna fp3 e riempe pos1 e pos2
void save_frasi_delete(int fp, int fp2, int* fp4, char** testo, char** lista_delete, memory* dati);// salva le frasi eliminate dalla delete (solo i puntatori, non elimina nulla)
void change(int* fp, int fp2, memory* dati, char** testo, char** lista_frasi);
void print(int ind1, int ind2, int fp, char** testo);
void deletes(int ind1, int ind2, int* fp, char** testo);

void undo_delete(int* fp, int* fp2, memory* dati, char** testo, char** lista_delete);//funzione che annulla una delete
void undo_change(int *fp,int *fp2, memory *dati,char **testo, char** lista_frasi, char** lista_delete);

int main() {
    int ind1 = 0, ind2 = 0, check = 0, x, i;
    int max = 1, max2 = 2, max3 = 1, max4 = 1; // i max si riferiscono reciprocamente a testo, dati, lista_frasi, lista_delete e sono le dimensioni allocate a loro
    int fp = 1, fp2 = 1, fp3 = 1, fp4 = 1; //i fp si riferiscono reciprocamente a testo, dati, lista_frasi, lista_delete e sono le prime celle vuote di ciascuna cosa
    char operazione; //c, d, p, q, u, r
    char** lista_frasi = NULL; //lista frasi salvate in ordine crescente
    char** lista_delete = NULL; //copio i puntatori in uno storico delle delete per raggiungere le stringhe più velocemente
    char** testo = NULL; //testo reale attuale
    memory* dati; //vettore dei dati per ogni change o delete

    testo = (char**)malloc(max * sizeof(char*));
    dati = (memory*)malloc(max2 * sizeof(memory));
    lista_frasi = (char**)malloc(max3 * sizeof(char*));
    lista_delete = (char**)malloc(max4 * sizeof(char*));
    save_info(0, 0, 0, 'n', dati);
    dati[0].pos1 = 0;
    dati[0].pos2 = 0;
    dati[0].fp_x = 0;
    //inizializzo le variabili

    do {

        scanf("%d", &ind1);
        operazione = getchar();
        if (operazione == ',') {
            scanf("%d", &ind2);
            operazione = getchar();
        }//inserimento operazione e ind1,2

        if (fp2 == max2 - 1) {//controllo sulla dimensione della struttura (numero di change delete si aggiorna sempre di 1)
            max2 = max2 + (max2 / 2);
            dati = realloc_struct(fp2, max2, dati);
        }

        switch (operazione) {
        case 'c':
            if (check == 1) {
                fp2++;
                check = 0;
            }
            save_info(ind1, ind2, fp2, operazione, dati); //salvo le info di base
            x = ind2 - ind1 + 1; //calcolo il numero di righe da aggiunge alla lista delle frasi
            if (fp3 + x >= max3) { //controllo sulla dimensione della lista_frasi
                do {
                    max3 = max3 * 2;
                } while (fp3 + x >= max3);
                lista_frasi = realloc_punt(fp3, max3, lista_frasi);
            }
            if (ind2 >= max) {//controllo dimensione testo
                do {
                    max = max * 2;
                } while (max <= ind2);
                testo = realloc_punt(fp, max, testo);
            }
            save_frasi(ind1, ind2, fp2, &fp3, lista_frasi, dati);//salvo e raccolgo le frasi, si aggiorna fp3
            change(&fp, fp2, dati, testo, lista_frasi);// faccio la change sul testo, si aggiorna fp
            dati[fp2].fp_x = fp;
            fp2++;// passo alla modifica successiva
            break;
        case 'p': print(ind1, ind2, fp, testo);
            break;
        case 'd':
            if (check == 1) {
                fp2++;
                check = 0;
            }
            save_info(ind1, ind2, fp2, operazione, dati);// salvo le info di base
            x = ind2 - ind1 + 1; //calcolo le righe da aggiungere alla lista_delete
            if (fp4 + x >= max4) { //controllo dimensione di lista_delete
                do {
                    max4 = max4 * 2;
                } while (fp4 + x >= max4);
                lista_delete = realloc_punt(fp4, max4, lista_delete);
            }
            save_frasi_delete(fp, fp2, &fp4, testo, lista_delete, dati);//aggiungo in lista_delete i puntatori delle frasi da togliere utilizzando testo
            deletes(ind1, ind2, &fp, testo);
            dati[fp2].fp_x = fp;
            fp2++;
            break;
        case 'u':
            for (i = 0; i < ind1 && fp2>0; i++) {
                if (dati[fp2 - 1].op == 'd') {
                    undo_delete(&fp, &fp2, dati, testo, lista_delete);
                    if (fp2 == 0) {
                        check = 1;
                    }
                }
                else {
                    undo_change(&fp,&fp2,dati, testo, lista_frasi, lista_delete);
                    if(fp2==0){
                      check=1;
                    }
                }
            }

            break;
        case 'r': //testo=redo(ind1, &fp, testo);
            break;

        default: break;
        }
        /*
        printf("LISTA FRASI\n");
        for (i = 1; i < fp3; i++) {
            printf("%d. %s\n", i, lista_frasi[i - 1]);
        }
        printf("LISTA DELETE\n");
        for (i = 1; i < fp4; i++) {
            printf("%d. %s\n", i, lista_delete[i - 1]);
        }
        printf("LISTA DATI\n");
        for (i = 1; i < fp2; i++) {
            printf("dati[%d]-> ind1: %d  ind2: %d  pos1: %d pos2: %d  op: %c fp_x: %d\n", i, dati[i].ind1, dati[i].ind2, dati[i].pos1, dati[i].pos2, dati[i].op, dati[i].fp_x);
        }
        */
    } while (operazione != 'q');

/*
    for (i = 1; i < fp3; i++) {
        free(lista_frasi[i - 1]);
    }
    free(testo);
    free(dati);
    free(lista_delete);
    free(lista_frasi);
*/
}

char** realloc_punt(int index, int dim, char** punt) {
    char** new_punt;
    int i;
    new_punt = (char**)malloc(sizeof(char*) * dim);
    for (i = 1; i < index; i++) {
        new_punt[i - 1] = punt[i - 1];
    }
    free(punt);
    return new_punt;
} //funzione che rialloca un puntatore doppio salvandone il contenuto

memory* realloc_struct(int index, int dim, memory* punt) {
    memory* new_dati;
    int i;

    new_dati = (memory*)malloc((dim) * sizeof(memory));
    for (i = 0; i < index; i++) {
        new_dati[i].ind1 = punt[i].ind1;
        new_dati[i].ind2 = punt[i].ind2;
        new_dati[i].pos1 = punt[i].pos1;
        new_dati[i].pos2 = punt[i].pos2;
        new_dati[i].op = punt[i].op;
        new_dati[i].fp_x = punt[i].fp_x;
    }
    free(punt);

    return new_dati;
}//funzione che rialloca un vettore di tipo memory salvandone il contenuto

void save_info(int ind1, int ind2, int fp2, char operazione, memory* dati) {
    dati[fp2].ind1 = ind1;
    dati[fp2].ind2 = ind2;
    dati[fp2].op = operazione;
}//funzione che salva ind1,ind2 e op

void save_frasi(int ind1, int ind2, int fp2, int* fp3, char** lista_frasi, memory* dati) {
    int i, j, x = ind2 - ind1 + 1;
    char stringa[MAX];

    getchar();

    for (i = 0, j = *fp3; i < x; i++, j++) {
        fgets(stringa, MAX, stdin);
        lista_frasi[j - 1] = (char*)malloc(sizeof(char) * (strlen(stringa) + 1));
        strcpy(lista_frasi[j - 1],stringa);
    }
    dati[fp2].pos1 = *fp3;
    dati[fp2].pos2 = j - 1;
    *fp3 = j;
    getchar();
}//funzione che legge e salva le stringhe nella lista delle frasi

void change(int* fp, int fp2, memory* dati, char** testo, char** lista_frasi) {
    int i, j;

    if (*fp <= dati[fp2].ind2) {
        *fp = dati[fp2].ind2 + 1;
    }
    for (i = dati[fp2].ind1, j = 0; i <= dati[fp2].ind2; i++, j++) {
        testo[i - 1] = lista_frasi[dati[fp2].pos1 - 1 + j];
    }

}

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
}

void save_frasi_delete(int fp, int fp2, int* fp4, char** testo, char** lista_delete, memory* dati) {
    int i, j;

    if (dati[fp2].ind2 > 0 && fp > 1) {
        if (dati[fp2].ind1 < 1) {
            dati[fp2].ind1++;
        }
        if (dati[fp2].ind2 >= fp) {
            dati[fp2].ind2 = fp - 1;
        }

        for (i = dati[fp2].ind1, j = 0; i <= dati[fp2].ind2; i++, j++) {
            lista_delete[*fp4 + j - 1] = testo[i - 1];
        }

        dati[fp2].pos1 = *fp4;
        dati[fp2].pos2 = *fp4 + j - 1;
        *fp4 = *fp4 + j;
    }
    else {
        dati[fp2].pos1 = -1;
        dati[fp2].pos2 = -1;
    }


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

void undo_delete(int* fp, int* fp2, memory* dati, char** testo, char** lista_delete) {
    int i, j, x;

    *fp2 = *fp2 - 1;
    if (*fp2 > 0) {
        if (dati[*fp2].ind2 == dati[*fp2 - 1].fp_x - 1) {
            for (i = dati[*fp2].pos1, j = *fp; i <= dati[*fp2].pos2; i++, j++) {
                testo[j - 1] = lista_delete[i - 1];
            }
        }
        else {
            x = dati[*fp2].ind2 - dati[*fp2].ind1 + 1;
            for (i = dati[*fp2 - 1].fp_x - 1, j = 1; j < dati[*fp2 - 1].fp_x - dati[*fp2].ind2 ; i--, j++) {
                testo[i - 1] = testo[i - 1 - x];
            }
            for (i = 0, j = dati[*fp2].ind1; i < x; i++, j++) {
                testo[j - 1] = lista_delete[dati[*fp2].pos1 - 1 + i];
            }
        }
        *fp = dati[*fp2 - 1].fp_x;
    }

}//dovrebbe funzionare

void undo_change(int *fp,int *fp2, memory *dati,char **testo, char** lista_frasi, char** lista_delete){
  int i,j;
  int pos1, ind1, ind2;
  char op;

  *fp2=*fp2-1;
  if (dati[*fp2].ind2<dati[*fp2-1].fp_x) {
    ind1=dati[*fp2].ind1;
    ind2=dati[*fp2].ind2;
    for (i = *fp2-1; i > 0; i--) {
      if (dati[i].ind1<=ind1 && ind2<=dati[i].ind2 && dati[i].op=='c') {
        pos1=dati[i].pos1+ind1-dati[i].ind1;
        //op=dati[i].op;
      }
    }
    if (op=='c') {
      for (i = dati[*fp2].ind1, j = pos1; i <= dati[*fp2].ind2; i++, j++) {
          testo[i - 1] = lista_frasi[j-1];
      }
    }
  /*  else{
      for (i = dati[*fp2].ind1, j = pos1; i <= dati[*fp2].ind2; i++, j++) {
          testo[i - 1] = lista_delete[j-1];
      }
    }*/
  }
  else{
    *fp=dati[*fp2-1].fp_x;
  }

}// non funziona
