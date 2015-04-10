#include "mt.h"
int main()
{
	int sock, s1, t;
	struct sockaddr_in name;
	char buf[MAX_MSG_LENGTH];
	/* Crear socket sobre el que se lee: dominio INET, protocolo UDP (DGRAM). */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("abriendo socket");
		exit(1);
	}
	/* Crear nombre, usamos INADDR_ANY para indicar que cualquiera puede enviar aquí. */
	name.sin_family = AF_INET;
	name.sin_addr.s_addr = INADDR_ANY;
	name.sin_port = htons(PORT);
	if (bind(sock, (void*) &name, sizeof(name))) {
		perror("binding datagram socket");
		exit(1);
	}

	/* Escuchar en el socket y permitir 5 conexiones en espera. */
    if (listen(sock, 1) < 0) {
        perror("escuchando");
        exit(1);
    }

	/* Aceptar una conexión entrante. */
    t = sizeof(name);
    if ((s1 = accept(sock, (struct sockaddr*) &name, (socklen_t*) &t)) == -1) {
        perror("aceptando la conexión entrante");
        exit(1);
    }

	/* Recibimos mensajes hasta que alguno sea el que marca el final. */
	for (;;) {
		ssize_t n = recv(s1, buf, MAX_MSG_LENGTH, 0);

        if (n < 0) { 
            perror("Recibiendo");
            close(s1);
            close(sock);
            exit(1);
        }

        buf[n] = 0;
        if (strncmp(buf, END_STRING, MAX_MSG_LENGTH) == 0) break;

		/* Reemplazo stdout y stderr por el socket de escritura. */
        dup2(s1, STDOUT_FILENO);
        dup2(s1, STDERR_FILENO);

        /* Ejecuto el comando recibido. */
        system(buf);

        /* Fuerzo la escritura de los buffers de stdout y stderr. */
        fflush(stdout);
        fflush(stderr);
	}
	
	close(s1);
	close(sock);

	return 0;
}

