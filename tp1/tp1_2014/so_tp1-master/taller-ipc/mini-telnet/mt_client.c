#include "mt.h"

int main(int argc, char* argv[]) {
	int sock;
	struct in_addr inp;
	struct sockaddr_in name;
	char input[MAX_MSG_LENGTH];

	/* Crear socket sobre el que se lee: dominio INET, protocolo UDP (DGRAM). */
	sock = socket(AF_INET, SOCK_DGRAM, 0);

	if (inet_aton(argv[1], &inp) != 0) {
		name.sin_family = AF_INET;
		name.sin_addr = inp;
		name.sin_port = htons(PORT);

		while (fgets(input, MAX_MSG_LENGTH, stdin)) {
			sendto(sock, input, strlen(input), 0, (struct sockaddr *) &name, sizeof(name));
			
			if (strncmp(input, END_STRING, MAX_MSG_LENGTH) == 0) {
				close(sock);
				break;
			}
		}

	}

	return 0;
}