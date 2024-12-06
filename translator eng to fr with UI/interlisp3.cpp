#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef WIDGET_H_
#define WIDGET_H_

// pour openmotif
#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Xm/MainW.h>
#include <Xm/CascadeB.h>
#include <Xm/RowColumn.h>


// pour l'écriture dans un widget
typedef Widget * output_widget ;

typedef struct {
    char* word;
    char* cat;
    char* translation;
} Entry;

/* =============================================== PROTOTYPES ==============================================*/

Widget initMotifWidgets(Widget, XtAppContext) ;
int initSetArg(Arg []) ;

void quit_call(Widget, XtPointer, XtPointer) ;
void control_insert(Widget, XtPointer, XtPointer) ;

void Done(Widget, XEvent *, String *, Cardinal *) ;

#endif /*WIDGET_H_*/


#ifndef COMMODE_H_
#define COMMODE_H_

// commodités générales

typedef char * String ;
typedef char const * ConstantString ;
typedef unsigned long int positive ;
#define FALSE 0
#define TRUE 1
#define elseif else if

#define eql ==
#define neql !=

//int allowInput = TRUE;

#endif /*COMMODE_H_*/


/* ========================================== GLOBALES ============================================*/

// Causes de throw et messages correspondants
// Pour Done (catch), read et read_list (throw)

XmTextPosition Start_pos = 0 ;



/* ============================================= MAIN ======================================================*/

int main(int argc, String argv[])
{ XtAppContext interface ;
  
  // créer la fenêtre principale et ses sous-parties motif
  Widget top_widget = XtVaAppInitialize(&interface, "Interlisp", NULL, 0, &argc, argv, NULL, NULL) ;
  Widget saisie = initMotifWidgets(top_widget, interface) ;
  
  // initialiser la zone de saisie (avec un petit changement dans l'initialisation)
  XmTextSetString(saisie, (String) "Entrez le mot a traduire:\n") ;
  XmTextPosition lastPos = XmTextGetLastPosition(saisie);
  Start_pos = lastPos;
  XmTextSetCursorPosition(saisie, Start_pos) ;

  
  // afficher le tout
  XtRealizeWidget(top_widget) ;
  
  // boucle principale
  XtAppMainLoop(interface) ;
 

  return 0 ;
}

/*============================================ FONCTIONS WIDGET ==========================================*/

/* ======================================== Dictionnaire =======================================*/
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
            entry.word = (char*)malloc(word_length + 1); //allocation memoire
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
            entry.cat = (char*)malloc(cat_length + 1);
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
            entry.translation = (char*)malloc(translation_length + 1);

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


//Fonction pour libérer la mémoire allouée pour une entrée plus facilement
void free_entry(Entry* entry) {
    free(entry->word);
    free(entry->cat);
    free(entry->translation);
}


// action lors d'un <Enter> : lance le read_eval_print
// boucler tant que le read n'a pas épuisé la ligne
// line contient toute la ligne
// S contient la partie de la ligne restant à traiter
void Done(Widget text, XEvent *event, String *params, Cardinal *n) {
    // Lire le contenu du fichier
    
    
    //ancien debug pour arreter apres le premier loop
    /* if (!allowInput){
    return;
    }*/
    
    FILE* file = fopen("engfr.dict", "r");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
   	}
   	
  
    // definition taille ligne max
    char line[1024];
    
    // buffer pour gerer les input utilisateurs et XmTextGetSubstring()
    size_t buffer_size = 50;
    char buffer[buffer_size];
    
        Entry entry;
        
        // Gestion des input utilisateur avec copie de l'input dans mot_traduire
    	XmTextGetSubstring(text, Start_pos, 100, buffer_size, buffer);
    	String mot_traduire = strdup(buffer);
    
    // Lire le texte à partir du widget de texte Motif
  
	int mot_trouver = 0;

	    // Traitement de chaque ligne et affichage des informations extraites
	    while (fgets(line, sizeof(line), file) != NULL) {
	    	
		entry = extract_info(line);
		 
		//loop de verification des mots + format (format peut etre ameliorer)
		if (strcmp(mot_traduire, entry.word) == 0) {
		    char format[100];
		    sprintf(format, "\nTraduction: %s\n", entry.translation);
		    // Afficher la traduction dans le widget de texte Motif
		    XmTextInsert(text, XmTextGetLastPosition(text), format);
		    mot_trouver++;
		    }
		}
		  
	    	    	
	    	if(mot_trouver == 0) { //si aucun mot trouver on print le message du dessous
		XmTextInsert(text, XmTextGetLastPosition(text), (String) "\nMot pas dans le dico\n"); }
		
		// debug pour verifier les valeurs Hexa de mot_traduire et entry.[word-translation-cat]
		/*for(int i = 0; mot_traduire[i] != '\0'; i++){
			   printf("%02x ", mot_traduire[i]);
			} printf("\n\n");
			
		for(int i = 0; entry.word[i] != '\0'; i++){
			    printf("%02x ", entry.word[i]);
			} printf("\n\n");  */ 
		
		// libere la memoire du mot a traduire et des tables entry et re-initialise la position du curseur
		XtFree(mot_traduire);
		XmTextPosition lastPos = XmTextGetLastPosition(text);
		Start_pos = lastPos;
		XmTextInsert(text, Start_pos, (String) "");
		free_entry(&entry);
		
		/*printf("%s\n", mot_traduire);		
		printf("%s\n", entry.word);
		allowInput = FALSE;*/
	// Fermer le fichier
	fclose(file);

}


/* =============================================== FONCTIONS ==============================================*/

// initialisation des widgets
Widget initMotifWidgets(Widget top_wid, XtAppContext app) 
{ Widget main_window = XtVaCreateManagedWidget("main_window", xmMainWindowWidgetClass, top_wid, NULL) ;

  Widget menu_bar = XmCreateMenuBar(main_window, (String) "main_list", NULL, 0) ;
  XtManageChild(menu_bar) ;

  // créer le bouton quit avec son callback
  Widget quit = XtVaCreateManagedWidget("Quit", xmCascadeButtonWidgetClass, menu_bar, NULL);
  XtAddCallback(quit, XmNactivateCallback, quit_call, NULL) ;

  // créer la zone de saisie et ses ressources 
  Arg args[10];
  int n = initSetArg(args) ;

  Widget saisie = XmCreateScrolledText(main_window, (String) "saisie", args, n) ;
  XtManageChild(saisie) ;
  
  // gérer le retour-charriot
  static XtActionsRec actions[] = {{(String) "Done", Done}} ;
  XtAppAddActions(app, actions, XtNumber(actions));

  static ConstantString traduction = "<Key>Return: Done()" ;
  XtTranslations mytranslations = XtParseTranslationTable(traduction) ;
  XtOverrideTranslations(saisie, mytranslations) ;

  // gérer le curseur
  XtAddCallback(saisie, XmNmodifyVerifyCallback, control_insert, NULL) ;

  return saisie ;
}

// initialiser le tableau des ressources pour le widget texte
int initSetArg(Arg args[])
{ int n = 0 ;
  XtSetArg(args[n], XmNrows, 30) ;                     // longeur initiale
  n++ ;
  XtSetArg(args[n], XmNcolumns, 80) ;                  // largeur initiale
  n++ ;
  XtSetArg(args[n], XmNeditable, True) ;               // le texte est modifiable
  n++ ;
  XtSetArg(args[n], XmNcursorPositionVisible, True) ;  // le curseur est visible
  n++ ;
  XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT) ;  // le widget comprend plusieurs lignes
  return n + 1 ;
}

// callback de fin ; déclenché par le clic sur le bouton Quit
void quit_call(Widget W, XtPointer app_data, XtPointer call_data)
{ printf("Quitting program\n"); exit(0); }

// callback empêchant l'insertion et la destruction ailleurs que dans la zone de saisie
// déclenché lors de toute tentative de modifier le texte
void control_insert(Widget W, XtPointer app_data, XtPointer call_data)
{ XmTextVerifyCallbackStruct * call = (XmTextVerifyCallbackStruct *) call_data ;
  if ((call->reason == XmCR_MOVING_INSERT_CURSOR and call->newInsert < Start_pos )
       or call->startPos < Start_pos )
  {call->doit = False ; return ;}
  return ;
}


