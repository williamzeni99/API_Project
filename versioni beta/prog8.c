#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 1026

typedef struct {
    int pos; //indice 1 del comando
    int fp_x; //fp_testo del testo in quel momento
    int check;
    int dim;
    char operazione;
}memory; //struttura per il salvataggio dei dati

char** realloc_punt(int index, int dim, char** punt);
char*** realloc_punt3(int index, int dim, char*** punt);
memory* realloc_struct(int index, int dim, memory* punt);
void libera_change(int fp_lista_change, int old_fp3, char*** lista_change);

void save_frasi(int ind1, int ind2, int* fp_lista_frasi, int fp_lista_change, char** lista_frasi, memory* dati);// salva le frasi raccolte con change, aggiorna fp_lista_change e riempe pos1 e pos2
void save_frasi_delete(int fp_testo, int fp_lista_frasi, int* fp4, char** testo, char** lista_delete, memory* dati);// salva le frasi eliminate dalla delete (solo i puntatori, non elimina nulla)
void change(int ind1, int ind2, int* fp_testo, int fp_lista_change, char** testo, char** lista_frasi, memory* dati);
void print(int ind1, int ind2, int fp_testo, char** testo);
void deletes(int ind1, int ind2, int* fp_testo, char** testo);
char** save_testo(int fp_testo, char** testo, int fp_lista_change, memory *dati, int ind1);
char** ricomponi_testo(memory *dati, int fp_lista_change, char*** lista_change, char **testo, int *max_testo);
//void undo_delete(int* fp_testo, int* fp_lista_frasi, memory* dati, char** testo, char** lista_delete);//funzione che annulla una delete
//void undo_change(int *fp_testo,int *fp_lista_frasi, memory *dati,char **testo, char** lista_frasi, char** lista_delete);

