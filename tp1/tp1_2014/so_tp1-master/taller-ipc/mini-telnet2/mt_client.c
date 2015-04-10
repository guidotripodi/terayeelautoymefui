#include "mt.h"

int main(int argc, char* argv[]) {
	int sock;
	struct in_addr inp;
	struct sockaddr_in name;
	char input[MAX_MSG_LENGTH];
	char output[MAX_MSG_LENGTH];

	/* Crear socket sobre el que se lee: dominio INET, protocolo UDP (DGRAM). */
	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (inet_aton(argv[1], &inp) != 0) {
		name.sin_family = AF_INET;
		name.sin_addr = inp;
		name.sin_port = htons(PORT);

	   	/* Conectarse. */
	    if (connect(sock, (struct sockaddr *)&name, sizeof(name)) == -1) {
	        perror("conectandose");
	        exit(1);
	    }

		for(;;) {
	        /* Leo el comando ingresado por el usuario. */
	        memset(input, 0, sizeof(input));
	        fgets(input, MAX_MSG_LENGTH, stdin);

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
				break;
			}
	    }

	}

	close(sock);

	return 0;
}