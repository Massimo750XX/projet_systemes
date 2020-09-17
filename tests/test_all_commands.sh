echo "TEST ALL TAG COMMANDS"
echo
# Create a hierarchy
echo "Creating hierarchy..."
tag create couleur film livre musique
tag create -f musique jazz ballade rock 
tag create -f livre roman BD manga essai dictionnaire scolaire biographie
tag create -f biographie autobiographie autofiction 
tag create -f roman policier historique science-fiction
tag create -f science-fiction "space opera"
tag create -f policier thriller
tag create -f thriller psychologique
echo " -------- Hierarchy print -------- "
tag print
touch test1
touch test2
touch test3
touch test4
touch test5
touch test6
echo
echo " -------- Tagging files -------- "
tag link test1 thriller roman policier ballade
tag link test2 film science-fiction "space opera"
tag link test3 historique jazz
tag link test4 autofiction roman
tag link test5 film thriller psychologique
tag link test6 musique jazz film
echo 
echo " -------- Printing tags of each file -------- "
echo "-- tag print test1"
tag print test1
echo
echo "-- tag print test2"
tag print test2
echo
echo "-- tag print test3"
tag print test3
echo
echo "-- tag print test4"
tag print test4
echo
echo "-- tag print test5"
tag print test5
echo 
echo "-- tag print test6"
tag print test6
echo 
echo " -------- Deleting tags from files -------- "
echo "-- tag unlink test4 roman"
tag unlink test4 roman
echo "-- tag print test4"
tag print test4
echo
echo "-- tag unlink test1 ballade thriller"
tag unlink test1 ballade thriller
echo "-- tag print test1"
tag print test1
echo
echo "-- tag unlink test6 --all"
tag unlink test6 --all
echo "-- tag print test6"
tag print test6
echo
echo " -------- Searching tags -------- "
echo "-- tag search psychologique -not policier -not thriller"
tag search psychologique -not policier -not thriller
echo
echo "-- tag search thriller film thriller"
tag search thriller film thriller
echo
echo "-- tag search science-fiction -not film"
tag search science-fiction -not film
echo
echo "-- tag search autofiction -not roman"
tag search autofiction -not roman
echo
echo "-- tag search autofiction"
tag search autofiction
echo
echo "-- tag search film"
tag search film
echo
echo " -------- Deleting tags from hierarchy -------- "
echo "-- tag delete musique"
yes | tag delete musique
echo "-- tag delete policier"
yes | tag delete policier
echo 
echo "-- tag print"
tag print
# Reset
echo
echo "-- tag reset"
yes | tag reset
rm test1 test2 test3 test4 test5 test6
