#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5432	// puerto de conexion

int main(void) {
	int sockfd, new_sockfd;  // descriptores de archivo
	struct sockaddr_in host_addr, client_addr;	// Informacion de las direcciones IP
	socklen_t sin_size;
	int recv_length=1, yes=1;
	char buffer[1024];

	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		perror("Error al crear el socket");

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		perror("Error al agregar la opcion SO_REUSEADDR en setsockopt");
	
	host_addr.sin_family = AF_INET;		 // 
	host_addr.sin_port = htons(PORT);	 //
	host_addr.sin_addr.s_addr = INADDR_ANY; // Asigno mi IPP
	memset(&(host_addr.sin_zero), '\0', 8); // El resto de la estructura en 0s

	if (bind(sockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr)) == -1)
		perror("Error haciendo el bind");

	if (listen(sockfd, 5) == -1)
		perror("Error al escuchar en el socket");

	while(1) {    // Accept loop
		sin_size = sizeof(struct sockaddr_in);
		new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
		if(new_sockfd == -1)
			perror("Error al aceptar la conexion");
		printf("server: Conexion aceptada desde %s desde  %d\n",inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		send(new_sockfd, ":v\n", 13, 0);
		recv_length = recv(new_sockfd, &buffer, 1024, 0);
		while(recv_length > 0) {
			printf("RECV: %d bytes\n", recv_length);
			printf("Recibiendo: %s\n",buffer);
			if (strcmp(buffer,"cifra\n")==0){
				send(new_sockfd, "que cifro?\n", 13, 0);
			}
			recv_length = recv(new_sockfd, &buffer, 1024, 0);
		}
		close(new_sockfd);
	}
	return 0;
}
