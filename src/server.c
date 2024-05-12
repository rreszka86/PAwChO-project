#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <time.h>

void main()
{
    time_t t;
    time(&t);
    struct tm timetm = *localtime(&t);
    char date_buffor[64];
    strftime(date_buffor, sizeof(date_buffor), "%d-%m-%Y %X %Z", &timetm);

    printf("Server start date: %s\n", date_buffor);
    printf("Author: Michal Reszka\n");

    int server_port = 8080;

    int s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {
        AF_INET,
        htons(server_port),
        0};

    bind(s, (struct sockaddr *)&addr, sizeof(addr));

    listen(s, 10);

    printf("Server is listening at port %d\n", server_port);

    char default_page[] = "index.html";
    char error_page[] = "404.html";

    while (1)
    {
        int client_fd = accept(s, 0, 0);

        char buffer[1024] = {0};
        int status = recv(client_fd, buffer, sizeof(buffer), 0);

        if(status == 0){
            printf("Connection closed by peer");
            continue;
        }

        char *f = buffer + 5;
        *strchr(f, ' ') = 0;

        struct stat st;
        int opened_fd = 0;
        char response_buffer[100];
        if (strcmp(f, "") == 0 || strcmp(f, default_page) == 0)
        {
            opened_fd = open(default_page, O_RDONLY);
            stat(default_page, &st);

            int buffer_size = sprintf(response_buffer, "HTTP/1.0 200 OK\nContent-Type: text/html\nContent-Length:%d\r\n\r\n", (int)st.st_size);
            send(client_fd, response_buffer, buffer_size, 0);
        }
        else
        {
            opened_fd = open(error_page, O_RDONLY);
            stat(error_page, &st);

            int buffer_size = sprintf(response_buffer, "HTTP/1.0 404 Not Found\nContent-Type: text/html\nContent-Length:%d\r\n\r\n", (int)st.st_size);
            send(client_fd, response_buffer, buffer_size, 0);
        }

        sendfile(client_fd, opened_fd, 0, st.st_size);

        close(opened_fd);
        close(client_fd);
    }

    close(s);

    printf("Server terminated\n");
}