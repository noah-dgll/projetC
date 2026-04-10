#include <stdio.h>   
#include <stdlib.h>  
#include <string.h> 
#include "cJSON.h"   // Bibliothèque pour manipuler du JSON en C

// on définit d'abord la taille maximale du fichier JSON que l'on peut lire
#define BUFFER_SIZE 50000
//pour la gestion des favoris
#define MAX_FAVORIS 100
int favoris[MAX_FAVORIS];
int nbFavoris = 0;
//pour le quiz
#define NB_QUESTIONS 10

// Fonction qui lit un fichier et met son contenu dans un buffer
int lireFichier(const char filename[], char buffer[], int maxSize) {
    FILE *f = fopen(filename, "r");                                 // Ouvre le fichier en mode lecture 
    if (f == NULL) return 0;                                        // Si le fichier n'existe pas ou ne s'ouvre pas -> erreur
    int n = fread(buffer, 1, maxSize - 1, f);                       // Lit le contenu du fichier dans le buffer
    buffer[n] = '\0';
    fclose(f);
    return 1;
}

// Fonction pour rechercher un héros par son ID
cJSON* rechercherParID(cJSON *root, int idRecherche) {
    int nbHeros = cJSON_GetArraySize(root);

    for (int i = 0; i < nbHeros; i++) {
        cJSON *hero = cJSON_GetArrayItem(root, i);
        cJSON *id = cJSON_GetObjectItemCaseSensitive(hero, "id");
        //si l'id du héros correspond à celui recherché par l'utilisateur
        if (cJSON_IsNumber(id) && id->valueint == idRecherche) {
            return hero;    //héros trouvé ! On arrête la recherche
        }
    }
    return NULL;
}

// Fonction pour rechercher un héros par son nom
cJSON* rechercherParNom(cJSON *root, const char *nomRecherche) {
    int nbHeros = cJSON_GetArraySize(root);
    for (int i = 0; i < nbHeros; i++) {
        cJSON *hero = cJSON_GetArrayItem(root, i);
        cJSON *name = cJSON_GetObjectItemCaseSensitive(hero, "name");

        // Vérifie que c'est bien une chaîne
        if (cJSON_IsString(name)) {
            // Compare le nom du héros avec celui recherché
            if (strcasecmp(name->valuestring, nomRecherche) == 0) { //ne fait pas attention aux majuscules (donc pratique)
                return hero;  // trouvé 
            }
        }
    }
    return NULL;  // pas trouvé
}

//fonctions pour gérer les favoris :

// fonction qui remplit le tableau favoris[] avec les ID des héros présents dans le fichier
void chargerFavoris(int favoris[], int *nbFavoris) {
    // Ouvre le fichier en lecture
    FILE *f = fopen("favoris.txt", "r");
    if (f == NULL) return;
    // Tant qu'on peut lire un entier dans le fichier
    while (fscanf(f, "%d", &favoris[*nbFavoris]) == 1) {  
        (*nbFavoris)++;
    }
    fclose(f);
}

// Fonction qui sauvegarde les favoris dans un fichier texte
// Cette version écrit les IDs + noms des héros pour une lecture claire
void sauvegarderFavoris(cJSON *root, int favoris[], int nbFavoris) {
    // On ouvre le fichier en écriture (écrase l'ancien contenu)
    FILE *f = fopen("favoris.txt", "w");
    if (f == NULL) {
        printf("Erreur ouverture fichier favoris\n");
        return;
    }

    // Titre dans le fichier
    fprintf(f, "===== FAVORIS =====\n\n");

    // Parcours de tous les favoris
    for (int i = 0; i < nbFavoris; i++) {
        cJSON *hero = rechercherParID(root, favoris[i]);
        if (hero != NULL) {
            cJSON *name = cJSON_GetObjectItemCaseSensitive(hero, "name");
            // Écriture formatée dans le fichier
            fprintf(f, "%d - %s\n",favoris[i],cJSON_IsString(name) ? name->valuestring : "Inconnu");
        }
    }
    fclose(f);
    printf("Favoris sauvegardés dans favoris.txt\n");
}

