echo "TEST - ERROR MANAGING"
echo
# Tagset
tag create utilisateur film livre genre couleur
tag create -f couleur rouge vert bleu rose jaune
tag create -f bleu bleu-clair azur turquoise
tag create musique
tag create -f genre western horreur drame
tag create -f jaune ocre
echo "--------- Hierarchy print --------- "
tag print
echo

# Tag
echo "---------- Tagging files ----------"
touch f1 f2 f3 f4 f5
tag link f1 vert rouge couleur
echo
tag link f2 film rose western azur rouge
echo
tag link f3 film azur ocre
echo
tag link f4 turquoise 
echo
tag link f5 jaune
echo

#Errors
echo "-------- Missing arguments --------"
echo "-- tag create"
tag create
echo
echo "-- tag delete"
tag delete
echo
echo "-- tag link f1"
tag link f1
echo

echo "----------- Syntax error ----------"
echo "-- tag lsqjfl"
tag lsqjfl
echo
echo "-- tag search couleur rouge qoskd"
tag search couleur rouge qoskd
echo
echo "-- tag link f1 lksjqdf"
tag link f1 lksjqdf
echo
echo "-- tag unlink blablabla"
tag unlink blablabla
echo

echo "---------- Inexisting tag ---------"
echo "-- tag create -f user Louis Blanche Asmaa" 
tag create -f user Louis Blanche Asmaa
echo
echo "-- tag link f1 western user" 
tag link f1 western user 
echo
echo "-- tag search wester rouge user"
tag search wester rouge user
echo
echo "-- tag delete user"
tag delete user
echo
echo "-- tag unlink f5 couleur jaune"
tag unlink f5 couleur jaune
echo
echo "-- tag unlink f1 vert rouge couleur user" 
tag unlink f1 vert rouge couleur user
echo

echo "----------- Existing tag ----------"
echo "--tag create couleur"
tag create couleur
echo
echo "--tag link f2 film"
tag link f2 film
echo

echo "--------- Inexisting file ---------"
echo "--tag print f8"
tag print f8
echo
echo "--tag link f8 couleur"
tag link f8 couleur
echo
echo "--tag unlink f8 couleur"
tag unlink f8 couleur
echo

echo "--------- Double research ---------"
echo "--tag search couleur couleur"
tag search couleur couleur
echo

# Reset
echo "----- End of test : Reset all -----"
yes | tag reset
rm f1 f2 f3 f4 f5
