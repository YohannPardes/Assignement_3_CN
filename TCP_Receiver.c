#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/time.h>

#define BUFFER_SIZE 1024 * 1024
#define MAX_LISTENING 1
#define MAX_FILE_SEND 100


int main(int argc, char* argv[])
{
	char* server_port = argv[2];
	int RECEIVER_PORT = atoi(server_port);
	char* ALGO = argv[4];

	/// opening socket ///
	fprintf(stdout, "Starting Receiver...\n");
	int sock = -1;

	struct sockaddr_in receiver; // to store the receiver address
	struct sockaddr_in sender; // to store the sender address

	socklen_t sender_len = sizeof(sender);

	// Reset receiver and sender structs
	memset(&receiver, 0, sizeof (receiver));
	memset(&sender, 0, sizeof(sender));

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		perror("socket(2)");
		return 1;
	}

	if(setsockopt(sock, SOL_SOCKET, TCP_CONGESTION, &ALGO, sizeof(ALGO)) < 0)
	{
		perror("setsockopt");
		close(sock);
		return 1;
	}

	receiver.sin_addr.s_addr = INADDR_ANY;
	receiver.sin_family = AF_INET; // IPv4

	receiver.sin_port = htons(RECEIVER_PORT);

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

	fprintf(stdout, "Waiting for TCP connection on port %d... \n", RECEIVER_PORT);

    int sender_sock = accept(sock, (struct sockaddr*)&sender, &sender_len);
    if (sender_sock < 0)
    {
        perror("accept(2)");
        close(sock);
        return 1;
    }


    // variable for metrics
    typedef struct run_details {
        unsigned int size;
        int id;
        struct timeval starting_time;
        struct timeval ending_time;
    }RunDetails;
    RunDetails run_list[MAX_FILE_SEND];
    int total_file_size = 0;
	// The receiver listening main loop
    int i = 0;
    int bytes_received = 1;
    int keep_going = 1;
    char buffer[BUFFER_SIZE] = {0};
    unsigned int current_file_size = 0;

    // the main loop for each file
    while(keep_going)
    {
        bytes_received = 0;
        current_file_size = 0;
        run_list[i].id = i + 1;
        gettimeofday(&run_list[i].starting_time, NULL);
        fprintf(stdout, "Sender %s:%d connected, beginning to receiver file.. \n", inet_ntoa(sender.sin_addr), ntohs(sender.sin_port));

        // the loop for one file (we can see the
        do
        {
            // allocating buffer
            bytes_received = recv(sender_sock, buffer, BUFFER_SIZE, 0);
            fprintf(stdout, "received %d bytes from the sender\n", bytes_received);
            if (bytes_received < 0)
            {
                perror("recv(2)");
                close(sender_sock);
                close(sock);
                return 1;
            }

            total_file_size += bytes_received; // incrementing the total data passed
            current_file_size += bytes_received;

        } while(current_file_size < 3 * 1024 * 1024 && bytes_received != 0);

        gettimeofday(&run_list[i].ending_time, NULL);
        run_list[i].size = current_file_size;
        i++;
        fprintf(stdout, "current file size = %d\n", run_list[i].size);

        if (strcmp(buffer, "Close connections.") == 0)
        {
            fprintf(stdout, "Got an exit message from sender.\nEnding connection\n");
            keep_going = 0;
        }
	}

    fprintf(stdout, "-----------------------------------------------\n"
                    "-              * Statistics *              -\n");

    long global_time_ms= 0;
    for (int j = 0; j < i; ++j)
    {
        long seconds = run_list[j].ending_time.tv_sec - run_list[j].starting_time.tv_sec;
        long micros = run_list[j].ending_time.tv_usec - run_list[j].starting_time.tv_usec;
        double mseconds = (double)micros / 1000 + (seconds * 1000);

        double speed = run_list[i].size / ((double)seconds)  ;
        fprintf(stdout,"- Run #%d Data: Time= %.2fms\n",j, mseconds);
        global_time_ms += mseconds;

    }

    double avg_time = global_time_ms / i;
    fprintf(stdout,"Average time: %.2fms\n", avg_time);

    double global_seconds_taken = global_time_ms * 1000;
    double avg_bandwidth = total_file_size / global_seconds_taken;
    fprintf(stdout,"Average bandwidth: %.2fMB/s\n", avg_bandwidth);

    fprintf(stdout, "-----------------------------------------------\n"
                    "Reicever end.\n");
    return 0;
}
