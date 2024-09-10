//main:
// 2018-10-11, Yves Roy, creation (247-637 S-0003)

//INCLUSIONS
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <stdlib.h>
#include <signal.h>
#include "main.h"
#include "piloteSerieUSB.h"
#include "interfaceTouche.h"
#include "interfaceMalyan.h"

//Definitions privees
#define MAIN_LONGUEUR_MAXIMALE 99
#define MSG_SIZE 10
#define S_SEQUENCE_LENGTH 16 // 16 points d'arrêt dans le déplacement en "S"

//Declarations de fonctions privees:
int main_initialise(void);
void main_termine(void);
void deplacementEnS(void);

//Definitions de variables privees:
//pas de variables privees

//Definitions de fonctions privees:
int main_initialise(void)
{
  if (piloteSerieUSB_initialise() != 0)
  {
    return -1;
  }
  if (interfaceTouche_initialise() != 0)
  {
    return -1;
  }
  if (interfaceMalyan_initialise() != 0)
  {
    return -1;
  }
  return 0;
}

void main_termine(void)
{
  piloteSerieUSB_termine();
  interfaceTouche_termine();
  interfaceMalyan_termine();
}

//Definitions de variables publiques:
//pas de variables publiques


//Definitions de fonctions publiques:
int main(int argc,char** argv)
{
 int erreur = 0;
 unsigned char toucheLue='D';
 char reponse[MAIN_LONGUEUR_MAXIMALE+1];
 char command[MSG_SIZE] = {0};  // Tableau pour stocker les commandes
 int nombre; // Variable pour le nombre d'octets lus

  if (main_initialise())
  {
    printf("main_initialise: erreur\n");
    return 0;
  }

  // Affichage du menu d'utilisateur
  fprintf(stdout,"Tapez:\n\r");
  fprintf(stdout, "Q\": pour terminer.\n\r");
  fprintf(stdout, "6\": pour démarrer le ventilateur.\n\r");
  fprintf(stdout, "7\": pour arrêter le ventilateur.\n\r");  
  fprintf(stdout, "8: Obtenir la position actuelle.\n\r");
  fprintf(stdout, "P: Déplacer la tête à la position X=20, Y=20, Z=20.\n\r");
  fprintf(stdout, "H: Positionner la tête d'impression à l'origine.\n\r");
  fprintf(stdout, "S: Parcours en S.\n\r");
  fprintf(stdout, "autre chose pour générer une erreur.\n\r");
  fflush(stdout);
  
    pid_t pid = fork();

    if (pid < 0) {
        perror("Erreur de fork");
        return EXIT_FAILURE;
    } else if (pid == 0) {
        // Processus fils
        while (1)
        {
          // Recevoir une commande du père
            if (command[0] == '6') 
            {
                interfaceMalyan_demarreLeVentilateur();
            } 
            else if (command[0] == '7')
            {
                interfaceMalyan_arreteLeVentilateur();
            } 
            else if (command[0] == '8')
            {
                interfaceMalyan_donneLaPosition();
            } 
            else if (command[0] == 'P')
            {
                interfaceMalyan_vaALaPosition(20, 20, 20);
            }
            else if (command[0] == 'H') 
            {
                interfaceMalyan_retourneALaMaison();
            }
            else 
            {
                interfaceMalyan_genereUneErreur(); 
            }
            // Récupérer la réponse de l'imprimante
            nombre = interfaceMalyan_recoitUneReponse(reponse, MAIN_LONGUEUR_MAXIMALE);
            if (nombre >= 0)
            {
                reponse[nombre] = '\0';
                printf("Réponse de l'imprimante: %s\n", reponse);  // Afficher la réponse
                // Vérifier si la réponse contient "ok"
                if (strstr(reponse, "ok") != NULL)
                {
                  printf("Réponse contient 'ok'.\n");  // Afficher un message si "ok" est trouvé
                }
                write(STDOUT_FILENO, "R", 1); // Écrire 'R' dans la sortie standard
            }
            sleep(1); 
        }   
    } 
    else 
    {
        // Processus père
      while (toucheLue != 'Q')
      {
        printf("Entrez une commande\n");
        toucheLue = interfaceTouche_lit();
        printf("Caractère lu = '%c'\n", toucheLue);

        switch (toucheLue)
        {
          case '6':
            if (interfaceMalyan_demarreLeVentilateur() < 0)
            {
              erreur = 1;
            }
          break;

          case '7':
            if (interfaceMalyan_arreteLeVentilateur() < 0)
            {
              erreur = 1;
            }
          break;

          case '8':
            if (interfaceMalyan_donneLaPosition() < 0)
            {
              printf("Erreur : Impossible d'obtenir la position\n");
            }
          break;

          case 'P':
            if (interfaceMalyan_vaALaPosition(20, 20, 20) < 0) 
            {
              printf("Erreur : Impossible d'aller à la position 20, 20, 20\n");
            }
          break;

          case 'H':
            if (interfaceMalyan_retourneALaMaison() < 0)
            {
              printf("Erreur : Impossible de retourner à la maison\n");
            }
          break;

          case 'S':
            // Appeler la fonction pour déplacement en S
            deplacementEnS();
          break;

          default:
          // Si aucune commande valide n'est saisie, génère une erreur
            // if (interfaceMalyan_genereUneErreur() < 0)
            // {
            //   erreur = 1;
            // }
          break;

        }
    if (erreur == 1)
    {
      printf("erreur lors de la gestion de la commande\n");
      break;
    }
    // else
    // {
        usleep(100000);                
//       nombre = interfaceMalyan_recoitUneReponse(reponse, MAIN_LONGUEUR_MAXIMALE);
//       if (nombre < 0)
//       {
//         erreur = errno;
//         printf("main: erreur lors de la lecture: %d\n", erreur);
//         perror("erreur: ");
//       }
//       else
//       {
//         reponse[nombre] = '\0';
//         printf("nombre reçu: %d, réponse: %s", nombre, reponse);      
// //      fflush(stdout);
//       }
//     }
     }
     kill(pid, SIGTERM); // Demander au fils de s'arrêter
     waitpid(pid, NULL, 0); // Attendre que le processus fils se termine
    }
     main_termine();
     return EXIT_SUCCESS;
  }




