#include <gtk/gtk.h>     
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

#define BUFFER_SIZE 50000
#define MAX_FAVORIS 100
#define NB_QUESTIONS 10

cJSON *root;
int favoris[MAX_FAVORIS];
int nbFavoris = 0;

//  JSON 
int lireFichier(const char filename[], char buffer[], int maxSize) {
    FILE *f = fopen(filename, "r");
    if (!f) return 0;
    int n = fread(buffer, 1, maxSize - 1, f);
    buffer[n] = '\0';
    fclose(f);
    return 1;
}

// RECHERCHE 
cJSON* rechercherParID(int id) {
    int nb = cJSON_GetArraySize(root);
    for (int i = 0; i < nb; i++) {
        cJSON *hero = cJSON_GetArrayItem(root, i);
        cJSON *hid = cJSON_GetObjectItem(hero, "id");
        if (cJSON_IsNumber(hid) && hid->valueint == id)
            return hero;
    }
    return NULL;
}

cJSON* rechercherParNom(const char *nom) {
    int nb = cJSON_GetArraySize(root);
    for (int i = 0; i < nb; i++) {
        cJSON *hero = cJSON_GetArrayItem(root, i);
        cJSON *name = cJSON_GetObjectItem(hero, "name");
        if (cJSON_IsString(name) && strcasecmp(name->valuestring, nom) == 0)
            return hero;
    }
    return NULL;
}

//  AFFICHAGE 
void set_text(GtkWidget *tv, const char *txt) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));
    gtk_text_buffer_set_text(buffer, txt, -1);
}

//  LISTE 
void afficher_liste(GtkWidget *btn, gpointer data) {
    GtkWidget *tv = data;
    int nb = cJSON_GetArraySize(root);
    char buffer[15000] = "";

    for (int i = 0; i < nb; i++) {
        cJSON *hero = cJSON_GetArrayItem(root, i);
        cJSON *id = cJSON_GetObjectItem(hero, "id");
        cJSON *name = cJSON_GetObjectItem(hero, "name");

        if (cJSON_IsNumber(id) && cJSON_IsString(name)) {
            char ligne[100];
            sprintf(ligne, "%d - %s\n", id->valueint, name->valuestring);
            strcat(buffer, ligne);
        }
    }
    set_text(tv, buffer);
}

//  DETAILS 
void afficher_details(GtkWidget *tv, cJSON *hero) {
    if (!hero) return;

    char buffer[2000] = "";

    cJSON *name = cJSON_GetObjectItem(hero, "name");
    sprintf(buffer, "Nom: %s\n", name->valuestring);

    cJSON *ps = cJSON_GetObjectItem(hero, "powerstats");

    if (cJSON_IsObject(ps)) {
        sprintf(buffer + strlen(buffer),
                "Intelligence: %d\nForce: %d\nVitesse: %d\nDurabilité: %d\nPuissance: %d\nCombat: %d\n",
                cJSON_GetObjectItem(ps,"intelligence")->valueint,
                cJSON_GetObjectItem(ps,"strength")->valueint,
                cJSON_GetObjectItem(ps,"speed")->valueint,
                cJSON_GetObjectItem(ps,"durability")->valueint,
                cJSON_GetObjectItem(ps,"power")->valueint,
                cJSON_GetObjectItem(ps,"combat")->valueint);
    }

    set_text(tv, buffer);
}

//  RECHERCHE NOM 
void chercher_nom(GtkWidget *btn, gpointer data) {
    GtkWidget **w = data;
    GtkWidget *entry = w[0];
    GtkWidget *tv = w[1];

    const char *txt = gtk_entry_get_text(GTK_ENTRY(entry));
    cJSON *hero = rechercherParNom(txt);

    if (!hero) {
        set_text(tv, "Héros non trouvé !");
        return;
    }

    afficher_details(tv, hero);
}

//  RECHERCHE ID 
void chercher_id(GtkWidget *btn, gpointer data) {
    GtkWidget **w = data;
    GtkWidget *entry = w[0];
    GtkWidget *tv = w[1];

    int id = atoi(gtk_entry_get_text(GTK_ENTRY(entry)));
    cJSON *hero = rechercherParID(id);

    if (!hero) {
        set_text(tv, "Héros non trouvé !");
        return;
    }

    afficher_details(tv, hero);
}

//  FAVORIS 
void ajouter_fav(GtkWidget *btn, gpointer entry) {
    int id = atoi(gtk_entry_get_text(GTK_ENTRY(entry)));

    for (int i = 0; i < nbFavoris; i++)
        if (favoris[i] == id) return;

    favoris[nbFavoris++] = id;
}

void afficher_fav(GtkWidget *btn, gpointer tv) {
    char buffer[2000] = "===== FAVORIS =====\n";

    for (int i = 0; i < nbFavoris; i++) {
        cJSON *hero = rechercherParID(favoris[i]);
        if (hero) {
            cJSON *name = cJSON_GetObjectItem(hero, "name");
            strcat(buffer, name->valuestring);
            strcat(buffer, "\n");
        }
    }
    set_text(tv, buffer);
}

