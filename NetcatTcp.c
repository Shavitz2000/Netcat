#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void server(int port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        close(fd);
        exit(1);
    }

    if (listen(fd, 1) < 0)
    {
        perror("listen");
        close(fd);
        exit(1);
    }

    printf("Listening on port %d...\n", port);
    int client_fd = accept(fd, NULL, NULL);
    if (client_fd < 0)
    {
        perror("accept");
        close(fd);
        exit(1);
    }

    char buf[1024];
    while (1)
    {
        ssize_t n = read(client_fd, buf, sizeof(buf));
        if (n <= 0)
            break;
        write(STDOUT_FILENO, buf, n);

        n = read(STDIN_FILENO, buf, sizeof(buf));
        if (n <= 0)
            break;
        write(client_fd, buf, n);
    }

    close(client_fd);
    close(fd);
}

void client(const char *ip, int port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    if (addr.sin_addr.s_addr == INADDR_NONE)
    {
        fprintf(stderr, "Invalid IP address: %s\n", ip);
        close(fd);
        exit(1);
    }
    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        close(fd);
        exit(1);
    }

    printf("Connect to %s:%d\n", ip, port);

    const char *msg = "Hello!\n";
    write(fd, msg, strlen(msg));

    char buf[1024];
    while (1)
    {
        ssize_t n = read(fd, buf, sizeof(buf));
        if (n <= 0)
            break;
        write(STDOUT_FILENO, buf, n);

        n = read(STDIN_FILENO, buf, sizeof(buf));
        if (n <= 0)
            break;
        write(fd, buf, n);
    }

    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc == 3 && strcmp(argv[1], "-l") == 0)
    {                             //-l for server
        int port = atoi(argv[2]); // ASCII TO INT
        server(port);
    }
    else if (argc == 3)
    {
        const char *ip = argv[1];
        int port = atoi(argv[2]);
        client(ip, port);
    }
    else
    {
        perror("Not a valid option");
        return -1;
    }

    return 0;
}
