# SHT

## SHT Utility

1. Install prerequisites

```bash
sudo apt update
sudo apt install -y build-essential g++ gcc git libtbb-dev libtbb12
```

2. Clone BLAKE3 sources

```bash
git clone https://github.com/BLAKE3-team/BLAKE3.git
cd BLAKE3/c
```

3. Compile objects (C, ASM, and TBB shim)

```bash
g++ -c -O3 -fno-exceptions -fno-rtti -DBLAKE3_USE_TBB -o blake3_tbb.o blake3_tbb.cpp
gcc -c -O3 -DBLAKE3_USE_TBB -o blake3.o blake3.c
gcc -c -O3 -DBLAKE3_USE_TBB -o blake3_dispatch.o blake3_dispatch.c
gcc -c -O3 -DBLAKE3_USE_TBB -o blake3_portable.o blake3_portable.c
gcc -c -O3 -DBLAKE3_USE_TBB -o blake3_sse2_x86-64_unix.o blake3_sse2_x86-64_unix.S
gcc -c -O3 -DBLAKE3_USE_TBB -o blake3_sse41_x86-64_unix.o blake3_sse41_x86-64_unix.S
gcc -c -O3 -DBLAKE3_USE_TBB -o blake3_avx2_x86-64_unix.o blake3_avx2_x86-64_unix.S
gcc -c -O3 -DBLAKE3_USE_TBB -o blake3_avx512_x86-64_unix.o blake3_avx512_x86-64_unix.S
```

4. Create static library

```bash
ar rcs libblake3_tbb.a blake3_tbb.o blake3.o blake3_dispatch.o blake3_portable.o blake3_sse2_x86-64_unix.o blake3_sse41_x86-64_unix.o blake3_avx2_x86-64_unix.o blake3_avx512_x86-64_unix.o
```

5. Build `sht` using the static library

```bash
cd ../..
cp BLAKE3/c/libblake3_tbb.a .
gcc -O3 -o sht sht.c libblake3_tbb.a -lstdc++ -ltbb
```

6. Quick test

```bash
printf 'hello\n' | ./sht
```

7. Optional install

```bash
sudo install -m 0755 ./sht /usr/local/bin/sht
```

## SHL

> This section is for setting up a SHT server using the SHL shell

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

