# SHT

## SHT Utility

1. Install prerequisites

```bash
sudo apt update
sudo apt install build-essential g++ libtbb-dev libtbb12
```

2. Clone the BLAKE3 repo

```bash
git clone https://github.com/BLAKE3-team/BLAKE3.git
cd BLAKE3/c
```

3. Compile the TBB C++ source

```bash
g++ -c -O3 -fno-exceptions -fno-rtti -DBLAKE3_USE_TBB -o blake3_tbb.o blake3_tbb.cpp
```

4. Build the example with multithreading enabled

```bash
gcc -O3 -DBLAKE3_USE_TBB -o example_tbb \
    blake3_tbb.o example_tbb.c blake3.c blake3_dispatch.c blake3_portable.c \
    blake3_sse2_x86-64_unix.S blake3_sse41_x86-64_unix.S \
    blake3_avx2_x86-64_unix.S blake3_avx512_x86-64_unix.S \
    -lstdc++ -ltbb
```

5. (Optional) Make a test file

```bash
fallocate -l 1G testfile.bin
```

6. Run the example

```bash
./example_tbb testfile.bin
```

## SHL

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

