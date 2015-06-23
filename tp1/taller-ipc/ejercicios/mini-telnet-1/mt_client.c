#include "mt.h"

int main(int argc, char* argv[]) {
    struct in_addr ip;
    struct sockaddr_in cliente;
    char input[MAX_MSG_LENGTH];
    int sock;

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (inet_aton(argv[1], &ip) != 0) {
        cliente.sin_family = AF_INET;
        cliente.sin_addr = ip;
        cliente.sin_port = htons(PORT);

        // Me conecto al servidor
        if (connect(sock, (void *) &cliente, sizeof(cliente)) == -1) {
            perror("Error al conectarse con el servidor");
            exit(1);
        }

        while (printf("> "), fgets(input, MAX_MSG_LENGTH, stdin), !feof(stdin)) {
            sendto(sock, input, strlen(input) + 1, 0, (struct sockaddr *) &cliente, sizeof(cliente));

            if (strncmp(input, END_STRING, MAX_MSG_LENGTH) == 0) {
                break;
            }

        }

    }

    close(sock);
    return 0;
}
