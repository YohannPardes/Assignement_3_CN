#include <stdio.h> // Standard input/output library
#include <arpa/inet.h> // For the in_addr structure and the inet_pton function
#include <sys/socket.h> // For the socket function
#include <unistd.h> // For the close function
#include <string.h> // For the memset function
#include <stdlib.h>
#include <time.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <sys/types.h>


/*
* @brief A random data generator function based on srand() and rand().
* @param size The size of the data to generate (up to 5^32 bytes).
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
#define FILE_SIZE 2 * 1024 * 1024

int main(int argc, char* argv[])
{
	char* SERVER_IP = argv[2];
	char* receiver_port = argv[4];
	char* ALGO = argv[6];

	int RECEIVER_PORT = atoi(receiver_port);

	int sock = -1;
	struct sockaddr_in receiver;

    fprintf(stdout, "Sender started.\n");
	//// PART 1 - creating and reading the File ////
	unsigned int size = 3 * 1024 * 1024;
	char * message = util_generate_random_data(size); // create a 5MB file to read from
    fprintf(stdout, "\nThe current message : %s ---\n", message);

	memset(&receiver, 0, sizeof(receiver));

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		perror("socket(2)");
		free(message);
		return 1;
	}

	if (inet_pton(AF_INET, SERVER_IP, &receiver.sin_addr) <= 0)
	{
		perror("inet_pton(3)");
		close(sock);
		free(message);
		return 1;
	}

	receiver.sin_family = AF_INET;
	receiver.sin_port = htons(RECEIVER_PORT);

	fprintf(stdout, "Connecting to %s : %d...\n", SERVER_IP, RECEIVER_PORT);

	if (connect(sock, (struct sockaddr *)&receiver, sizeof (receiver))< 0)
	{
		perror("connect(2)");
		close(sock);
		free(message);
		return 1;
	}

    // setup congestion control algo
    if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, ALGO, strlen((ALGO))) < 0)
    {
        perror("setsockopt");
        close(sock);
        return 1;
    }

    fprintf(stdout, "Successfully connected to the server!\n"
                                  "Sending message to the server...\n");

	int choice = 0;
	do
	{
		int bytes_sent = send(sock, message, size, 0);
		if (bytes_sent <= 0) {
			perror("send(2)");
			close(sock);
			free(message);
			return 1;
		}

		fprintf(stdout, "Sent %d bytes to the server!\n", bytes_sent);
		fprintf(stdout, "Send the file again ? \n1 - Yes\n"
						"\n2 - No\n");
        do
        {
            fscanf(stdin, "%d", &choice);

        } while (choice != 1 && choice != 2);

        fprintf(stdout, "choice : %d\n", choice); // todo - TEMP

	} while (choice == 1);

	fprintf(stdout, "Sending exit message..\n");
	char * exit_message = "Close connections.";
	int bytes_sent = send(sock, exit_message, strlen(exit_message) + 1, 0);
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

