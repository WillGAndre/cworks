#include "common.h"

int main(int argc, char *argv[]) {
	client *cln;

	if (geteuid() != 0) {
		printf(
				"\nYou need to be root to run this.\n\nApplication Terminated!\n\n");
		exit(0);
	}

	cln = client_new();
	validate_args(argc, argv, cln);

	printf("\n\nListening packets from: \n");
	printf("============================\n\n");
	printf("Source		:	%s\n", cln->srchost);
	printf("Listening Port	:	%u\n", cln->dest_port);
	printf("Filename	:	%s\n", cln->filename);
	printf("Encoding Type 	: 	TCP Acknowledgement\n\n");

	recv_packets(cln);

	return EXIT_SUCCESS;
}

void recv_packets(client *c) {
	int recv_socket;
	FILE *output;

	if ((output = fopen(c->filename, "wb")) == NULL) {
		printf("recv_packets() fopen: unable to open the file %s.\n",
				c->filename);
		exit(EXIT_FAILURE);
	}

	// reading loop.
	for (;;) {

		// Open socket for reading
		recv_socket = socket(AF_INET, SOCK_RAW, 6);
		if (recv_socket < 0) {
			perror("recv_packets() socket: unable to open the socket.\n");
			exit(EXIT_FAILURE);
		}

		// Listen for return packet on a passive socket
		read(recv_socket, (struct recv_tcp *) &recv_pkt, 9999);

		if ((recv_pkt.tcp.ack == 1)
			&& (recv_pkt.ip.daddr == host_convert(c->srchost)
				&& (recv_pkt.tcp.source == htons(c->dest_port)))) {
			printf("Receiving Data: %c\n", recv_pkt.tcp.ack_seq);
			fprintf(output, "%c", recv_pkt.tcp.ack_seq);
			fflush(output);
		}
		close(recv_socket);
	}
	fclose(output);
}

bool if_exists(char *array[], char *value) {
	int count = 0;
	for (count = 0; count < 10; ++count) {
		if (strcmp(array[count], value) == 0) {
			return true;
		}
	}
	return false;
}

void SystemFatal(char *msg) {
	printf("\n%s\n\n", msg);
	exit(EXIT_FAILURE);
}

void validate_args(int argc, char *argv[], client *c) {
	int count = 0;

	for (count = 1; count < argc; ++count) {
		if (!if_exists(options, argv[count])) {
			printf("\n**Invalid Option: %s\n", argv[count]);
			printf("\nPress ENTER for examples.\n\n");
			getchar();
			print_usage(argv);
			exit(EXIT_FAILURE);
		}

		if (strcmp(argv[count], "-listen_port") == 0) {
			if (if_exists(options, argv[count + 1]) == false) {
				c->dest_port = atoi(argv[count + 1]);
				c->destport = 1;
				count += 1;
			} else {
				SystemFatal("INVALID [value] FOR OPTION -listen_port");
			}
		} else if (strcmp(argv[count], "-source") == 0) {
			if (if_exists(options, argv[count + 1]) == false) {
				strcpy(c->srchost, argv[count + 1]);
				c->source_host = host_convert(argv[count + 1]);
				c->source = 1;
				count += 1;
			} else {
				SystemFatal("INVALID [value] FOR OPTION -source");
			}
		} else if (strcmp(argv[count], "-ofile") == 0) {
			if (if_exists(options, argv[count + 1]) == false) {
				strcpy(c->filename, argv[count + 1]);
				c->file = 1;
				count += 1;
			} else {
				SystemFatal("INVALID [value] FOR OPTION -ofile");
			}
		}
	}
}

client *client_new(void) {
	client *c = malloc(sizeof(client));
	c->source_host = 0;
	c->dest_port = 80;
	c->file = 0;
	c->destport = 0;
	c->source = 0;
	return c;
}

void print_usage(char *argv[]) {
	printf(
			"Covert Transfer Usage: \n%s -source [source_ip] -dest_port port -ofile filename -[encode type]\n\n",
			argv[0]);
	printf(
			"-listen_port [port]	- IP port you want data to go to. [DEFAULT PORT == 80]\n");
	printf(
			"-source [source_ip]	- Host where you want to data to originate from.\n"
					"			  (Source can include a bounce server address).\n");
	printf(
			"-ofile [filename]	- Name of the file to save the decoded data.\n\n");
}

unsigned int host_convert(char *hostname) {
	static struct in_addr i;
	struct hostent *h;
	i.s_addr = inet_addr(hostname);
	if (i.s_addr == -1) {
		h = gethostbyname(hostname);
		if (h == NULL) {
			fprintf(stderr, "cannot resolve %s\n", hostname);
			exit(0);
		}
		bcopy(h->h_addr, (char *) &i.s_addr, h->h_length);
	}
	return i.s_addr;
}
