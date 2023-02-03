#include <unistd.h>

int main(void)
{
    execve("/bin/sh", NULL, NULL);

    return 0;
}
