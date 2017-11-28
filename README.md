## Table de Hachage distribué

Ce projet est consituter de deux programmes qui interagissent via l'API socket:
- un client qui peut envoyer des messages à un serveur
- des serveurs qui répondent aux clients et interagissent entre eux


## Types de messages

# Forme générale
TYPE | LONGUEUR DU DATA | DATA

# GET 
Le client demande à savoir qui possède un certain hash
1 | LG | hash

# PUT
Le client indique au serveur qu'une IP possède un hash
2 | LG | IP HASH

# HAVE
Le serveur répond au client et lui donne la liste des IP qui possède un certain hash:
4 | LG | IP IP ...IP

# EXIT
Le client peut dire à un serveur de s'arrêter:
5 | LG | PASSWORD

# NEW
Un serveur notifie un autre serveur qu'il vient de démarrer:
6 | LG | IP du serveur qui démarre

# DECO
Un serveur notifie un autre serveur qu'il s'arrête:
7 | LG | IP du serveur qui s'arrête
