#include "stdafx.h"
#include "ip_header.h"

#include <string.h>
#include <addrip_v4.h>

#include <boost/lexical_cast.hpp>

namespace utm {

ip_header::ip_header(void)
{
}

ip_header::~ip_header(void)
{
}

bool ip_header::parse_raw_buffer(const unsigned char *ippkt)
{
	// "Version" field. It must be 4.
	version = (ippkt[0] >> 4); if (version != 4) return false;

	// IP Header Lenght (in 32-bits words)
	ip_hl = (ippkt[0] & 0x0F);

	// TOS
	tos = ippkt[1];

	// Total packet lenght
	length = (ippkt[2]<<8)+ippkt[3];

	// Identification
	identification = (ippkt[4]<<8)+ippkt[5];

	// TTL
	ttl = ippkt[8];

	// "Protocol" field
	proto = ippkt[9];

	// "Source IP" field
	src_ip_addr.m_addr = (ippkt[12] << 24) + (ippkt[13] << 16) + (ippkt[14] << 8) + ippkt[15];
#ifdef _DEBUG
//	addrip_v4::to_strbuffer(src_ip_addr.m_addr, src_ip_addr.m_addr2);
#endif

	// "Destination IP" field
	dst_ip_addr.m_addr = (ippkt[16] << 24) + (ippkt[17] << 16) + (ippkt[18] << 8) + ippkt[19];
#ifdef _DEBUG
//	addrip_v4::to_strbuffer(dst_ip_addr.m_addr, dst_ip_addr.m_addr2);
#endif

	ftpdata_ip = 0;
	ftpdata_port = 0;
	ftpdata_mode = FTPMODE_NONE;

	// offset for actual data after IP header
	int ipd = ip_hl*4;

	if (proto == 1)
	{
		flags = (ippkt[ipd] << 8) + ippkt[ipd+1];
	};

	if ((proto == 6) || (proto == 17))
	{
		src_port = (ippkt[ipd]<<8) + ippkt[ipd+1];
		dst_port = (ippkt[ipd+2]<<8) + ippkt[ipd+3];
	}
	else
	{
		src_port = 0;
		dst_port = 0;
	};

	if (proto == 6)
	{
		// TCP Sequence number
		seq_num = ippkt[ipd+4] << 24;
		seq_num += ippkt[ipd+5] << 16;
		seq_num += ippkt[ipd+6] << 8;
		seq_num += ippkt[ipd+7];

		// TCP Acknowlegement number
		ack_num = ippkt[ipd+8] << 24;
		ack_num += ippkt[ipd+9] << 16;
		ack_num += ippkt[ipd+10] << 8;
		ack_num += ippkt[ipd+11];

		// Data offset + flags
		flags = ippkt[ipd+12] << 8;
		flags += ippkt[ipd+13];

		tcp_hl = ((ippkt[ipd + 12] >> 4) & 0x0F);

		UINT pktd = ipd + tcp_hl*4;

		if ((src_port == 21) && (length > pktd))
		{
			// Parse control ftp packet from the server.

			const char *srvstr = (const char *)&ippkt[pktd];
			if (strncmp(srvstr, "227 ", 4) == 0)
			{
				// "227 Entering Passive Mode (h1,h2,h3,h4,p1,p2)"
				// Maybe new passive FTP connection ???

				if (*(srvstr + 26) == '(')
				{
					char szTmp[30];
					memset(szTmp, 0, sizeof(szTmp));
					strncpy_s(szTmp, 30, srvstr + 27, 28);

					int cnt = 0;
					for (int j = 0; j < 28; j++)
					{
						if (isdigit(szTmp[j])) continue;
						if (szTmp[j] == ',')
						{
							cnt++;
							continue;
						};
						szTmp[j] = 0;
						break;
					};

					if (cnt == 5)
					{
						int h1 = 0, h2 = 0, h3 = 0, h4 = 0, p1 = 0, p2 = 0;
						sscanf_s(szTmp, "%d,%d,%d,%d,%d,%d", &h1, &h2, &h3, &h4, &p1, &p2);

						ftpdata_ip = h1 << 24;
						ftpdata_ip += h2 << 16;
						ftpdata_ip += h3 << 8;
						ftpdata_ip += h4;
						ftpdata_port = p1 << 8;
						ftpdata_port += p2;
						ftpdata_mode = FTPMODE_PASSIVE;
					}
				};
			};
		};

		if ((dst_port == 21) && (length > pktd))
		{
			// Parse control ftp packet from the client - 
			// Maybe new active FTP connection ???
			// "PORT h1,h2,h3,h4,p1,p2"

			const char *cltstr = (const char *)&ippkt[pktd];
			if (strncmp(cltstr, "PORT ", 5) == 0)
			{
				char szTmp[30];
				memset(szTmp, 0, sizeof(szTmp));
				strncpy_s(szTmp, 30, cltstr + 5, 28);

				int cnt = 0;
				for (int j = 0; j < 28; j++)
				{
					if ((szTmp[j] >= 32) && (szTmp[j] < 127))
					{
						if (isdigit(szTmp[j]))	continue;
						if (szTmp[j] == ',')
						{
							cnt++;
							continue;
						};
					};
					szTmp[j] = 0;
				};

				if (cnt == 5)
				{
					int h1 = 0, h2 = 0, h3 = 0, h4 = 0, p1 = 0, p2 = 0;
					sscanf_s(szTmp, "%d,%d,%d,%d,%d,%d", &h1, &h2, &h3, &h4, &p1, &p2);

					ftpdata_ip = h1 << 24;
					ftpdata_ip += h2 << 16;
					ftpdata_ip += h3 << 8;
					ftpdata_ip += h4;
					ftpdata_port = p1 << 8;
					ftpdata_port += p2;
					ftpdata_mode = FTPMODE_ACTIVE;
				}
			};
		};
	};

	return TRUE;
};

void ip_header::swap_src_dst()
{
	for (int i = 0; i < 6; i++)	std::swap(src_mac[i], dst_mac[i]);
	std::swap(src_ip_addr, dst_ip_addr);
	std::swap(src_port, dst_port);
}

void ip_header::compare(const ip_header& rhs, std::string& result)
{
	std::string dlm(",");
	std::string str;

	if (src_ip_addr != rhs.src_ip_addr)
	{
		str = "src_ip=" + rhs.src_ip_addr.to_string();
	}

	if (dst_ip_addr != rhs.dst_ip_addr)
	{
		if (!str.empty()) str += dlm;
		str += "dest_ip=" + rhs.dst_ip_addr.to_string();
	}

	if (proto != rhs.proto)
	{
		if (!str.empty()) str += dlm;
		str += "proto=" + boost::lexical_cast<std::string>(proto);
	}
	else
	{
		if ((proto == 6) || (proto == 17))
		{
			if (src_port != rhs.src_port)
			{
				if (!str.empty()) str += dlm;
				str += "src_port=" + boost::lexical_cast<std::string>(rhs.src_port);
			}

			if (dst_port != rhs.dst_port)
			{
				if (!str.empty()) str += dlm;
				str += "dest_port=" + boost::lexical_cast<std::string>(rhs.dst_port);
			}
		}
	}

	if (length != rhs.length)
	{
		if (!str.empty()) str += dlm;
		str += "length=" + boost::lexical_cast<std::string>(rhs.length);
	}

	result = str;
}

void ip_header::test_fill_packet(int packet_num)
{
	memset(this, 0, sizeof(ip_header));

	unsigned char sm[6] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5 };	memcpy(src_mac, sm, 6);
	unsigned char dm[6] = { 0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA }; memcpy(dst_mac, dm, 6);
	src_ip_addr = utm::addrip_v4("192.168.1.2");
	dst_ip_addr = utm::addrip_v4("10.0.0.180");
	ip_hl = 5;
	tcp_hl = 0;
	version = 4;

	if (packet_num == 0)
	{
		length = 60;
		proto = 1;
	};
}

}
