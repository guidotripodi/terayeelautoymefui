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

		while (fgets(input, MAX_MSG_LENGTH, stdin)) {
			if (strncmp(input, END_STRING, MAX_MSG_LENGTH) == 0) {
				close(sock);
				break;
			}

			sendto(sock, input, strlen(input), 0, (struct sockaddr *) &cliente, sizeof(cliente));
		}

	}

	return 0;
}
