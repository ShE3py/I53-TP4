#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "afn.h"

/*
 * FUNCTION: afn_init
 * ------------------
 * initialise et retourne un AFN dont les états sont numérotés de 0 à `Q`
 * ajoute le symbole '&' en début d'alphabet si celui-ci n'y est pas déjà
 * 
 * param: 
 *        Q  - plus grand état de l'automate
 *        nbInitaux - nombre d'états initiaux
 *        listInitiaux - tableau de `nbInitiaux` entiers représentant les états initiaux
 *        nbFinals - nombre d'états finals
 *        listFinals - tableau de `nbFinals` entiers représentant les états finals
 *        Sigma - alphabet de l'automate
 * 
 * return:
 *        un AFN dont la tableau de transition est allouée mais vide
 */
AFN  afn_init(int Q, int nbInitiaux, int * listInitiaux, int nbFinals, int * listFinals, char *Sigma){
  AFN A; 
  if ( (A=malloc(sizeof(struct AFN))) == NULL){
    printf("malloc error A");
    exit(1);
  }
  A->Q = Q;

  A->lenI = nbInitiaux;
  if ( (A->I = malloc(sizeof(int)*nbInitiaux)) == NULL){
    printf("malloc error A->I");
    exit(1);
  }
  for (int i=0; i<nbInitiaux; i++) A->I[i] = listInitiaux[i];
  
  A->lenF = nbFinals;
  if ( (A->F = malloc(sizeof(int)*nbFinals)) == NULL){
    printf("malloc error A->F");
    exit(1);
  }
  for (int i=0; i<nbFinals; i++) A->F[i] = listFinals[i];


  A->lenSigma = strlen(Sigma);
  if (Sigma[0] != '&') A->lenSigma++;
  if ( (A->Sigma = malloc(sizeof(char)*(A->lenSigma+1)))==NULL){
    printf("malloc error A->Sigma");
    exit(1);
  }
  if (Sigma[0] != '&'){
    strcpy(A->Sigma+1,Sigma);
    A->Sigma[0]='&';
  } else {
    strcpy(A->Sigma,Sigma);
  }
  for (int i=0; i<MAX_SYMBOLES; i++) A->dico[i]=-1;
  for (int i=0; i<A->lenSigma; i++) A->dico[A->Sigma[i]-ASCII_FIRST]=i;

  if ((A->delta = malloc( sizeof(int***)*(Q+1)))==NULL){
    printf("malloc error A->Sigma");
    exit(1);
  }
  for (int q=0; q<A->Q+1; q++){
    if( (A->delta[q] = malloc( sizeof(int **)*A->lenSigma))==NULL){
      printf("malloc error A->Sigma[%d]", q);
      exit(1);
    }
    for (int s=0; s<A->lenSigma; s++)	A->delta[q][s]=NULL;
  }
  return A;
}

/*
 * FUNCTION: afn_print
 * -------------------
 * affiche l'AFN `A`
 * 
 * param: 
 *        A  - un AFN
 */
