### Arborescence du projet :
- `src/`: les fichiers en-têtes et sources,
- `resources/`: les fichiers représentant des automates finis prédéfinis,
- `out/`: les fichiers sources compilés,
  - `out/png/`: les AFN générés par le programme et dessinés avec Graphviz.
- `grammaire.txt`: la grammaire formelle des expressions régulières acceptées par ce projet.

### Exécutables générés :
- `./test`: tests unitaires sur deux AFD et de deux AFN définis dans `resources/` vérfiants
que certains mots sont acceptés ou refusés par ces AF. Dessine aussi les deux AFN, leur
union, leur concaténation et l'étoile de Klenne du premier dans `out/png/`.
- `./mydot <expression régulière...>` : dessine les automates associés à une ou plusieurs
expressions régulières dans `out/png/` ; exemple : `./mydot a b a+b`.
- `./mygrep <expression régulière> <chaîne à tester>` : détermine si une chaîne est acceptée
par une expression régulière. Dessine l'AFN final dans `out/png/grep.png`.

Remarque : la commande `dot` de Graphviz doit être installée pour que les images soient
créées.

### GNU Make :
- `make` pour générer tous les programmes ;
- `make clean` pour supprimer le dossier `out/` et les exécutables générés.

### Fichiers sources :
- `src/af.[hc]`: constantes partagées par tous les AF, ainsi qu'une fonction pour
initialiser le `dico` d'un AF.
- `src/afd.[hc]`: fonctions pour intéragir avec des AFD.
- `src/afn.[hc]`: fonctions pour intéragir avec des AFN.
- `src/compregex.[hc]`: fonctions pour convertir une expression régulière en un AFN.
- `src/util/misc.[hc]`: fonctions communes d'assertion et de lecture de fichiers.
- `src/util/stack.[hc]`: fonctions pour représenter une pile d'états (ici, des `int`).
- `src/util/vstack.[hc]`: fonctions pour représenter une pile d'AFN (utilisé dans
l'analyse syntaxique).
- `src/util/set.[hc]`: fonctions pour représenter un ensemble trié d'états.
- `src/test.c`, `src/mydot.c`, `src/mygrep.c`: fonctions principales des exécutables du
même nom.

Le code source est intégralement commenté.

### Exemples d'utilisation
```
$ ./mygrep "(0+1)*0" "01101010"
"01101010" est acceptée

$ ./mygrep "(0+1)*0" "1"
"1" est rejetée

$ ./mygrep a* ""
"" est acceptée
```
```
$ ./mydot a b a+b a.b a*
Toutes les expressions régulières ont été dessinées.
```

### Remarque sur les erreurs
La majorité des erreurs devraient être détectées ;
```
$ ./mydot "(a"
(a
  ^

erreur syntaxique: parenthèse fermante attendue
```
```
$ ./mydot "a)bc"
a)bc
 ^^^

erreur syntaxique: caractères en surplus
```
```
$ ./mydot a+
a+
  ^

erreur syntaxique: symbole attendu
```

Le programme se terminera immédiatement sur une erreur et ne libèrera pas la
mémoire qu'il aura alloué afin de simplifier le code ; dans ce contexte, le système
d'exploitation prendra le relai et les fuites sont donc négligeables.
