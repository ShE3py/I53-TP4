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
}