int main() {
    int ind1 = 0, ind2 = 0, check = 0, check_u=0, x, i,j;
    int max_testo = 10, max_lista_frasi = 10, max_lista_change = 10; // i max_testo si riferiscono reciprocamente a testo, lista_frasi, lista_change sono le dimensioni allocate a loro
    int fp_testo = 1, fp_lista_frasi = 1, fp_lista_change = 1;
    int fp_lista_max=0;
    char operazione; //c, d, p, q, u, r
    char** lista_frasi = NULL; //lista frasi salvate in ordine crescente
    char*** lista_change = NULL; //copio i puntatori in uno storico delle delete per raggiungere le stringhe più velocemente
    char** testo = NULL; //testo reale attuale
    memory* dati; //vettore dei dati per ogni change o delete

    testo = (char**)malloc(max_testo * sizeof(char*));
    lista_frasi = (char**)malloc(max_lista_frasi * sizeof(char*));
    lista_change = (char***)malloc(max_lista_change * sizeof(char**));
    dati = (memory*)malloc(max_lista_change * sizeof(memory));
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

        if (fp_lista_change > max_lista_change - 5) {//controllo sulla dimensione della struttura (numero di change delete si aggiorna sempre di 1)
            max_lista_change = max_lista_change + (max_lista_change / 2);
            dati = realloc_struct(fp_lista_change, max_lista_change, dati);
            lista_change=realloc_punt3(fp_lista_change, max_lista_change, lista_change);
        }

        switch (operazione) {
        case 'c':
            if (fp_lista_max<fp_lista_change) {
              fp_lista_max=fp_lista_change;
            }
            if (fp_lista_change==0) {
              fp_lista_change++;
            }
            if (check_u==1) {
              check_u=0;
              fp_lista_max=fp_lista_change;
            }
            x = ind2 - ind1 + 1; //calcolo il numero di righe da aggiunge alla lista delle frasi
            if (fp_lista_frasi + x >= max_lista_frasi) { //controllo sulla dimensione della lista_frasi
                do {
                    max_lista_frasi = max_lista_frasi * 2;
                } while (fp_lista_frasi + x >= max_lista_frasi);
                lista_frasi = realloc_punt(fp_lista_frasi, max_lista_frasi, lista_frasi);
            }
            if (ind2 >= max_testo-1) {//controllo dimensione testo
                do {
                    max_testo = max_testo * 2;
                } while (max_testo <= ind2);
                testo = realloc_punt(fp_testo, max_testo, testo);
            }
            save_frasi(ind1, ind2, &fp_lista_frasi, fp_lista_change, lista_frasi, dati);//salvo e raccolgo le frasi, si aggiorna fp_lista_change
            change(ind1, ind2, &fp_testo, fp_lista_change, testo, lista_frasi, dati);// faccio la change sul testo, si aggiorna fp_testo
            dati[fp_lista_change].fp_x = fp_testo;
            dati[fp_lista_change].operazione = operazione;
            lista_change[fp_lista_change]=save_testo(fp_testo,testo, fp_lista_change, dati, ind1);
            fp_lista_change++;// passo alla modifica successiva
            break;
        case 'p':
            print(ind1, ind2, fp_testo, testo);
            break;
        case 'd':
            if (fp_lista_max<fp_lista_change) {
              fp_lista_max=fp_lista_change;
            }
            if (fp_lista_change==0){
              fp_lista_change++;
            }
            if (check_u==1) {
              check_u=0;
              fp_lista_max=fp_lista_change;
              //lista_change[fp_lista_change]=testo;
              //fp_lista_change++;
            }
            deletes(ind1, ind2, &fp_testo, testo);
            dati[fp_lista_change].fp_x = fp_testo;
            dati[fp_lista_change].operazione = operazione;
            lista_change[fp_lista_change]=save_testo(fp_testo,testo,fp_lista_change, dati, ind1);
            fp_lista_change++;
            break;
        case 'u':
            check_u=1;
            if(ind1<fp_lista_change-1){
              fp_lista_change=fp_lista_change-1-ind1;
              fp_testo=dati[fp_lista_change].fp_x;
              if (dati[fp_lista_change].check==1){
                testo=ricomponi_testo(dati, fp_lista_change,lista_change,testo, &max_testo);
              }
              else{
                max_testo=dati[fp_lista_change].dim;
                testo=lista_change[fp_lista_change];
              }
            }
            else{
              fp_lista_change=-1;
              fp_testo=dati[0].fp_x;
              max_testo=fp_testo;
              testo=lista_change[0];
            }
            fp_lista_change++;
            break;
        case 'r':
            if (check_u == 1 && fp_lista_max>0) {
              if (fp_lista_change == 0) {
                fp_lista_change = 1;
              }
              if (ind1 + fp_lista_change - 1 >= fp_lista_max) {
                fp_lista_change = fp_lista_max;
              }
              else {
                fp_lista_change = fp_lista_change - 1 + ind1;
              }
              fp_testo = dati[fp_lista_change].fp_x;
              if (dati[fp_lista_change].check == 1) {
                testo = ricomponi_testo(dati, fp_lista_change, lista_change, testo, &max_testo);
              }
              else {
                max_testo = dati[fp_lista_change].dim;
                testo = lista_change[fp_lista_change];
              }
              fp_lista_change++;
            }

            break;

        default: break;
        }
/*
        printf("LISTA FRASI\n");
        for (i = 1; i < fp_lista_frasi; i++) {
            printf("%d. %s\n", i, lista_frasi[i - 1]);
        }

        printf("LISTA DATI\n");
        for (i = 1; i < fp_lista_change; i++) {
            printf("dati[%d]-> pos: %d fp_x: %d check: %d  dim: %d\n", i, dati[i].pos, dati[i].fp_x, dati[i].check,dati[i].dim);
        }

        for (i = 1; i < fp_lista_change; i++) {
          printf("TESTO %d\n",i );
          for (j = 1; j < dati[i].dim+1; j++) {
            printf("%s",lista_change[i][j-1]);
          }
        }
*/
    } while (operazione != 'q');

