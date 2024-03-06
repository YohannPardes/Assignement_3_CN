#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

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
	fprintf(stdout, "Starting Receiver...");
	int sock = -1;
	struct sockaddr_in receiver; // to store the receiver address

	struct sockaddr_in sender; // to store the sender address

	socklen_t sender_len = sizeof(sender);

	// store socket for reusing receiver address
	int opt = 1;

	// Reset receiver and sender structs
	memset(&receiver, 0, sizeof (receiver));
	memset(&sender, 0, sizeof(sender));

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		perror("socket(2)");
		return 1;
	}

	if(setsockopt(sock, SOL_SOCKET, TCP_CONGESTION, &opt, sizeof(opt)) < 0)
	{
		perror("setsockopt(2)");
		close(sock);
		return 1;
	}

	receiver.sin_addr.s_addr = INADDR_ANY;
	receiver.sin_family = AF_INET; // IPv4

	receiver.sin_port = htons(SERVER_PORT);

	if (bind(sock, (struct sockaddr*) &receiver, sizeof (receiver)) < 0)
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

	fprintf(stdout, "Waiting for TCP connection on port %d... \n", SERVER_PORT);

	// The receiver listening main loop
	while(1)
	{
		int sender_sock = accept(sock, (struct sockaddr*)&sender, &sender_len);
		if (sender_sock < 0)
		{
			perror("accept(2)");
			close(sock);
			return 1;
		}

		fprintf(stdout, "Sender %s:%d connected, beginning to receiver file.. ", inet_ntoa(sender.sin_addr), ntohs(sender.sin_port));

		// allocating buffer
		char buffer[BUFFER_SIZE] = {0};
		int bytes_received = recv(sender_sock, buffer, BUFFER_SIZE, 0);
		if (bytes_received < 0)
		{
			perror("recv(2)");
			close(sender_sock);
			close(sock);
			return 1;
		}

		if (strcmp(ALGO,"reno"))
		{


		}
		else if (strcmp(ALGO, "cubic"))
		{

		}



		//////// TCP RENO ////////


		fprintf(stdout, "File transfer completed.");


	}


}