void afn_print(AFN A){
  printf("Q = {0,..,%d}\n", A->Q);
  printf("I = {");
  for (int i=0; i<A->lenI; i++) printf("%d,",A->I[i]);
  printf("\b}\n");

  printf("F = {");
  for (int i=0; i<A->lenF; i++) printf("%d,",A->F[i]);
  printf("\b}\n");

  int state_size = (int)(ceil(log10( (double)A->Q)));
  int padding = (state_size>=5)? (state_size-5)/2+1: 1;
  int first_column_size = state_size>=5 ? state_size+2 : 7;
  int max_cell_size = 0;

  for (int q=0; q<A->Q; q++){
    for (int s=0; s<A->lenSigma; s++){
      if (A->delta[q][s]!=NULL){
	int cell_size = 0;
	
	while (A->delta[q][s][cell_size]!=-1) cell_size++;
	max_cell_size = (max_cell_size < cell_size ? cell_size : max_cell_size);
      }
    }
  }
  int total_cell_size = max_cell_size*(state_size+1)+1;

  int line_length = first_column_size+1+(total_cell_size+1)*A->lenSigma;
  char * line = malloc(sizeof(char)*(line_length+2));
  for (int i=0; i<=line_length; i++) line[i]='-';
  line[line_length+1]='\0';
  printf("%s\n",line);
  printf("|%*sdelta |", padding, "");
  for (int i=0; i<A->lenSigma; i++) printf("%*c |", total_cell_size-1, A->Sigma[i]);
  printf("\n");
  printf("%s\n",line);

  char *buffer = malloc(sizeof(char)*(total_cell_size+1));
  for (int q=0; q<A->Q+1; q++){
    printf("|%*d |", padding+5, q);
    for (int i=0; i<A->lenSigma; i++){
      int s = A->dico[A->Sigma[i]-ASCII_FIRST];
      if (A->delta[q][s] != NULL){
	int j=0;
	buffer[0]='{';
	buffer[1]='\0';
	while (A->delta[q][s][j]!=-1) {
	  sprintf(buffer,"%s%d,",buffer, A->delta[q][s][j++]);
	}
	printf("%*s\b}|", total_cell_size ,buffer );
      } else {
	printf("%*s|",total_cell_size,"");
      }
    }
    printf("\n");
    printf("%s\n",line);
  }
  free(buffer);
  free(line);
}

/*
 * FUNCTION: afn_free
 * -------------------
 * libère la mémoire de l'AFN `A` initialisé par la fonction afn_init 
 * 
 * param: 
 *        A  - un AFN
 */
void afn_free(AFN A){
  free(A->I);
  free(A->F);
  free(A->Sigma);
  for (int q=0; q<A->Q+1; q++){
    for (int s=0; s<A->lenSigma; s++){
      if (A->delta[q][s]!=NULL)
	free(A->delta[q][s]);
    }
    free(A->delta[q]);
  }
  free(A->delta);
  free(A);  
}


/*
 * FUNCTION: afn_ajouter_transition
 * --------------------------------
 * ajoute la transition  `q1` -- `s` --> `q2` à l'automate `A` où l'ensemble des transitions
 * partant de l'état `q1` et étiquetées par le symbole `s` delta[q][s] est un tableau 
 * d'entiers trié dans l'ordre croissant et se terminant par -1, NULL si vide
 * 
 * param: 
 *        A  - un AFN
 *        q1 - état de départ de la transition    
 *        s  - étiquette de la transition
 *        q2 - état d'arrivée de la transition    
 */
void afn_ajouter_transition(AFN A, int q1, char s, int q2){
}

/*
 * FUNCTION: afn_finit
 * ------------------
 * initialise et renvoie un AFN à partir du fichier `file` écrit au format:
 * 
 *  'nombre_etat
 *  'nombre_etats_initiaux
 *  'etat_initial_1 etat_initial_2 ... etat_initial_n
 *  'nombre_etats_finals
 *  'etat_final_1 etat_final_2 ... etat_final_n
 *  'alphabet
 *  'etat_i1 symbole_k1 etat_j1
 *  'etat_i2 symbole_k2 etat_j2
 *  '...
 *  'etat_in symbole_kn etat_jn 
 * 
 * param :
 *         file - un nom de fichier
 * return:
 *         un AFN complet
 */
AFN afn_finit(char *file);

/*
 * FUNCTION: afn_epsilon_fermeture
 * -------------------------------
 * renvoie un pointeur vers l'epsilon fermeture de l'ensemble d'états `R` 
 *
 * param:
 *        A - un AFN
 *        R - un ensemble d'états sous forme de tableau d'entiers triés par ordre croissant
 * terminant par -1, NULL si vide
 */

int * afn_epsilon_fermeture(AFN A, int *R);
/*
 * FUNCTION: afn_simuler
 * ---------------------
 * renvoie 1 si la chaine `s` est acceptée par l'AFN `A`, 0 sinon
 *
 * param:
 *        A - un AFN
 *        s - la chaine de caractères à analyser
 */
int afn_simuler(AFN A, char *s);
