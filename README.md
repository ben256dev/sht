1. Create a shared authorized_keys file

```bash
sudo mkdir -p /etc/ssh/sht_keys
sudo chmod 755 /etc/ssh/sht_keys
sudo nano /etc/ssh/sht_keys/authorized_keys
```

2. Update sshd_config

```text
Match User sht
    AuthorizedKeysFile /etc/ssh/sht_keys/authorized_keys
    PermitUserEnvironment yes
    ForceCommand /bin/customsh
```

Then reload:

```bash
sudo systemctl reload sshd
```

