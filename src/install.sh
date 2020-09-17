echo 'installation xattr...'
sudo apt-get install xattr
echo 'installation glib....'
sudo apt-get install libglib2.0-dev
echo 'création répertoire /.tag...'
sudo mkdir /.tag
echo 'création fichiers tag_hierarchy et paths.txt...'
sudo touch /.tag/tag_hierarchy
sudo touch /.tag/paths.txt
sudo touch /.tag/dest.txt
sudo touch /.tag/remplace.txt
make
sudo mv tag /.tag
gcc update-mv2.c -o upmv
gcc update-mv3.c -o upmv2
gcc update-rm2.c -o uprm2
gcc update-cp.c -o upcp
gcc update-cp2.c -o upcp2
sudo cp mvt.sh /.tag
sudo cp rmta.sh /.tag/
sudo cp cpt.sh /.tag
sudo chmod a=wrx /.tag/rmta.sh
sudo chmod a=wrx /.tag/mvt.sh
sudo chmod a=wrx /.tag/cpt.sh
sudo chmod a=wrx /.tag/tag_hierarchy
sudo chmod a=wrx /.tag/paths.txt
sudo chmod a=wrx /.tag/dest.txt
sudo chmod a=wrx /.tag/remplace.txt
sudo chmod a=wrx /.tag

sudo mv upmv /.tag
sudo mv uprm2 /.tag
sudo mv upmv2 /.tag
sudo mv upcp /.tag
sudo mv upcp2 /.tag
echo 'export PATH=$PATH:/.tag' >> ~/.bashrc
echo 'relance le terminal...'
echo "alias mv='mvt.sh'" >> ~/.bashrc
echo "alias cp='cpt.sh'" >> ~/.bashrc
echo "alias rm='rmta.sh'" >> ~/.bashrc
exec bash
echo 'fin installation...'
