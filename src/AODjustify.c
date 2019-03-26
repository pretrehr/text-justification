/**
 * \file AODjustify.c
 * \brief Programme de justification de texte.
 * \author Jean-Baptiste Bonnechère
 * \author Raphaël Prêtre
 * \version 1.0
 * \date 5 novembre 2018
 *
 * Programme de justification de texte par programmation dynamique.
 *
 */


#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>


/**
 * \fn bool isvalueinarray(long long val, long long *arr, long long size)
 * \brief Verifie si val est dans arr.
 *
 * \param val: valeur à trouver
 * \param arr: tableau de valeur
 * \param size: taille du tableau
 * \return true si val dans arr, false sinon.
 */

bool isvalueinarray(long long val, long long *arr, long long size) {
  for (long long i=0; i < size; i++) {
    if (arr[i] == val) {
      return true;
    }
  }
  return false;
}


/*******************************************************************************
      Calcule le nombre de caracteres situes entre les mots i et j inclus,
            en supposant qu'il n'y a qu'un espace entre chaque mot
*******************************************************************************/

/**
 * \fn long long longueur_entre_mots(char **words, long long i, long long j)
 * \brief Calcule le nombre de caracteres situes entre les mots i et j inclus, en supposant qu'il n'y a qu'un espace entre chaque mot
 *
 * \param words: liste de mots
 * \param i: premier mot
 * \param j: dernier mot
 */

long long longueur_entre_mots(char **words, long long i, long long j) {
  long long length = strlen(words[i]);
  for (long long m = i+1; m<=j; m++) {
    length += strlen(words[m])+1;
  }
  return length;
}


/*******************************************************************************
        Effectue nb_tirages_init tirages aléatoires sans remise, avec chaque valeur comprise dans [0, maxi-1]
*******************************************************************************/
/**
 * \fn long long* tirage_alea(long long nb_tirages_init, long long maxi)
 * \brief Effectue nb_tirages_init tirages aléatoires sans remise, avec chaque valeur comprise dans [0, maxi-1]
 *
 * \param nb_tirages_init: nombre de tirages à réaliser
 * \param maxi: seuil de valeur tirée
 * \return tableau des valeurs tirées
 */



long long* tirage_alea(long long nb_tirages_init, long long maxi) {
  long long nb_tirages = nb_tirages_init;
  long long r;
  srand(time(NULL));
  long long* tab = malloc(nb_tirages*sizeof(long long));
  for (long long i=0; i<nb_tirages; i++) {
    tab[i] = -1;
  }
  while (nb_tirages>0) {
    r = rand()%(maxi);
    if (!(isvalueinarray(r, tab, nb_tirages_init))) {
      nb_tirages--;
      tab[nb_tirages] = r;
    }
  }
  return tab;
}


/*******************************************************************************
    Pour un espace d'une ligne donnée (définie par son premier mot i et son
     dernier mot k), renvoie le nombre d'espaces nécessaires à cet endroit.
            On y ajoute éventuellement un espace supplémentaire.
*******************************************************************************/
/**
 * \fn void print_espace(FILE* fichier, long long width, long long i, long long k, long long length, bool esp_sup)
 * \brief Pour un espace d'une ligne donnée (définie par son premier mot i et son dernier mot k), renvoie le nombre d'espaces nécessaires à cet endroit. On y ajoute éventuellement un espace supplémentaire.
 *
 * \param fichier: fichier dans lequel écrire
 * \param width: largeur de la page
 * \param i: premier mot de la ligne
 * \param k: dernier mot de la ligne
 * \param length: nombre de caractères imprimables sur la ligne en y ajoutant un espace entre chaque mot
 * \param esp_sup: ajout d'un espace supplementaire ou non
 */


void print_espace(FILE* fichier, long long width, long long i, long long k, long long length, bool esp_sup) {
  long long espace_a_combler = width-length;
  long long nb_esp_min = espace_a_combler/(k-i);
  for (long long j=0; j<nb_esp_min; j++) {
    fprintf(fichier, " ");
  }
  if (esp_sup) {
    fprintf(fichier, " ");
  }
}



/*******************************************************************************
                  Ajout de l'extention du fichier d'entree
*******************************************************************************/

/**
 * \fn char* ajout_extention_entree(char *nom)
 * \brief Ajout de l'extention du fichier d'entree
 *
 * \param nom: nom du fichier
 * \return nom du fichier avec le .in final
 */

char* ajout_extention_entree(char* nom) {
  char* buffer = malloc((strlen(nom)+4)*sizeof(char));
  sprintf(buffer, "%s.in", nom);
  return buffer;
}


/*******************************************************************************
                  Ajout de l'extention du fichier de sortie
*******************************************************************************/

