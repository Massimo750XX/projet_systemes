# Tagset
tag create utilisateur film livre genre couleur musique
tag create -f livre roman BD manga essai dictionnaire scolaire biographie
tag create -f musique jazz ballade rock 
tag create -f biographie autobiographie autofiction 
tag create -f roman policier historique science-fiction
tag create -f science-fiction "space opera"
tag create -f policier thriller
tag create -f thriller psychologique
tag create -f couleur rouge vert bleu rose jaune
tag create -f bleu bleu-clair azur turquoise
tag create -f genre western horreur drame
tag create -f jaune ocre
echo "Tagset created :"
tag print
# Tag files
touch f1
touch f2
touch f3
touch f4
touch f5
tag link f1 thriller roman ballade vert rouge couleur
tag link f2 film science-fiction "space opera" rose western azur rouge
tag link f3 historique film azur ocre
tag link f4 autofiction turquoise
tag link f5 thriller psychologique historique jaune