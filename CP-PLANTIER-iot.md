# Compte Rendu TP3 

Ce tp avait pour but de mettre en oeuvre le protocol MQTT sur l'esp. 
Pour cela nous avons développé un nouveau mode sur l'esp pour gérer les alertes incendies. 



différents outils vu en cours employés dans l'iot.
Pour ce tp nous utilisons un esp32 

## Informations générales  

Pour ce tp nous utilisons la même maquette que pour le tp2. 
Sont fonctionnement global était le suivant: 
L'esp avait 2 modes, un mode d'auto régulation et un mode manuel qui permettait de piloter le module via un échange de requètes HTTP. L'esp hébergeait un serveur asynchrone permettant d'afficher le statut:
+ des capteurs
   + Temperature 
   + Chauffage
+ des actionneurs
   + Ventilateur : vitesse de rotation
   + le chaufage :on/off 

Pour ce tp nous admétons que cette centrale appartient a un réseau de modules identique disséminés dans un campus. Chaque module est identifié par son adresse MAC et possède une localisation dans le campus tel que {BATIMENT/ETAGE/COULOIR}

Nous ajoutons une nouvelle fonctionalité au module : la détection de feu et la gestion d'un alerte incendie. 
Pour cela, nous introduisons un nouveau mode : incendie. L'esp bascule dans ce nouveau mode dès qu'une suspiscion de feu est détectée.i
Dès lors, tous les actionneurs sont alors éteints et la temperature du module est publiée sur un topic specifique du broker MQTT. Si d'autre modules du batiments suspectent un incendie, alors l'alerte d'incendie est avérée.
Dans un cas d'incendie avéré alors des extincteurs sont activés ( sous la forme d'une led clignotante ). Dès lors qu'un état d'incendie est avéré, seuls les secours peuvent réactiver le module dans le mode automatique via une publication sur le topic personnel de l'esp. 



## La détection du feu 

La détection du feu est faite selon deux facteurs : 
Une augmentation anormalement rapide de température est une suspection de feu. Auquel cas, l'esp publie sur le topic /$BATIMENT. Si un autre esp du meme batiment suspecte également un feu ou si un feu est déclaré dans le batiment  alors, l'esp passe en mode incendie.

Un seuil de température de feu est également détecté. Si ce seuil est dépassé, l'esp passe directement en mode incendie.

## Les échanges de donnés MQTT 
Un broker MQTT accessible sur le meme réseau que l'esp est déployé.
La description de ces topics est la suivante 
+ /devices:tous les devices conectés du réseau publient régulièrement sur leurs topic  leurs position et leurs mac address pour indiquer qu'il sont encore actifs ( ex : -t /ack -m '/batiment/etage/salle/mac'  ). Ils sont également a l'écoute d'un  topic  personnel /batiment/etage/salle/mac/command pour recevoir des informations personelles. ( réactivation du module ) 

+ /$batiment: les esp publient sur ce topic la temperature mesurée par l'esp lors d'une suspection d'incendie de leurs localisation. Ils sont également a l'écoute de leurs batiment pour corhoborer les température et déclarer l'état d'alerte si plusieurs suspections d'incendies sont détectées. 

### Abonements 

Un esp qui est conecté s'abonne a 
+ son propre topic esp.fullTopic:/batiment/etage/salle/esp-mac-adresse
+ son propre topic de commande /batiment/etage/salle/esp-mac-adresse/command

### Publication 

En fonctionnement normal, L'esp publie tout les MQTT_ACK_PERIOD cycles  sur fullTopic/ackson c pour indiquer qu'il est toujours en vie et sa derniere mesure de température 
Si un feu est suspecté il publie sur le topic de son batiment.

Les pompiers peuvent publier sur le topic /full esp topic/command un message de typer {"fireMode":"on"} pour activer ou desactiver les extincteurs. 
Pour une desactivation les esp reprennes un mode automatique .


