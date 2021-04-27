#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 1025 //consegna: il testo non ha righe più lunghe di 1024
#define ISTR 20 //istruzione è di tipo %d,%d%c quindi lunghezza comando max è di 14

int get_ind1(char comando[MAX]);//ritorna dal comando il primo indice
int get_ind2(char comando[MAX]);//ritorna dal comando il secondo indice
char** check_lenght(char comando[ISTR], int *fp, int *max, char** testo); //allunga la profondità del testo nel caso venga sforato max
void change(char comando[ISTR], int *fp, char** testo);// funzione del problema
void print(char comando[ISTR], int *fp, char** testo);// funzione del problema
void delete(char comando[ISTR], int *fp, char** testo);// funzione del problema

void main() {
  char comando[ISTR];//stringa che contiene il comando (es 1,2c o 3,4d ecc..)
  int max=10, fp=1;//max è profondità del testo allocata, fp è l'indice alla prima riga vuota
  char operazione; //c, d, p, u, r, q
  char** testo; //vettore di stringhe non ancora allocato

  testo=(char**)malloc(max * sizeof(char*)); //imposto la prima profondità

  do {
    scanf("%s",comando);
    operazione=comando[strlen(comando)-1];//c, d, p, u , r, q si trovano sempre in fondo al comando

    switch (operazione) {
      case 'c': testo=check_lenght(comando, &fp, &max, testo); //solo in change la memoria può aumentare
                change(comando, &fp, testo);
                break;
      case 'd': delete(comando, &fp, testo);
                break;
      case 'p': print(comando, &fp, testo);
                break;
      case 'u': //undo(ind1, &fp, testo);
                break;
      case 'r': //redo(ind1, &fp, testo);
                break;
      default: break;
    }
  } while(operazione!='q');//se operazione q termina il programma
}

void change(char comando[ISTR], int *fp, char** testo){
  char stringa[MAX], dot;
  int i, ind1, ind2;

  ind1=get_ind1(comando);
  ind2=get_ind2(comando);
  //raccolgo i due indici

  if(*fp<=ind2){
    *fp=ind2+1;
  }//aggiorno fp

  dot=getchar(); //raccolgo il '/n' dal buffer stdin
  for (i = ind1; i <= ind2; i++){
    fgets(stringa,MAX,stdin);//leggo la riga da modificare
    testo[i-1]=(char*)malloc(strlen(stringa)+1);//intesto la memoria
    strcpy(testo[i-1],stringa);//copio stringa nel testo
  }
  dot=getchar();// raccolgo '.'
}/*nota bene questa funzione non libera memoria e rialloca indipentemente dal fatto
che sia necessario. BAD, da modificare. Con questa funzione 5/5 al test*/

int get_ind1(char comando[MAX]){
  int i, j;
  char* ind;

  for (i = 0; comando[i]!=','; i++) {} //scorro fino alla virgola
  ind=(char*)malloc(sizeof(int)*(i)+1);
  for (j = 0; j < i; j++) {
    ind[j]=comando[j];
  }
  ind[j]='\0';
  //in questo modo ind contiene il primo indice del comando come stringa

  return atoi(ind);//funzione che converte una stringa in intero
}

int get_ind2(char comando[MAX]){
  int i,j,k;
  char* ind;

  for (i = 1; comando[i]!=','; i++) {}
  k=strlen(comando)-1-i;
  ind=(char*)malloc(sizeof(int)* (k));
  for (j = 0; j <k; j++) {
    ind[j]=comando[i+1];
    i++;
  }
  ind[j]='\0';
  //stesso ragionamento di get_ind1, qui ricavo il secondo indice

  return atoi(ind);
}

char** check_lenght(char comando[ISTR], int *fp, int *max, char **testo){
  char **new_testo;
  int ind2=get_ind2(comando), i;

  if (ind2>=*max) {
    do {
      *max=(*max)*2;
    } while(*max<=ind2);//se ind2 è oltre la memoria allocata, la duplica
    new_testo=(char**)malloc(sizeof(char*)*(*max)); //creo il nuovo testo
    for (i = 0; i < *fp; i++) {
      new_testo[i]=testo[i]; //copio ogni stringa del vecchio testo nel nuovo
    }
  }
  else{
    new_testo=testo; //nel caso non sia necessario non fa nulla
  }

  return new_testo;
}

void print(char comando[ISTR], int *fp, char** testo){
  int i,j,check=1;
  int ind1=get_ind1(comando);
  int ind2=get_ind2(comando);

  if(ind1<1){
    printf(".\n");
    ind1++;
    if (ind2<1) {
      check=0; //se caso 0,0p non deve fare nessuna print ma solo la stampa del punto
    }
  } //questo if è necessario ai casi del tipo 0,VALp oppure 0,0p

  for (i=ind1; i<=ind2 && i<*fp && check==1; i++) {
    printf("%s",testo[i-1]);
  }//printa fino alla prima cella vuota (fp)
  for (j = 0; j < ind2-i+1 && check==1; j++) {
    printf(".\n" );
  }//se la prima cella vuota è prima di ind2 stampa dei punti
}

void delete(char comando[ISTR], int *fp, char** testo){
  int ind1=get_ind1(comando);
  int ind2=get_ind2(comando);
  int new_fp, i,j;

  if(ind1<1){
    ind1++; //caso 0,VALd lo trasformo nel caso 1,VALd perchè lo 0 non esiste
  }

  if(ind2>0){ //caso 0,0d escluso
    if (*fp>ind2) {
      new_fp=*fp-(ind2-ind1+1);
      for (i =ind1,j=1; j<(*fp)-ind2; i++,j++) {
        testo[i-1]=testo[ind2-ind1+i];
      }
    }//se l'ind2 è sotto la prima riga vuota aggiorno fp e sposto le stringhe verso il basso di fp-ind2 volte
    else{
      new_fp=ind1; //se ind2>fp allora mi basta aggiornare il fp. BAD non libero memoria
    }

    *fp=new_fp;
  }
}

/*nota bene: l'indice del testo, per richiesta del problema, parte da 1.
Per non aver sprechi di memoria utilizzo testo[i-1] così nel caso ind1 è 1 riempio
testo[0]. fp è concorde con il sistema di riferimento del testo. Esempio:
ho scritto 4 righe --> fp è 5, l'ultima riga piena è testo[3]*/