void ajouterFavori(int favoris[], int *nbFavoris, int id) {
    // Vérifie si déjà présent
    for (int i = 0; i < *nbFavoris; i++) {
        if (favoris[i] == id) {
            printf("Déjà dans les favoris !\n");
            return;
        }
    }
    favoris[*nbFavoris] = id;
    (*nbFavoris)++;
    printf("Ajouté aux favoris !\n");
}

void retirerFavori(int favoris[], int *nbFavoris, int id) {
    for (int i = 0; i < *nbFavoris; i++) {
        if (favoris[i] == id) {
            // Décale les éléments
            for (int j = i; j < *nbFavoris - 1; j++) {
                favoris[j] = favoris[j + 1];
            }
            (*nbFavoris)--;
            printf("Retiré des favoris !\n");
            return;
        }
    }
    printf("Héros non présent dans les favoris\n");
}

//quizzzzzzzzz
void lancerQuiz() {
    // Tableau des questions
    const char *questions[NB_QUESTIONS] = {
        "Quel héros (masculin) est Kryptonien ?",
        "Quel héros est connu pour être un détective masqué ?",
        "Quel héros tient ses pouvoirs d'une piqûre d'arraignée ?",
        "Quelle héroïne est une Amazone ?",
        "Quel héros porte une armure haute technologie ?",
        "Quel héros est un super soldat américain ?",
        "Quel héros devient vert et très puissant en colère ?",
        "Quel est le héros provenant d'Asgard et entre autres dieu du tonnerre ?",
        "Quel héros mutant possède des griffes en adamantium ?",
        "Quelle héroïne est une espionne russe ?"
    };

    // Tableau des réponses
    const char *reponses[NB_QUESTIONS] = {
        "Superman",
        "Batman",
        "Spider-Man",
        "Wonder Woman",
        "Iron Man",
        "Captain America",
        "Hulk",
        "Thor",
        "Wolverine",
        "Black Widow"
    };

    int score = 0;
    char reponseUser[50];

    printf("\n===== QUIZ SUPER-HEROS =====\n");

    // Boucle sur les 10 questions
    for (int i = 0; i < NB_QUESTIONS; i++) {
        printf("\nQuestion %d :\n", i + 1);
        printf("%s\n", questions[i]);

        printf("Votre réponse : ");
        scanf(" %[^\n]", reponseUser);

        // Comparaison (insensible à la casse)
        if (strcasecmp(reponseUser, reponses[i]) == 0) {    //ne ce soucie pas des majuscules
            printf("Bonne réponse !\n");
            score++;
        } else {
            printf("Faux ! Réponse attendue : %s\n", reponses[i]);
        }
    }
    // Score final
    printf("\n===== RESULTAT =====\n");
    printf("Score : %d / %d\n", score, NB_QUESTIONS);
}

