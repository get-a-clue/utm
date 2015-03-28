#pragma once
#include <utm.h>

#ifdef UTM_WIN
#pragma comment(lib, "ws2_32.lib")
#endif

// ICMP packet types
#define ICMP_ECHO_REPLY 0
#define ICMP_DEST_UNREACH 3
#define ICMP_TTL_EXPIRE 11
#define ICMP_ECHO_REQUEST 8

// Minimum ICMP packet size, in bytes
#define ICMP_MIN 8

#define DEFAULT_PACKET_SIZE 40
#define DEFAULT_TTL 30
#define MAX_PING_DATA_SIZE 1024
#define MAX_PING_PACKET_SIZE (MAX_PING_DATA_SIZE + sizeof(IPHeader))

namespace utm {

// The IP header
struct IPHeader {
    BYTE h_len:4;           // Length of the header in dwords
    BYTE version:4;         // Version of IP
    BYTE tos;               // Type of service
    USHORT total_len;       // Length of the packet in dwords
    USHORT ident;           // unique identifier
    USHORT flags;           // Flags
    BYTE ttl;               // Time to live
    BYTE proto;             // Protocol number (TCP, UDP etc)
    USHORT checksum;        // IP checksum
    ULONG source_ip;
    ULONG dest_ip;
};

// ICMP header
struct ICMPHeader {
    BYTE type;          // ICMP packet type
    BYTE code;          // Type sub code
    USHORT checksum;
    USHORT id;
    USHORT seq;
	__int64 tick;
};

class rawping
{
public:
	rawping(void);
	~rawping(void);

private:
	static USHORT CalcIpChecksum(USHORT* buffer, int size);
	static void InitPingPacket(ICMPHeader* icmp_hdr, USHORT process_id, USHORT seq_no, __int64 tick);

public:
	static int SendPing(SOCKET sd, USHORT current_pid, USHORT seq_no, const char* destaddr, __int64 tick);
	static void ParsePing(const unsigned char *raw_packet, IPHeader* ip_hdr, ICMPHeader* icmp_hdr);
};

}