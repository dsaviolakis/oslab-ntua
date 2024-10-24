#Challenge 0:
touch .hello_there

#Challenge 1:
chmod -rw .hello_there

#Challenge 2:
killall -s SIGCONT riddle #In new tab

#Challenge 3:
export ANSWER=42 && source ~/.bashrc

#Challenge 4:
mkfifo magic_mirror

#Challenge 5:
./riddle 99>&1

#Challenge 6:
gcc chall6.c -o chall6 && ./chall6

#Challenge 7:
ln .hello_there .hey_there

#Challenge 8:
touch bf00 && gcc chall8.c -o chall8 && ./chall8

#Challenge 9:
nc -lvvp 49842 #In a new tab

#Challenge 10:
touch secret_number && ln secret_number secret_number2

#Challenge 11:
touch secret_file && ./chall11
./riddle #In new tab

#Challenge 12:
./chall12 #In new tab

#Challenge 13:
truncate .hello_there -s 32K #In a new tab

#Challenge 14:
./chall14

#Challenge 15:
gcc -shared -o tier2.so -fPIC tier2.c

#Challenge 16:
#Already in tier2.so

#Challenge 17 :( 
rm tier3.so && gcc -shared -o tier3.so -fPIC tier3.c -ldl

#Challenge 18:

