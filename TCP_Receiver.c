#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <stdio.h>

#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1460
#define MAX_WAIT_TIME = 2 // seconds
#define MAX_LISTENING 1


int main(int argc, char* argv[])
{
	char* server_port = argv[0];
	int SERVER_PORT = atoi(server_port);
	char* ALGO = argv[1];

	/// opening socket ///
	int sock = -1;
	struct sockaddr_in server; // to store the server address

	struct sockaddr_in sender; // to store the sender address

	socklen_t client_len = sizeof(sender);

	// store socket for reusing server address
	int opt = 1;

	// Reset server and client strucs
	memset(&server, 0, sizeof (server));
	memset(&sender, 0, sizeof(sender));

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		perror("socket(2)");
		return 1;
	}

	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		perror("setsockopt(2)");
		close(sock);
		return 1;
	}

	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_family = AF_INET;

	server.sin_port = htons(SERVER_PORT);

	if (bind(socl, (truct sockaddr*) &server, sizeof (server)) < 0)
	{
		perror("bind(2)");
		close(sock);
		return 1;
	}

	if (listen(sock, MAX_LISTENING) < 0)
	{
		perror("listen(2)");
		close(sock);
		return 1;
	}

	fprintf(stdout, "Listening for incoming connections on port %d... \n", SERVER_PORT);



}