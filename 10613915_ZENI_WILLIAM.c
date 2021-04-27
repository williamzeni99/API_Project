#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 1026

typedef struct {
    int pos; //posizione nello storico delle frasi
    int fp_x; //fp_testo corrente
    int check;//controllo per ricomponi_testo
    int dim; //dimensione del file
    char operazione;//c o d
}memory; //struttura per il salvataggio dei dati

char** realloc_punt(int index, int dim, char** punt);
char*** realloc_punt3(int index, int dim, char*** punt);
memory* realloc_struct(int index, int dim, memory* punt);
void save_frasi(int ind1, int ind2, int* fp_lista_frasi, int fp_lista_change, char** lista_frasi, memory* dati);
void save_frasi_delete(int fp_testo, int fp_lista_frasi, int* fp4, char** testo, char** lista_delete, memory* dati);
void change(int ind1, int ind2, int* fp_testo, int fp_lista_change, char** testo, char** lista_frasi, memory* dati);
void print(int ind1, int ind2, int fp_testo, char** testo);
void deletes(int ind1, int ind2, int* fp_testo, char** testo);
char** save_testo(int fp_testo, char** testo, int fp_lista_change, memory *dati, int ind1);
char** ricomponi_testo(memory *dati, int fp_lista_change, char*** lista_change, char **testo, int *max_testo);
char **ricopia_testo(char ***lista_change, int fp_lista_change, memory* dati);


int main() {
    int ind1 = 0, ind2 = 0, check_u=0, x; //check_u=1 sse operazione=u, torna a 0 se fa change o delete
    int max_testo = 10, max_lista_frasi = 10, max_lista_change = 10; //memoria allocata per ogni puntatore
    int fp_testo = 1, fp_lista_frasi = 1, fp_lista_change = 1; //i vari fp sono indici che fanno puntate i puntatori sempre al primo puntatore vuoto
    int fp_lista_max=0; //valore max raggiunto da fp_lista_change, serve per sapere fino a che punto posso fare le redo
    char operazione; //c, d, p, q, u, r
    char** lista_frasi = NULL; //storico delle frasi
    char*** lista_change = NULL;//storico dei cambiamenti al testo
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

        if (fp_lista_change > max_lista_change - 5) {
            max_lista_change = max_lista_change + (max_lista_change / 2);
            dati = realloc_struct(fp_lista_change, max_lista_change, dati);
            lista_change=realloc_punt3(fp_lista_change, max_lista_change, lista_change);
        }//controllo sulla dimensione della struttura

        switch (operazione) {
        case 'c':
            if (fp_lista_max<fp_lista_change) {
              fp_lista_max=fp_lista_change;
            }//aggiorno fp_lista_max ogni change
            if (fp_lista_change==0) {
              fp_lista_change++;
            }//condizione particolare nel caso vada a 0(deve puntare sempre alla prima vuota quindi min 1)
            if (check_u==1) {
              check_u=0;
              fp_lista_max=fp_lista_change;
            }//nel caso dopo un undo ci sia una change fp_lista_max diventa l'fp_lista_change attuale
            x = ind2 - ind1 + 1; //calcolo il numero di righe da aggiunge alla lista delle frasi
            if (fp_lista_frasi + x >= max_lista_frasi) {
                do {
                    max_lista_frasi = max_lista_frasi * 2;
                } while (fp_lista_frasi + x >= max_lista_frasi);
                lista_frasi = realloc_punt(fp_lista_frasi, max_lista_frasi, lista_frasi);
            }//controllo sulla dimensione della lista_frasi e nel caso riallocazione
            if (ind2 >= max_testo-1) {
                do {
                    max_testo = max_testo * 2;
                } while (max_testo <= ind2);
                testo = realloc_punt(fp_testo, max_testo, testo);
            }//controllo dimensione testo e nel caso riallocazione
            save_frasi(ind1, ind2, &fp_lista_frasi, fp_lista_change, lista_frasi, dati);//salvo e raccolgo le frasi, si aggiorna fp_lista_frasi
            change(ind1, ind2, &fp_testo, fp_lista_change, testo, lista_frasi, dati);// faccio la change sul testo, si aggiorna fp_testo
            dati[fp_lista_change].fp_x = fp_testo;
            dati[fp_lista_change].operazione = operazione;
            lista_change[fp_lista_change]=save_testo(fp_testo,testo, fp_lista_change, dati, ind1);//riempio la struttura dati con i valori necessari
            fp_lista_change++;// passo alla modifica successiva
            break;

        case 'p':
            print(ind1, ind2, fp_testo, testo); //printo il mio testo attuale
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
            }
            deletes(ind1, ind2, &fp_testo, testo);
            dati[fp_lista_change].fp_x = fp_testo;
            dati[fp_lista_change].operazione = operazione;
            lista_change[fp_lista_change]=save_testo(fp_testo,testo,fp_lista_change, dati, ind1);
            fp_lista_change++;// stesso ragionamento della change
            break;

        case 'u':
            check_u=1;//rendo noto al programma che sono entrato a fare una undo
            if(ind1<fp_lista_change-1){
              fp_lista_change=fp_lista_change-1-ind1;//mi muovo nel vettore lista_change fino all'undo appropriata
              fp_testo=dati[fp_lista_change].fp_x;//aggiorno fp_testo
              if (dati[fp_lista_change].check==1){
                testo=ricomponi_testo(dati, fp_lista_change,lista_change,testo, &max_testo);
              }//se è un tipo di testo terminale lo ricompongo e lo ricopia (aggiorno max)
              else{
                testo=ricopia_testo(lista_change, fp_lista_change,dati);
                max_testo=dati[fp_lista_change].fp_x*2;
              }//se è un testo totale lo ricpio solamente e aggiorno max
            }//controllo sulla possibilità di fare undo
            else{
              fp_lista_change=-1;
              fp_testo=dati[0].fp_x;
              max_testo=fp_testo;
              testo=lista_change[0];
            }//se non posso farlo porto tutto a 0
            fp_lista_change++;//passo alla modifica successiva
            break;

        case 'r':
            if (check_u == 1 && fp_lista_max>0) {
              if (fp_lista_change == 0) {
                fp_lista_change = 1;
              }//caso particolare trattato
              if (ind1 + fp_lista_change - 1 >= fp_lista_max) {
                fp_lista_change = fp_lista_max;
              }//se il numero di redo va oltre al numero consentito vado al max consentito
              else {
                fp_lista_change = fp_lista_change - 1 + ind1;
              }//altrimenti vado alla redo corretta
              fp_testo = dati[fp_lista_change].fp_x;//aggiorno fp_testo
              if (dati[fp_lista_change].check == 1) {
                testo = ricomponi_testo(dati, fp_lista_change, lista_change, testo, &max_testo);
              }//se è un testo terminale lo ricompongo e aggiorno max_testo
              else {
                testo=ricopia_testo(lista_change, fp_lista_change,dati);
                max_testo=dati[fp_lista_change].fp_x*2;
              }//se è un testo totale lo ricopio solamente e aggiorno max_testo
              fp_lista_change++;
            }//la redo posso farla solo se è mai stata fatta un undo o una modifica in generale
            break;

        default: break;//caso in cui non riconosca il comando
        }

    } while (operazione != 'q'); //se operazione=q termina il  programma
}

