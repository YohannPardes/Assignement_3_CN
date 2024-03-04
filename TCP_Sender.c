#include <stdio.h> // Standard input/output library
#include <arpa/inet.h> // For the in_addr structure and the inet_pton function
#include <sys/socket.h> // For the socket function
#include <unistd.h> // For the close function
#include <string.h> // For the memset function
#include <stdlib.h>
#include <time.h>

/*
* @brief A random data generator function based on srand() and rand().
* @param size The size of the data to generate (up to 2^32 bytes).
* @return A pointer to the buffer.
*/
char *util_generate_random_data(unsigned int size) {
	char *buffer = NULL;
	// Argument check.
	if (size == 0)
		return NULL;
	buffer = (char *)calloc(size, sizeof(char));
	// Error checking.
	if (buffer == NULL)
		return NULL;
	// Randomize the seed of the random number generator.
	srand(time(NULL));
	for (unsigned int i = 0; i < size; i++)
		*(buffer + i) = ((unsigned int)rand() % 256);
	return buffer;
}

#define BUFFER_SIZE 1460

int main(int argc, char* argv[])
{
	char* SERVER_IP = argv[0];
	char* server_port = argv[1]; // todo - convert to int
	char* ALGO = argv[2];

	int SERVER_PORT = 8080; // todo

	int sock = -1;
	struct sockaddr_in receiver;

	//// PART 1 - creating and reading the File ////
	char * message = util_generate_random_data(3145728); // create a 3MB file to read from

	char buffer[BUFFER_SIZE] = {0};

	memset(&receiver, 0, sizeof(receiver));

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		perror("socket(2)");
		free(message);
		return 1;
	}

	if (inet_pton(AF_INET, SERVER_IP, &receiver) <= 0)
	{
		perror("inet_pton(3)");
		close(sock);
		free(message);
		return 1;
	}

	receiver.sin_family = AF_INET;
	receiver.sin_port = htons(SERVER_PORT);

	fprintf(stdout, "Connecting to %s:%d...\n", SERVER_IP, SERVER_PORT);

	if (connect(sock, (struct sockaddr *)&receiver, sizeof (receiver))< 0)
	{
		perror("connect(2)");
		close(sock);
		free(message);
		return 1;
	}

	fprintf(stdout, "Successfully connected to the server!\n"
					"Sending message to the server: %s\n", message);

	int choice;
	do
	{
		int bytes_sent = send(sock, message, strlen(message) + 1, 0);
		if (bytes_sent <= 0) {
			perror("send(2)");
			close(sock);
			free(message);
			return 1;
		}

		fprintf(stdout, "Sent %d bytes to the server!\n"
						"Waiting for the server to respond...\n", bytes_sent); // todo - check if we need a response from the server

		fprintf(stdout, "Send the file again ? \n1 - Yes"
						"\n2 - No");
		choice = scanf("%d", &choice);

	} while (choice == 1);

	fprintf(stdout, "Sending exit message..");
	char * exit_message = "Close connections.";
	int bytes_sent = send(sock, message, strlen(exit_message) + 1, 0);
	if (bytes_sent <= 0) {
		perror("send(2)");
		close(sock);
		free(message);
		return 1;
	}

	fprintf(stdout, "Sent exit message successfully!\n");

	close(sock);
	fprintf(stdout, "Connection closed!\n");

	free(message);
	return 0;
}