/*
    for (i = 1; i < fp_lista_frasi; i++) {
        free(lista_frasi[i - 1]);
    }
    for (i = 0; i < fp_lista_change; i++) {
      free(lista_change[i]);
    }
    free(lista_frasi);
    free(testo);
    free(dati);
    free(lista_change);
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

void save_frasi(int ind1, int ind2, int* fp_lista_frasi, int fp_lista_change, char** lista_frasi, memory* dati) {
    int i, j, x = ind2 - ind1 + 1;
    char stringa[MAX];

    getchar();

    for (i = 0, j = *fp_lista_frasi; i < x; i++, j++) {
        fgets(stringa, MAX, stdin);
        lista_frasi[j - 1] = (char*)malloc(sizeof(char) * (strlen(stringa) + 1));
        strcpy(lista_frasi[j - 1],stringa);
    }
    dati[fp_lista_change].pos=*fp_lista_frasi;
    *fp_lista_frasi = j;
    getchar();
}//funzione che legge e salva le stringhe nella lista delle frasi

void change(int ind1, int ind2, int* fp_testo, int fp_lista_change, char** testo, char** lista_frasi, memory* dati) {
    int i, j;

    if (*fp_testo <= ind2) {
        *fp_testo = ind2 + 1;
    }
    for (i = ind1, j = 0; i <= ind2; i++, j++) {
        testo[i - 1] = lista_frasi[dati[fp_lista_change].pos - 1 + j];
    }

}

void print(int ind1, int ind2, int fp_testo, char** testo) {
    int i, j, check = 1;

    if (ind1 < 1) {
        printf(".\n");
        ind1++;
        if (ind2 < 1) {
            check = 0;
        }
    }

    for (i = ind1; i <= ind2 && i < fp_testo && check == 1; i++) {
        printf("%s", testo[i - 1]);
    }
    for (j = 0; j < ind2 - i + 1 && check == 1; j++) {
        printf(".\n");
    }
}

void deletes(int ind1, int ind2, int* fp_testo, char** testo) {
    int new_fp, i, j;

    if (ind1 < 1) {
        ind1++;
    }

    if (ind2 > 0 && ind1 < *fp_testo) {
        if (*fp_testo > ind2) {
            new_fp = *fp_testo - (ind2 - ind1 + 1);
            for (i = ind1, j = 1; j < (*fp_testo) - ind2; i++, j++) {
                testo[i - 1] = testo[ind2 - ind1 + i];
            }
        }
        else {
            new_fp = ind1;
        }
        *fp_testo = new_fp;

    }
} // stessa delete nessuna modifica (vedi prog1)

char** save_testo(int fp_testo, char** testo, int fp_lista_change, memory *dati, int ind1){
  char **new_testo;
  int i,j;

  if(ind1==dati[fp_lista_change-1].fp_x && dati[fp_lista_change].operazione == 'c'){
    new_testo=(char**)malloc((fp_testo-dati[fp_lista_change-1].fp_x)*sizeof(char*));
    for (i = dati[fp_lista_change-1].fp_x, j=1 ; i < fp_testo; i++,j++) {
      new_testo[j-1]=testo[i-1];
    }
    if (dati[fp_lista_change-1].fp_x>1) {
      dati[fp_lista_change].check=1;
    }
    dati[fp_lista_change].dim=fp_testo-dati[fp_lista_change-1].fp_x;
  }
  else{
    new_testo=(char**)malloc((fp_testo-1)*sizeof(char*));
    for (i = 1; i < fp_testo; i++) {
      new_testo[i-1]=testo[i-1];
    }
    dati[fp_lista_change].dim=fp_testo-1;
  }

  return new_testo;
}

char** ricomponi_testo(memory *dati, int fp_lista_change, char*** lista_change, char **testo, int *max_testo){
  char** new_testo;
  int i,j,k,l, dim=0;

  for (i = fp_lista_change; dati[i].check==1 && i > 0; i--){}
  new_testo=(char**)malloc((dati[fp_lista_change].fp_x-1)*2*sizeof(char*));
  for (j = i, l=0; j <=fp_lista_change ; j++) {
    for (k = 0; k <dati[j].dim ; k++,l++) {
      new_testo[l]=lista_change[j][k];
    }
  }
  *max_testo=(dati[fp_lista_change].fp_x-1)*2;
  free(testo);
  return new_testo;


}

void libera_change(int fp_lista_change, int old_fp3, char*** lista_change){
  int i;

  for ( i = fp_lista_change; i < old_fp3; i++) {
    free(lista_change[i]);
  }
}
