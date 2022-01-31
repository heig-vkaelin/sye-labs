# SYE 2021 - Laboratoire 02

## Explication du cheminement pour l'ajout d'un appel système dans SO3

### Liste des fichiers modifiés:

1. so3/arch/arm32/include/asm/syscall.h
2. so3/include/process.h
3. so3/kernel/process.c
4. so3/kernel/syscalls.c
5. usr/lib/libc/include/syscall.h
6. usr/lib/libc/crt0.S 

On ajoute tout d'abord dans le fichier **1.** une constante permettant de donner une valeure numérique à l'appel système que l'on souhaite ajouter. Cette valeur servira à identifier l'appel système dans toute la codebase Kernel et user.

Cette valeur est ensuite utilisée dans un switch case dans le fichier **4.** qui lui va appeler la fonction définie dans le fichier **3.**. Cette fonction contient le déroulement de l'appel système. Il faut donc rajouter un case dans le switch avec la nouvelle constante définie qui appelle la fonction créée.

Dans le fichier **2.** on ajoute simplement l'entête de cette fonction afin de pouvoir l'utiliser dans d'autres fichiers.

Pour le côté user, il faut ajouter le même numéro à l'appel système dans le fichier **5.** que dans le fichier **1.**. Autrement on ne pourra pas dire au Kernel quel appel système lancer.

Finalement, on ajoute une valeur à la macro ``SYSCALLSTUB`` dans le code assembleur du fichier **6.** qui vaut toujours ce même numéro. Cette macro stockera le numéro dans un registre afin de la sauvegarder.

Cela permettra d'avoir la bonne valeur de l'appel système lorsque l'utilisateur lancera l'application. Cette valeur sera celle utilisée dans le switch case du fichier **4.** dans le Kernel.
