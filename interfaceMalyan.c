//interfaceMalyan
//Historique: 
//2018-11-12, Yves Roy, creation

//INCLUSIONS
#include "main.h"
#include "piloteSerieUSB.h"
#include "interfaceMalyan.h"

//Definitions privees
#define INTERFACEMALYAN_LONGUEUR_MAXIMALE_DES_COMMANDES 64
#define INTERFACEMALYAN_LONGUEUR_MAXIMALE_DES_REPONSES 64

//Declarations de fonctions privees:
int interfaceMalyan_ecritUneCommande(char *Commande, unsigned char Longueur);
int interfaceMalyan_recoitUneReponse(char *Reponse, unsigned char LongueurMaximale);

//Definitions de variables privees
unsigned char interfaceMalyan_commande[INTERFACEMALYAN_LONGUEUR_MAXIMALE_DES_COMMANDES];
unsigned char interfaceMalyan_reponse[INTERFACEMALYAN_LONGUEUR_MAXIMALE_DES_REPONSES];

//Definitions de fonctions privees:
int interfaceMalyan_ecritUneCommande(char *Commande, unsigned char Longueur)
{
int retour;
  retour = piloteSerieUSB_ecrit(Commande, Longueur);
  if (retour != (int)Longueur)
  {
    return -1;
  }
  piloteSerieUSB_attendLaFinDeLEcriture();
  return retour;
}

int interfaceMalyan_recoitUneReponse(char *Reponse, unsigned char LongueurMaximale)
{
  return piloteSerieUSB_lit(Reponse, LongueurMaximale);  
}

//Definitions de variables publiques:
//pas de variables publiques

//Definitions de fonctions publiques:
int interfaceMalyan_initialise(void)
{
  return 0;
}

int interfaceMalyan_termine(void)
{
  return 0;
}

int interfaceMalyan_demarreLeVentilateur(void)
{
  return interfaceMalyan_ecritUneCommande("M106\n", 5);  
}

int interfaceMalyan_arreteLeVentilateur(void)
{
  return interfaceMalyan_ecritUneCommande("M107\n", 5);
}

int interfaceMalyan_genereUneErreur(void)
{
  return interfaceMalyan_ecritUneCommande("x000\n", 5);
}

int interfaceMalyan_donneLaPosition(void)
{
  return interfaceMalyan_ecritUneCommande("M114\n", 5);  // Commande M114 pour obtenir la position
}

int interfaceMalyan_vaALaPosition(int x,int y,int z)
{
  char commande[64]; 
  sprintf(commande, "G0 X%d Y%d Z%d\n", x, y, z);  // Commande G0 pour déplacer
  return interfaceMalyan_ecritUneCommande(commande, 15);
}

int interfaceMalyan_retourneALaMaison(void)
{
  return interfaceMalyan_ecritUneCommande("G28\n", 4);  // Commande G28 pour retourner à l'origine
}