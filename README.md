## Table de Hachage distribué

Ce projet est consituter de deux programmes qui interagissent via l'API socket:
- un client qui peut envoyer des messages à un serveur
- des serveurs qui répondent aux clients et interagissent entre eux


## Types de messages

# Forme générale
TYPE | LONGUEUR DU DATA | DATA

# GET 
Le client demande à savoir qui possède un certain hash
0 | LG | hash

# PUT
Le client indique au serveur qu'une IP possède un hash
1 | LG | IP HASH

# WANT
Pas implementé: Retient les utilisateurs qui voulaient le hash

# HAVE
Le serveur répond au client et lui donne la liste des IP qui possède un certain hash:
3 | LG | IP IP ...IP

# EXIT
Le client peut dire à un serveur de s'arrêter:
4 | LG | PASSWORD

# NEW
Un serveur notifie un autre serveur qu'il vient de démarrer:
5 | LG | IP du serveur qui démarre

# DECO
Un serveur notifie un autre serveur qu'il s'arrête:
6 | 0 | NULL

#KEEP_ALIVE
Un serveur demande a un autre serveur de lui repondre pour le keep alive
7 | LG | 0

#YES
On autorise la connexion d'un serveur
8 | 0 | NULL

#NEW_SERV
Message envoyé lorqu'on partage la liste des serveurs lors de la connexion d'un
nouveau serveur
9 | LG | IP

#Implementation du serveur
5 sockets:
- 1: Un socket pour recevoir les messages du client
	l'ip liée est celle qu'on donne a la creation du serveur
- 2: Un socket pour repondre au client
- 3: Un socket pour recevoir entre serveurs
- 4: Un socket pour repondre au serveurs
- 5: Un socket pour le keep alive
Donc 4 port reservés parametrable dans (dht.h:27->31)
Objectif: Séparer les communications entre clients <-> serveurs
et entre serveur <-> serveur , et aussi pour peut etre reduire le congestionnement sur un socket.

Un thread pour le keep alive 
-> Evite le blocage de la reception quand on fait un keep alive

Un thread pour la suppresion d'un serveur dans la liste de serveurs:
+ un semaphore pour eviter d'avoir une liste de serveur fausse.
Implementation pas finie: faite que dans deconnexion_serv (dht.c:833)

Utilisation du select pour ecouter plusieurs socket a la fois.
-> Eviter des attentes actives et perdre des messages

#Implementation du client
1 socket pour l'envoi et la reception.
utilisation de select pour eviter les attentes infinies.

#Pages de manuel
client.1
server.1