//  COMPARAISON 
void comparer(GtkWidget *btn, gpointer data) {
    GtkWidget **w = data;
    int id1 = atoi(gtk_entry_get_text(GTK_ENTRY(w[0])));
    int id2 = atoi(gtk_entry_get_text(GTK_ENTRY(w[1])));
    GtkWidget *tv = w[2];

    cJSON *h1 = rechercherParID(id1);
    cJSON *h2 = rechercherParID(id2);

    if (!h1 || !h2) {
        set_text(tv, "Erreur comparaison");
        return;
    }

    char buffer[2000];
    sprintf(buffer, "Comparaison OK entre %d et %d", id1, id2);

    set_text(tv, buffer);
}

//  QUIZ 
int qIndex = 0, score = 0;

const char *questions[NB_QUESTIONS] = {
    "Kryptonien ?", "Détective ?", "Araignée ?", "Amazone ?", "Armure ?",
    "Soldat ?", "Vert ?", "Tonnerre ?", "Griffes ?", "Espionne ?"
};

const char *rep[NB_QUESTIONS] = {
    "Superman","Batman","Spider-Man","Wonder Woman",
    "Iron Man","Captain America","Hulk","Thor","Wolverine","Black Widow"
};

void valider(GtkWidget *btn, gpointer data) {
    GtkWidget **w = data;
    GtkWidget *label = w[0];
    GtkWidget *entry = w[1];

    const char *r = gtk_entry_get_text(GTK_ENTRY(entry));

    if (strcasecmp(r, rep[qIndex]) == 0) score++;

    qIndex++;
    gtk_entry_set_text(GTK_ENTRY(entry), "");

    if (qIndex < NB_QUESTIONS)
        gtk_label_set_text(GTK_LABEL(label), questions[qIndex]);
    else {
        char res[100];
        sprintf(res, "Score: %d/%d", score, NB_QUESTIONS);
        gtk_label_set_text(GTK_LABEL(label), res);
    }
}

void quiz(GtkWidget *btn, gpointer data) {
    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win), "Quiz");

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(win), box);

    GtkWidget *label = gtk_label_new(questions[0]);
    GtkWidget *entry = gtk_entry_new();
    GtkWidget *val = gtk_button_new_with_label("Valider");

    GtkWidget *w[2] = {label, entry};

    g_signal_connect(val, "clicked", G_CALLBACK(valider), w);

    gtk_box_pack_start(GTK_BOX(box), label, 0,0,5);
    gtk_box_pack_start(GTK_BOX(box), entry, 0,0,5);
    gtk_box_pack_start(GTK_BOX(box), val, 0,0,5);

    gtk_widget_show_all(win);

    qIndex = 0;
    score = 0;
}

//  MAIN 
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    char buffer[BUFFER_SIZE];
    lireFichier("SuperHeros.json", buffer, BUFFER_SIZE);
    root = cJSON_Parse(buffer);

    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win), "Super-Héros");
    gtk_window_set_default_size(GTK_WINDOW(win), 700, 500);
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(win), box);

    GtkWidget *entry = gtk_entry_new();
    GtkWidget *tv = gtk_text_view_new();

    GtkWidget *btn_liste = gtk_button_new_with_label("Liste");
    GtkWidget *btn_nom = gtk_button_new_with_label("Recherche Nom");
    GtkWidget *btn_id = gtk_button_new_with_label("Recherche ID");
    GtkWidget *btn_fav = gtk_button_new_with_label("Voir Favoris");
    GtkWidget *btn_add = gtk_button_new_with_label("Ajouter Favori");
    GtkWidget *btn_quiz = gtk_button_new_with_label("Quiz");

    GtkWidget *data_nom[2] = {entry, tv};
    GtkWidget *data_id[2] = {entry, tv};

    g_signal_connect(btn_liste, "clicked", G_CALLBACK(afficher_liste), tv);
    g_signal_connect(btn_nom, "clicked", G_CALLBACK(chercher_nom), data_nom);
    g_signal_connect(btn_id, "clicked", G_CALLBACK(chercher_id), data_id);
    g_signal_connect(btn_fav, "clicked", G_CALLBACK(afficher_fav), tv);
    g_signal_connect(btn_add, "clicked", G_CALLBACK(ajouter_fav), entry);
    g_signal_connect(btn_quiz, "clicked", G_CALLBACK(quiz), NULL);

    gtk_box_pack_start(GTK_BOX(box), entry, 0,0,5);
    gtk_box_pack_start(GTK_BOX(box), btn_liste, 0,0,5);
    gtk_box_pack_start(GTK_BOX(box), btn_nom, 0,0,5);
    gtk_box_pack_start(GTK_BOX(box), btn_id, 0,0,5);
    gtk_box_pack_start(GTK_BOX(box), btn_add, 0,0,5);
    gtk_box_pack_start(GTK_BOX(box), btn_fav, 0,0,5);
    gtk_box_pack_start(GTK_BOX(box), btn_quiz, 0,0,5);
    gtk_box_pack_start(GTK_BOX(box), tv, 1,1,5);

    gtk_widget_show_all(win);
    gtk_main();

    cJSON_Delete(root);
    return 0;
}
