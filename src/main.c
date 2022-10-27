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

  afn_free(B);
}
