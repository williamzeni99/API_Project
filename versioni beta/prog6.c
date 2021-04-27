#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 1026

typedef struct {
    int pos; //indice 1 del comando
    int fp_x; //fp del testo in quel momento
}memory; //struttura per il salvataggio dei dati

char** realloc_punt(int index, int dim, char** punt);
char*** realloc_punt3(int index, int dim, char*** punt);
memory* realloc_struct(int index, int dim, memory* punt);
void libera_change(int fp3, int old_fp3, char*** lista_change);

void save_frasi(int ind1, int ind2, int* fp2, int fp3, char** lista_frasi, memory* dati);// salva le frasi raccolte con change, aggiorna fp3 e riempe pos1 e pos2
void save_frasi_delete(int fp, int fp2, int* fp4, char** testo, char** lista_delete, memory* dati);// salva le frasi eliminate dalla delete (solo i puntatori, non elimina nulla)
void change(int ind1, int ind2, int* fp, int fp3, char** testo, char** lista_frasi, memory* dati);
void print(int ind1, int ind2, int fp, char** testo);
void deletes(int ind1, int ind2, int* fp, char** testo);
char** save_testo(int fp, char** testo);

//void undo_delete(int* fp, int* fp2, memory* dati, char** testo, char** lista_delete);//funzione che annulla una delete
//void undo_change(int *fp,int *fp2, memory *dati,char **testo, char** lista_frasi, char** lista_delete);

