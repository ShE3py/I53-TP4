#include <stdio.h>

#include "fsa/compat.h"

int main(int argc, char *argv[]){
  int Q=3,q0=0,nbFinals=2,listFinals[2]={1,3}, nbInitaux=2, listInitiaux[2]={0,1};
  char Sigma[]="abc";

  AFD  A;
  A = afd_init(Q,q0,nbFinals,listFinals,Sigma);

  afd_ajouter_transition(A, 0,'a',2);
  afd_ajouter_transition(A, 0,'b',1);
  afd_ajouter_transition(A, 0,'c',0);
  afd_ajouter_transition(A, 1,'a',2);
  afd_ajouter_transition(A, 1,'b',0);
  afd_ajouter_transition(A, 1,'c',3);
  afd_ajouter_transition(A, 2,'a',3);
  afd_ajouter_transition(A, 2,'b',1);
  afd_ajouter_transition(A, 2,'c',3);
  afd_ajouter_transition(A, 3,'a',2);
  afd_ajouter_transition(A, 3,'b',1);
  afd_ajouter_transition(A, 3,'c',3);

  afd_print(A);
  printf("\"b\" = %i\n", afd_simuler(A, "b")); // 1
  printf("\"ab\" = %i\n", afd_simuler(A, "ab")); // 1
  printf("\"baba\" = %i\n\n", afd_simuler(A, "baba")); // 0
  

  afd_free(A);
  
  AFN B;
  B = afn_init(Q,nbInitaux,listInitiaux,nbFinals,listFinals,Sigma);

  afn_ajouter_transition(B, 0,'a',2);
  afn_ajouter_transition(B, 0,'a',1);
  afn_ajouter_transition(B, 0,'a',0);
  afn_ajouter_transition(B, 1,'a',2);
  afn_ajouter_transition(B, 1,'b',0);
  afn_ajouter_transition(B, 1,'c',3);
  afn_ajouter_transition(B, 2,'a',3);
  afn_ajouter_transition(B, 2,'b',1);
  afn_ajouter_transition(B, 2,'c',3);
  afn_ajouter_transition(B, 3,'a',2);
  afn_ajouter_transition(B, 3,'b',1);
  afn_ajouter_transition(B, 3,'c',3);

  afn_print(B);
  nfa_dot(B, "test");
  
  printf("\"b\" = %i\n", afn_simuler(B, "b")); // 0
  printf("\"\" = %i\n", afn_simuler(B, "")); // 1
  printf("\"cacbabbaa\" = %i\n", afn_simuler(B, "cacbabbaa")); // 1

  afn_free(B);
  
  AFD C = afd_finit("afd.txt");
  afd_print(C);
  afd_free(C);
  
  AFN D = afn_finit("nfd.txt");
  afn_print(D);
  nfa_dot(D, "test2");
  
  afn_free(D);
}
