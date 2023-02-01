# Override - level00

We've started executing the binary to see what was asking.

It asked a password and if you were wrong the output was: 

**Invalid Password!**

We've disassembled the binary before using:

```objdump -d level00```

and after

```gdb level00```.

We found the scanf call and the cmp call:

```   0x080484e7 <+83>:	cmp    $0x149c,%eax```

from here it was easy to guess that the password was:

```0x149c``` that in decimals is ```5276```.

We've tried and...

**Authenticated!**

we got the flag to get to level01.