/**
 * \fn char* ajout_extention_sortie(char *nom)
 * \brief Ajout de l'extention du fichier de sortie
 *
 * \param nom: nom du fichier
 * \return nom du fichier avec le .out final
 */

char* ajout_extention_sortie(char* nom) {
  char* buffer = malloc((strlen(nom)+5)*sizeof(char));
  sprintf(buffer, "%s.out", nom);
  return buffer;
}


int main(int argc, char *argv[]) {
  assert(argc == 3);
  char* nom_in = ajout_extention_entree(argv[2]);
  FILE* fichier = fopen(nom_in, "r");
  if (fichier == NULL) {
      free(nom_in);
      fprintf(stderr, "AODjustify ERROR>fichier inexistant\n");
      return 1;
  }

  //Récupération du texte du fichier source
  long long width = atoi(argv[1]);
  long long fd = open(nom_in,O_RDONLY);
  fseek(fichier, 0L, SEEK_END);
  long long size = ftell(fichier);
  rewind(fichier);
  char *source = (char*) mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);

  //Découpage en mots
  long long nChars = 0; //Nombre de caractères du mot considéré
  long long len = 0; //Nombre de mots du paragraphe considéré
  long long nPara = 0; //Nombre de paragraphes du fichier source
  long long *longueur_para = calloc(1,sizeof(long long)); //Tableau indiquant le nombre de mots pour chaque paragraphe du fichier source
  char ***words = calloc(1, sizeof(char **)); //Liste de mots, découpés en paragraphes
  words[0] = calloc(1, sizeof(char*));
  words[0][0] = calloc(2, sizeof(char)); //calloc 2 char : un pour le caractère lu, un pour le caractère de fin de chaine
  while('\0' != *source) {
    if(*source == '\n') { //Rencontre du premier saut de ligne
      source++;
      while (*source == ' ') { //Saut des éventuels espaces
        source++;
      }
      if(*source == '\n') { //Deuxieme saut de ligne --> paragraphe supplementaire
        len++;
        longueur_para[nPara] = len;
        nPara++;
        longueur_para = realloc(longueur_para, (1+nPara)*sizeof(long long));
        len = 0;
        nChars = 0;
        words = realloc(words, (1+nPara)*sizeof(char **));
        words[nPara] = calloc(1, sizeof(char*));
        words[nPara][len] = calloc(2, sizeof(char));
        source++;
        continue;
      }
      else if (len > 0 && nChars>0) { //Pas de deuxieme saut de ligne --> le premier saut de ligne est simplement un separateur de mot
        len++;
        nChars = 0;
        words[nPara] = realloc(words[nPara], (1 + len) * sizeof(char *));
        words[nPara][len] = calloc(2, sizeof(char));
        continue;
      }
    }
    while (*source == ' ' || *source == '\t') { //Rencontre d'un espace ou d'une tabulation
      source++;
      if (*source != ' ' && *source != '\t') { //On trouve un caractere imprimable apres --> nouveau mot
        if (nChars>0) {
          len++;
          words[nPara] = realloc(words[nPara], (1 + len) * sizeof(char *));
          words[nPara][len] = calloc(2, sizeof(char));
          nChars = 0;
        }
      }
      continue;
    }
    if (*source == '\n') {//Rencontre d'un autre saut de ligne --> retour au cas où on rencontre un premier saut de ligne
      continue;
    }
    words[nPara][len][nChars] = *source; //On stocke le caractere lu
    words[nPara][len][nChars + 1] = '\0'; //Fin de chaine sur le caractere d'apres
    nChars++;
    if (nChars>width) { //Si un mot possede plus de caracteres qu'une ligne ne peut en contenir --> erreur
      longueur_para[nPara] = len;
      for (long long para = 0; para <= nPara; para++) {
        for (long long mot = 0; mot <= longueur_para[para]; mot++) {
          free(words[para][mot]);
        }
        free(words[para]);
      }
      free(words);
      free(longueur_para);
      fclose(fichier);
      fprintf(stderr, "AODjustify ERROR>le fichier possède un mot de longueur supérieure à %lld caractères : justification sur %lld caractères impossible\n", width, width);
      return 1;
    }
    source++;
    words[nPara][len] = realloc(words[nPara][len], (2 + nChars) * sizeof(char));
  }
  len++;
  longueur_para[nPara] = len;
  nPara++;


  long long** table = malloc(nPara*sizeof(long long*)); //table[n][k] stocke le
  //cout minimal dans le cas où le mot k du paragraphe n est en debut de ligne
  long long** indices = malloc(nPara*sizeof(long long*)); //Si le mot k est en
  //debut de ligne dans le paragraphe n, alors indices[n][k] est le rang du mot
  //qui commencera la ligne suivante avec le coût moindre pour la suite du paragraphe
  long long cout; //coût de la ligne elle commence au mot k et finit au mot k+i
  long long dp; //coût du paragraphe commençant au mot k+i+1
  long long longueur_mots; //nombre de caractères compris entre les mots k et
  // k+i en supposant qu'il n'y a qu'un seul espace entre les mots
  long long nb_tirages; //nombre d'espaces restant à répartir
  long long* tirages; //tirage aléatoire des mots de la ligne après lesquels
  //il faut rajouter un espace supplémentaire
  bool esp_sup; //présence ou non d'un espace supplémentaire
  long long norme = 0; //Norme total du texte justifié
  FILE* fichier_out = fopen(argv[2], "w");
  for(long long para = 0; para < nPara; para++) {
    len = longueur_para[para];
    table[para] = calloc(len,sizeof(long long));
    indices[para] = calloc(len,sizeof(long long));
    table[para][len-1] = 0;
    indices[para][len-1] = len;
    for (long long k = len-2; k>=0; k--) {
      table[para][k] = 9223372036854775807; //infini
      for (long long i = 0; k+i<len; i++) {
        longueur_mots = longueur_entre_mots(words[para],k, k+i);
        if (k+i==len-1 && longueur_mots<=width) { //si on est sur la derniere ligne, le coût est nul
          cout = 0;
        }
        else if (longueur_mots > width) {//si on dépasse la largeur de la page, le coût est infini
          cout = 9223372036854775807;
        }
        else {//sinon calcul de la norme 3 du nombre d'espaces restant
          cout = (long long) pow((double)(width-longueur_mots),(double)3);
        }
        if (cout == 9223372036854775807) {//si on a dépassé la largeur de la page, inutile de tester avec les mots suivants
          break;
        }
        if (k+i==len-1) {//si le dernier mot de la ligne est le dernier mot du
        //paragraphe, il n'y a pas de coût supplémentaire
          dp = 0;
        }
        else {//sinon on ajoute le coût associé au reste du paragraphe
          dp = table[para][k+i+1];
        };
        if (cout + dp < table[para][k]) {//on actualise la table de coûts de manière à avoir le minimum à chaque fois
          table[para][k] = cout + dp;
          indices[para][k] = k+i+1;
        }
      }
    }
    for(long long i=0; i<len; i=indices[para][i]) {
      longueur_mots = longueur_entre_mots(words[para], i, indices[para][i]-1);
      nb_tirages = width - longueur_mots;
      if (indices[para][i]-i-2>0) {//s'il y a plus de 2 mots sur la ligne, on
      //effectue un tirage aléatoire des éventuels espaces supplémentaires à rajouter
        tirages = tirage_alea((nb_tirages)%(indices[para][i]-i-1), indices[para][i]-i-1);
      }
      else {
        tirages = tirage_alea(0, 0);
      }
      for (long long j=i; j<indices[para][i]; j++) {
        //on determine si on ajoute un espace ou non apres le mot de rang m sur
        //la ligne en fonction de si m fait partie du tirage aleatoire effectué
        esp_sup = isvalueinarray(j-i, tirages, (indices[para][i]-1==i)?0:nb_tirages%(indices[para][i]-1-i));
        if (j < len) {
          if (j < indices[para][i]-1) {
            fprintf(fichier_out, "%s ", words[para][j]); //On affiche le mot suivi d'un espace
            if (indices[para][i]<len) {
              //On y ajoute éventuellement des espaces supplementaires
              print_espace(fichier_out, width, i, indices[para][i]-1, longueur_mots, esp_sup);
            }
          }
          else {
            //Si le mot est le dernier du paragraphe, on n'ajoute pas d'espace apres
            fprintf(fichier_out, "%s", words[para][j]);
          }
        }
        free(words[para][j]);
      }
      free(tirages);
      fprintf(fichier_out, "\n");
    }
    fprintf(fichier_out, "\n");
    free(words[para]);
    free(indices[para]);
    norme += table[para][0]; //On ajoute à la norme du texte la norme du paragraphe considere
    //(la norme du paragraphe étant le coût du premier mot du paragraphe)
    free(table[para]);
  }
  fprintf(stderr, "AODjustify CORRECT>%lld\n", norme);
  free(words);
  free(indices);
  free(table);
  free(longueur_para);
  free(nom_in);
  fclose(fichier);
  char* nom_out;
  nom_out = ajout_extention_sortie(argv[2]);
  rename(argv[2], nom_out);
  free(nom_out);
  fclose(fichier_out);
  return 0;
}