int main() {
    // On déclare un grand tableau de caractères pour stocker le JSON
    char buffer[BUFFER_SIZE];

    if (!lireFichier("SuperHeros.json", buffer, BUFFER_SIZE)) {
        printf("Erreur lecture fichier\n");
        return 1;                                                   // Stoppe le programme en cas d'erreur
    }

    // On transforme le texte JSON en structure manipulable en C
    cJSON *root = cJSON_Parse(buffer);
    //pour les favoris
    chargerFavoris(favoris, &nbFavoris);

    // On vérifie que le parsing a réussi ET que c'est bien un tableau
    if (root == NULL || !cJSON_IsArray(root)) {
        printf("Erreur parsing JSON\n");
        return 1;
    }

    // On récupère le nombre d'éléments dans le tableau (nombre de héros)
    int nbHeros = cJSON_GetArraySize(root);

    //MENU PRINCIPAL
    int choix_menu;
    do {
        printf("\n===== MENU =====\n");
        printf("1 - Voir la liste\n");
        printf("2 - Rechercher par ID\n");
        printf("3 - Rechercher par nom\n");
        printf("4 - Filtrer\n");
        printf("5 - Comparer 2 héros\n");
        printf("6 - Ajouter aux favoris\n");
        printf("7 - Retirer des favoris\n");
        printf("8 - Quiz!\n");
        printf("9 - Plus d'infos sur les héros\n");
        printf("0 - Quitter\n");

        scanf("%d", &choix_menu);

        switch (choix_menu) {
            case 1:
                printf("\n===== LISTE DES SUPER-HEROS =====\n\n");

                for (int i = 0; i < nbHeros; i++) {
                    cJSON *hero = cJSON_GetArrayItem(root, i);
                    cJSON *id = cJSON_GetObjectItemCaseSensitive(hero, "id");
                    cJSON *name = cJSON_GetObjectItemCaseSensitive(hero, "name");

                    // On affiche ID + nom pour chaque héros
                    if (cJSON_IsNumber(id) && cJSON_IsString(name)) {
                    printf("%d - %s\n", id->valueint, name->valuestring);
                    }
                }
                break;

            case 2:
                int choix_heros;
                printf("\nEntrez l'ID du héros : ");
                scanf("%d", &choix_heros);

                // RECHERCHE DU HEROS 
                cJSON *hero_cherche = rechercherParID(root, choix_heros);

                if (hero_cherche == NULL) {
                    printf("Héros non trouvé !\n");
                    break;
                }
                printf("\n===== DETAILS DU HEROS =====\n\n");

                // Nom
                cJSON *name = cJSON_GetObjectItemCaseSensitive(hero_cherche, "name");
                printf("Nom: %s\n", cJSON_IsString(name) ? name->valuestring : "Inconnu");

                // POWERSTATS 
                cJSON *powerstats = cJSON_GetObjectItemCaseSensitive(hero_cherche, "powerstats");

                if (cJSON_IsObject(powerstats)) {

                    printf("\n--- Powerstats ---\n");

                    cJSON *intel = cJSON_GetObjectItemCaseSensitive(powerstats, "intelligence");
                    cJSON *strength = cJSON_GetObjectItemCaseSensitive(powerstats, "strength");
                    cJSON *speed = cJSON_GetObjectItemCaseSensitive(powerstats, "speed");
                    cJSON *durability = cJSON_GetObjectItemCaseSensitive(powerstats, "durability");
                    cJSON *power = cJSON_GetObjectItemCaseSensitive(powerstats, "power");
                    cJSON *combat = cJSON_GetObjectItemCaseSensitive(powerstats, "combat");

                    printf("Intelligence: %d\n", cJSON_IsNumber(intel) ? intel->valueint : 0);
                    printf("Force: %d\n", cJSON_IsNumber(strength) ? strength->valueint : 0);
                    printf("Vitesse: %d\n", cJSON_IsNumber(speed) ? speed->valueint : 0);
                    printf("Durabilité: %d\n", cJSON_IsNumber(durability) ? durability->valueint : 0);
                    printf("Puissance: %d\n", cJSON_IsNumber(power) ? power->valueint : 0);
                    printf("Combat: %d\n", cJSON_IsNumber(combat) ? combat->valueint : 0);
                }

                // APPARENCE 
                cJSON *appearance = cJSON_GetObjectItemCaseSensitive(hero_cherche, "appearance");

                if (cJSON_IsObject(appearance)) {

                    printf("\n--- Apparence ---\n");

                    cJSON *gender = cJSON_GetObjectItemCaseSensitive(appearance, "gender");
                    cJSON *race = cJSON_GetObjectItemCaseSensitive(appearance, "race");

                    printf("Genre: %s\n", cJSON_IsString(gender) ? gender->valuestring : "Inconnu");
                    printf("Race: %s\n", cJSON_IsString(race) ? race->valuestring : "Inconnu");

                    // TAILLE (Tableau) 
                    cJSON *height = cJSON_GetObjectItemCaseSensitive(appearance, "height");

                    if (cJSON_IsArray(height) && cJSON_GetArraySize(height) >= 2) {

                        cJSON *h1 = cJSON_GetArrayItem(height, 0); // taille en pieds (anglais)
                        cJSON *h2 = cJSON_GetArrayItem(height, 1); // taille en cm

                        printf("Taille: %s (%s)\n",
                            cJSON_IsString(h1) ? h1->valuestring : "?",
                            cJSON_IsString(h2) ? h2->valuestring : "?");
                    }

                    // POIDS (TABLEAU)
                    cJSON *weight = cJSON_GetObjectItemCaseSensitive(appearance, "weight");

                    if (cJSON_IsArray(weight) && cJSON_GetArraySize(weight) >= 2) {
                        cJSON *w1 = cJSON_GetArrayItem(weight, 0); //poids en livres (anglais)
                        cJSON *w2 = cJSON_GetArrayItem(weight, 1); //poids en kg

                        printf("Poids: %s (%s)\n",
                            cJSON_IsString(w1) ? w1->valuestring : "?",
                            cJSON_IsString(w2) ? w2->valuestring : "?");
                    }

                    // COULEURS 
                    cJSON *eye = cJSON_GetObjectItemCaseSensitive(appearance, "eyeColor");
                    cJSON *hair = cJSON_GetObjectItemCaseSensitive(appearance, "hairColor");

                    printf("Couleur des yeux: %s\n", cJSON_IsString(eye) ? eye->valuestring : "Inconnu");
                    printf("Couleur des cheveux: %s\n", cJSON_IsString(hair) ? hair->valuestring : "Inconnu");
                }
                break;

            case 3:
                char nomRecherche[50];

                printf("\nEntrez le nom du héros : ");
                scanf(" %[^\n]", nomRecherche);  // lit une ligne avec espaces

                cJSON *hero_trouve = rechercherParNom(root, nomRecherche);

                if (hero_trouve == NULL) {
                    printf("Héros non trouvé !\n");
                } else {
                    printf("\n===== DETAILS DU HEROS =====\n\n");

                    // Nom
                    cJSON *name = cJSON_GetObjectItemCaseSensitive(hero_trouve, "name");
                    printf("Nom: %s\n", cJSON_IsString(name) ? name->valuestring : "Inconnu");

                    // POWERSTATS 
                    cJSON *powerstats = cJSON_GetObjectItemCaseSensitive(hero_trouve, "powerstats");

                    if (cJSON_IsObject(powerstats)) {

                        printf("\n--- Powerstats ---\n");

                        cJSON *intel = cJSON_GetObjectItemCaseSensitive(powerstats, "intelligence");
                        cJSON *strength = cJSON_GetObjectItemCaseSensitive(powerstats, "strength");
                        cJSON *speed = cJSON_GetObjectItemCaseSensitive(powerstats, "speed");
                        cJSON *durability = cJSON_GetObjectItemCaseSensitive(powerstats, "durability");
                        cJSON *power = cJSON_GetObjectItemCaseSensitive(powerstats, "power");
                        cJSON *combat = cJSON_GetObjectItemCaseSensitive(powerstats, "combat");

                        printf("Intelligence: %d\n", cJSON_IsNumber(intel) ? intel->valueint : 0);
                        printf("Force: %d\n", cJSON_IsNumber(strength) ? strength->valueint : 0);
                        printf("Vitesse: %d\n", cJSON_IsNumber(speed) ? speed->valueint : 0);
                        printf("Durabilité: %d\n", cJSON_IsNumber(durability) ? durability->valueint : 0);
                        printf("Puissance: %d\n", cJSON_IsNumber(power) ? power->valueint : 0);
                        printf("Combat: %d\n", cJSON_IsNumber(combat) ? combat->valueint : 0);
                    }

                    // APPARENCE 
                    cJSON *appearance = cJSON_GetObjectItemCaseSensitive(hero_trouve, "appearance");

                    if (cJSON_IsObject(appearance)) {

                        printf("\n--- Apparence ---\n");

                        cJSON *gender = cJSON_GetObjectItemCaseSensitive(appearance, "gender");
                        cJSON *race = cJSON_GetObjectItemCaseSensitive(appearance, "race");

                        printf("Genre: %s\n", cJSON_IsString(gender) ? gender->valuestring : "Inconnu");
                        printf("Race: %s\n", cJSON_IsString(race) ? race->valuestring : "Inconnu");

                        // TAILLE (Tableau) 
                        cJSON *height = cJSON_GetObjectItemCaseSensitive(appearance, "height");

                        if (cJSON_IsArray(height) && cJSON_GetArraySize(height) >= 2) {

                            cJSON *h1 = cJSON_GetArrayItem(height, 0); // taille en pieds (anglais)
                            cJSON *h2 = cJSON_GetArrayItem(height, 1); // taille en cm

                            printf("Taille: %s (%s)\n",
                                cJSON_IsString(h1) ? h1->valuestring : "?",
                                cJSON_IsString(h2) ? h2->valuestring : "?");
                        }

                        // POIDS (TABLEAU)
                        cJSON *weight = cJSON_GetObjectItemCaseSensitive(appearance, "weight");

                        if (cJSON_IsArray(weight) && cJSON_GetArraySize(weight) >= 2) {
                            cJSON *w1 = cJSON_GetArrayItem(weight, 0); //poids en livres (anglais)
                            cJSON *w2 = cJSON_GetArrayItem(weight, 1); //poids en kg

                            printf("Poids: %s (%s)\n",
                                cJSON_IsString(w1) ? w1->valuestring : "?",
                                cJSON_IsString(w2) ? w2->valuestring : "?");
                        }

                        // COULEURS 
                        cJSON *eye = cJSON_GetObjectItemCaseSensitive(appearance, "eyeColor");
                        cJSON *hair = cJSON_GetObjectItemCaseSensitive(appearance, "hairColor");

                        printf("Couleur des yeux: %s\n", cJSON_IsString(eye) ? eye->valuestring : "Inconnu");
                        printf("Couleur des cheveux: %s\n", cJSON_IsString(hair) ? hair->valuestring : "Inconnu");
                    }
                }
                break;
            case 4:
                int choix_filtre;
                typedef struct {
                    char nom[50];
                    int force;                    
                    int intelligence;
                    int vitesse;
                    int durabilite;
                    int puissance;
                    int combat;
                } Hero;
                Hero heroes[100];   // tableau de héros contenent leur nom et leurs powerstats respectives
                

                do{
                    int count = 0;
                    
                    printf("\n===== FILTRES =====\n");
                    printf("1 - Filtrer par force brute\n");
                    printf("2 - Filtrer par intelligence\n");
                    printf("3 - Filtrer par vitesse\n");
                    printf("4 - Filtrer par durabilité\n");
                    printf("5 - Filtrer par puissance\n");                    
                    printf("6 - Filtrer par talent en combat\n");
                    printf("0 - Retour au menu\n");

                    scanf("%d", &choix_filtre);
                    switch (choix_filtre) {
                        case 1: //par force
                            //on récupère le nom et la force de nos héros
                            for (int i = 0; i < nbHeros; i++) {
                                cJSON *hero = cJSON_GetArrayItem(root, i);
                                cJSON *nom = cJSON_GetObjectItemCaseSensitive(hero, "name");
                                cJSON *powerstats = cJSON_GetObjectItemCaseSensitive(hero, "powerstats");

                                if (cJSON_IsString(nom) && cJSON_IsObject(powerstats)) {
                                    cJSON *force = cJSON_GetObjectItemCaseSensitive(powerstats, "strength");
                
                                    if (cJSON_IsNumber(force)) {
                                        strcpy(heroes[count].nom, nom->valuestring);
                                        heroes[count].force = force->valueint;
                                        count++;
                                    }
                                }
                            }   
                            //on trie les héros par force décroissante
                            for (int i = 0; i < count - 1; i++) {
                                for (int j = 0; j < count - i - 1; j++) {
                                    // Si le héros actuel est moins fort que le suivant -> on échange
                                    if (heroes[j].force < heroes[j + 1].force) {
                                        Hero temp = heroes[j];
                                        heroes[j] = heroes[j + 1];
                                        heroes[j + 1] = temp;
                                    }
                               }
                            }
                            //affichage
                            printf("\n===== HEROS PAR FORCE (DECROISSANT) =====\n\n");

                            for (int i = 0; i < count; i++) {
                                printf("%s - Force: %d\n", heroes[i].nom, heroes[i].force);
                            }
                            break;
                        case 2://tri par intelligence
                            //on récupère le nom et l'intelligence de nos héros
                            for (int i = 0; i < nbHeros; i++) {
                                cJSON *hero = cJSON_GetArrayItem(root, i);
                                cJSON *nom = cJSON_GetObjectItemCaseSensitive(hero, "name");
                                cJSON *powerstats = cJSON_GetObjectItemCaseSensitive(hero, "powerstats");

                                if (cJSON_IsString(nom) && cJSON_IsObject(powerstats)) {
                                    cJSON *intelligence = cJSON_GetObjectItemCaseSensitive(powerstats, "intelligence");
                    
                                    if (cJSON_IsNumber(intelligence)) {
                                        strcpy(heroes[count].nom, nom->valuestring);
                                        heroes[count].intelligence = intelligence->valueint;
                                        count++;
                                    }
                                }
                            }
                            //on trie les héros par intelligence décroissante
                            for (int i = 0; i < count - 1; i++) {
                                for (int j = 0; j < count - i - 1; j++) {
                                    // Si le héros actuel est moins intelligent que le suivant -> on échange
                                    if (heroes[j].intelligence < heroes[j + 1].intelligence) {
                                        Hero temp = heroes[j];
                                        heroes[j] = heroes[j + 1];
                                        heroes[j + 1] = temp;
                                    }
                                }
                            }
                            //affichage
                            printf("\n===== HEROS PAR INTELLIGENCE (DECROISSANTE) =====\n\n");

                            for (int i = 0; i < count; i++) {
                                printf("%s - Intelligence: %d\n", heroes[i].nom, heroes[i].intelligence);
                            }
                            break;
                        case 3://tri par vitesse
                            //on récupère le nom et la vitesse de nos héros
                            for (int i = 0; i < nbHeros; i++) {
                                cJSON *hero = cJSON_GetArrayItem(root, i);
                                cJSON *nom = cJSON_GetObjectItemCaseSensitive(hero, "name");
                                cJSON *powerstats = cJSON_GetObjectItemCaseSensitive(hero, "powerstats");

                                if (cJSON_IsString(nom) && cJSON_IsObject(powerstats)) {
                                    cJSON *vitesse = cJSON_GetObjectItemCaseSensitive(powerstats, "speed");
                    
                                    if (cJSON_IsNumber(vitesse)) {
                                        strcpy(heroes[count].nom, nom->valuestring);
                                        heroes[count].vitesse = vitesse->valueint;
                                        count++;
                                    }
                                }
                            }
                            //on trie les héros par vitesse décroissante
                            for (int i = 0; i < count - 1; i++) {
                                for (int j = 0; j < count - i - 1; j++) {
                                    // Si le héros actuel est moins rapide que le suivant -> on échange
                                    if (heroes[j].vitesse < heroes[j + 1].vitesse) {
                                        Hero temp = heroes[j];
                                        heroes[j] = heroes[j + 1];
                                        heroes[j + 1] = temp;
                                    }
                                }
                            }
                            //affichage
                            printf("\n===== HEROS PAR VITESSE (DECROISSANTE) =====\n\n");

                            for (int i = 0; i < count; i++) {
                                printf("%s - Vitesse: %d\n", heroes[i].nom, heroes[i].vitesse);
                            }
                            break;
                        case 4://tri par durabilité
                            //on récupère le nom et la durabilité de nos héros
                            for (int i = 0; i < nbHeros; i++) {
                                cJSON *hero = cJSON_GetArrayItem(root, i);
                                cJSON *nom = cJSON_GetObjectItemCaseSensitive(hero, "name");
                                cJSON *powerstats = cJSON_GetObjectItemCaseSensitive(hero, "powerstats");

                                if (cJSON_IsString(nom) && cJSON_IsObject(powerstats)) {
                                    cJSON *durabilite = cJSON_GetObjectItemCaseSensitive(powerstats, "durability");
                    
                                    if (cJSON_IsNumber(durabilite)) {
                                        strcpy(heroes[count].nom, nom->valuestring);
                                        heroes[count].durabilite = durabilite->valueint;
                                        count++;
                                    }
                                }
                            }
                            //on trie les héros par durabilité décroissante
                            for (int i = 0; i < count - 1; i++) {
                                for (int j = 0; j < count - i - 1; j++) {
                                    // Si le héros actuel est moins durable que le suivant -> on échange
                                    if (heroes[j].durabilite < heroes[j + 1].durabilite) {
                                        Hero temp = heroes[j];
                                        heroes[j] = heroes[j + 1];
                                        heroes[j + 1] = temp;
                                    }
                                }
                            }
                            //affichage
                            printf("\n===== HEROS PAR DURABILITE (DECROISSANTE) =====\n\n");

                            for (int i = 0; i < count; i++) {
                                printf("%s - Durabilité: %d\n", heroes[i].nom, heroes[i].durabilite);
                            }
                            break;
                        case 5://tri par puissance
                            //on récupère le nom et la puissance de nos héros
                            for (int i = 0; i < nbHeros; i++) {
                                cJSON *hero = cJSON_GetArrayItem(root, i);
                                cJSON *nom = cJSON_GetObjectItemCaseSensitive(hero, "name");
                                cJSON *powerstats = cJSON_GetObjectItemCaseSensitive(hero, "powerstats");

                                if (cJSON_IsString(nom) && cJSON_IsObject(powerstats)) {
                                    cJSON *puissance = cJSON_GetObjectItemCaseSensitive(powerstats, "power");
                
                                    if (cJSON_IsNumber(puissance)) {
                                        strcpy(heroes[count].nom, nom->valuestring);
                                        heroes[count].puissance = puissance->valueint;
                                        count++;
                                    }
                                }
                            }
                            //on trie les héros par puissance décroissante
                            for (int i = 0; i < count - 1; i++) {
                                for (int j = 0; j < count - i - 1; j++) {
                                    // Si le héros actuel est moins puissant que le suivant -> on échange
                                    if (heroes[j].puissance < heroes[j + 1].puissance) {
                                        Hero temp = heroes[j];
                                        heroes[j] = heroes[j + 1];
                                        heroes[j + 1] = temp;
                                    }
                                }
                            }
                            //affichage
                            printf("\n===== HEROS PAR PUISSANCE (DECROISSANTE) =====\n\n");

                            for (int i = 0; i < count; i++) {
                                printf("%s - Puissance: %d\n", heroes[i].nom, heroes[i].puissance);
                            }
                            break;
                        case 6://tri par talent en combat
                            //on récupère le nom et le talent en combat de nos héros
                            for (int i = 0; i < nbHeros; i++) {
                                cJSON *hero = cJSON_GetArrayItem(root, i);
                                cJSON *nom = cJSON_GetObjectItemCaseSensitive(hero, "name");
                                cJSON *powerstats = cJSON_GetObjectItemCaseSensitive(hero, "powerstats");

                                if (cJSON_IsString(nom) && cJSON_IsObject(powerstats)) {
                                    cJSON *combat = cJSON_GetObjectItemCaseSensitive(powerstats, "combat");
                    
                                    if (cJSON_IsNumber(combat)) {
                                        strcpy(heroes[count].nom, nom->valuestring);
                                        heroes[count].combat = combat->valueint;
                                        count++;
                                    }
                                }
                            }
                            //on trie les héros par talent en combat décroissant
                            for (int i = 0; i < count - 1; i++) {
                                for (int j = 0; j < count - i - 1; j++) {
                                    // Si le héros actuel est moins combattant que le suivant -> on échange
                                    if (heroes[j].combat < heroes[j + 1].combat) {
                                        Hero temp = heroes[j];
                                        heroes[j] = heroes[j + 1];
                                        heroes[j + 1] = temp;
                                    }   
                                }
                            }
                            //affichage
                            printf("\n===== HEROS PAR TALENT EN COMBAT (DECROISSANT) =====\n\n");

                            for (int i = 0; i < count; i++) {
                                printf("%s - combat: %d\n", heroes[i].nom, heroes[i].combat);
                            }
                            break;
                        }
                    } while (choix_filtre != 0);
                break;
            case 5: 
                int id1, id2;
                // Demande les deux IDs
                printf("\nEntrez l'ID du premier héros : ");
                scanf("%d", &id1);

                printf("Entrez l'ID du deuxième héros : ");
                scanf("%d", &id2);

                // Recherche des héros
                cJSON *hero1 = rechercherParID(root, id1);
                cJSON *hero2 = rechercherParID(root, id2);

                // Vérification
                if (hero1 == NULL || hero2 == NULL) {
                    printf("Un des héros est introuvable !\n");
                    break;
                }
                // Récupération des noms
                cJSON *name1 = cJSON_GetObjectItemCaseSensitive(hero1, "name");
                cJSON *name2 = cJSON_GetObjectItemCaseSensitive(hero2, "name");

                // Récupération des powerstats
                cJSON *ps1 = cJSON_GetObjectItemCaseSensitive(hero1, "powerstats");
                cJSON *ps2 = cJSON_GetObjectItemCaseSensitive(hero2, "powerstats");

                if (!cJSON_IsObject(ps1) || !cJSON_IsObject(ps2)) {
                    printf("Impossible de comparer (stats manquantes)\n");
                    break;
                }

                printf("\n===== COMPARAISON =====\n\n");
                // Récupération des stats
                cJSON *intel1 = cJSON_GetObjectItemCaseSensitive(ps1, "intelligence");
                cJSON *intel2 = cJSON_GetObjectItemCaseSensitive(ps2, "intelligence");

                cJSON *str1 = cJSON_GetObjectItemCaseSensitive(ps1, "strength");
                cJSON *str2 = cJSON_GetObjectItemCaseSensitive(ps2, "strength");

                cJSON *spd1 = cJSON_GetObjectItemCaseSensitive(ps1, "speed");
                cJSON *spd2 = cJSON_GetObjectItemCaseSensitive(ps2, "speed");

                cJSON *dur1 = cJSON_GetObjectItemCaseSensitive(ps1, "durability");
                cJSON *dur2 = cJSON_GetObjectItemCaseSensitive(ps2, "durability");

                cJSON *pow1 = cJSON_GetObjectItemCaseSensitive(ps1, "power");
                cJSON *pow2 = cJSON_GetObjectItemCaseSensitive(ps2, "power");

                cJSON *cmb1 = cJSON_GetObjectItemCaseSensitive(ps1, "combat");
                cJSON *cmb2 = cJSON_GetObjectItemCaseSensitive(ps2, "combat");

                // Affichage simple
                printf("%s vs %s\n\n",
                    cJSON_IsString(name1) ? name1->valuestring : "Hero1",
                    cJSON_IsString(name2) ? name2->valuestring : "Hero2");

                printf("Intelligence : %d vs %d\n",
                    cJSON_IsNumber(intel1) ? intel1->valueint : 0,
                    cJSON_IsNumber(intel2) ? intel2->valueint : 0);

                printf("Force        : %d vs %d\n",
                    cJSON_IsNumber(str1) ? str1->valueint : 0,
                    cJSON_IsNumber(str2) ? str2->valueint : 0);

                printf("Vitesse      : %d vs %d\n",
                    cJSON_IsNumber(spd1) ? spd1->valueint : 0,
                    cJSON_IsNumber(spd2) ? spd2->valueint : 0);

                printf("Durabilité   : %d vs %d\n",
                    cJSON_IsNumber(dur1) ? dur1->valueint : 0,
                    cJSON_IsNumber(dur2) ? dur2->valueint : 0);

                printf("Puissance    : %d vs %d\n",
                    cJSON_IsNumber(pow1) ? pow1->valueint : 0,
                    cJSON_IsNumber(pow2) ? pow2->valueint : 0);

                printf("Combat       : %d vs %d\n",
                    cJSON_IsNumber(cmb1) ? cmb1->valueint : 0,
                    cJSON_IsNumber(cmb2) ? cmb2->valueint : 0);
                break;
            case 6:
                int id_fav;
                printf("ID à ajouter : ");
                scanf("%d", &id_fav);

                ajouterFavori(favoris, &nbFavoris, id_fav);
                sauvegarderFavoris(root, favoris, nbFavoris);           
                break;
            case 7:
                int id_plusFav;
                printf("ID à retirer : ");
                scanf("%d", &id_plusFav);

                retirerFavori(favoris, &nbFavoris, id_plusFav);
                sauvegarderFavoris(root, favoris, nbFavoris);
                break;
            case 8:
                lancerQuiz();
                break;
            case 9:
                    printf("\nhttps://fr.wikipedia.org/wiki/Liste_de_super-héros_au_cinéma\n");
                    printf("https://www.imdb.com/fr/\n");
        }
    } while (choix_menu != 0);
    
    // Libère la mémoire utilisée par cJSON 
    cJSON_Delete(root);

    return 0;  
}
