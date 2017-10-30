rm woody_woodpacker
rm hello
rm packer
rm packer.o
gcc main.c -o woody_woodpacker
gcc -o hello hello.c
nasm -f elf64 -o packer.o packer.s
ld -o packer packer.o
./woody_woodpacker hello packer
./hello
