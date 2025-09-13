make -n
grep -r "TARGET\|EXEC\|koopa" /root/
make
make clean
make
build/compiler -koopa hello.c -o hello.koopa
rm .bash_history
make clean
make
build/compiler -koopa hello.c -o hello.koopa
exit
