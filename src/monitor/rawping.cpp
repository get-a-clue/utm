#include "StdAfx.h"
#include "rawping.h"

namespace utm {

rawping::rawping(void)
{
}


rawping::~rawping(void)
{
}

USHORT rawping::CalcIpChecksum(USHORT* buffer, int size)
{
    unsigned long cksum = 0;
    
    // Sum all the words together, adding the final byte if size is odd
    while (size > 1) {
        cksum += *buffer++;
        size -= sizeof(USHORT);
    }
    if (size) {
        cksum += *(UCHAR*)buffer;
    }

    // Do a little shuffling
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);
    
    // Return the bitwise complement of the resulting mishmash
    return (USHORT)(~cksum);
}

void rawping::InitPingPacket(ICMPHeader* icmp_hdr, USHORT process_id, USHORT seq_no, __int64 tick)
{
    icmp_hdr->type = ICMP_ECHO_REQUEST;
    icmp_hdr->code = 0;
    icmp_hdr->checksum = 0;
    icmp_hdr->id = process_id;
    icmp_hdr->seq = seq_no;
	icmp_hdr->tick = tick;

	char* datapart = (char*)icmp_hdr + sizeof(ICMPHeader);
	memcpy(datapart, "abcdefghijklmnopqrstuvwx", DEFAULT_PACKET_SIZE - sizeof(ICMPHeader));

    icmp_hdr->checksum = CalcIpChecksum((USHORT*)icmp_hdr, DEFAULT_PACKET_SIZE);
}

int rawping::SendPing(SOCKET sd, USHORT current_pid, USHORT seq_no, const char* destaddr, __int64 tick)
{
/*
	int seq_no = 0;
	int packet_size = DEFAULT_PACKET_SIZE;

	DWORD dwPid = GetCurrentProcessId() / 4;
*/
	char tmp[DEFAULT_PACKET_SIZE];
	ICMPHeader* icmp_hdr = (ICMPHeader*)&tmp;
	InitPingPacket(icmp_hdr, current_pid, seq_no, tick);

	sockaddr_in dest;
	dest.sin_addr.s_addr = inet_addr(destaddr);
    dest.sin_family = AF_INET;

    int result = sendto(sd, (const char*)icmp_hdr, DEFAULT_PACKET_SIZE, 0, (const sockaddr*)&dest, sizeof(dest));
	return result;
}

void rawping::ParsePing(const unsigned char *raw_packet /* pointer to buffer with ip v.4 packet */, IPHeader* ip_hdr, ICMPHeader* icmp_hdr)
{
	// "Protocol" field
	ip_hdr->proto = *(raw_packet + 9);

	ip_hdr->total_len = (*(raw_packet + 2) << 8) + *(raw_packet + 3);

	// Identification
	ip_hdr->ident = (*(raw_packet + 4) << 8) + *(raw_packet + 5);

	// TTL
	ip_hdr->ttl = *(raw_packet + 8);

	// "Source IP" field
	ip_hdr->source_ip = *(raw_packet + 12) << 24;
	ip_hdr->source_ip += *(raw_packet + 13) << 16;
	ip_hdr->source_ip += *(raw_packet + 14) << 8;
	ip_hdr->source_ip += *(raw_packet + 15);

	// "Destination IP" field
	ip_hdr->dest_ip = *(raw_packet + 16) << 24;
	ip_hdr->dest_ip += *(raw_packet + 17) << 16;
	ip_hdr->dest_ip += *(raw_packet + 18) << 8;
	ip_hdr->dest_ip += *(raw_packet + 19);

	icmp_hdr->type = *(raw_packet + 20);
	icmp_hdr->code = *(raw_packet + 21);
	icmp_hdr->checksum = (*(raw_packet + 22) << 8) + *(raw_packet + 23);
	icmp_hdr->id = (*(raw_packet + 24) << 8) + *(raw_packet + 25);
	icmp_hdr->seq = (*(raw_packet + 26) << 8) + *(raw_packet + 27);

	DWORD dwLow = (*(raw_packet + 31) << 24) + (*(raw_packet + 30) << 16) + (*(raw_packet + 29) << 8) + *(raw_packet + 28);
	DWORD dwHigh = (*(raw_packet + 35) << 24) + (*(raw_packet + 34) << 16) + (*(raw_packet + 33) << 8) + *(raw_packet + 32);

	__int64 high = dwHigh;
	icmp_hdr->tick = (high << 32) + dwLow;
}

}