echo 'désinstallation de la commande tag...'
tag reset
sudo rm -rf /.tag
/bin/cp /etc/skel/.bashrc ~/
exec bash