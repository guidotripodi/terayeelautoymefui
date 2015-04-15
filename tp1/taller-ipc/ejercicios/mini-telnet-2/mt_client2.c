#include "mt.h"

int main(int argc, char* argv[]) {
	struct in_addr ip;
	struct sockaddr_in cliente;
	char input[MAX_MSG_LENGTH];
	char output[MAX_MSG_LENGTH];
	int sock;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (inet_aton(argv[1], &ip) != 0) {
		cliente.sin_family = AF_INET;
		cliente.sin_addr = ip;
		cliente.sin_port = htons(PORT);

		if (connect(sock, (struct sockaddr *) &cliente, sizeof(cliente)) == -1) {
	        perror("conectandose");
	        exit(1);
		}

		/* Leo el comando del usuario */
		while (fgets(input, MAX_MSG_LENGTH, stdin)) {
			/* Envío el comando al servidor. */
	        if (send(sock, input, strlen(input), 0) == -1) {
	            perror("enviando");
	            exit(1);
	        }
			
		    memset(output, 0, sizeof(output));
	        /* Leo el output del comando y lo imprimo en pantalla. */
	        ssize_t n = recv(sock, output, MAX_MSG_LENGTH, 0);
	        
	        if (n < 0) { 
	            perror("recibiendo");
	            exit(1);
	        }
	        output[n] = 0;
	        printf("Respuesta del servidor: \n%s", output);

			if (strncmp(input, END_STRING, MAX_MSG_LENGTH) == 0) {
				close(sock);
				break;
			}

			memset(input, 0, sizeof(input));
		}

	}

	return 0;
}

