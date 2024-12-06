#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure pour stocker les informations extraites
// “word [01 (Catégorie)] [02 Traduction]”

typedef struct {
    char* word;
    char* cat;
    char* translation;
} Entry;


// Fonction pour extraire les informations d'une ligne
Entry extract_info(char* line) {
    Entry entry;
    /* NOTE: La methode d'extraction de donnee ne differe que dans les parametres,
    apres la premiere extraction je ne vais commenter que les changements*/

    // Premiere methode: Trouver word. Première guillemet pour commencer
    char* start_word = strchr(line, '"');
    if (start_word != NULL) {
        char* end_word = strchr(start_word , '['); // fin du mot avec '['

        if (end_word != NULL) {
            size_t word_length = end_word - start_word - 2; // on reduit la taille de 2 pour supprimer [ et tab ou espace
            entry.word = malloc(word_length + 1); //allocation memoire
            strncpy(entry.word, start_word + 1, word_length); // copie de la string
            entry.word[word_length] = '\0'; //rajouter fin de charactere 
        }
    }


    // 2 eme extraction: Extraire la partie categorie (optionel)
    char* cat_start = strstr(line, "[01 ("); // cette fois le debut est sur [01(]
    if (cat_start != NULL) {
        cat_start += 5;  // On saute pour arrive au mot a l'interieur des parentheses
        char* cat_end = strchr(cat_start, ')'); //fin de mot a ')'
        if (cat_end != NULL) {
            size_t cat_length = cat_end - cat_start;
            entry.cat = malloc(cat_length + 1);
            strncpy(entry.cat, cat_start, cat_length);
            entry.cat[cat_length] = '\0';
            }
        } else {
            entry.cat = strdup("Pas de categorie pour ce mot"); //cas si pas de categorie pour le mot
    }

    // 3 eme extraction: Extraire la traduction
    char* translation_start = strstr(line, "[02 "); // cette fois le debut est sur [02 ]
    if (translation_start != NULL) {
        translation_start += 4;  // Saut pour passer à la première lettre de la traduction
        char* translation_end = strchr(translation_start, '"'); //fin de mot a '"'
        
        if (translation_end != NULL) {
            size_t translation_length = translation_end - translation_start;
            entry.translation = malloc(translation_length + 1);

        // cas particulier 1: Gestion des cas ou un genre est present
        char* genre_f = strstr(line, "(f)");
        char* genre_m = strstr(line, "(m)");
        
        if(genre_f != NULL || genre_m != NULL) { //si le genre est present dans la ligne
            strncpy(entry.translation, translation_start, translation_length-4); //on retire le genre de la fin
            } else {
                strncpy(entry.translation, translation_start, translation_length); // sinon tel quel          
            }

        entry.translation[translation_length] = '\0'; //rajoute du caractere de fin

        // Cas particulier 2: La ligne se termine par un ']' et non un '"'
        if (translation_length > 0 && entry.translation[translation_length -1] == ']'){
            entry.translation[translation_length -1 ] = '\0';      
            }
        }

    }

    return entry;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Fonction pour libérer la mémoire allouée pour une entrée
void free_entry(Entry* entry) {
    free(entry->word);
    free(entry->cat);
    free(entry->translation);
}

int main() {
    // Lire le contenu du fichier

    FILE* file = fopen("engfr.dict", "r");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return 1;
    }
    char line[1024];
    
    Entry entry;
    

    // Traitement de chaque ligne et affichage des informations extraites
    while (fgets(line, sizeof(line), file) != NULL) {
        entry = extract_info(line);
        printf("Mot anglais: %s\n", entry.word);
        printf("Categorie: %s\n", entry.cat);
        printf("Traduction francais: %s\n\n", entry.translation);
    }
    // Libérer la mémoire allouée
    free_entry(&entry);
    // Fermer le fichier
    fclose(file);

    return 0;
}
