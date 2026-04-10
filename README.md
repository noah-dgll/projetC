# ProjetC_superheros

# Description du projet

L’objectif de ce mini-projet était de créer une application en ligne de commande qui permet aux utilisateurs de parcourir des informations sur différents super-héros, de voir leurs statistiques détaillées et de comparer leurs attributs.

Initialement développée en version console, elle a été transformée en application avec interface graphique grâce à GTK.

Cependant, je ne suis pas parvenu à faire fonctionner la bibliothèque GTK, que ce soit sur mon ordinateur ou bien sur ceux de l'université. Ainsi je ne garantie pas le bon fonctionnement de l'application si vous parvenez à la faire tourner.

Je ne parlerais donc par la suite que de la version console dont le code se trouve dans "main.c".

L’utilisateur peut :

* Afficher la liste des super-héros
* Rechercher un héros par ID ou par nom
* Filtrer les héros selon leurs statistiques (force, intelligence, vitesse…)
* Comparer deux héros
* Gérer une liste de favoris (ajout / suppression / sauvegarde)
* Jouer à un quiz de 10 questions avec score final

# Prérequis

Avant d’utiliser le projet, assurez-vous d’avoir installé :

* Un compilateur C (nécessaire mais normalement intégré à VScode)
* GTK 3 et pkg-config (si vous souhaitez faire tourner l'application, ce que je ne suis pas parvenu à faire)

# Compilation

Dans le terminal du "main.c", entrez :

gcc main.c cJSON.c -o superheros

# Exécution
Une fois le code compilé, entrez :

./superheros

# utilisation

Entrez simplement les chiffres qui vous intéressent en vous laissant guider par le menu affiché. Amusez-vous à explorer toutes les fonctionnalités !

# Structure du projet

* "main.c" -> code principal de l’application
* "SuperHeros.json" -> base de données des héros
* "favoris.txt" -> fichier contenant les favoris
* "README.md" -> documentation du projet
* "cJSON.c" et "cJSON.h" -> 
* "appGTK.c" -> code de l'application, non fonctionnel pour moi donc

# Outils et technologies utilisés

* Langage C
* Application avec nterface graphique : GTK
* Parsing JSON : cJSON
* Environnement : VSCode

