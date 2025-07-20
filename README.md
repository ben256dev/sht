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

4. Compile all C and assembly sources to objects

```bash
gcc -c -O3 -DBLAKE3_USE_TBB -o blake3.o blake3.c
gcc -c -O3 -DBLAKE3_USE_TBB -o blake3_dispatch.o blake3_dispatch.c
gcc -c -O3 -DBLAKE3_USE_TBB -o blake3_portable.o blake3_portable.c
gcc -c -O3 -DBLAKE3_USE_TBB -o blake3_sse2_x86-64_unix.o blake3_sse2_x86-64_unix.S
gcc -c -O3 -DBLAKE3_USE_TBB -o blake3_sse41_x86-64_unix.o blake3_sse41_x86-64_unix.S
gcc -c -O3 -DBLAKE3_USE_TBB -o blake3_avx2_x86-64_unix.o blake3_avx2_x86-64_unix.S
gcc -c -O3 -DBLAKE3_USE_TBB -o blake3_avx512_x86-64_unix.o blake3_avx512_x86-64_unix.S
```

5. Archive object files into static library

```bash
ar rcs libblake3_tbb.a blake3_tbb.o blake3.o blake3_dispatch.o blake3_portable.o \
    blake3_sse2_x86-64_unix.o blake3_sse41_x86-64_unix.o blake3_avx2_x86-64_unix.o blake3_avx512_x86-64_unix.o
```

6. Use Static Library to build ``sht``

```bash
cd ../..
cp BLAKE3/c/libblake3_tbb.a .
gcc -O3 -o sht sht.c libblake3_tbb.a -lstdc++ -ltbb
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

