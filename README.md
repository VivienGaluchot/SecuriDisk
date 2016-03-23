# SecuriDisk
Protocole sécurisé d'échange de commande client/serveur.

* Vivien Galuchot : vivien.galuchot@etu.univ-amu.fr
* Vincent Hernandez : vincent.hernandez@etu.univ-amu.fr

## Compiler
Le programme est fait pour les sytemes Linux. Les fontions cryptographiques sont intégrées, il ne nécéssite pas de bibliothèque externe.
Pour compiler :
* se positionner à la racine du fichier
* lancer la commander `make`

## Utilisation du serveur
Lancer le serveur : `./serveur`

## Utilisation du client
Lancer le client et se connecter à au serveur d'ip <ip> : `./client <ip>`

Commandes disponibles :
* ouvrir : `Ouvrir`
* fermer :`Fermer`
* etat : `Etat`
