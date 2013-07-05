Écrire avec un plotter
======================

Pseudo-code
-----------

Voici le pseudo-code::

  LINE_HEIGHT = 100
  CHAR_WIDTH = 80
  
  Prendre le texte à écrire.
  Le séparer en lignes avec un nombre maximal de caractères dans chacune.
  
  offset_y = 0
  offset_x = 0
  
  Pour chaque ligne dans le texte:
    offset_y += LINE_HEIGHT
    Pour chaque caractère dans le texte à écrire:
      offset_x += CHAR_WIDTH
      Trouver le nombre UTF-8 pour ce caractère.
      Trouver le fichier correspondant pour ce nombre.
      Si le fichier existe:
        Lire le contenu de ce fichier.
        Pour chaque commande HPGL:
          si c'est PA:
            ajouter offset_x l'argument x (le format est PAx,y ou PAx y;)
            ajouter offset_y l'argument y
            ajouter cette commande à la liste de commandes à envoyer
  envoyer les commandes