// Fonction pour effectuer un déplacement en "S"
void deplacementEnS(void)
 {
  int x = 0;
  int y = 0;

    printf("Début du déplacement en S...\n");
    interfaceMalyan_retourneALaMaison();

  // Début de la séquence en S

                    interfaceMalyan_vaALaPosition(x + 100, y, 0); // Déplacement à droite de 1 cm
                    sleep(1);
                    interfaceMalyan_vaALaPosition(x + 200, y, 0); // Déplacement à droite de 1 cm
                    sleep(1);
                    interfaceMalyan_vaALaPosition(x + 300, y, 0); // Déplacement à droite de 1 cm
                    sleep(1);
                    interfaceMalyan_vaALaPosition(x + 300, y + 100, 0); // Déplacement à gauche de 1 cm
                    sleep(1);
                    interfaceMalyan_vaALaPosition(x + 200, y + 100, 0); // Déplacement à gauche de 1 cm
                    sleep(1);
                    interfaceMalyan_vaALaPosition(x + 100, y + 100, 0); // Déplacement à gauche de 1 cm
                    sleep(1);
                    interfaceMalyan_vaALaPosition(x, y + 100, 0); // Descendre de 1 cm
                    sleep(1);
                    interfaceMalyan_vaALaPosition(x, y + 200, 0); // Descendre de 1 cm
                    sleep(1);
                    interfaceMalyan_vaALaPosition(x, y + 300, 0); // Descendre de 1 cm
                    sleep(1);
                    interfaceMalyan_vaALaPosition(x + 100, y + 300, 0); // Déplacement à droite de 1 cm
                    sleep(1);
  
  // Retourner à la position de départ
  interfaceMalyan_retourneALaMaison();
  sleep(1);
  printf("Déplacement en S terminé.\n");

}





