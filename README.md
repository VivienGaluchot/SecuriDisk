# SecuriDisk
Protocole sécurisé d'échange de commande client/serveur.

* Vivien Galuchot : vivien.galuchot@etu.univ-amu.fr
* Vincent Hernandez : vincent.hernandez@etu.univ-amu.fr

Pour les fonctions AES et SHA-256, nous avons utilisé la bibliothèque cryptographique de [B-Con](https://github.com/B-Con/crypto-algorithms)

Etant donné qu'on avait très peut d'information sur le protocole à implémenter, on a choisit de faire le suivant :
[Diagramme de séquence du protocole](https://drive.google.com/file/d/0Bzk8Zprw1bI-TG5Sa0xjbTk0VFk/view?usp=sharing)

A l'initialisation, le serveur ferme le lecteur. Si le lecteur est ouvert ou fermé manuellement, l'action sera détectée et signalée lors de la prochaine commande.

## Compiler
Le programme est fait pour les sytemes Linux. Les fontions cryptographiques sont intégrées, il ne nécéssite pas de bibliothèque externe.
Pour compiler :
* se positionner à la racine du fichier
* lancer la commande `make`

## Utilisation du serveur
Lancer le serveur : `./serveur`

## Utilisation du client
Lancer le client et se connecter à au serveur d'ip <ip> : `./client <ip>`

Un seul utilisateur est disponnible :
* identifiant : `JeanMichel`
* mot de passe : `Crapeaud`

Commandes disponibles :
* ouvrir : `Ouvrir`
* fermer : `Fermer`
* etat : `Etat`