char** realloc_punt(int index, int dim, char** punt) {
    char** new_punt;
    int i;
    new_punt = (char**)malloc(sizeof(char*) * dim);
    for (i = 1; i < index; i++) {
        new_punt[i - 1] = punt[i - 1];
    }
    return new_punt;
} //funzione che rialloca un puntatore doppio salvandone il contenuto

char*** realloc_punt3(int index, int dim, char*** punt){
  char*** new_punt;
  int i;
  new_punt = (char***)malloc(sizeof(char**) * dim);
  for (i = 1; i < index; i++) {
      new_punt[i] = punt[i];
  }
  return new_punt;
}//funzione che rialloca un puntatore triplo salvandone il contenuto

memory* realloc_struct(int index, int dim, memory* punt) {
    memory* new_dati;
    int i;

    new_dati = (memory*)malloc((dim) * sizeof(memory));
    for (i = 0; i < index; i++) {
        new_dati[i].pos = punt[i].pos;
        new_dati[i].fp_x = punt[i].fp_x;
    }
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
}//funzione change del progetto, cambiamenti effettuati solo sul testo e aggiornamento di fp_testo

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
}//print del progetto, printa solo testo

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
}// funzione delete del progetto, cambiamenti effettuati solo sul testo e aggiornamento di fp_testo

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
}//funzione che salva il testo nella lista_change e aggiorna i dati della struttura

char** ricomponi_testo(memory *dati, int fp_lista_change, char*** lista_change, char **testo, int *max_testo){
  char** new_testo;
  int i,j,k,l;

  for (i = fp_lista_change; dati[i].check==1 && i > 0; i--){}
  new_testo=(char**)malloc((dati[fp_lista_change].fp_x-1)*2*sizeof(char*));
  for (j = i, l=0; j <=fp_lista_change ; j++) {
    for (k = 0; k <dati[j].dim ; k++,l++) {
      new_testo[l]=lista_change[j][k];
    }
  }
  *max_testo=(dati[fp_lista_change].fp_x-1)*2;
  return new_testo;
}//funzione che ricompone il testo nel caso il testo sia di tipo terminale

char **ricopia_testo(char ***lista_change, int fp_lista_change, memory* dati){
  int i;
  char** new_testo;

  new_testo=(char**)malloc(dati[fp_lista_change].fp_x*2*sizeof(char*));
  for (i = 1; i < dati[fp_lista_change].fp_x; i++) {
    new_testo[i-1]=lista_change[fp_lista_change][i-1];
  }

  return new_testo;
}//funzione che prende un testo dalla lista_change, lo copia e lo ritorna (dimensione raddoppiata per evitare realloc successive inutile)

/*Progetto API 2020 di William Zeni Cod.Persona 10613915
Data di consegna finale: 6/9/2020 */
