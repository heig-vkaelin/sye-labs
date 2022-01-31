# SYE 2021 - Laboratoire 06

## Appel système `renice` et gestion des priorités

1. **Lancer l’application « time_loop » en arrière-plan (avec le symbole &) depuis le shell. Que se passe-t-il et pourquoi ?**

    Réponse : Le processus time_loop ne se lance pas immédiatement, il est dans l'état Ready. Comme on le lance en arrière-plan, le processus du shell ne fait pas de waitpid dessus. Le shell ne se met donc jamais en état waiting et continue d'être en état Running. De plus, comme ils ont les mêmes priorités, l'ordonnanceur ne change pas la main, il n'y a pas de changement de contexte.

2. **Changer la priorité du shell pour que le processus « time_loop » deviennent plus prioritaire. Que se passe-t-il et pourquoi ?**

    Réponse : Après avoir lancé time_loop en arrière plan, le processus time_loop se trouve dans l'état Ready et le shell en Running. Le processus time_loop a une priorité plus élevée que celle du processus shell. L'ordonnanceur va donc donner la main au processus time_loop et mettre en attente le processus shell. Le résultat sera l'affichage d'un compteur s'incrémentant chaque seconde.


## Gestion de la mémoire

1. **Dans le noyau, où se trouve le code gérant l'allocation dynamique de la mémoire (gestion du tas) ?**

    Réponse : so3/mm/heap.c

2. **Quel est l'algorithme de gestion mémoire utilisé dans ce contexte ?**

    Réponse : Il s'agit de l'algorithme Quick-fit. On voit dans le fichier du point 1) une liste de structure appelée quick_list. Ce qui correspond bien à la liste dynamique à deux dimensions nécessaire dans l'implémentation de cet algorithme.

3. **Dans le noyau, où se trouve le code de la gestion de la MMU ?**

    Réponse : Son initialisation et configuration se trouvent dans so3/mm/init.c et son implémentation dans so3/arch/arm32/mmu.c

4. **Exécuter plusieurs fois l’application `memory`. Que constate-on au niveau des adresses physique et virtuelle ? Pourquoi ?**

    Réponse : L'adresse virtuelle ne change jamais contrairement à l'adresse physique. La fin de l'adresse physique est cependant toujours la même que la fin de l'adresse virtuelle car il s'agit de l'offset. Lors de la traduction, la MMU traduit le numéro de page en numéro de frame (la partie qui change) mais l'offset de fin reste le même.

    L'addresse virtuelle ne change pas car l'application memory a une pile assignée et celle-ci a une adresse bien définie qui ne change pas non plus. Lors de l'allocation dynamique, la première plage libre de la pile est attribuée et aura donc toujours la même adresse comme la pile n'est pas utilisée avant.

    Pour l'addresse physique, rien ne garantit que la même plage d'adresses sera libre entre chaque exécution. En effet, la mémoire physique est utilisée et partagée par de nombreux programmes. Le _loader_ doit donc trouver un emplacement libre dans la RAM à chaque exécution afin de pouvoir placer l'image binaire.
