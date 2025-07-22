#!/bin/bash
cp sht /bin/
sudo chmod 755 /bin/sht

cp shl /bin/
sudo chmod 755 /bin/shl
chown sht:sht /bin/shl

cp shl-lpasswd /bin/
sudo chmod 700 /bin/shl-lpasswd
chown root:root /bin/shl-lpasswd

cp shl-upasswd /bin/
sudo chmod 700 /bin/shl-upasswd
chown root:root /bin/shl-upasswd

cp shl-mkuser /bin/
sudo chmod 700 /bin/shl-mkuser
chown root:root /bin/shl-mkuser

sudo mkdir -p /etc/sht
sudo cp sht.conf /etc/sht

sudo mkdir -p /etc/sht/templates
sudo cp otp_email.html /etc/sht/templates/otp_email.html

sudo mkdir -p /etc/sht/users

sudo chown -R sht:sht /etc/sht

