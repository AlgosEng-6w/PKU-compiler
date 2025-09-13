make
file /root/src/sysy.l
head -c 3 /root/src/sysy.l | hexdump -C
cat -A /root/src/sysy.l | head -5
cat -A /root/src/sysy.l | head -5
sed -i '1s/^\xEF\xBB\xBF//' /root/src/sysy.l
dos2unix /root/src/sysy.l
sed -i -e '1s/^\xEF\xBB\xBF//' -e 's/\r$//' /root/src/sysy.l
file /root/src/sysy.l
make clean
make
head -c 50 /root/src/sysy.l | od -c
cat /root/src/sysy.l | head -3 | od -c
exit
make clean
exit