int main() {
    int ind1 = 0, ind2 = 0, check = 0, check_u=0, check_r=0, x, old_fp3;
    int max = 10, max2 = 10, max3 = 10; // i max si riferiscono reciprocamente a testo, lista_frasi, lista_change sono le dimensioni allocate a loro
    int fp = 1, fp2 = 1, fp3 = 1;
    char operazione; //c, d, p, q, u, r
    char** lista_frasi = NULL; //lista frasi salvate in ordine crescente
    char*** lista_change = NULL; //copio i puntatori in uno storico delle delete per raggiungere le stringhe più velocemente
    char** testo = NULL; //testo reale attuale
    memory* dati; //vettore dei dati per ogni change o delete

    testo = (char**)malloc(max * sizeof(char*));
    lista_frasi = (char**)malloc(max2 * sizeof(char*));
    lista_change = (char***)malloc(max3 * sizeof(char**));
    dati = (memory*)malloc(max3 * sizeof(memory));
    dati[0].pos = 0;
    dati[0].fp_x = 1;
    //inizializzo le variabili

    do {

        scanf("%d", &ind1);
        operazione = getchar();
        if (operazione == ',') {
            scanf("%d", &ind2);
            operazione = getchar();
        }//inserimento operazione e ind1,2

        if (fp3 > max3 - 3) {//controllo sulla dimensione della struttura (numero di change delete si aggiorna sempre di 1)
            max3 = max3 + (max3 / 2);
            dati = realloc_struct(fp3, max3, dati);
            lista_change=realloc_punt3(fp3, max3, lista_change);
        }

        switch (operazione) {
        case 'c':
            if (check==1) {
              check=0;
              fp3++;
            }
            if (check_u==1) {
              check_u=0;
              //libera_change(fp3,old_fp3,lista_change);
            }
            if(check_r==1){
              check_r=0;
              fp3++;
            }
            x = ind2 - ind1 + 1; //calcolo il numero di righe da aggiunge alla lista delle frasi
            if (fp2 + x >= max2) { //controllo sulla dimensione della lista_frasi
                do {
                    max2 = max2 * 2;
                } while (fp2 + x >= max2);
                lista_frasi = realloc_punt(fp2, max2, lista_frasi);
            }
            if (ind2 >= max) {//controllo dimensione testo
                do {
                    max = max * 2;
                } while (max <= ind2);
                testo = realloc_punt(fp, max, testo);
            }
            save_frasi(ind1, ind2, &fp2, fp3, lista_frasi, dati);//salvo e raccolgo le frasi, si aggiorna fp3
            change(ind1, ind2, &fp, fp3, testo, lista_frasi, dati);// faccio la change sul testo, si aggiorna fp
            dati[fp3].fp_x = fp;
            lista_change[fp3]=save_testo(fp,testo);
            fp3++;// passo alla modifica successiva
            break;
        case 'p':
            print(ind1, ind2, fp, testo);
            break;
        case 'd':
            if (check==1){
              check=0;
              fp3++;
            }
            if (check_u==1) {
              check_u=0;
              //libera_change(fp3,old_fp3,lista_change);
            }
            if(check_r==1){
              check_r=0;
              fp3++;
            }
            deletes(ind1, ind2, &fp, testo);
            dati[fp3].fp_x = fp;
            lista_change[fp3]=save_testo(fp,testo);
            fp3++;
            break;
        case 'u':

            if(ind1<fp3-1){
              fp3=fp3-1-ind1;
              fp=dati[fp3].fp_x;
              max=fp;
              testo=lista_change[fp3];
            }
            else{
              check=1;
              fp3=-1;
              fp=dati[0].fp_x;
              max=fp;
              testo=lista_change[0];
            }

            fp3++;
            break;
        case 'r':

            break;

        default: break;
        }
/*
        printf("LISTA FRASI\n");
        for (i = 1; i < fp2; i++) {
            printf("%d. %s\n", i, lista_frasi[i - 1]);
        }

        printf("LISTA DATI\n");
        for (i = 1; i < fp3; i++) {
            printf("dati[%d]-> pos: %d fp_x: %d\n", i, dati[i].pos, dati[i].fp_x);
        }

        for (i = 1; i < fp3; i++) {
          printf("TESTO %d\n",i );
          for (j = 1; j < dati[i].fp_x; j++) {
            printf("%s",lista_change[i][j-1]);
          }
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

char*** realloc_punt3(int index, int dim, char*** punt){
  char*** new_punt;
  int i;
  new_punt = (char***)malloc(sizeof(char**) * dim);
  for (i = 1; i < index; i++) {
      new_punt[i] = punt[i];
  }
  free(punt);
  return new_punt;
}

memory* realloc_struct(int index, int dim, memory* punt) {
    memory* new_dati;
    int i;

    new_dati = (memory*)malloc((dim) * sizeof(memory));
    for (i = 0; i < index; i++) {
        new_dati[i].pos = punt[i].pos;
        new_dati[i].fp_x = punt[i].fp_x;
    }
    free(punt);

    return new_dati;
}//funzione che rialloca un vettore di tipo memory salvandone il contenuto
/*
void save_info(int ind1, int ind2, int fp2, char operazione, memory* dati) {
    dati[fp2].ind1 = ind1;
    dati[fp2].ind2 = ind2;
    dati[fp2].op = operazione;
}//funzione che salva ind1,ind2 e op
*/
void save_frasi(int ind1, int ind2, int* fp2, int fp3, char** lista_frasi, memory* dati) {
    int i, j, x = ind2 - ind1 + 1;
    char stringa[MAX];

    getchar();

    for (i = 0, j = *fp2; i < x; i++, j++) {
        fgets(stringa, MAX, stdin);
        lista_frasi[j - 1] = (char*)malloc(sizeof(char) * (strlen(stringa) + 1));
        strcpy(lista_frasi[j - 1],stringa);
    }
    dati[fp3].pos=*fp2;
    *fp2 = j;
    getchar();
}//funzione che legge e salva le stringhe nella lista delle frasi

void change(int ind1, int ind2, int* fp, int fp3, char** testo, char** lista_frasi, memory* dati) {
    int i, j;

    if (*fp <= ind2) {
        *fp = ind2 + 1;
    }
    for (i = ind1, j = 0; i <= ind2; i++, j++) {
        testo[i - 1] = lista_frasi[dati[fp3].pos - 1 + j];
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

char** save_testo(int fp, char** testo){
  char **new_testo;
  int i;
  new_testo=(char**)malloc(fp*sizeof(char*));

  for (i = 1; i < fp; i++) {
    new_testo[i-1]=testo[i-1];
  }

  return new_testo;
}

void libera_change(int fp3, int old_fp3, char*** lista_change){
  int i;

  for ( i = fp3; i < old_fp3; i++) {
    free(lista_change[i]);
  }
}
/*
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
    else{
      for (i = dati[*fp2].ind1, j = pos1; i <= dati[*fp2].ind2; i++, j++) {
          testo[i - 1] = lista_delete[j-1];
      }
    }
  }
  else{
    *fp=dati[*fp2-1].fp_x;
  }

}// non funziona
*/
