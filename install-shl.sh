#!/bin/bash
cp sht /bin/
sudo chmod 755 /bin/sht

cp shl /bin/
sudo chmod 755 /bin/shl
chown sht:sht /bin/shl

for f in shl-lpasswd shl-upasswd shl-mkuser shl-mkalias; do
    cp $f /bin/
    sudo chmod 700 /bin/$f
    chown root:root /bin/$f
done

sudo cp sudoers /etc/sudoers.d/sht 
chown root:root /etc/sudoers.d/sht

sudo mkdir -p /etc/sht
sudo cp sht.conf /etc/sht

sudo mkdir -p /etc/sht/templates
sudo cp otp_email.html /etc/sht/templates/otp_email.html

sudo mkdir -p /etc/sht/users

sudo chown -R sht:sht /etc/sht

