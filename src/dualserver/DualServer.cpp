/**************************************************************************
*   Copyright (C) 2005 by Achal Dhir                                      *
*   achaldhir@gmail.com                                                   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
// Dual Service.cpp

#include "stdafx.h"

#include <stdio.h>
#include <winsock2.h>
#include <time.h>
#include <tchar.h>
#include <ws2tcpip.h>
#include <limits.h>
#include <iphlpapi.h>
#include <process.h>
#include <math.h>
#include "DualServer.h"
#include "dhcpleaselist.h"

#include <map>

namespace utm {

CDualServer::CDualServer()
{
	DnsServerPort = 53;
	m_bUseDnsCache = TRUE;
	m_bUseDnsForwarders = TRUE;
	m_bLoop = TRUE;
	m_pszFlagFolder = NULL;
	m_pWildDns = NULL;

	kRunning = true;

    stopServiceEvent = 0;
    cacheInd = 0;
    newInd = 0;
    verbatim = false;
    dhcpService = false;
    dnsService = true;
    strcpy(arpa, ".in-addr.arpa");

    MyPreInit();
}

CDualServer::~CDualServer()
{
	if (pAlexDnsServers != NULL)
		free(pAlexDnsServers);

	if (pAlexDnsAllowedHosts != NULL)
		free(pAlexDnsAllowedHosts);

	if (pAlexDnsWildHosts != NULL)
		free(pAlexDnsWildHosts);

	if (pAlexDhcpOptions != NULL)
		free(pAlexDhcpOptions);

	if (pAlexDhcpRange != NULL)
		free(pAlexDhcpRange);

}

void CDualServer::MyPreInit()
{
	pIpNoListenOn = NULL;

    pclog = NULL;
    DnsServerPort = 53;
	default_loglevel = 0;
	cfig.dnsLogLevel = default_loglevel;
	cfig.dhcpLogLevel = default_loglevel;

	pAlexDnsServers = NULL;
	pAlexDnsAllowedHosts = NULL;
	pAlexDnsWildHosts = NULL;

	pAlexDhcpOptions = NULL;
	pAlexDhcpRange = NULL;

	AlexDhcpStaticHosts.clear();
}

void CDualServer::MySetDnsServerPort(int port)
{
    DnsServerPort = port;
}

void CDualServer::MySetCallbackLogMess(pCallbackLogMess cb)
{
	pclog = cb;
}

void CDualServer::MySetDefaultLogLevel(int level)
{
	default_loglevel = level;
	cfig.dnsLogLevel = level;
	cfig.dhcpLogLevel = level;
}

void CDualServer::MySetDnsServers(char* p)
{
	if (pAlexDnsServers != NULL)
	{
		free(pAlexDnsServers);
	}

	int len = strlen(p);
	pAlexDnsServers = (char *)malloc(len + 1);
	strncpy_s(pAlexDnsServers, len + 1, p, _TRUNCATE);
}

void CDualServer::MySetDnsAllowedHosts(char *p)
{
	if (pAlexDnsAllowedHosts != NULL)
	{
		free(pAlexDnsAllowedHosts);
	}

	int len = strlen(p);
	pAlexDnsAllowedHosts = (char *)malloc(len + 1);
	strncpy_s(pAlexDnsAllowedHosts, len + 1, p, _TRUNCATE);
}

void CDualServer::MySetDnsWildHosts(char *p)
{
	if (pAlexDnsWildHosts != NULL)
	{
		free(pAlexDnsWildHosts);
	}
	
	int len = strlen(p);
	pAlexDnsWildHosts = (char *)malloc(len + 1);
	strncpy_s(pAlexDnsWildHosts, len + 1, p, _TRUNCATE);
}

void CDualServer::MySetDhcpOptions(char *p)
{
	if (pAlexDhcpOptions != NULL)
	{
		free(pAlexDhcpOptions);
	}

	int len = strlen(p);
	pAlexDhcpOptions = (char *)malloc(len + 1);
	strncpy_s(pAlexDhcpOptions, len + 1, p, _TRUNCATE);
}

void CDualServer::MySetDhcpRange(char *p)
{
	if (pAlexDhcpRange != NULL)
	{
		free(pAlexDhcpRange);
	}

	int len = strlen(p);
	pAlexDhcpRange = (char *)malloc(len + 1);
	strncpy_s(pAlexDhcpRange, len + 1, p, _TRUNCATE);
}

void CDualServer::MyServiceDualMain()
{
	int nostatic = 0;
	init(NULL, &nostatic);

	if (nostatic < 2)
	{
		data9 dhcpr;
		data5 dnsr;
		tv.tv_sec = 2;
		tv.tv_usec = 0;

		stopServiceEvent = CreateEvent(0, TRUE, FALSE, 0);

		do
		{
			network.busy = false;

			if (!network.dhcpConn[0].ready && !network.dnsUdpConn[0].ready)
			{
				Sleep(1000);
				continue;
			}

			if (!network.ready)
			{
				Sleep(1000);
				continue;
			}

			FD_ZERO(&readfds);

			if (dhcpService)
			{
				if (network.httpConn.ready)
					FD_SET(network.httpConn.sock, &readfds);

				for (int i = 0; i < MAX_SERVERS && network.dhcpConn[i].loaded; i++)
					if (network.dhcpConn[i].ready)
						FD_SET(network.dhcpConn[i].sock, &readfds);

				if (network.dhcpReplConn.ready)
					FD_SET(network.dhcpReplConn.sock, &readfds);
			}

			if (dnsService)
			{
				for (int i = 0; i < MAX_SERVERS && network.dnsUdpConn[i].loaded; i++)
				{
					if (network.dnsUdpConn[i].ready)
					{
						FD_SET(network.dnsUdpConn[i].sock, &readfds);
					}
				}

				for (int i = 0; i < MAX_SERVERS && network.dnsTcpConn[i].loaded; i++)
				{
					if (network.dnsTcpConn[i].ready)
					{
						FD_SET(network.dnsTcpConn[i].sock, &readfds);
					}
				}

				if (network.forwConn.ready)
				{
					FD_SET(network.forwConn.sock, &readfds);
				}
			}

			if (select(network.maxFD, &readfds, NULL, NULL, &tv))
			{
				if (network.ready)
				{
					network.busy = true;

					if (dhcpService)
					{
#ifdef _DEBUG
						if (network.httpConn.ready && FD_ISSET(network.httpConn.sock, &readfds))
						{
							data19 *req = (data19*)calloc(1, sizeof(data19));

							if (req)
							{
								req->sockLen = sizeof(req->remote);
								req->sock = accept(network.httpConn.sock, (sockaddr*)&req->remote, &req->sockLen);

								if (req->sock == INVALID_SOCKET)
								{
									sprintf(logBuff, "Accept Failed, Error=%u\n", WSAGetLastError());
									logDHCPMess(logBuff, 2);
									free(req);
								}
								else
									procHTTP(req);
							}
							else
							{
								sprintf(logBuff, "Memory Error");
								logDHCPMess(logBuff, 0);
							}
						}
#endif
						if (network.dhcpReplConn.ready && FD_ISSET(network.dhcpReplConn.sock, &readfds))
						{
							dhcpr.sockLen = sizeof(dhcpr.remote);
							dhcpr.bytes = recvfrom(network.dhcpReplConn.sock,
												   dhcpr.raw,
												   sizeof(dhcpr.raw),
												   0,
												   (sockaddr*)&dhcpr.remote,
												   &dhcpr.sockLen);
						}
					}

					for (int i = 0; i < MAX_SERVERS && network.dhcpConn[i].loaded; i++)
					{
						if (network.dhcpConn[i].ready && FD_ISSET(network.dhcpConn[i].sock, &readfds) && gdmess(&dhcpr, i) && sdmess(&dhcpr))
							alad(&dhcpr);
					}

					if (dnsService)
					{
						for (int i = 0; i < MAX_SERVERS && network.dnsUdpConn[i].loaded; i++)
						{
							if (network.dnsUdpConn[i].ready && FD_ISSET(network.dnsUdpConn[i].sock, &readfds))
							{
								if (gdnmess(&dnsr, i))
								{
									if (dnsr.dnsp->header.rcode == RCODE_NOTIMPL || dnsr.dnsp->header.rcode == RCODE_REFUSED)
										sdnmess(&dnsr);
									else if (scanloc(&dnsr))
									{
										if (htons(dnsr.dnsp->header.ancount))
										{
											if (cfig.dnsLogLevel == 2)
											{
												if (dnsr.qtype == DNS_TYPE_SOA)
													sprintf(logBuff, "SOA Sent for zone %s", dnsr.query);
												else if (dnsr.qtype == DNS_TYPE_NS)
													sprintf(logBuff, "NS Sent for zone %s", dnsr.query);
												else if (dnsr.cache.dataType == CACHED)
													sprintf(logBuff, "%s resolved from Cache to %s", dnsr.query, getResult(&dnsr));
												else
													sprintf(logBuff, "%s resolved Locally to %s", dnsr.query, getResult(&dnsr));

												logDNSMess(&dnsr, logBuff, 2);
											}
										}
										else if (dnsr.dnsp->header.rcode == RCODE_NAMEERROR || dnsr.dnsp->header.rcode == RCODE_NOERROR)
										{
											dnsr.dnsp->header.rcode = RCODE_NAMEERROR;
											if (cfig.dnsLogLevel == 2)
											{
												sprintf(logBuff, "%s not found", dnsr.query);
												logDNSMess(&dnsr, logBuff, 2);
											}
										}
										sdnmess(&dnsr);
									}
									else
									{
										if (m_bUseDnsForwarders)
										{
											if (!fdnmess(&dnsr))
												sdnmess(&dnsr);
										}
									}
								}
							}
						}

						for (int i = 0; i < MAX_SERVERS && network.dnsTcpConn[i].loaded; i++)
						{
							if (network.dnsTcpConn[i].ready && FD_ISSET(network.dnsTcpConn[i].sock, &readfds))
							{
								dnsr.sockInd = i;
								dnsr.sockLen = sizeof(dnsr.remote);
								errno = 0;
								dnsr.sock = accept(network.dnsTcpConn[i].sock, (sockaddr*)&dnsr.remote, &dnsr.sockLen);

								if (dnsr.sock == INVALID_SOCKET)
								{
									if (cfig.dnsLogLevel)
									{
										sprintf(logBuff, "Accept Failed, Error=%u", WSAGetLastError());
										logDHCPMess(logBuff, 1);
									}
								}
								else
									procTCP(&dnsr);
							}
						}

						if (network.forwConn.ready && FD_ISSET(network.forwConn.sock, &readfds))
						{
							if (frdnmess(&dnsr))
							{
								sdnmess(&dnsr);

								if (cfig.dnsLogLevel == 2)
								{
									if (dnsr.cache.dnsIndex < MAX_SERVERS)
									{
										if (dnsr.dnsp->header.ancount)
										{
											if (getResult(&dnsr))
												sprintf(logBuff, "%s resolved from Forwarding server as %s", dnsr.query, tempbuff);
											else
												sprintf(logBuff, "%s resolved from Forwarding server", dnsr.query);
										}
										else
											sprintf(logBuff, "%s not found by Forwarding Server", dnsr.query);
									}
									else
									{
										if (dnsr.dnsp->header.ancount)
										{
											if (getResult(&dnsr))
												sprintf(logBuff, "%s resolved from Child Server as %s", dnsr.query, tempbuff);
											else
												sprintf(logBuff, "%s resolved from Child Server", dnsr.query);
										}
										else
											sprintf(logBuff, "%s not found by Child Server", dnsr.query);
									}

									logDNSMess(&dnsr, logBuff, 2);
								}
							}
						}
					}
				}
			}

			cacheInd = newInd;
			checkSize(cacheInd);
		}
		while (WaitForSingleObject(stopServiceEvent, 0) == WAIT_TIMEOUT);

		sprintf(logBuff, "Closing Network Connections...");
		logMess(logBuff, 1);
		closeConn();
		sprintf(logBuff, "Dual Server Stopped !\n");
		logMess(logBuff, 1);

		if (cfig.logfile)
		{
			fclose(cfig.logfile);
			cfig.logfile = 0;
		}

		WSACleanup();

		CloseHandle(stopServiceEvent);
		stopServiceEvent = 0;
	}
}


void CDualServer::closeConn()
{
	if (network.httpConn.ready)
		closesocket(network.httpConn.sock);

    if (dhcpService)
    {
        for (int i = 0; i < MAX_SERVERS && network.dhcpConn[i].loaded; i++)
        	if (network.dhcpConn[i].ready)
            	closesocket(network.dhcpConn[i].sock);

        if (cfig.replication && network.dhcpReplConn.ready)
            closesocket(network.dhcpReplConn.sock);
    }

    if (dnsService)
    {
        for (int i = 0; i < MAX_SERVERS && network.dnsUdpConn[i].loaded; i++)
        	if (network.dnsUdpConn[i].ready)
           		closesocket(network.dnsUdpConn[i].sock);

        for (int i = 0; i < MAX_SERVERS && network.dnsTcpConn[i].loaded; i++)
        	if (network.dnsTcpConn[i].ready)
            	closesocket(network.dnsTcpConn[i].sock);

        if (network.forwConn.ready)
        	closesocket(network.forwConn.sock);
    }
}

void CDualServer::runProg()
{
	verbatim = true;

	data9 dhcpr;
	data5 dnsr;
	tv.tv_sec = 20;
	tv.tv_usec = 0;

	do
	{
		network.busy = false;

		if (!network.dhcpConn[0].ready && !network.dnsUdpConn[0].ready)
		{
			Sleep(1000);
			continue;
		}

		if (!network.ready)
		{
			Sleep(1000);
			continue;
		}

		FD_ZERO(&readfds);

		if (dhcpService)
		{
			if (network.httpConn.ready)
				FD_SET(network.httpConn.sock, &readfds);

			for (int i = 0; i < MAX_SERVERS && network.dhcpConn[i].loaded; i++)
				if (network.dhcpConn[i].ready)
					FD_SET(network.dhcpConn[i].sock, &readfds);

			if (network.dhcpReplConn.ready)
				FD_SET(network.dhcpReplConn.sock, &readfds);
		}

		if (dnsService)
		{
			for (int i = 0; i < MAX_SERVERS && network.dnsUdpConn[i].loaded; i++)
				if (network.dhcpConn[i].ready)
					FD_SET(network.dnsUdpConn[i].sock, &readfds);

			for (int i = 0; i < MAX_SERVERS && network.dnsTcpConn[i].loaded; i++)
				if (network.dhcpConn[i].ready)
					FD_SET(network.dnsTcpConn[i].sock, &readfds);

			if (network.forwConn.ready)
				FD_SET(network.forwConn.sock, &readfds);
		}

		if (select(network.maxFD, &readfds, NULL, NULL, &tv))
		{
			if (network.ready)
			{
				network.busy = true;

				if (dhcpService)
				{
#ifdef _DEBUG
					if (network.httpConn.ready && FD_ISSET(network.httpConn.sock, &readfds))
					{
						data19 *req = (data19*)calloc(1, sizeof(data19));

						if (req)
						{
							req->sockLen = sizeof(req->remote);
							req->sock = accept(network.httpConn.sock, (sockaddr*)&req->remote, &req->sockLen);

							if (req->sock == INVALID_SOCKET)
							{
								sprintf(logBuff, "Accept Failed, Error=%u\n", WSAGetLastError());
								logDHCPMess(logBuff, 2);
								free(req);
							}
							else
								procHTTP(req);
						}
						else
						{
							sprintf(logBuff, "Memory Error");
							logDHCPMess(logBuff, 0);
						}
					}
#endif
					if (network.dhcpReplConn.ready && FD_ISSET(network.dhcpReplConn.sock, &readfds))
					{
						dhcpr.sockLen = sizeof(dhcpr.remote);
						dhcpr.bytes = recvfrom(network.dhcpReplConn.sock,
											   dhcpr.raw,
											   sizeof(dhcpr.raw),
											   0,
											   (sockaddr*)&dhcpr.remote,
											   &dhcpr.sockLen);
					}
				}

				for (int i = 0; i < MAX_SERVERS && network.dhcpConn[i].loaded; i++)
				{
					if (network.dhcpConn[i].ready && FD_ISSET(network.dhcpConn[i].sock, &readfds) && gdmess(&dhcpr, i) && sdmess(&dhcpr))
						alad(&dhcpr);
				}

				if (dnsService)
				{
					for (int i = 0; i < MAX_SERVERS && network.dnsUdpConn[i].loaded; i++)
					{
						if (network.dnsUdpConn[i].ready && FD_ISSET(network.dnsUdpConn[i].sock, &readfds))
						{
							if (gdnmess(&dnsr, i))
							{
								if (dnsr.dnsp->header.rcode == RCODE_NOTIMPL || dnsr.dnsp->header.rcode == RCODE_REFUSED)
									sdnmess(&dnsr);
								else if (scanloc(&dnsr))
								{
									if (htons(dnsr.dnsp->header.ancount))
									{
										if (cfig.dnsLogLevel == 2)
										{
											if (dnsr.qtype == DNS_TYPE_SOA)
												sprintf(logBuff, "SOA Sent for zone %s", dnsr.query);
											else if (dnsr.qtype == DNS_TYPE_NS)
												sprintf(logBuff, "NS Sent for zone %s", dnsr.query);
											else if (dnsr.cache.dataType == CACHED)
												sprintf(logBuff, "%s resolved from Cache to %s", dnsr.query, getResult(&dnsr));
											else
												sprintf(logBuff, "%s resolved Locally to %s", dnsr.query, getResult(&dnsr));

											logDNSMess(&dnsr, logBuff, 2);
										}
									}
									else if (dnsr.dnsp->header.rcode == RCODE_NAMEERROR || dnsr.dnsp->header.rcode == RCODE_NOERROR)
									{
										dnsr.dnsp->header.rcode = RCODE_NAMEERROR;
										if (cfig.dnsLogLevel == 2)
										{
											sprintf(logBuff, "%s not found", dnsr.query);
											logDNSMess(&dnsr, logBuff, 2);
										}
									}
									sdnmess(&dnsr);
								}
								else if (!fdnmess(&dnsr))
									sdnmess(&dnsr);
							}
						}
					}

					for (int i = 0; i < MAX_SERVERS && network.dnsTcpConn[i].loaded; i++)
					{
						if (network.dnsTcpConn[i].ready && FD_ISSET(network.dnsTcpConn[i].sock, &readfds))
						{
							dnsr.sockInd = i;
							dnsr.sockLen = sizeof(dnsr.remote);
							errno = 0;
							dnsr.sock = accept(network.dnsTcpConn[i].sock, (sockaddr*)&dnsr.remote, &dnsr.sockLen);

							if (dnsr.sock == INVALID_SOCKET)
							{
								if (cfig.dnsLogLevel)
								{
									sprintf(logBuff, "Accept Failed, Error=%u", WSAGetLastError());
									logDHCPMess(logBuff, 1);
								}
							}
							else
								procTCP(&dnsr);
						}
					}

					if (network.forwConn.ready && FD_ISSET(network.forwConn.sock, &readfds))
					{
						if (frdnmess(&dnsr))
						{
							sdnmess(&dnsr);

							if (cfig.dnsLogLevel == 2)
							{
								if (dnsr.cache.dnsIndex < MAX_SERVERS)
								{
									if (dnsr.dnsp->header.ancount)
									{
										if (getResult(&dnsr))
											sprintf(logBuff, "%s resolved from Forwarding server as %s", dnsr.query, tempbuff);
										else
											sprintf(logBuff, "%s resolved from Forwarding server", dnsr.query);
									}
									else
										sprintf(logBuff, "%s not found by Forwarding Server", dnsr.query);
								}
								else
								{
									if (dnsr.dnsp->header.ancount)
									{
										if (getResult(&dnsr))
											sprintf(logBuff, "%s resolved from Child Server as %s", dnsr.query, tempbuff);
										else
											sprintf(logBuff, "%s resolved from Child Server", dnsr.query);
									}
									else
										sprintf(logBuff, "%s not found by Child Server", dnsr.query);
								}

								logDNSMess(&dnsr, logBuff, 2);
							}
						}
					}
				}
			}
		}

		cacheInd = newInd;
		checkSize(cacheInd);
	}
	while (true);

	kRunning = false;
    sprintf(logBuff, "Closing Network Connections...");
    logMess(logBuff, 1);
	closeConn();
    sprintf(logBuff, "Dual Server Stopped !\n");
    logMess(logBuff, 1);

	if (cfig.logfile)
	{
		fclose(cfig.logfile);
		cfig.logfile = 0;
	}

	WSACleanup();
}

bool CDualServer::chkQu(char *query)
{
	if (strlen(query) >= UCHAR_MAX)
		return 0;

	while (true)
	{
		char *dp = strchr(query, '.');
		if (dp)
		{
			MYWORD size = dp - query;
			if (size >= 64)
				return 0;
			query += (size + 1);
		}
		else if (strlen(query) >= 64)
			return 0;
		else
			return 1;
	}
}

MYWORD CDualServer::fQu(char *query, dnsPacket *mess, char *raw)
{
	MYBYTE *xname = (MYBYTE*)query;
	MYBYTE *xraw = (MYBYTE*)raw;
	MYWORD retvalue = 0;
	bool goneout = false;

	while (true)
	{
		MYWORD size = *xraw;
		xraw++;

		if (!size)
			break;
		else if (size <= 63)
		{
			if (!goneout)
				retvalue += (size + 1);

			memcpy(xname, xraw, size);
			xname += size;
			xraw += size;

			if (!*xraw)
				break;

			*xname = '.';
			xname++;
		}
		else
		{
			if (!goneout)
				retvalue += 2;

			goneout = true;
			size %= 128;
			size %= 64;
			size *= 256;
			size += *xraw;
			xraw = (MYBYTE*)mess + size;
		}
	}
	*xname = 0;

	if (!goneout)
		retvalue++;

	return retvalue;
}

MYWORD CDualServer::qLen(char *query)
{
	MYWORD fullsize = 1;
	while (true)
	{
		char *dp = strchr(query, '.');
		if (dp != NULL)
		{
			int size = dp - query;
			query += (size + 1);
			fullsize += (size + 1);
		}
		else
		{
			int size = strlen(query);
			if (size)
				fullsize += (size + 1);
			break;
		}
	}
	//printf("%i\n",fullsize);
	return fullsize;
}

MYWORD CDualServer::pQu(char *raw, char *query)
{
	MYWORD fullsize = 1;
	while (true)
	{
		char *i = strchr(query, '.');

		if (i != NULL)
		{
			int size = i - query;
			*raw = size;
			raw++;
			memcpy(raw, query, size);
			raw += size;
			query += (size + 1);
			fullsize += (size + 1);
		}
		else
		{
			int size = strlen(query);
			if (size)
			{
				*raw = size;
				raw++;
				strcpy(raw, query);
				fullsize += (size + 1);
			}
			break;
		}
	}
	//printf("%i\n",fullsize);
	return fullsize;
}

MYWORD CDualServer::fUShort(void *raw)
{
	return ntohs(*((MYWORD*)raw));
}

MYDWORD CDualServer::fULong(void *raw)
{
	return ntohl(*((MYDWORD*)raw));
}

MYDWORD CDualServer::fIP(void *raw)
{
	return(*((MYDWORD*)raw));
}

MYBYTE CDualServer::pUShort(void *raw, MYWORD data)
{
	*((MYWORD*)raw) = htons(data);
	return sizeof(MYWORD);
}

MYBYTE CDualServer::pULong(void *raw, MYDWORD data)
{
	*((MYDWORD*)raw) = htonl(data);
	return sizeof(MYDWORD);
}

MYBYTE CDualServer::pIP(void *raw, MYDWORD data)
{
	*((MYDWORD*)raw) = data;
	return sizeof(MYDWORD);
}

void CDualServer::addRRNone(data5 *req)
{
	if (network.dns[0])
		req->dnsp->header.ra = 1;
	else
		req->dnsp->header.ra = 0;

	req->dnsp->header.at = 0;
	req->dnsp->header.aa = 0;

	req->dnsp->header.qr = 1;
	req->dnsp->header.ancount = 0;
	req->dnsp->header.nscount = 0;
	req->dnsp->header.adcount = 0;
}

void CDualServer::addRRExt(data5 *req)
{
	MYWORD xid = req->dnsp->header.xid;

	//printf("%s=%s\n", req->cname, req->query);

	if (strcasecmp(req->cname, req->query))
	{
		dnsPacket *packet;

		if (req->cache.dataType == CACHED && req->cache.response)
			packet = (dnsPacket*)req->cache.response;
		else
		{
			memcpy(req->temp, req->dnsp, req->bytes);
			packet = (dnsPacket*)req->temp;
		}

		req->dnsp->header.aa = 0;
		req->dnsp->header.at = 0;
		req->dnsp->header.qdcount = htons(1);
		req->dnsp->header.ancount = htons(1);

		//manuplate the response
		req->dp = &req->dnsp->data;
		req->dp += pQu(req->dp, req->query);
		req->dp += pUShort(req->dp, DNS_TYPE_A);
		req->dp += pUShort(req->dp, DNS_CLASS_IN);
		req->dp += pQu(req->dp, req->query);
		req->dp += pUShort(req->dp, DNS_TYPE_CNAME);
		req->dp += pUShort(req->dp, DNS_CLASS_IN);
		req->dp += pULong(req->dp, cfig.lease);
		req->dp += pUShort(req->dp, qLen(req->cname));
		req->dp += pQu(req->dp, req->cname);

		char *dp = &packet->data;

		for (int i = 1; i <= ntohs(packet->header.qdcount); i++)
		{
			dp += fQu(req->cname, packet, dp);
			dp += 4;
		}

		MYWORD dl = 0;
		for (int i = 1; i <= ntohs(packet->header.ancount); i++)
		{
			dp += fQu(req->cname, packet, dp);
			req->dp += pQu(req->dp, req->cname);
			memcpy(req->dp, dp, 8);
			req->dp += 8;
			int type = fUShort(dp);
			dp += 2; //type
			dp += 2; //class
			dp += 4; //ttl
			MYWORD zLen = fUShort(dp);
			dp += 2; //datalength

			switch (type)
			{
				case DNS_TYPE_A:
					req->dp += pUShort(req->dp, zLen);
					req->dp += pULong(req->dp, fULong(dp));
					break;
				case DNS_TYPE_CNAME:
					fQu(req->cname, packet, dp);
					dl = pQu(req->dp + 2, req->cname);
					req->dp += pUShort(req->dp, dl);
					req->dp += dl;
					break;
			}
			dp += zLen;
			req->dnsp->header.ancount = htons(htons(req->dnsp->header.ancount) + 1);
		}

		//req->bytes = req->dp - req->raw;
	}
	else if (req->cache.dataType == CACHED && req->cache.response)
	{
		memcpy(req->dnsp, req->cache.response, req->cache.bytes);
		req->dnsp->header.xid = xid;
		req->dp = req->raw + req->cache.bytes;
	}
	else
		req->dp = req->raw + req->bytes;
}

void CDualServer::addRRA(data5 *req)
{
	if (strcasecmp(req->query, req->cname))
	{
		req->dnsp->header.ancount = htons(htons(req->dnsp->header.ancount) + 1);
		req->dp += pQu(req->dp, req->query);
		req->dp += pUShort(req->dp, DNS_TYPE_CNAME);
		req->dp += pUShort(req->dp, DNS_CLASS_IN);
		req->dp += pULong(req->dp, cfig.lease);
		req->dp += pUShort(req->dp, qLen(req->cname));
		req->dp += pQu(req->dp, req->cname);
	}

	for (; req->iterBegin != dnsCache[cacheInd].end(); req->iterBegin++)
	{
		data7 *cache = req->iterBegin->second;

		if (strcasecmp(cache->mapname, req->mapname))
			break;

		if (cache->ip)
		{
			req->dnsp->header.ancount = htons(htons(req->dnsp->header.ancount) + 1);
			req->dp += pQu(req->dp, req->cname);
			req->dp += pUShort(req->dp, DNS_TYPE_A);
			req->dp += pUShort(req->dp, DNS_CLASS_IN);

			if (cache->dataType == LOCAL_A)
			{
				if ((MYDWORD)(cache->expiry - time(NULL)) >= cfig.lease)
					req->dp += pULong(req->dp, cfig.lease - 1);
				else
				{
					time_t tmp_t = cache->expiry - time(NULL);
					req->dp += pULong(req->dp, static_cast<unsigned int>(tmp_t));
				}
			}
			else
				req->dp += pULong(req->dp, cfig.lease);

			req->dp += pUShort(req->dp, 4);
			req->dp += pIP(req->dp, cache->ip);
		}
	}
	//req->bytes = req->dp - req->raw;
}

void CDualServer::addRRPtr(data5 *req)
{
	for (; req->iterBegin != dnsCache[cacheInd].end(); req->iterBegin++)
	{
		data7 *cache = req->iterBegin->second;

		if (strcasecmp(cache->mapname, req->mapname))
			break;

		req->dp += pQu(req->dp, req->query);
		req->dp += pUShort(req->dp, DNS_TYPE_PTR);
		req->dp += pUShort(req->dp, DNS_CLASS_IN);
		req->dnsp->header.ancount = htons(htons(req->dnsp->header.ancount) + 1);

		if (cache->dataType == LOCAL_PTR_AUTH || cache->dataType == LOCAL_PTR_NAUTH)
		{
			if ((MYDWORD)(cache->expiry - time(NULL)) >= cfig.lease)
				req->dp += pULong(req->dp, cfig.lease - 1);
			else
			{
				time_t tmp_t = cache->expiry - time(NULL);
				req->dp += pULong(req->dp, static_cast<unsigned int>(tmp_t));
			}
		}
		else
			req->dp += pULong(req->dp, cfig.lease);

		if (!strchr(cache->hostname, '.'))
			sprintf(req->cname, "%s.%s", cache->hostname, cfig.zone);
		else
			strcpy(req->cname, cache->hostname);

		req->dp += pUShort(req->dp, qLen(req->cname));
		req->dp += pQu(req->dp, req->cname);
	}
	//req->bytes = req->dp - req->raw;
}

void CDualServer::addRRServerA(data5 *req)
{
	if (strcasecmp(req->query, req->cname))
	{
		req->dnsp->header.ancount = htons(htons(req->dnsp->header.ancount) + 1);
		req->dp += pQu(req->dp, req->query);
		req->dp += pUShort(req->dp, DNS_TYPE_CNAME);
		req->dp += pUShort(req->dp, DNS_CLASS_IN);
		req->dp += pULong(req->dp, cfig.lease);
		req->dp += pUShort(req->dp, qLen(req->cname));
		req->dp += pQu(req->dp, req->cname);
	}

	req->dnsp->header.ancount = htons(htons(req->dnsp->header.ancount) + 1);
	req->dp += pQu(req->dp, req->cname);
	req->dp += pUShort(req->dp, DNS_TYPE_A);
	req->dp += pUShort(req->dp, DNS_CLASS_IN);
	req->dp += pULong(req->dp, cfig.lease);
	req->dp += pUShort(req->dp, 4);
	req->dp += pIP(req->dp, network.dnsUdpConn[req->sockInd].server);

	for (;req->iterBegin != dnsCache[cacheInd].end(); req->iterBegin++)
	{
		data7 *cache = req->iterBegin->second;

		if (strcasecmp(cache->mapname, req->mapname))
			break;

		if (cache->ip && cache->ip != network.dnsUdpConn[req->sockInd].server)
		{
			req->dnsp->header.ancount = htons(htons(req->dnsp->header.ancount) + 1);
			req->dp += pQu(req->dp, req->cname);
			req->dp += pUShort(req->dp, DNS_TYPE_A);
			req->dp += pUShort(req->dp, DNS_CLASS_IN);
			req->dp += pULong(req->dp, cfig.lease);
			req->dp += pUShort(req->dp, 4);
			req->dp += pIP(req->dp, cache->ip);
		}
	}
	//req->bytes = req->dp - req->raw;
}

void CDualServer::addRRAny(data5 *req, bool adFlag)
{
	while (req->iterBegin != dnsCache[cacheInd].end())
	{
		data7 *cache = req->iterBegin->second;

		if (strcasecmp(cache->mapname, req->mapname))
			break;

		if (adFlag)
			req->dnsp->header.adcount = htons(htons(req->dnsp->header.adcount) + 1);
		else
			req->dnsp->header.ancount = htons(htons(req->dnsp->header.ancount) + 1);

		switch (cache->dataType)
		{
			case LOCAL_A:
			case SERVER_A:
			case STATIC_A_AUTH:
				req->dp += pQu(req->dp, req->cname);
				req->dp += pUShort(req->dp, DNS_TYPE_A);
				req->dp += pUShort(req->dp, DNS_CLASS_IN);

				if (cache->dataType == LOCAL_A)
				{
					if ((MYDWORD)(cache->expiry - time(NULL)) >= cfig.lease)
						req->dp += pULong(req->dp, cfig.lease - 1);
					else
					{
						time_t tmp_t = cache->expiry - time(NULL);
						req->dp += pULong(req->dp, static_cast<unsigned int>(tmp_t));
					}
				}
				else
					req->dp += pULong(req->dp, cfig.lease);

				req->dp += pUShort(req->dp, 4);
				req->dp += pIP(req->dp, cache->ip);
				break;

			case LOCAL_PTR_AUTH:
			case LOCAL_PTR_NAUTH:
			case STATIC_PTR_AUTH:
			case STATIC_PTR_NAUTH:
			case SERVER_PTR_AUTH:
			case SERVER_PTR_NAUTH:
				req->dp += pQu(req->dp, req->cname);
				req->dp += pUShort(req->dp, DNS_TYPE_PTR);
				req->dp += pUShort(req->dp, DNS_CLASS_IN);

				if (cache->dataType == LOCAL_PTR_AUTH || cache->dataType == LOCAL_PTR_NAUTH)
				{
					if ((MYDWORD)(cache->expiry - time(NULL)) >= cfig.lease)
						req->dp += pULong(req->dp, cfig.lease - 1);
					else
					{
						time_t tmp_t = cache->expiry - time(NULL);
						req->dp += pULong(req->dp, static_cast<unsigned int>(tmp_t));
					}
				}
				else
					req->dp += pULong(req->dp, cfig.lease);

				if (cache->isZone || strchr(cache->hostname, '.'))
					strcpy(req->temp, cache->hostname);
				else
					sprintf(req->temp, "%s.%s", cache->hostname, cfig.zone);

				req->dp += pUShort(req->dp, qLen(req->temp));
				req->dp += pQu(req->dp, req->temp);
				break;

			case EXT_CNAME:
				req->dp += pQu(req->dp, req->cname);
				req->dp += pUShort(req->dp, DNS_TYPE_CNAME);
				req->dp += pUShort(req->dp, DNS_CLASS_IN);
				req->dp += pULong(req->dp, cfig.lease);
				req->dp += pUShort(req->dp, qLen(cache->hostname));
				req->dp += pQu(req->dp, cache->hostname);
				return;
				break;

			case LOCAL_CNAME:
				req->dp += pQu(req->dp, req->cname);
				req->dp += pUShort(req->dp, DNS_TYPE_CNAME);
				req->dp += pUShort(req->dp, DNS_CLASS_IN);
				req->dp += pULong(req->dp, cfig.lease);
				sprintf(req->cname, "%s.%s", cache->hostname, cfig.zone);
				req->dp += pUShort(req->dp, qLen(req->cname));
				req->dp += pQu(req->dp, req->cname);
				strcpy(req->mapname, cache->hostname);
				myLower(req->mapname);
				req->iterBegin = dnsCache[cacheInd].find(req->mapname);
				adFlag = true;
				continue;
		}
		req->iterBegin++;
	}
}

void CDualServer::addRRWildA(data5 *req, MYDWORD ip)
{
	// alexbegin
	req->dnsp->header.at = 1;
	req->dnsp->header.aa = 1;
	// alexend

	req->dnsp->header.ancount = htons(htons(req->dnsp->header.ancount) + 1);
	req->dp += pQu(req->dp, req->query);
	req->dp += pUShort(req->dp, DNS_TYPE_A);
	req->dp += pUShort(req->dp, DNS_CLASS_IN);
	req->dp += pULong(req->dp, cfig.lease);
	req->dp += pUShort(req->dp, 4);
	req->dp += pIP(req->dp, ip);
	//req->bytes = req->dp - req->raw;
}

void CDualServer::addRRLocalhostA(data5 *req)
{
	if (strcasecmp(req->query, req->mapname))
	{
		req->dnsp->header.ancount = htons(htons(req->dnsp->header.ancount) + 1);
		req->dp += pQu(req->dp, req->query);
		req->dp += pUShort(req->dp, DNS_TYPE_CNAME);
		req->dp += pUShort(req->dp, DNS_CLASS_IN);
		req->dp += pULong(req->dp, cfig.lease);
		req->dp += pUShort(req->dp, qLen(req->mapname));
		req->dp += pQu(req->dp, req->mapname);
	}

	req->dnsp->header.ancount = htons(htons(req->dnsp->header.ancount) + 1);
	req->dp += pQu(req->dp, req->mapname);
	req->dp += pUShort(req->dp, DNS_TYPE_A);
	req->dp += pUShort(req->dp, DNS_CLASS_IN);
	req->dp += pULong(req->dp, cfig.lease);
	req->dp += pUShort(req->dp, 4);
	req->dp += pIP(req->dp, req->cache.ip);
	//req->bytes = req->dp - req->raw;
}

void CDualServer::addRRLocalhostPtr(data5 *req)
{
	req->dnsp->header.ancount = htons(htons(req->dnsp->header.ancount) + 1);
	req->dp += pQu(req->dp, req->query);
	req->dp += pUShort(req->dp, DNS_TYPE_PTR);
	req->dp += pUShort(req->dp, DNS_CLASS_IN);
	req->dp += pULong(req->dp, cfig.lease);
	req->dp += pUShort(req->dp, qLen(req->cache.hostname));
	req->dp += pQu(req->dp, req->cache.hostname);
	//req->bytes = req->dp - req->raw;
}

void CDualServer::addRRMX(data5 *req)
{
	if (cfig.mxCount[cacheInd])
	{
		for (int m = 0; m < cfig.mxCount[cacheInd]; m++)
		{
			addRRMXOne(req, m);
		}
	}

	//req->bytes = req->dp - req->raw;
}

void CDualServer::addRRNSA(data5 *req)
{
	//printf("%s=%u\n", cfig.ns, cfig.expireTime);
	if (cfig.authorized && cfig.expireTime > time(NULL))
	{
		req->dnsp->header.ancount = htons(htons(req->dnsp->header.ancount) + 1);
		req->dnsp->header.at = 1;
		req->dnsp->header.aa = 1;

		req->dp += pQu(req->dp, cfig.zone);
		req->dp += pUShort(req->dp, DNS_TYPE_NS);
		req->dp += pUShort(req->dp, DNS_CLASS_IN);

		req->dp += pULong(req->dp, cfig.expire);

		req->dp += pUShort(req->dp, qLen(cfig.ns));
		req->dp += pQu(req->dp, cfig.ns);

//		req->dp += pQu(req->dp, cfig.servername_fqn);
//		req->dp += pUShort(req->dp, DNS_TYPE_A);
//		req->dp += pUShort(req->dp, DNS_CLASS_IN);

//		req->dp += pULong(req->dp, cfig.expire);

//		req->dp += pUShort(req->dp, 4);
//		req->dp += pIP(req->dp, cfig.nsIP);
	}
	//req->bytes = req->dp - req->raw;
}

void CDualServer::addRRNSPtr(data5 *req)
{
	if (cfig.authorized && cfig.expireTime > time(NULL))
	{
		req->dnsp->header.ancount = htons(htons(req->dnsp->header.ancount) + 1);
		req->dnsp->header.at = 1;
		req->dnsp->header.aa = 1;

		req->dp += pQu(req->dp, cfig.authority);
		req->dp += pUShort(req->dp, DNS_TYPE_NS);
		req->dp += pUShort(req->dp, DNS_CLASS_IN);

		req->dp += pULong(req->dp, cfig.expire);

		req->dp += pUShort(req->dp, qLen(cfig.ns));
		req->dp += pQu(req->dp, cfig.ns);

//		req->dp += pQu(req->dp, cfig.ns);
//		req->dp += pUShort(req->dp, DNS_TYPE_A);
//		req->dp += pUShort(req->dp, DNS_CLASS_IN);

//		req->dp += pULong(req->dp, cfig.expire);

//		req->dp += pUShort(req->dp, 4);
//		req->dp += pIP(req->dp, cfig.nsIP);
	}
	//req->bytes = req->dp - req->raw;
}

void CDualServer::addRRSOA(data5 *req, MYDWORD serial)
{
	if (cfig.authorized)
	{
		req->dnsp->header.ancount = htons(htons(req->dnsp->header.ancount) + 1);
		req->dnsp->header.at = 1;
		req->dnsp->header.aa = 1;

		req->dp += pQu(req->dp, req->query);
		req->dp += pUShort(req->dp, DNS_TYPE_SOA);
		req->dp += pUShort(req->dp, DNS_CLASS_IN);
		req->dp += pULong(req->dp, cfig.lease);
		char *data = req->dp;
		req->dp += 2;
		req->dp += pQu(req->dp, cfig.ns);
		sprintf(req->temp, "admin.%s", cfig.zone);
		req->dp += pQu(req->dp, req->temp);
		req->dp += pULong(req->dp, serial);
		req->dp += pULong(req->dp, cfig.refresh);
		req->dp += pULong(req->dp, cfig.retry);
		req->dp += pULong(req->dp, cfig.expire);
		req->dp += pULong(req->dp, cfig.minimum);
		pUShort(data, (req->dp - data) - 2);
	}
	//req->bytes = req->dp - req->raw;
}

void CDualServer::addAuth(data5 *req)
{
	if (cfig.authorized && cfig.expireTime > time(NULL))
	{
		req->dnsp->header.at = 1;
		req->dnsp->header.aa = 1;

		req->dnsp->header.nscount = htons(1);

		if (req->qtype == DNS_TYPE_PTR)
			req->dp += pQu(req->dp, cfig.authority);
		else
			req->dp += pQu(req->dp, cfig.zone);

		req->dp += pUShort(req->dp, DNS_TYPE_NS);
		req->dp += pUShort(req->dp, DNS_CLASS_IN);

		if (cfig.expire >= INT_MAX)
			req->dp += pULong(req->dp, UINT_MAX);
		else
			req->dp += pULong(req->dp, cfig.expire);

		req->dp += pUShort(req->dp, qLen(cfig.ns));
		req->dp += pQu(req->dp, cfig.ns);

		addRRAd(req);
	}
	//req->bytes = req->dp - req->raw;
}

void CDualServer::addRRAd(data5 *req)
{
	if (cfig.authorized && cfig.expireTime > time(NULL))
	{
		req->dnsp->header.adcount = htons(htons(req->dnsp->header.adcount) + 1);
		req->dp += pQu(req->dp, cfig.ns);
		req->dp += pUShort(req->dp, DNS_TYPE_A);
		req->dp += pUShort(req->dp, DNS_CLASS_IN);
		req->dp += pULong(req->dp, cfig.lease);
		req->dp += pUShort(req->dp, 4);
		req->dp += pIP(req->dp, cfig.nsIP);
	}
	//req->bytes = req->dp - req->raw;
}

void CDualServer::addRRAOne(data5 *req)
{
	data7 *cache = req->iterBegin->second;
	req->dnsp->header.ancount = htons(htons(req->dnsp->header.ancount) + 1);

	if (cache->isZone || strchr(cache->mapname, '.'))
		strcpy(req->cname, cache->mapname);
	else
		sprintf(req->cname, "%s.%s", cache->mapname, cfig.zone);

	req->dp += pQu(req->dp, req->cname);
	req->dp += pUShort(req->dp, DNS_TYPE_A);
	req->dp += pUShort(req->dp, DNS_CLASS_IN);

	if (cache->dataType == LOCAL_A)
	{
		if ((MYDWORD)(cache->expiry - time(NULL)) >= cfig.lease)
			req->dp += pULong(req->dp, cfig.lease - 1);
		else
		{
			time_t tmp_t = cache->expiry - time(NULL);
			req->dp += pULong(req->dp, static_cast<unsigned int>(tmp_t));
		}
	}
	else
		req->dp += pULong(req->dp, cfig.lease);

	req->dp += pUShort(req->dp, 4);
	req->dp += pIP(req->dp, cache->ip);
	//req->bytes = req->dp - req->raw;
}

void CDualServer::addRRPtrOne(data5 *req)
{
	data7 *cache = req->iterBegin->second;
	req->dnsp->header.ancount = htons(htons(req->dnsp->header.ancount) + 1);
	sprintf(req->cname, "%s%s", cache->mapname, arpa);
	req->dp += pQu(req->dp, req->cname);
	req->dp += pUShort(req->dp, DNS_TYPE_PTR);
	req->dp += pUShort(req->dp, DNS_CLASS_IN);

	if (cache->dataType == LOCAL_PTR_AUTH || cache->dataType == LOCAL_PTR_NAUTH)
	{
		if ((MYDWORD)(cache->expiry - time(NULL)) >= cfig.lease)
			req->dp += pULong(req->dp, cfig.lease - 1);
		else
		{
			time_t tmp_t = cache->expiry - time(NULL);
			req->dp += pULong(req->dp, static_cast<unsigned int>(tmp_t));
		}
	}
	else
		req->dp += pULong(req->dp, cfig.lease);

	if (cache->isZone || strchr(cache->hostname, '.'))
		strcpy(req->cname, cache->hostname);
	else
		sprintf(req->cname, "%s.%s", cache->hostname, cfig.zone);

	req->dp += pUShort(req->dp, qLen(req->cname));
	req->dp += pQu(req->dp, req->cname);

	//req->bytes = req->dp - req->raw;
}

void CDualServer::addRRSTAOne(data5 *req)
{
	data7 *cache = req->iterBegin->second;
	req->dnsp->header.ancount = htons(htons(req->dnsp->header.ancount) + 1);

	if (cache->isZone || strchr(cache->mapname, '.'))
		strcpy(req->cname, cache->mapname);
	else
		sprintf(req->cname, "%s.%s", cache->mapname, cfig.zone);

	req->dp += pQu(req->dp, req->cname);
	req->dp += pUShort(req->dp, DNS_TYPE_A);
	req->dp += pUShort(req->dp, DNS_CLASS_IN);
	req->dp += pULong(req->dp, cfig.lease);
	req->dp += pUShort(req->dp, 4);
	req->dp += pIP(req->dp, cache->ip);
	//req->bytes = req->dp - req->raw;
}

void CDualServer::addRRCNOne(data5 *req)
{
	data7 *cache = req->iterBegin->second;
	req->dnsp->header.ancount = htons(htons(req->dnsp->header.ancount) + 1);

	if (cache->isZone || strchr(cache->mapname, '.'))
		strcpy(req->cname, cache->mapname);
	else
		sprintf(req->cname, "%s.%s", cache->mapname, cfig.zone);

	req->dp += pQu(req->dp, req->cname);
	req->dp += pUShort(req->dp, DNS_TYPE_CNAME);
	req->dp += pUShort(req->dp, DNS_CLASS_IN);
	req->dp += pULong(req->dp, cfig.lease);

	if (strchr(cache->hostname, '.'))
		strcpy(req->cname, cache->hostname);
	else
		sprintf(req->cname, "%s.%s", cache->hostname, cfig.zone);

	req->dp += pUShort(req->dp, qLen(req->cname));
	req->dp += pQu(req->dp, req->cname);
	//req->bytes = req->dp - req->raw;
}

void CDualServer::addRRMXOne(data5 *req, MYBYTE m)
{
	//req->dp += pQu(req->dp, req->query);
	req->dnsp->header.ancount = htons(htons(req->dnsp->header.ancount) + 1);
	req->dp += pQu(req->dp, cfig.zone);
	req->dp += pUShort(req->dp, DNS_TYPE_MX);
	req->dp += pUShort(req->dp, DNS_CLASS_IN);
	req->dp += pULong(req->dp, cfig.lease);
	req->dp += pUShort(req->dp, strlen(cfig.mxServers[cacheInd][m].hostname) + 4);
	req->dp += pUShort(req->dp, cfig.mxServers[cacheInd][m].pref);
	req->dp += pQu(req->dp, cfig.mxServers[cacheInd][m].hostname);
	//req->bytes = req->dp - req->raw;
}

void CDualServer::GetActiveLeases(string *ps)
{
	time_t ti;
	time(&ti);

	data7 *dhcpEntry = NULL;
	data7 *cache = NULL;

	dhcpMap::iterator p;
	MYBYTE bp_chaddr[16];

	char tmp[512];
	WCHAR wsz[22];

	utm::dhcpleaselist dll;

	for (p = dhcpCache.begin(); p != dhcpCache.end(); p++)
	{
		if ((dhcpEntry = p->second) && dhcpEntry->local && dhcpEntry->expiry >= ti)
		{
			utm::singledhcplease sdl;

			MYBYTE bp_hlen = fromUUE(bp_chaddr, dhcpEntry->mapname);
			hex2String(tmp, bp_chaddr, bp_hlen, ':');
			sdl.mac.assign(tmp);

			sdl.ipaddr.m_addr = dhcpEntry->ip;
			sdl.ipaddr.revert_octets();

			if (dhcpEntry->expiry >= INT_MAX)
			{
				sdl.leasetime.clear();
			}
			else
			{
				sdl.leasetime.from_uint(static_cast<unsigned int>(dhcpEntry->expiry));
			}

			cache = findEntry(cacheInd, IP2String(tmp, htonl(dhcpEntry->ip)));
			if (cache && cache->hostname)
			{
				strncpy_s(tmp, 21, cache->hostname, _TRUNCATE);

				memset(wsz, 0, sizeof(wsz));
				MultiByteToWideChar(CP_OEMCP, NULL, tmp, -1, wsz, 22);

				memset(tmp, 0, sizeof(tmp));
				WideCharToMultiByte(CP_ACP, NULL, wsz, -1, tmp, sizeof(tmp), NULL, NULL);
				sdl.hostname.assign(tmp);
			}

			dll.add_element(sdl);
		}
	}

	dll.xml_create();
	dll.xml_get_string(*ps);

}

#ifdef _DEBUG
void CDualServer::procHTTP(data19 *req)
{
	req->ling.l_onoff = 1; //0 = off (l_linger ignored), nonzero = on
	req->ling.l_linger = 30; //0 = discard data, nonzero = wait for data sent
	setsockopt(req->sock, SOL_SOCKET, SO_LINGER, (const char*)&req->ling, sizeof(req->ling));

	time_t t = time(NULL);
	MYBYTE bp_chaddr[16];
	dhcpMap::iterator p;
	int ind = 0;
	MYDWORD iip = 0;
	data7 *dhcpEntry = NULL;
	data7 *cache = NULL;
	MYDWORD memSize = 2048 + (135 * dhcpCache.size()) + ((cfig.dhcpSize - dhcpCache.size()) * 26);
	req->buffer = (char*)calloc(1, memSize);

	if (!req->buffer)
	{
		sprintf(logBuff, "Memory Error");
		logDHCPMess(logBuff, 0);
		closesocket(req->sock);
		free(req);
		return;
	}

	errno = 0;
	req->bytes = recv(req->sock, req->buffer, memSize, 0);
	errno = WSAGetLastError();

	if (errno)
	{
		sprintf(logBuff, "Client %s, HTTP Message Receive failed, Error %u", IP2String(tempbuff, req->remote.sin_addr.s_addr), errno);
		logDHCPMess(logBuff, 1);
		free(req->buffer);
		closesocket(req->sock);
		free(req);
		return;
	}
	else if (cfig.dnsLogLevel >= 2)
	{
		sprintf(logBuff, "Client %s, HTTP Request Received", IP2String(tempbuff, req->remote.sin_addr.s_addr));
		logDHCPMess(logBuff, 2);
		//printf("%s\n", req->buffer);
	}

	char *fp = req->buffer;
	char *maxData = req->buffer + (memSize - 512);
	tm *ttm = gmtime(&t);
	strftime(tempbuff, sizeof(tempbuff), "%a, %d %b %Y %H:%M:%S GMT", ttm);
	fp += sprintf(fp, send200, tempbuff, tempbuff);
	char *contentStart = fp;
	fp += sprintf(fp, "<html>\n<head>\n<meta http-equiv=\"refresh\" content=\"60\">\n<meta http-equiv=\"cache-control\" content=\"no-cache\">\n</head>\n");
	fp += sprintf(fp, "<body bgcolor=\"#cccccc\">\n<h2>%s</h2>\n", sVersion);
	//fp += sprintf(fp, "<body bgcolor=\"#cccccc\">\n<h2>Host: %s</h2>\n", cfig.servername);
	fp += sprintf(fp, "<table border=\"1\" width=\"600\" bgcolor=\"#b8b8b8\">\n");
	fp += sprintf(fp, "<tr><th colspan=\"5\"><font size=\"5\"><i>Active Leases</i></font></th></tr>\n");
	fp += sprintf(fp, "<tr><th>Mac Address</th><th>IP</th><th>Lease Expiry</th><th>Hostname (first 20 chars)</th></tr>\n");

	for (p = dhcpCache.begin(); p != dhcpCache.end() && fp < maxData; p++)
	{
		if ((dhcpEntry = p->second) && dhcpEntry->local && dhcpEntry->expiry >= t)
		{
			fp += sprintf(fp, "<tr>");
			MYBYTE bp_hlen = fromUUE(bp_chaddr, dhcpEntry->mapname);
			fp += sprintf(fp, line200, hex2String(tempbuff, bp_chaddr, bp_hlen, ':'));
			fp += sprintf(fp, line200, IP2String(tempbuff, dhcpEntry->ip));

			if (dhcpEntry->expiry >= INT_MAX)
				fp += sprintf(fp, line200, "Infinity");
			else
			{
				tm *ttm = localtime(&dhcpEntry->expiry);
				strftime(tempbuff, sizeof(tempbuff), "%d-%b-%y %X", ttm);
				fp += sprintf(fp, line200, tempbuff);
			}

			cache = findEntry(cacheInd, IP2String(tempbuff, htonl(dhcpEntry->ip)));

			if (cache && cache->hostname)
			{
				if (strlen(cache->hostname) <= 20)
					fp += sprintf(fp, line200, cache->hostname);
				else
				{
					memcpy(tempbuff, cache->hostname, 20);
					tempbuff[20] = 0;
					fp += sprintf(fp, line200, tempbuff);
				}
			}
			else
				fp += sprintf(fp, line200, "&nbsp;");

			fp += sprintf(fp, "</tr>\n");
		}
	}

	fp += sprintf(fp, "</table>\n<br>\n<table border=\"1\" width=\"600\" bgcolor=\"#b8b8b8\">\n");
	fp += sprintf(fp, "<tr><th colspan=\"5\"><font size=\"5\"><i>Free Dynamic Leases</i></font></th></tr>\n");
	MYBYTE colNum = 0;

	for (char rangeInd = 0; rangeInd < MAX_RANGES && cfig.dhcpRanges[rangeInd].rangeStart && fp < maxData; rangeInd++)
	{
		for (ind = 0, iip = cfig.dhcpRanges[rangeInd].rangeStart; iip <= cfig.dhcpRanges[rangeInd].rangeEnd; iip++, ind++)
		{
			if (cfig.dhcpRanges[rangeInd].expiry[ind] < t)
			{
				if (!colNum)
				{
					fp += sprintf(fp, "<tr>");
					colNum = 1;
				}
				else if (colNum < 5)
					colNum++;
				else
				{
					fp += sprintf(fp, "</tr>\n<tr>");
					colNum = 1;
				}

				fp += sprintf(fp, line200, IP2String(tempbuff, htonl(iip)));
			}
		}
	}

	if (colNum)
		fp += sprintf(fp, "</tr>\n");

	fp += sprintf(fp, "</table>\n<br>\n<table border=\"1\" width=\"600\" bgcolor=\"#b8b8b8\">\n");
	fp += sprintf(fp, "<tr><th colspan=\"5\"><font size=\"5\"><i>Free Static Leases</i></font></th></tr>\n");
	fp += sprintf(fp, "<tr><th>Mac Address</th><th>IP</th><th>Mac Address</th><th>IP</th></tr>\n");
	colNum = 0;

	for (p = dhcpCache.begin(); p != dhcpCache.end() && fp < maxData; p++)
	{
		if ((dhcpEntry = p->second) && dhcpEntry->fixed && dhcpEntry->expiry < t)
		{
			if (!colNum)
			{
				fp += sprintf(fp, "<tr>");
				colNum = 1;
			}
			else if (colNum == 1)
			{
				colNum = 2;
			}
			else if (colNum == 2)
			{
				fp += sprintf(fp, "</tr>\n<tr>");
				colNum = 1;
			}

			MYBYTE bp_hlen = fromUUE(bp_chaddr, dhcpEntry->mapname);
			fp += sprintf(fp, line200, hex2String(tempbuff, bp_chaddr, bp_hlen, ':'));
			fp += sprintf(fp, line200, IP2String(tempbuff, dhcpEntry->ip));
		}
	}

	if (colNum)
		fp += sprintf(fp, "</tr>\n");

	fp += sprintf(fp, "</table>\n</body>\n</html>");
	memcpy((contentStart - 12), tempbuff, sprintf(tempbuff, "%u", (fp - contentStart)));
	req->bytes = fp - req->buffer;

	send(req->sock, req->buffer, req->bytes, 0);
	closesocket(req->sock);
	free(req->buffer);
	free(req);

	return;
}
#endif

void CDualServer::sendHTTP(void *lpParam)
{
	//printf("Here\n");
	data19 *req = (data19*)lpParam;
	send(req->sock, req->buffer, req->bytes, 0);
	Sleep(1000*(20));
	closesocket(req->sock);
	free(req->buffer);
	free(req);
	_endthread();
	return;
}

void CDualServer::procTCP(data5 *req)
{
	req->ling.l_onoff = 1; //0 = off (l_linger ignored), nonzero = on
	req->ling.l_linger = 10; //0 = discard data, nonzero = wait for data sent
	setsockopt(req->sock, SOL_SOCKET, SO_LINGER, (const char*)&req->ling, sizeof(req->ling));

	errno = 0;
	req->bytes = recvTcpDnsMess(req->sock, req->raw, true);
	//printf("%u\n",req->bytes);

	if (req->bytes < 2)
	{
		sprintf(logBuff, "Error Getting TCP DNS Message");
		logDNSMess(logBuff, 1);
		closesocket(req->sock);
		return;
	}

	MYWORD pktSize = fUShort(req->raw);
	req->dp = req->raw + 2;
	req->dnsp = (dnsPacket*)(req->dp);

	if (req->dnsp->header.opcode != OPCODE_STANDARD_QUERY)
	{
		switch (req->dnsp->header.opcode)
		{
			case OPCODE_INVERSE_QUERY:
				sprintf(logBuff, "Inverse query not supported");
				break;

			case OPCODE_SRVR_STAT_REQ:
				sprintf(logBuff, "Server Status Request not supported");
				break;

			case OPCODE_NOTIFY:
				sprintf(logBuff, "Notify not supported");
				break;

			case OPCODE_DYNAMIC_UPDATE:
				sprintf(logBuff, "Dynamic Update not needed/supported by Dual Server");
				break;

			default:
				sprintf(logBuff, "OpCode %u not supported", req->dnsp->header.opcode);
		}

		logTCPMess(req, logBuff, 1);

		addRRNone(req);
		req->dnsp->header.rcode = RCODE_NOTIMPL;
		sendTCPmess(req);
		closesocket(req->sock);
		return;
	}

	if (req->dnsp->header.qr != 0 || ntohs(req->dnsp->header.qdcount) != 1 || ntohs(req->dnsp->header.ancount))
	{
		sprintf(logBuff, "DNS Query Format Error");
		logTCPMess(req, logBuff, 1);
		addRRNone(req);
		req->dnsp->header.rcode = RCODE_FORMATERROR;
		sendTCPmess(req);
		closesocket(req->sock);
		return;
	}

	req->dp = &req->dnsp->data;

	for (int i = 1; i <= ntohs(req->dnsp->header.qdcount); i++)
	{
		req->dp += fQu(req->query, req->dnsp, req->dp);
		req->qtype = fUShort(req->dp);
		req->dp += 2;
		req->qclass = fUShort(req->dp);
		req->dp += 2;
	}

	if (req->qclass != DNS_CLASS_IN)
	{
		sprintf(logBuff, "DNS Class %u not supported", req->qclass);
		logTCPMess(req, logBuff, 1);
		addRRNone(req);
		req->dnsp->header.rcode = RCODE_NOTIMPL;
		sendTCPmess(req);
		closesocket(req->sock);
		return;
	}

	if (!req->qtype)
	{
		sprintf(logBuff, "missing query type");
		logTCPMess(req, logBuff, 1);
		addRRNone(req);
		req->dnsp->header.rcode = RCODE_FORMATERROR;
		sendTCPmess(req);
		closesocket(req->sock);
		return;
	}

	bool allowed = false;
	MYDWORD clientIP = req->remote.sin_addr.s_addr;

	for (int i = 0; i < MAX_SERVERS && cfig.zoneServers[i]; i++)
	{
		if (clientIP == cfig.zoneServers[i] || clientIP == network.allServers[i])
		{
			allowed = true;
			break;
		}
	}

	if (!allowed)
	{
		sprintf(logBuff, "DNS TCP Query, Access Denied");
		logTCPMess(req, logBuff, 1);
		addRRNone(req);
		req->dnsp->header.rcode = RCODE_REFUSED;
		sendTCPmess(req);
		closesocket(req->sock);
		return;
	}

	strcpy(req->mapname, req->query);
	strcpy(req->cname, req->query);
	bool AXFRError = false;

	if (req->qtype != DNS_TYPE_NS && req->qtype != DNS_TYPE_SOA && req->qtype != DNS_TYPE_AXFR && req->qtype != DNS_TYPE_IXFR)
	{
		addRRNone(req);
		req->dnsp->header.rcode = RCODE_NOTIMPL;
		sendTCPmess(req);
		sprintf(logBuff, "DNS TCP Query Type %s not supported", strqtype(tempbuff, static_cast<unsigned char>(req->qtype)));
		logTCPMess(req, logBuff, 1);
	}
	else if (strcasecmp(req->mapname, cfig.zone) && strcasecmp(req->mapname, cfig.authority))
	{
		addRRNone(req);
		req->dnsp->header.rcode = RCODE_NOTIMPL;
		sendTCPmess(req);
		sprintf(logBuff, "%s, Forwarding TCP query not supported", req->query);
		logTCPMess(req, logBuff, 1);
	}
	else if (!cfig.authorized)
	{
		addRRNone(req);
		req->dnsp->header.rcode = RCODE_NOTAUTH;
		sendTCPmess(req);
		sprintf(logBuff, "Server is not authorized for zone %s", req->query);
		logTCPMess(req, logBuff, 1);
	}
	else if (!dhcpService && cfig.expireTime < time(NULL))
	{
		addRRNone(req);
		req->dnsp->header.rcode = RCODE_NOTAUTH;
		sendTCPmess(req);
		sprintf(logBuff, "Zone %s expired", req->query);
		logTCPMess(req, logBuff, 1);
	}
	else
	{
		switch (req->qtype)
		{
			case DNS_TYPE_SOA:
				if (!strcasecmp(req->query, cfig.zone))
				{
					addRRNone(req);
					req->dnsp->header.aa = 0;
					req->dnsp->header.at = 0;
					addRRSOA(req, cfig.serial1);
					sendTCPmess(req);
					sprintf(logBuff, "SOA Sent for zone %s", req->query);
					logTCPMess(req, logBuff, 2);
				}
				else if (!strcasecmp(req->query, cfig.authority))
				{
					addRRNone(req);
					req->dnsp->header.aa = 0;
					req->dnsp->header.at = 0;
					addRRSOA(req, cfig.serial2);
					sendTCPmess(req);
					sprintf(logBuff, "SOA Sent for zone %s", req->query);
					logTCPMess(req, logBuff, 2);
				}
				break;

			case DNS_TYPE_NS:
				if (!strcasecmp(req->mapname, cfig.zone))
				{
					addRRNone(req);
					req->dnsp->header.aa = 0;
					req->dnsp->header.at = 0;
					addRRNSA(req);
					addRRAd(req);
					sendTCPmess(req);
					sprintf(logBuff, "NS Sent for Zone %s", req->query);
					logTCPMess(req, logBuff, 2);
				}
				else if (!strcasecmp(req->query, cfig.authority))
				{
					addRRNone(req);
					req->dnsp->header.aa = 0;
					req->dnsp->header.at = 0;
					addRRNSPtr(req);
					addRRAd(req);
					sendTCPmess(req);
					sprintf(logBuff, "NS Sent for Zone", req->query);
					logTCPMess(req, logBuff, 2);
				}
				break;

			case DNS_TYPE_AXFR:
			case DNS_TYPE_IXFR:

				if (!strcasecmp(req->mapname, cfig.zone))
				{
					MYDWORD tempserial = cfig.serial1;
					MYWORD records = 0;

					addRRNone(req);
					req->dp = &req->dnsp->data;
					req->dnsp->header.qdcount = 0;
					addRRSOA(req, cfig.serial1);

					if (!sendTCPmess(req))
					{
						AXFRError = true;
						break;
					}
					else
						records++;

					addRRNone(req);
					req->dp = &req->dnsp->data;
					req->dnsp->header.qdcount = 0;
					addRRNSA(req);

					if (!sendTCPmess(req))
					{
						AXFRError = true;
						break;
					}
					else
						records++;

					time_t t = time(NULL);
					req->iterBegin = dnsCache[cacheInd].begin();

					while (!AXFRError && req->iterBegin != dnsCache[cacheInd].end())
					{
						addRRNone(req);
						req->dnsp->header.qdcount = 0;
						req->dp = &req->dnsp->data;

						if (req->iterBegin->second->expiry > t)
						{
							//printf("%s=%u\n",req->iterBegin->second->mapname, req->iterBegin->second->expiry);

							switch (req->iterBegin->second->dataType)
							{
								case LOCAL_A:
									addRRAOne(req);
									break;

								case SERVER_A:
								case STATIC_A_AUTH:
									addRRSTAOne(req);
									break;

								case LOCAL_CNAME:
								case EXT_CNAME:
									addRRCNOne(req);
									break;

								default:
									req->iterBegin++;
									continue;
							}

							if (tempserial != cfig.serial1)
							{
								AXFRError = true;
								break;
							}

							if (!sendTCPmess(req))
							{
								AXFRError = true;
								break;
							}
							else
								records++;
						}
						req->iterBegin++;
					}

					for (int m = 0; m < cfig.mxCount[cacheInd]; m++)
					{
						addRRNone(req);
						req->dnsp->header.qdcount = 0;
						req->dp = &req->dnsp->data;
						addRRMXOne(req, m);

						if (tempserial != cfig.serial1)
						{
							AXFRError = true;
							break;
						}

						if (!sendTCPmess(req))
						{
							AXFRError = true;
							break;
						}
						else
							records++;
					}

					addRRNone(req);
					req->dp = &req->dnsp->data;
					req->dnsp->header.qdcount = 0;
					addRRSOA(req, cfig.serial1);

					if (!AXFRError && tempserial == cfig.serial1)
					{
						if (sendTCPmess(req))
						{
							records++;
							sprintf(logBuff, "Zone %s with %u RRs Sent", req->query, records);
							logTCPMess(req, logBuff, 2);
						}
					}
				}
				else if (!strcasecmp(req->query, cfig.authority))
				{
					if (!dhcpService && cfig.replication == 2 && clientIP == cfig.zoneServers[0])
					{
						addRRNone(req);
						req->dnsp->header.rcode = RCODE_REFUSED;
						sendTCPmess(req);
						closesocket(req->sock);
						return;
					}

					MYDWORD tempserial = cfig.serial2;
					MYWORD records = 0;

					addRRNone(req);
					req->dp = &req->dnsp->data;
					req->dnsp->header.qdcount = 0;
					addRRSOA(req, cfig.serial2);

					if (!sendTCPmess(req))
					{
						AXFRError = true;
						break;
					}
					else
						records++;

					addRRNone(req);
					req->dp = &req->dnsp->data;
					req->dnsp->header.qdcount = 0;
					addRRNSPtr(req);

					if (!sendTCPmess(req))
					{
						AXFRError = true;
						break;
					}
					else
						records++;

					time_t t = time(NULL);
					req->iterBegin = dnsCache[cacheInd].begin();

					while (!AXFRError && req->iterBegin != dnsCache[cacheInd].end())
					{
						addRRNone(req);
						req->dnsp->header.qdcount = 0;
						req->dp = &req->dnsp->data;

						if (req->iterBegin->second->expiry > t)
						{
							switch (req->iterBegin->second->dataType)
							{
								case LOCAL_PTR_AUTH:
								case STATIC_PTR_AUTH:
								case SERVER_PTR_AUTH:
									addRRPtrOne(req);
									break;

								default:
									req->iterBegin++;
									continue;
							}

							if (tempserial != cfig.serial2)
							{
								AXFRError = true;
								break;
							}

							if (!sendTCPmess(req))
							{
								AXFRError = true;
								break;
							}
							else
								records++;

						}
						req->iterBegin++;
					}

					addRRNone(req);
					req->dp = &req->dnsp->data;
					req->dnsp->header.qdcount = 0;
					addRRSOA(req, cfig.serial2);

					if (!AXFRError && tempserial == cfig.serial2)
					{
						if (sendTCPmess(req))
						{
							records++;
							sprintf(logBuff, "Zone %s with %u RRs Sent", req->query, records);
							logTCPMess(req, logBuff, 2);
						}
					}
				}
				break;
		}
	}

	closesocket(req->sock);
	return;
}

MYWORD CDualServer::sendTCPmess(data5 *req)
{
	errno = 0;
	req->dnsp->header.ra = 0;
	req->bytes = req->dp - req->raw;
	pUShort(req->raw, req->bytes - 2);

	if (req->bytes != send(req->sock, req->raw, req->bytes, 0) || WSAGetLastError())
		return 0;

	return req->bytes;
}

MYWORD CDualServer::gdnmess(data5 *req, MYBYTE sockInd)
{
	memset(req, 0, sizeof(data5));
	req->sockLen = sizeof(req->remote);
	errno = 0;

	req->bytes = recvfrom(network.dnsUdpConn[sockInd].sock,
	                      req->raw,
	                      sizeof(req->raw),
	                      0,
	                      (sockaddr*)&req->remote,
	                      &req->sockLen);

	errno = WSAGetLastError();

	if (errno || req->bytes <= 0)
		return 0;

	req->sockInd = sockInd;
	req->dnsp = (dnsPacket*)req->raw;

	if (req->dnsp->header.qr != 0)
		return 0;

	if (req->dnsp->header.opcode != OPCODE_STANDARD_QUERY)
	{
		if (cfig.dnsLogLevel)
		{
			switch (req->dnsp->header.opcode)
			{
				case OPCODE_INVERSE_QUERY:
					sprintf(logBuff, "Inverse query not supported");
					break;

				case OPCODE_SRVR_STAT_REQ:
					sprintf(logBuff, "Server Status Request not supported");
					break;

				case OPCODE_NOTIFY:
					sprintf(logBuff, "Notify not supported");
					break;

				case OPCODE_DYNAMIC_UPDATE:
					sprintf(logBuff, "Dynamic Update not needed/supported by Dual Server");
					break;

				default:
					sprintf(logBuff, "OpCode %u not supported", req->dnsp->header.opcode);
			}
			logDNSMess(req, logBuff, 1);
		}

		addRRNone(req);
		req->dnsp->header.rcode = RCODE_NOTIMPL;
		return req->bytes;
	}

	if (ntohs(req->dnsp->header.qdcount) != 1 || ntohs(req->dnsp->header.ancount))
	{
		if (cfig.dnsLogLevel)
		{
			sprintf(logBuff, "DNS Query Format Error");
			logDNSMess(req, logBuff, 1);
		}

		addRRNone(req);
		req->dnsp->header.rcode = RCODE_FORMATERROR;
		return req->bytes;
	}

	req->dp = &req->dnsp->data;

	for (int i = 1; i <= ntohs(req->dnsp->header.qdcount); i++)
	{
		req->dp += fQu(req->query, req->dnsp, req->dp);
		req->qtype = fUShort(req->dp);
		req->dp += 2;
		req->qclass = fUShort(req->dp);
		req->dp += 2;
	}

	if (req->qclass != DNS_CLASS_IN)
	{
		if (cfig.dnsLogLevel)
		{
			sprintf(logBuff, "DNS Class %u not supported", req->qclass);
			logDNSMess(req, logBuff, 1);
		}
		addRRNone(req);
		req->dnsp->header.rcode = RCODE_NOTIMPL;
		return req->bytes;
	}

	if (!req->qtype)
	{
		if (cfig.dnsLogLevel)
		{
			sprintf(logBuff, "missing query type");
			logDNSMess(req, logBuff, 1);
		}

		addRRNone(req);
		req->dnsp->header.rcode = RCODE_FORMATERROR;
		return req->bytes;
	}

	req->qLen = strlen(req->query);
	MYDWORD ip = req->remote.sin_addr.s_addr;
	MYDWORD iip = ntohl(ip);

	for (int i = 0; i < MAX_RANGES && cfig.dnsRanges[i].rangeStart; i++)
	{
		if (iip >= cfig.dnsRanges[i].rangeStart && iip <= cfig.dnsRanges[i].rangeEnd)
			return req->bytes;
	}

	for (int i = 0; i < MAX_RANGES && cfig.dhcpRanges[i].rangeStart; i++)
	{
		if (iip >= cfig.dhcpRanges[i].rangeStart && iip <= cfig.dhcpRanges[i].rangeEnd)
			return req->bytes;
	}

	if (findEntry(cacheInd, IP2String(req->cname, iip)))
		return req->bytes;

	if (findServer(network.allServers, ip))
		return req->bytes;

	if (req->remote.sin_addr.s_addr == cfig.zoneServers[0] || req->remote.sin_addr.s_addr == cfig.zoneServers[1])
		return req->bytes;

	addRRNone(req);
	req->dnsp->header.rcode = RCODE_REFUSED;

	if (cfig.dnsLogLevel)
	{
		sprintf(logBuff, "DNS UDP Query, Access Denied");
		logDNSMess(req, logBuff, 1);
	}
	return req->bytes;
}

MYWORD CDualServer::sdnmess(data5 *req)
{
	errno = 0;

	req->bytes = req->dp - req->raw;

	req->bytes = sendto(network.dnsUdpConn[req->sockInd].sock,
	                    req->raw,
	                    req->bytes,
	                    0,
	                    (sockaddr*)&req->remote,
	                    sizeof(req->remote));

	errno = WSAGetLastError();

	if (errno || req->bytes <= 0)
		return 0;

	return req->bytes;
}

MYWORD CDualServer::scanloc(data5 *req)
{
	if (!req->query[0])
		return 0;

	strcpy(req->cname, req->query);
	strcpy(req->mapname, req->query);
	myLower(req->mapname);
	req->dnType = makeLocal(req->mapname);
	//printf("LocalCode=%u query=%s mapname=%s\n", req->dnType, req->query, req->mapname);

	switch (req->qtype)
	{
		case DNS_TYPE_PTR:
			break;

		case DNS_TYPE_A:
			if (req->dnType == DNTYPE_A_BARE)
			{
				sprintf(req->cname, "%s.%s", req->query, cfig.zone);
			}
			break;

		case DNS_TYPE_MX:
			if (!strcasecmp(req->query, cfig.zone) && (cfig.authorized || cfig.mxServers[cacheInd][0].hostname[0]))
			{
				addRRNone(req);
				addRRMX(req);
				addAuth(req);
				return 1;
			}
			else
				return 0;

			break;

		case DNS_TYPE_NS:
			if (cfig.authorized)
			{
				if (req->dnType == DNTYPE_P_ZONE)
				{
					addRRNone(req);
					addRRNSPtr(req);
					addRRAd(req);
					return 1;
				}
				else if (req->dnType == DNTYPE_A_ZONE)
				{
					addRRNone(req);
					addRRNSA(req);
					addRRAd(req);
					return 1;
				}
			}
			return 0;
			break;

		case DNS_TYPE_SOA:
			if (cfig.authorized)
			{
				if (req->dnType == DNTYPE_P_ZONE)
				{
					addRRNone(req);
					addRRSOA(req, cfig.serial2);
					return 1;
				}
				else if (req->dnType == DNTYPE_A_ZONE)
				{
					addRRNone(req);
					addRRSOA(req, cfig.serial1);
					return 1;
				}
			}
			return 0;
			break;

		case DNS_TYPE_ANY:
			req->iterBegin = dnsCache[cacheInd].find(req->mapname);

			if (req->iterBegin != dnsCache[cacheInd].end() && req->iterBegin->second->dataType != CACHED)
			{
				switch (req->dnType)
				{
					case DNTYPE_A_BARE:
						addRRNone(req);
						sprintf(req->cname, "%s.%s", req->mapname, cfig.zone);
						req->dnsp->header.ancount = htons(htons(req->dnsp->header.ancount) + 1);
						req->dp += pQu(req->dp, req->mapname);
						req->dp += pUShort(req->dp, DNS_TYPE_CNAME);
						req->dp += pUShort(req->dp, DNS_CLASS_IN);
						req->dp += pULong(req->dp, cfig.lease);
						req->dp += pUShort(req->dp, qLen(req->cname));
						req->dp += pQu(req->dp, req->cname);
						req->dnsp->header.ancount = htons(1);
						addRRAny(req, true);
						return 1;
						break;

					case DNTYPE_A_ZONE:
						addRRNone(req);
						if (cfig.authorized)
						{
							addRRSOA(req, cfig.serial1);
							addRRNSA(req);
							req->dnsp->header.ancount = htons(2);
						}

						for (int m = 0; m < cfig.mxCount[cacheInd]; m++)
							addRRMXOne(req, m);

						addRRAny(req, false);

						//if (cfig.authorized)
						//	addRRAd(req);

						return 1;
						break;


					case DNTYPE_P_ZONE:
						addRRNone(req);
						if (cfig.authorized)
						{
							addRRSOA(req, cfig.serial1);
							addRRNSPtr(req);
							req->dnsp->header.ancount = htons(2);
						}

						//if (cfig.authorized)
						//	addRRAd(req);

						return 1;
						break;
					default:
						addRRNone(req);
						addRRAny(req, false);
						return 1;
						break;
				}
			}
			else
			{
				switch (req->dnType)
				{
					case DNTYPE_A_ZONE:
						addRRNone(req);
						if (cfig.authorized)
						{
							addRRSOA(req, cfig.serial1);
							addRRNSA(req);
							req->dnsp->header.ancount = htons(2);
						}

						for (int m = 0; m < cfig.mxCount[cacheInd]; m++)
							addRRMXOne(req, m);

//						if (cfig.authorized)
//							addRRAd(req);

						return 1;
						break;


					case DNTYPE_P_ZONE:
						addRRNone(req);
						if (cfig.authorized)
						{
							addRRSOA(req, cfig.serial1);
							addRRNSPtr(req);
							req->dnsp->header.ancount = htons(2);
						}

//						if (cfig.authorized)
//							addRRAd(req);

						return 1;
						break;

					case DNTYPE_A_LOCAL:
					case DNTYPE_P_LOCAL:
					case DNTYPE_A_EXT:
					case DNTYPE_P_EXT:
						return 0;
						break;

					default:
						addRRNone(req);
						return 1;
						break;
				}
			}

			break;

		case DNS_TYPE_AAAA:

		// alexbegin			
/*
			if (cfig.authorized && req->dnType != DNTYPE_A_EXT && req->dnType != DNTYPE_P_EXT)
			{
				if (cfig.dnsLogLevel)
				{
					sprintf(logBuff, "%s, DNS Query Type %s not supported", req->query, strqtype(tempbuff, req->qtype));
					logDNSMess(req, logBuff, 1);
				}
				addRRNone(req);
				req->dnsp->header.rcode = RCODE_NOTIMPL;
				addAuth(req);
				return 1;
			}
			return 0;
*/

			addRRNone(req);
			req->dnsp->header.rcode = RCODE_NOTIMPL;
			addAuth(req);
			return 1;

		// 	alexend

			break;

		default:

			if (cfig.authorized && req->dnType != DNTYPE_A_EXT && req->dnType != DNTYPE_P_EXT)
			{
				if (cfig.dnsLogLevel)
				{
					sprintf(logBuff, "%s, DNS Query Type %s not supported", req->query, strqtype(tempbuff, static_cast<unsigned char>(req->qtype)));
					logDNSMess(req, logBuff, 1);
				}
				addRRNone(req);
				req->dnsp->header.rcode = RCODE_NOTIMPL;
				addAuth(req);
				return 1;
			}
			else
				return 0;

			break;
	}

	time_t t = time(NULL);

	if (m_bUseDnsCache)
	{
		for (int m = 0; m < 3; m++)
		{
			//printf("%s has %u Entries\n", req->mapname, dnsCache[cacheInd].count(req->mapname));
			req->iterBegin = dnsCache[cacheInd].find(req->mapname);
			//if (req->iterBegin != dnsCache[cacheInd].end() && req->iterBegin->second->expiry >= t)
			if (req->iterBegin != dnsCache[cacheInd].end())
			{
				memcpy(&req->cache, req->iterBegin->second, sizeof(data7));
				//printf("mapname=%s, datatype=%i exp=%u\n",req->cache.mapname, req->cache.dataType,req->cache.expiry);

				switch (req->cache.dataType)
				{
					case LOCAL_A:
					case STATIC_A_AUTH:
						addRRNone(req);
						addRRA(req);
						addAuth(req);
						return 1;

					case LOCAL_PTR_AUTH:
					case STATIC_PTR_AUTH:
					case SERVER_PTR_AUTH:
						addRRNone(req);
						addRRPtr(req);
						addAuth(req);
						return 1;

					case LOCALHOST_A:
						addRRNone(req);
						addRRLocalhostA(req);
						addAuth(req);
						return 1;

					case LOCALHOST_PTR:
						addRRNone(req);
						addRRLocalhostPtr(req);
						addAuth(req);
						return 1;

					case STATIC_A_NAUTH:
						addRRNone(req);
						addRRA(req);
						return 1;

					case LOCAL_PTR_NAUTH:
					case SERVER_PTR_NAUTH:
					case STATIC_PTR_NAUTH:
						addRRNone(req);
						addRRPtr(req);
						return 1;

					case SERVER_A:
						addRRNone(req);
						addRRServerA(req);
						addAuth(req);
						return 1;

					case CACHED:
						addRRNone(req);
						addRRExt(req);
						return 1;

					case LOCAL_CNAME:
					case EXT_CNAME:

						if (!strchr(req->cache.hostname, '.'))
							sprintf(req->cname, "%s.%s", req->cache.hostname, cfig.zone);
						else
							strcpy(req->cname, req->cache.hostname);

						strcpy(req->mapname, req->cache.hostname);
						myLower(req->mapname);
						continue;

					default:
						break;
				}
			}

			break;
		}
	}

	if (req->qtype == DNS_TYPE_A)
	{
		if (m_pWildDns != NULL)
		{
			if (m_pWildDns->Search2Level(req->mapname))
			{
				if (m_pWildDns->m_defaultDnsServer)
					addRRWildA(req, m_pWildDns->m_defaultDnsServer);

				return 1;
		
			}
		}
		else
		{
			if (cfig.wildHosts[0].wildcard[0])
			{
				for (MYBYTE i = 0; i < 32 && cfig.wildHosts[i].wildcard[0]; i++)
				{
					if (wildcmp(req->mapname, cfig.wildHosts[i].wildcard))
					{
	// alexbegin

						addRRNone(req);

						if (cfig.wildHosts[i].ip)
							addRRWildA(req, cfig.wildHosts[i].ip);

	// alexend

						return 1;
					}
				}
			}
		}
	}

	//printf("%u=%u, LOCAL_CNAME=%i\n", req->cache.dataType, req->dnType, LOCAL_CNAME);

	if (req->cache.dataType == NONE && req->dnType == DNTYPE_A_BARE)
	{
		addRRNone(req);
		addAuth(req);
		return 1;
	}
	else if (req->cache.dataType == LOCAL_CNAME)
	{
		addRRNone(req);
		addRRA(req);
		addAuth(req);
		return 1;
	}
	else if (req->cache.dataType == EXT_CNAME)
	{
		//printf("%u=%u\n", req->cache.dataType, EXT_CNAME);
		req->dnType = makeLocal(req->mapname);
		req->dp = &req->dnsp->data;
		req->dp += pQu(req->dp, req->cname);
		req->dp += pUShort(req->dp, DNS_TYPE_A);
		req->dp += pUShort(req->dp, DNS_CLASS_IN);
		req->bytes = req->dp - req->raw;
	}

	return 0;
}

MYWORD CDualServer::fdnmess(data5 *req)
{
	//checkSize();
	req->qLen = strlen(req->cname);
	MYBYTE zoneDNS;
	int nRet = -1;

	char mapname[8];
	sprintf(mapname, "%u", req->dnsp->header.xid);
	data7 *queue = findEntry(cacheInd, mapname, QUEUE);

	for (zoneDNS = 0; zoneDNS < 32 && cfig.dnsRoutes[zoneDNS].zLen; zoneDNS++)
	{
		if (req->qLen == cfig.dnsRoutes[zoneDNS].zLen && !strcasecmp(req->cname, cfig.dnsRoutes[zoneDNS].zone))
			req->dnType = DNTYPE_SUBZONE;
		else if (req->qLen > cfig.dnsRoutes[zoneDNS].zLen)
		{
			char *dp = req->cname + (req->qLen - cfig.dnsRoutes[zoneDNS].zLen - 1);

			if (*dp == '.' && !strcasecmp(dp + 1, cfig.dnsRoutes[zoneDNS].zone))
				req->dnType = DNTYPE_SUBZONE;
		}

		if (req->dnType == DNTYPE_SUBZONE)
		{
			if (queue && cfig.dnsRoutes[zoneDNS].dns[1])
				cfig.dnsRoutes[zoneDNS].currentDNS = 1 - cfig.dnsRoutes[zoneDNS].currentDNS;

			if (req->remote.sin_addr.s_addr != cfig.dnsRoutes[zoneDNS].dns[cfig.dnsRoutes[zoneDNS].currentDNS])
			{
				req->addr.sin_family = AF_INET;
				req->addr.sin_addr.s_addr = cfig.dnsRoutes[zoneDNS].dns[cfig.dnsRoutes[zoneDNS].currentDNS];
				req->addr.sin_port = htons(DnsServerPort);
				errno = 0;

				nRet = sendto(network.forwConn.sock,
							  req->raw,
							  req->bytes,
							  0,
							  (sockaddr*)&req->addr,
							  sizeof(req->addr));

				errno = WSAGetLastError();

				if (errno || nRet <= 0)
				{
					if (cfig.dnsLogLevel)
					{
						sprintf(logBuff, "Error Forwarding UDP DNS Message to Child Server %s", IP2String(tempbuff, req->addr.sin_addr.s_addr));
						logDNSMess(req, logBuff, 1);
						addRRNone(req);
						req->dnsp->header.rcode = RCODE_SERVERFAIL;
					}

					if (cfig.dnsRoutes[zoneDNS].dns[1])
						cfig.dnsRoutes[zoneDNS].currentDNS = 1 - cfig.dnsRoutes[zoneDNS].currentDNS;

					return 0;
				}
				else
				{
					if (cfig.dnsLogLevel == 2)
					{
						sprintf(logBuff, "%s forwarded to Child Server %s", req->cname, IP2String(tempbuff, cfig.dnsRoutes[zoneDNS].dns[cfig.dnsRoutes[zoneDNS].currentDNS]));
						logDNSMess(req, logBuff, 2);
					}
				}
			}

			break;
		}
	}

	if (req->dnType != DNTYPE_SUBZONE)
	{
		if (cfig.authorized &&  (req->dnType == DNTYPE_A_LOCAL || req->dnType == DNTYPE_P_LOCAL))
		{
			if (cfig.dnsLogLevel == 2)
			{
				sprintf(logBuff, "%s not found", req->query);
				logDNSMess(req, logBuff, 2);
			}

			addRRNone(req);
			req->dnsp->header.rcode = RCODE_NAMEERROR;
			addAuth(req);
			return 0;
		}

		if (!req->dnsp->header.rd)
		{
			addRRNone(req);
			req->dnsp->header.rcode = RCODE_NAMEERROR;
			if (cfig.dnsLogLevel)
			{
				sprintf(logBuff, "%s is not local (recursion not desired)", req->query);
				logDNSMess(req, logBuff, 2);
			}
			return 0;
		}

		if (!network.dns[0])
		{
			addRRNone(req);
			req->dnsp->header.rcode = RCODE_NAMEERROR;
			req->dnsp->header.ra = 0;
			if (cfig.dnsLogLevel)
			{
				sprintf(logBuff, "%s not found (recursion not available)", req->query);
				logDNSMess(req, logBuff, 2);
			}
			return 0;
		}

		if (queue && network.dns[1] && queue->dnsIndex < MAX_SERVERS && network.currentDNS == queue->dnsIndex)
		{
			network.currentDNS++;

			if (network.currentDNS >= MAX_SERVERS || !network.dns[network.currentDNS])
				network.currentDNS = 0;
		}

		if (req->remote.sin_addr.s_addr != network.dns[network.currentDNS])
		{
			req->addr.sin_family = AF_INET;
			req->addr.sin_addr.s_addr = network.dns[network.currentDNS];
			req->addr.sin_port = htons(DnsServerPort);
			errno = 0;

			nRet = sendto(network.forwConn.sock,
						  req->raw,
						  req->bytes,
						  0,
						  (sockaddr*)&req->addr,
						  sizeof(req->addr));

			errno = WSAGetLastError();

			if (errno || nRet <= 0)
			{
				if (cfig.dnsLogLevel)
				{
					sprintf(logBuff, "Error forwading UDP DNS Message to Forwarding Server %s", IP2String(tempbuff, network.dns[network.currentDNS]));
					logDNSMess(req, logBuff, 1);
					addRRNone(req);
					req->dnsp->header.rcode = RCODE_SERVERFAIL;
				}

				if (network.dns[1])
				{
					network.currentDNS++;

					if (network.currentDNS >= MAX_SERVERS || !network.dns[network.currentDNS])
						network.currentDNS = 0;
				}

				return 0;
			}
			else
			{
				if (cfig.dnsLogLevel == 2)
				{
					sprintf(logBuff, "%s forwarded to Forwarding Server %s", req->cname, IP2String(tempbuff, network.dns[network.currentDNS]));
					logDNSMess(req, logBuff, 2);
				}
			}
		}
	}

	//printf("LocalCode=%u query=%s cname=%s mapname=%s\n", req->dnType, req->query, req->cname, req->mapname);

	time_t t = time(NULL);

	if (!queue)
	{
		queue = (data7*)calloc(1, sizeof(data7));
		if (queue)
		{
			queue->mapname = cloneString(mapname);
			queue->addr = (SOCKADDR_IN*)calloc(1, sizeof(req->remote));
			queue->query = cloneString(req->query);

			if (!queue->mapname || !queue->addr || !queue->query)
			{
				if (queue->mapname)
					free(queue->mapname);

				if (queue->addr)
					free(queue->addr);

				if (queue->query)
					free(queue->query);

				free(queue);
				sprintf(logBuff, "Memory Allocation Error");
				logDNSMess(logBuff, 1);
				return 0;
			}

			memcpy(queue->addr, &req->remote, sizeof(req->remote));
			queue->expiry = 2 + t;
			queue->dataType = QUEUE;
			addEntry(cacheInd, queue);
		}
		else
		{
			sprintf(logBuff, "Memory Allocation Error");
			logDNSMess(logBuff, 1);
			return 0;
		}
	}
	else
	{
		queue->expiry = 2 + t;
		memcpy(queue->addr, &req->remote, sizeof(req->remote));

		if (strcasecmp(queue->query, req->query))
		{
			char *query = cloneString(req->query);

			if (query)
			{
				free(queue->query);
				queue->query = query;
			}
			else
			{
				sprintf(logBuff, "Memory Allocation Error");
				logDNSMess(logBuff, 1);
				return 0;
			}
		}
	}

	queue->sockInd = req->sockInd;

	if (req->dnType == DNTYPE_SUBZONE)
		queue->dnsIndex = 128 + (2 * zoneDNS) + cfig.dnsRoutes[zoneDNS].currentDNS;
	else
		queue->dnsIndex = network.currentDNS;

	//printf("%u %u\n", zoneDNS, queue->dnsIndex);

	return (nRet);
}

MYWORD CDualServer::frdnmess(data5 *req)
{
	memset(req, 0, sizeof(data5));
	req->sockLen = sizeof(req->remote);
	errno = 0;

	req->bytes = recvfrom(network.forwConn.sock,
	                      req->raw,
	                      sizeof(req->raw),
	                      0,
	                      (sockaddr*)&req->remote,
	                      &req->sockLen);

	//printf("%u\n", req->bytes);

	errno = WSAGetLastError();

	if (errno || req->bytes <= 0)
		return 0;

	req->dnsp = (dnsPacket*)req->raw;

	char mapname[8];
	MYWORD type = 0;
	sprintf(mapname, "%u", req->dnsp->header.xid);
	data7 *queue = findEntry(cacheInd, mapname);

	if (queue && queue->expiry)
	{
		queue->expiry = 0;

		if (queue->dnsIndex < MAX_SERVERS)
		{
			if (req->remote.sin_addr.s_addr != network.dns[network.currentDNS])
			{
				for (MYBYTE i = 0; i < MAX_SERVERS && network.dns[i]; i++)
				{
					if (network.dns[i] == req->remote.sin_addr.s_addr)
					{
						network.currentDNS = i;
						break;
					}
				}
			}
		}
		else if (queue->dnsIndex >= 128 && queue->dnsIndex < 192)
		{
			data6 *dnsRoute = &cfig.dnsRoutes[(queue->dnsIndex - 128) / 2];

			if (dnsRoute->dns[0] == req->remote.sin_addr.s_addr)
				dnsRoute->currentDNS = 0;
			else if (dnsRoute->dns[1] == req->remote.sin_addr.s_addr)
				dnsRoute->currentDNS = 1;
		}

		if (queue->dataType == DNS_CHECK)
		{
			if (cfig.dnsLogLevel)
			{
				if (queue->dnsIndex < MAX_SERVERS)
				{
					sprintf(logBuff, "Forwarding DNS Server %s responded", IP2String(tempbuff, req->remote.sin_addr.s_addr));
					logDNSMess(logBuff, 1);
				}
				else if (queue->dnsIndex >= 128 && queue->dnsIndex < 192)
				{
					sprintf(logBuff, "Child DNS Server %s responded", IP2String(tempbuff, req->remote.sin_addr.s_addr));
					logDNSMess(logBuff, 1);
				}
			}

			return 0;
		}
		else if (queue->dataType == QUEUE)
		{
			memcpy(&req->remote, queue->addr, sizeof(req->remote));
			strcpy(req->query, queue->query);
			req->sockInd = queue->sockInd;
			req->cache.dnsIndex = queue->dnsIndex;

			req->dp = &req->dnsp->data;

			for (int i = 1; i <= ntohs(req->dnsp->header.qdcount); i++)
			{
				req->dp += fQu(req->cname, req->dnsp, req->dp);
				type = fUShort(req->dp);
				req->dp += 4; //type and class
			}

			if ((type == DNS_TYPE_A || type == DNS_TYPE_PTR) && !req->dnsp->header.rcode && !req->dnsp->header.tc && req->dnsp->header.ancount)
			{
				time_t expiry = 0;
				bool resultFound = false;

				for (int i = 1; i <= ntohs(req->dnsp->header.ancount); i++)
				{
					req->dp += fQu(tempbuff, req->dnsp, req->dp);
					type = fUShort(req->dp);

					//printf("%s %u=%u\n", tempbuff, type, DNS_TYPE_A);

					if (type == DNS_TYPE_A)
					{
						resultFound = true;
						strcpy(req->mapname, req->cname);
						myLower(req->mapname);
						makeLocal(req->mapname);
					}
					else if (type == DNS_TYPE_PTR)
					{
						strcpy(req->mapname, req->cname);
						myLower(req->mapname);
						char *dp = strstr(req->mapname, arpa);

						if (dp && !strcasecmp(dp, arpa))
						{
							*dp = 0;
							resultFound = true;
						}
					}

					req->dp += 4; //type and class

					if (!expiry || fULong(req->dp) < (MYDWORD)expiry)
						expiry = fULong(req->dp);

					req->dp += 4; //ttl
					int zLen = fUShort(req->dp);
					req->dp += 2; //datalength
					req->dp += zLen;
				}

				time_t t = time(NULL);

				if (resultFound)
				{
					MYWORD cacheSize = req->dp - req->raw;

					if (cfig.minCache && expiry < cfig.minCache)
						expiry = cfig.minCache;

					if (cfig.maxCache && expiry > cfig.maxCache)
						expiry = cfig.maxCache;

					if (expiry < INT_MAX - t)
						expiry += t;
					else
						expiry = INT_MAX;

					data7 *cache = findEntry(cacheInd, req->mapname, CACHED);

					if (cache)
					{
						if (cache->bytes < cacheSize)
						{
							MYBYTE *response = (MYBYTE*)calloc(1, cacheSize);

							if (response)
							{
								if (cache->response)
									free(cache->response);

								cache->response = response;
							}
							else
							{
								sprintf(logBuff, "Memory Allocation Error");
								logDNSMess(logBuff, 1);
								return 0;
							}
						}
						cache->expiry = expiry;
					}
					else
					{
						//checkSize();
						cache = (data7*)calloc(1, sizeof(data7));

						if (!cache)
						{
							sprintf(logBuff, "Memory Allocation Error");
							logDNSMess(logBuff, 1);
							return 0;
						}

						cache->mapname = cloneString(req->mapname);
						cache->response = (MYBYTE*)calloc(1, cacheSize);

						if (!cache->mapname || !cache->response)
						{
							if (cache->mapname)
								free(cache->mapname);

							if (cache->response)
								free(cache->response);

							free(cache);

							sprintf(logBuff, "Memory Allocation Error");
							logDNSMess(logBuff, 1);
							return 0;
						}

						cache->expiry = expiry;
						cache->dataType = CACHED;
						addEntry(cacheInd, cache);

					}

					memcpy(cache->response, req->dnsp, cacheSize);
					((dnsPacket*)cache->response)->header.nscount = 0;
					((dnsPacket*)cache->response)->header.adcount = 0;
					cache->bytes = cacheSize;
				}
			}

			req->cache.dataType = NON_CACHED;
			addRRExt(req);
			return 1;
		}
	}
	return 0;
}

void CDualServer::addToCache(MYBYTE ind, char *hostname, MYDWORD ip, time_t expiry, MYBYTE aType, MYBYTE pType, MYDWORD serial)
{
	//printf("Adding to %u, %s=%s exp=%u\n",ind, hostname,IP2String(tempbuff, ip),expiry);
	char tempbuff[256];
	char logBuff[256];

	if (!hostname || !hostname[0] || !ip)
		return;

	data7 *cache = NULL;
	hostMap::iterator p;

	if (pType)
	{
		IP2String(tempbuff, htonl(ip));
		p = dnsCache[ind].find(tempbuff);

		for (;p != dnsCache[ind].end(); p++)
		{
			if (strcasecmp(p->second->mapname, tempbuff))
				break;

			if (!strcasecmp(p->second->hostname, hostname))
			{
				cache = p->second;
				break;
			}
		}

		if (!cache)
		{
			//checkSize();
			cache = (data7*)calloc(1, sizeof(data7));

			if (cache)
			{
				cache->mapname = cloneString(tempbuff);
				cache->hostname = cloneString(hostname);

				if (!cache->mapname || !cache->hostname)
				{
					if (cache->mapname)
						free(cache->mapname);

					if (cache->hostname)
						free(cache->hostname);

					free(cache);

					sprintf(logBuff, "Memory Allocation Error");
					logDNSMess(logBuff, 1);
					return ;
				}

				cache->dataType = pType;
				cache->expiry = expiry;
				addEntry(ind, cache);
			}
			else
			{
				sprintf(logBuff, "Memory Allocation Error");
				logDNSMess(logBuff, 1);
				return ;
			}
		}
		else
		{
			if (strcasecmp(hostname, cache->hostname))
			{
				free(cache->hostname);
				cache->hostname = cloneString(hostname);
			}

			if (pType == LOCAL_PTR_AUTH)
				cache->expiry = expiry;
			else if (cache->expiry < expiry)
			{
				cache->dataType = pType;
				cache->expiry = expiry;
			}
		}

		cache->serial = serial;

		if (!strcasecmp(hostname, cfig.zone))
			cache->isZone = 1;
	}

	if (aType)
	{
		cache = NULL;
		strcpy(tempbuff, hostname);
		makeLocal(tempbuff);
		myLower(tempbuff);

		p = dnsCache[ind].find(tempbuff);

		for (; p != dnsCache[ind].end(); p++)
		{
			if (strcasecmp(p->second->mapname, tempbuff))
				break;

			if (p->second->ip == ip)
			{
				cache = p->second;
				break;
			}
		}

		if (!cache)
		{
			//checkSize();
			cache = (data7*)calloc(1, sizeof(data7));
			if (cache)
			{
				cache->mapname = cloneString(tempbuff);

				if (!cache->mapname)
				{
					sprintf(logBuff, "Memory Allocation Error");
					free(cache);
					logDNSMess(logBuff, 1);
					return ;
				}

				cache->ip = ip;
				cache->dataType = aType;
				cache->expiry = expiry;
				addEntry(ind, cache);
			}
			else
			{
				sprintf(logBuff, "Memory Allocation Error");
				logDNSMess(logBuff, 1);
				return ;
			}
		}
		else
		{
			cache->ip = ip;

			if (aType == LOCAL_A)
				cache->expiry = expiry;
			else if (cache->expiry < expiry)
			{
				cache->dataType = aType;
				cache->expiry = expiry;
			}
		}

		cache->serial = serial;

		if (!strcasecmp(hostname, cfig.zone))
			cache->isZone = 1;
	}
}

char* CDualServer::getResult(data5 *req)
{
	tempbuff[0] = 0;
	extbuff[0] = 0;
	char *raw = &req->dnsp->data;

	for (int i = 1; i <= ntohs(req->dnsp->header.qdcount); i++)
	{
		raw += fQu(extbuff, req->dnsp, raw);
		raw += 4;
	}

	for (int i = 1; i <= ntohs(req->dnsp->header.ancount); i++)
	{
		raw += fQu(extbuff, req->dnsp, raw);
		int type = fUShort(raw);
		raw += 2; //type
		raw += 2; //class
		raw += 4; //ttl
		int zLen = fUShort(raw);
		raw += 2; //datalength

		if (type == DNS_TYPE_A)
			return IP2String(tempbuff, fIP(raw));
		else if (type == DNS_TYPE_AAAA)
			return IP62String(tempbuff, (MYBYTE*)raw);
		else if (type == DNS_TYPE_PTR)
		{
			fQu(tempbuff, req->dnsp, raw);
			return tempbuff;
		}
		else if (type == DNS_TYPE_MX)
			fQu(tempbuff, req->dnsp, (raw + 2));
		else if (type == DNS_TYPE_CNAME)
			fQu(tempbuff, req->dnsp, raw);
		else if (type == DNS_TYPE_NS)
			fQu(tempbuff, req->dnsp, raw);

		raw += zLen;
	}

	if (tempbuff[0])
		return tempbuff;
	else
		return NULL;
}


bool CDualServer::checkRange(char rangeInd, bool macFound, bool vendFound, bool userFound)
{
	if (!cfig.hasFilter)
		return true;

	MYBYTE rangeSetInd = cfig.dhcpRanges[rangeInd].rangeSetInd;
	data14 *rangeSet = &cfig.rangeSet[rangeSetInd];
	//printf("checkRange entering, rangeInd=%i rangeSetInd=%i\n", rangeInd, rangeSetInd);
	//printf("checkRange entered, macFound=%i vendFound=%i userFound=%i\n", macFound, vendFound, userFound);

	if((!macFound && !rangeSet->macSize[0]) || (macFound && cfig.macArray[rangeSetInd]))
		if((!vendFound && !rangeSet->vendClassSize[0]) || (vendFound && cfig.vendArray[rangeSetInd]))
			if((!userFound && !rangeSet->userClassSize[0]) || (userFound && cfig.userArray[rangeSetInd]))
				return true;

	//printf("checkRange, returning false rangeInd=%i rangeSetInd=%i\n", rangeInd, rangeSetInd);
	return false;
}

MYDWORD CDualServer::resad(data9 *req)
{
	time_t t = time(NULL);
	MYDWORD minRange = 0;
	MYDWORD maxRange = 0;

	if (req->dhcpp.header.bp_giaddr)
	{
		setLeaseExpiry(req->dhcpp.header.bp_giaddr, INT_MAX);
		setLeaseExpiry(req->remote.sin_addr.s_addr, INT_MAX);
	}

	MYDWORD iipNew = 0;
	MYDWORD iipExp = 0;
	MYDWORD rangeStart = 0;
	MYDWORD rangeEnd = 0;
	char rangeInd = -1;
	bool rangeFound = false;
	bool macFound = false;
	bool vendFound = false;
	bool userFound = false;
	memset(cfig.macArray, 0, sizeof(cfig.macArray));
	memset(cfig.vendArray, 0, sizeof(cfig.vendArray));
	memset(cfig.userArray, 0, sizeof(cfig.userArray));

	if (cfig.hasFilter)
	{
		for (MYBYTE rangeSetInd = 0; rangeSetInd < MAX_RANGES && cfig.rangeSet[rangeSetInd].active; rangeSetInd++)
		{
			data14 *rangeSet = &cfig.rangeSet[rangeSetInd];

			for (MYBYTE i = 0; i < 32 && rangeSet->macSize[i]; i++)
			{
				if(memcmp(req->dhcpp.header.bp_chaddr, rangeSet->macStart[i], rangeSet->macSize[i]) >= 0 && memcmp(req->dhcpp.header.bp_chaddr, rangeSet->macEnd[i], rangeSet->macSize[i]) <= 0)
				{
					cfig.macArray[rangeSetInd] = true;
					macFound = true;
					//printf("mac Found, rangeSetInd=%i\n", rangeSetInd);
					break;
				}
			}

			for (MYBYTE i = 0; i < 32 && req->vendClassSize && rangeSet->vendClassSize[i]; i++)
			{
				if(rangeSet->vendClassSize[i] == req->vendClassSize && !memcmp(req->vendClass, rangeSet->vendClass[i], rangeSet->vendClassSize[i]))
				{
					cfig.vendArray[rangeSetInd] = true;
					vendFound = true;
					//printf("vend Found, rangeSetInd=%i\n", rangeSetInd);
					break;
				}
			}

			for (MYBYTE i = 0; i < 32 && req->userClassSize && rangeSet->userClassSize[i]; i++)
			{
				if(rangeSet->userClassSize[i] == req->userClassSize && !memcmp(req->userClass, rangeSet->userClass[i], rangeSet->userClassSize[i]))
				{
					cfig.userArray[rangeSetInd] = true;
					userFound = true;
					//printf("user Found, rangeSetInd=%i\n", rangeSetInd);
					break;
				}
			}
		}
	}

//	printArray("macArray", (char*)cfig.macArray);
//	printArray("vendArray", (char*)cfig.vendArray);
//	printArray("userArray", (char*)cfig.userArray);

	if (chad(req))
	{
		bool rangeOK = req->dhcpEntry->fixed;

		if (!rangeOK && req->dhcpEntry->rangeInd >= 0)
			rangeOK = checkRange(req->dhcpEntry->rangeInd, macFound, vendFound, userFound);

		if (rangeOK)
		{
			MYDWORD mask = INADDR_NONE;

			if (req->dhcpEntry->bitmask)
				mask = htonl(mask << (32 - req->dhcpEntry->bitmask));
			else if (req->dhcpEntry->rangeInd >= 0)
				mask = cfig.dhcpRanges[req->dhcpEntry->rangeInd].mask;
			else
				mask = cfig.mask;

			if (req->dhcpp.header.bp_giaddr)
				calcRangeLimits(req->dhcpp.header.bp_giaddr, mask, &minRange, &maxRange);
			else if (htonl(network.dhcpConn[req->sockInd].mask) > htonl(mask))
				calcRangeLimits(network.dhcpConn[req->sockInd].server, network.dhcpConn[req->sockInd].mask, &minRange, &maxRange);
			else
				calcRangeLimits(network.dhcpConn[req->sockInd].server, mask, &minRange, &maxRange);

			if (htonl(req->dhcpEntry->ip) >= minRange && htonl(req->dhcpEntry->ip) <= maxRange)
			{
				setLeaseExpiry(req->dhcpEntry, 20, false);
				return req->dhcpEntry->ip;
			}
			else if (req->dhcpEntry->fixed)
			{
				req->dhcpEntry->no_route = 1;
				setLeaseExpiry(req->dhcpEntry, 20, false);
				return req->dhcpEntry->ip;
			}
		}
	}

	if (req->hostname[0])
	{
		char hostname[256];
		strcpy(hostname, req->hostname);
		myLower(hostname);
		hostMap::iterator it = dnsCache[cacheInd].find(hostname);

		for (; it != dnsCache[cacheInd].end(); it++)
		{
			data7 *cache = it->second;

			//printf("%u\n", cache->mapname);

			if (strcasecmp(cache->mapname, hostname))
				break;

			if (cache && cache->ip)
			{
				char k = getRangeInd(cache->ip);

				if (k >= 0)
				{
					if (checkRange(k, macFound, vendFound, userFound))
					{
						char ind = getIndex(k, cache->ip);
						data13 *range = &cfig.dhcpRanges[k];

						if (ind >= 0 && range->expiry[ind] <= t && !range->dhcpEntry[ind])
						{
							if (req->dhcpp.header.bp_giaddr)
								calcRangeLimits(req->dhcpp.header.bp_giaddr, range->mask, &minRange, &maxRange);
							else if (htonl(network.dhcpConn[req->sockInd].mask) > htonl(range->mask))
								calcRangeLimits(network.dhcpConn[req->sockInd].server, network.dhcpConn[req->sockInd].mask, &minRange, &maxRange);
							else
								calcRangeLimits(network.dhcpConn[req->sockInd].server, range->mask, &minRange, &maxRange);

							MYDWORD iip = htonl(cache->ip);

							if (iip >= minRange && iip <= maxRange)
							{
								iipNew = iip;
								rangeInd = k;
								break;
							}
						}
					}
				}
			}
		}
	}

	if (!iipNew && req->reqIP)
	{
		char k = getRangeInd(req->reqIP);

		if (k >= 0)
		{
			if (checkRange(k, macFound, vendFound, userFound))
			{
				data13 *range = &cfig.dhcpRanges[k];
				char ind = getIndex(k, req->reqIP);

				if (ind >= 0 && range->expiry[ind] <= t)
				{
					if (req->dhcpp.header.bp_giaddr)
						calcRangeLimits(req->dhcpp.header.bp_giaddr, range->mask, &minRange, &maxRange);
					else if (htonl(network.dhcpConn[req->sockInd].mask) > htonl(range->mask))
						calcRangeLimits(network.dhcpConn[req->sockInd].server, network.dhcpConn[req->sockInd].mask, &minRange, &maxRange);
					else
						calcRangeLimits(network.dhcpConn[req->sockInd].server, range->mask, &minRange, &maxRange);

					MYDWORD iip = htonl(req->reqIP);

					if (iip >= minRange && iip <= maxRange)
					{
						iipNew = iip;
						rangeInd = k;
					}
				}
			}
		}
	}

	for (char k = 0; !iipNew && k < MAX_RANGES && cfig.dhcpRanges[k].rangeStart; k++)
	{
		if (checkRange(k, macFound, vendFound, userFound))
		{
			data13 *range = &cfig.dhcpRanges[k];
			rangeStart = range->rangeStart;
			rangeEnd = range->rangeEnd;

			if (req->dhcpp.header.bp_giaddr)
				calcRangeLimits(req->dhcpp.header.bp_giaddr, range->mask, &minRange, &maxRange);
			else if (htonl(network.dhcpConn[req->sockInd].mask) > htonl(range->mask))
				calcRangeLimits(network.dhcpConn[req->sockInd].server, network.dhcpConn[req->sockInd].mask, &minRange, &maxRange);
			else
				calcRangeLimits(network.dhcpConn[req->sockInd].server, range->mask, &minRange, &maxRange);

			if (rangeStart < minRange)
				rangeStart = minRange;

			if (rangeEnd > maxRange)
				rangeEnd = maxRange;

			if (rangeStart <= rangeEnd)
			{
				rangeFound = true;

				for (MYDWORD m = rangeStart; m <= rangeEnd; m++)
				{
					if (!range->expiry[m - range->rangeStart])
					{
						iipNew = m;
						rangeInd = k;
						break;
					}
					else if (!iipExp && range->expiry[m - range->rangeStart] < t)
					{
						iipExp = m;
						rangeInd = k;
					}
				}
			}
		}
	}

	if (!iipNew && iipExp)
	{
		char ind = iipExp - cfig.dhcpRanges[rangeInd].rangeStart;
		req->dhcpEntry = cfig.dhcpRanges[rangeInd].dhcpEntry[ind];

		if (req->dhcpEntry)
		{
			dhcpCache.erase(req->dhcpEntry->mapname);
			free(req->dhcpEntry->mapname);
			req->dhcpEntry->mapname = cloneString(toUUE(tempbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen));

			if (!req->dhcpEntry->mapname)
			{
				sprintf(logBuff, "Memory Allocation Error");
				logDHCPMess(logBuff, 1);
				return 0;
			}

			dhcpCache[req->dhcpEntry->mapname] = req->dhcpEntry;
			req->dhcpEntry->ip = htonl(iipExp);
			req->dhcpEntry->rangeInd = rangeInd;
			setLeaseExpiry(req->dhcpEntry, 20, false);

			if (!req->dhcpEntry->fixed)
			{
				if (req->dhcpp.header.bp_giaddr)
					req->dhcpEntry->source = req->dhcpp.header.bp_giaddr;
				else
					req->dhcpEntry->source = network.dhcpConn[req->sockInd].server;
			}

			return req->dhcpEntry->ip;
		}
		else
			iipNew = iipExp;
	}

	if (iipNew)
	{
		if (!req->dhcpEntry)
		{
			req->dhcpEntry = (data7*)calloc(1, sizeof(data7));

			if (!req->dhcpEntry)
			{
				sprintf(logBuff, "Memory Allocation Error");
				logDHCPMess(logBuff, 1);
				return 0;
			}

			req->dhcpEntry->mapname = cloneString(toUUE(tempbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen));

			if (!req->dhcpEntry->mapname)
			{
				sprintf(logBuff, "Memory Allocation Error");
				free(req->dhcpEntry);
				logDHCPMess(logBuff, 1);
				return 0;
			}

			dhcpCache[req->dhcpEntry->mapname] = req->dhcpEntry;
		}

		req->dhcpEntry->ip = htonl(iipNew);
		req->dhcpEntry->rangeInd = rangeInd;
		setLeaseExpiry(req->dhcpEntry, 20, false);

		if (!req->dhcpEntry->fixed)
		{
			if (req->dhcpp.header.bp_giaddr)
				req->dhcpEntry->source = req->dhcpp.header.bp_giaddr;
			else
				req->dhcpEntry->source = network.dhcpConn[req->sockInd].server;
		}

		return req->dhcpEntry->ip;
	}

	if (!iipNew)
	{
		if (cfig.dhcpLogLevel)
		{
			if (rangeFound)
			{
				if (req->dhcpp.header.bp_giaddr)
					sprintf(logBuff, "No free leases for DHCP discover for %s (%s) from RelayAgent %s", hex2String(extbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen, ':'), req->hostname, IP2String(tempbuff, req->dhcpp.header.bp_giaddr));
				else
					sprintf(logBuff, "No free leases for DHCP discover for %s (%s) from interface %s", hex2String(extbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen, ':'), req->hostname, IP2String(tempbuff, network.dhcpConn[req->sockInd].server));
			}
			else
			{
				if (req->dhcpp.header.bp_giaddr)
					sprintf(logBuff, "No Matching DHCP Range for DHCP discover for %s (%s) from RelayAgent %s", hex2String(extbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen, ':'), req->hostname, IP2String(tempbuff, req->dhcpp.header.bp_giaddr));
				else
					sprintf(logBuff, "No Matching DHCP Range for DHCP discover for %s (%s) from interface %s", hex2String(extbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen, ':'), req->hostname, IP2String(tempbuff, network.dhcpConn[req->sockInd].server));
			}
			logDHCPMess(logBuff, 1);
		}
	}
	return 0;
}

MYDWORD CDualServer::chad(data9 *req)
{
	req->dhcpEntry = dhcpCache[toUUE(tempbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen)];

	if (req->dhcpEntry && req->dhcpEntry->ip && (req->dhcpEntry->fixed || (req->dhcpp.header.bp_giaddr && req->dhcpEntry->source == req->dhcpp.header.bp_giaddr) || req->dhcpEntry->source == network.dhcpConn[req->sockInd].server))
	{
		if (!req->hostname[0])
		{
			data7 *cache = findEntry(cacheInd, IP2String(tempbuff, htonl(req->dhcpEntry->ip)));

			if (cache && cache->hostname)
				strcpy(req->hostname, cache->hostname);
			else
				sprintf(req->hostname, "%s", hex2String(tempbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen, '-'));
		}
		return req->dhcpEntry->ip;
	}
	else
		return 0;
}

MYDWORD CDualServer::sdmess(data9 *req)
{
	time_t t = time(NULL);

	//printf("Request=%u Req IP=%s\n",req->req_type, IP2String(tempbuff, req->dhcpp.header.bp_ciaddr));
	if (req->req_type == DHCP_MESS_NONE)
	{
		req->dhcpp.header.bp_yiaddr = chad(req);

		if (!req->dhcpp.header.bp_yiaddr)
		{
			if (cfig.dhcpLogLevel)
			{
				sprintf(logBuff, "No Static Entry found for BOOTP request from client %s", hex2String(tempbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen, ':'));
				logDHCPMess(logBuff, 1);
			}
			return 0;
		}
	}
	else if (req->req_type == DHCP_MESS_DECLINE)
	{
		if (chad(req) == req->dhcpp.header.bp_ciaddr)
		{
			setLeaseExpiry(req->dhcpp.header.bp_ciaddr, INT_MAX);

			req->dhcpEntry->ip = 0;

			if (cfig.dhcpLogLevel)
			{
				sprintf(logBuff, "IP Address %s declined by Client %s (%s), locked", IP2String(tempbuff, req->dhcpp.header.bp_ciaddr), hex2String(extbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen, ':'), req->hostname);
				logDHCPMess(logBuff, 1);
			}
		}

		return 0;
	}
	else if (req->req_type == DHCP_MESS_RELEASE)
	{
		if (chad(req) == req->dhcpp.header.bp_ciaddr)
		{
			req->lease = 0;
			updateDHCP(req);

			if (req->dhcpEntry->local && cfig.replication)
				sendRepl(req);

			if (cfig.dhcpLogLevel >= 2)
			{
				sprintf(logBuff, "IP Address %s released by Client %s (%s)", IP2String(tempbuff, req->dhcpp.header.bp_ciaddr), hex2String(extbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen, ':'), req->hostname);
				logDHCPMess(logBuff, 2);
			}
		}

		return 0;
	}
	else if (req->req_type == DHCP_MESS_INFORM)
	{
		recvRepl(req);
		return 0;
	}
	else if (req->req_type == DHCP_MESS_DISCOVER && strcasecmp(req->hostname, cfig.servername))
	{
		req->dhcpp.header.bp_yiaddr = resad(req);

		if (!req->dhcpp.header.bp_yiaddr)
			return 0;

		req->resp_type = DHCP_MESS_OFFER;
	}
	else if (req->req_type == DHCP_MESS_REQUEST)
	{
		//printf("%s\n", IP2String(tempbuff, req->dhcpp.header.bp_ciaddr));

		if (req->server)
		{
			if (req->server == network.dhcpConn[req->sockInd].server)
			{
				if (req->reqIP && req->reqIP == chad(req) && req->dhcpEntry->expiry > t)
				{
					req->resp_type = DHCP_MESS_ACK;
					req->dhcpp.header.bp_yiaddr = req->reqIP;
				}
				else if (req->dhcpp.header.bp_ciaddr && req->dhcpp.header.bp_ciaddr == chad(req) && req->dhcpEntry->expiry > t)
				{
					req->resp_type = DHCP_MESS_ACK;
					req->dhcpp.header.bp_yiaddr = req->dhcpp.header.bp_ciaddr;
				}
				else
				{
					req->resp_type = DHCP_MESS_NAK;
					req->dhcpp.header.bp_yiaddr = 0;

					if (cfig.dhcpLogLevel >= 1)
					{
						sprintf(logBuff, "DHCP Request from Client %s (%s) without Discover, NAKed", hex2String(extbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen, ':'), req->hostname);
						logDHCPMess(logBuff, 1);
					}
				}
/*
				else
				{
					if (cfig.dhcpLogLevel >= 1)
					{
						sprintf(logBuff, "DHCP Request from Client %s (%s), without Discover, ignored", hex2String(extbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen, ':'), req->hostname);
						logDHCPMess(logBuff, 1);
					}
					return 0;
				}
*/
			}
			else
			{
				if (!req->hostname[0])
					sprintf(req->hostname, "%s", hex2String(tempbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen, '-'));

				if (cfig.dhcpLogLevel >= 2)
				{
					if (req->dhcpp.header.bp_sname[0])
						sprintf(logBuff, "DHCP Request from Client %s (%s), for Server %s, ignored", hex2String(extbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen, ':'), req->hostname, req->dhcpp.header.bp_sname);
					else
						sprintf(logBuff, "DHCP Request from Client %s (%s), for Server %s, ignored", hex2String(extbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen, ':'), req->hostname, IP2String(tempbuff, req->server));

					logDHCPMess(logBuff, 2);
				}
				return 0;
			}
		}
		else if (req->dhcpp.header.bp_ciaddr && req->dhcpp.header.bp_ciaddr == chad(req) && req->dhcpEntry->expiry > t)
		{
			req->resp_type = DHCP_MESS_ACK;
			req->dhcpp.header.bp_yiaddr = req->dhcpp.header.bp_ciaddr;
		}
		else if (req->reqIP && req->reqIP == chad(req) && req->dhcpEntry->expiry > t)
		{
			req->resp_type = DHCP_MESS_ACK;
			req->dhcpp.header.bp_yiaddr = req->reqIP;
		}
		else
		{
			req->resp_type = DHCP_MESS_NAK;
			req->dhcpp.header.bp_yiaddr = 0;

			if (cfig.dhcpLogLevel >= 1)
			{
				sprintf(logBuff, "DHCP Request from Client %s (%s) without Discover, NAKed", hex2String(extbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen, ':'), req->hostname);
				logDHCPMess(logBuff, 1);
			}
		}
/*
		else
		{
			if (cfig.dhcpLogLevel >= 1)
			{
				sprintf(logBuff, "DHCP Request from Client %s (%s) without Discover, ignored", hex2String(extbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen, ':'), req->hostname);
				logDHCPMess(logBuff, 1);
			}
			return 0;
		}
*/
	}
	else
		return 0;

	addOptions(req);
	int packSize = req->vp - (MYBYTE*)&req->dhcpp;
	packSize++;

	if (req->req_type == DHCP_MESS_NONE)
		packSize = req->bytes;

	if (req->dhcpp.header.bp_giaddr)
	{
		req->remote.sin_port = htons(IPPORT_DHCPS);
		req->remote.sin_addr.s_addr = req->dhcpp.header.bp_giaddr;
	}
	else if (req->dhcpp.header.bp_broadcast || !req->remote.sin_addr.s_addr)
	{
		req->remote.sin_port = htons(IPPORT_DHCPC);
		req->remote.sin_addr.s_addr = INADDR_BROADCAST;
	}
	else
	{
		req->remote.sin_port = htons(IPPORT_DHCPC);
	}

	req->dhcpp.header.bp_op = BOOTP_REPLY;
	errno = 0;

	req->bytes = sendto(network.dhcpConn[req->sockInd].sock,
	                    req->raw,
	                    packSize,
	                    0, //MSG_DONTROUTE,
	                    (sockaddr*)&req->remote,
	                    sizeof(req->remote));

	if (errno || req->bytes <= 0)
		return 0;

	//printf("goes=%s %i\n",IP2String(tempbuff, req->dhcpp.header.bp_yiaddr),req->sockInd);
	return req->dhcpp.header.bp_yiaddr;
}

MYDWORD CDualServer::alad(data9 *req)
{
	time_t t = time(NULL);

	//printf("in\n");
	if (req->dhcpEntry && (req->req_type == DHCP_MESS_NONE || req->resp_type == DHCP_MESS_ACK))
	{
		if (req->reqIP || req->req_type == DHCP_MESS_NONE)
		{
			cfig.serial1 = static_cast<unsigned int>(t);
			cfig.serial2 = static_cast<unsigned int>(t);

			if (cfig.replication == 2)
			{
				if (cfig.expire > (MYDWORD)(INT_MAX - t))
					cfig.expireTime = INT_MAX;
				else
					cfig.expireTime = t + cfig.expire;
			}
			else
				cfig.expireTime = INT_MAX;
		}

		MYDWORD retVal = updateDHCP(req);

		if (retVal)
		{
			if (cfig.dhcpLogLevel == 2)
			{
				if (req->lease && req->reqIP)
				{
					sprintf(logBuff, "Client %s (%s) allotted %s for %u seconds", hex2String(extbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen, ':'), req->hostname, IP2String(tempbuff, req->dhcpp.header.bp_yiaddr), req->lease);
				}
				else if (req->req_type)
				{
					sprintf(logBuff, "Client %s (%s) renewed %s for %u seconds", hex2String(extbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen, ':'), req->hostname, IP2String(tempbuff, req->dhcpp.header.bp_yiaddr), req->lease);
				}
				else
				{
					sprintf(logBuff, "BOOTP Client %s (%s) allotted %s", hex2String(extbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen, ':'), req->hostname, IP2String(tempbuff, req->dhcpp.header.bp_yiaddr));
				}
				logDHCPMess(logBuff, 2);
			}

			sendRepl(req);
			return retVal;
		}
	}
	else if (cfig.dhcpLogLevel == 2 && req->resp_type == DHCP_MESS_OFFER)
	{
		sprintf(logBuff, "Client %s (%s) offered %s", hex2String(extbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen, ':'), req->hostname, IP2String(tempbuff, req->dhcpp.header.bp_yiaddr));
		logDHCPMess(logBuff, 2);
	}
	//printf("%u=out\n", req->resp_type);
	return 0;
}

void CDualServer::addOptions(data9 *req)
{
	if (!req->messsize && req->req_type == DHCP_MESS_NONE)
		req->messsize = req->bytes;
	else if (!req->messsize)
		req->messsize = sizeof(dhcp_packet);

	data3 op;
	int i;

	if (req->req_type && req->resp_type)
	{
		op.opt_code = DHCP_OPTION_MESSAGETYPE;
		op.size = 1;
		op.value[0] = req->resp_type;
		pvdata(req, &op);
	}

	if (req->dhcpEntry && req->resp_type != DHCP_MESS_DECLINE && req->resp_type != DHCP_MESS_NAK)
	{
		strcpy(req->dhcpp.header.bp_sname, cfig.servername);

		if (req->dhcpEntry->fixed)
		{
			//printf("%u,%u\n", req->dhcpEntry->options, *req->dhcpEntry->options);
			MYBYTE *opPointer = req->dhcpEntry->options;

			if (opPointer)
			{
				MYBYTE requestedOnly = *opPointer;
				opPointer++;

				while (*opPointer && *opPointer != DHCP_OPTION_END)
				{
					op.opt_code = *opPointer;
					opPointer++;
					op.size = *opPointer;
					opPointer++;

					if (!requestedOnly || req->paramreqlist[*opPointer])
					{
						memcpy(op.value, opPointer, op.size);
						pvdata(req, &op);
					}
					opPointer += op.size;
				}
			}
		}

		if (req->req_type && req->resp_type)
		{
			if (req->dhcpEntry->rangeInd >= 0)
			{
				MYBYTE *opPointer = cfig.dhcpRanges[req->dhcpEntry->rangeInd].options;
				//printf("Range=%i Pointer=%u\n", req->dhcpEntry->rangeInd,opPointer);

				if (opPointer)
				{
					MYBYTE requestedOnly = *opPointer;
					opPointer++;

					while (*opPointer && *opPointer != DHCP_OPTION_END)
					{
						op.opt_code = *opPointer;
						opPointer++;
						op.size = *opPointer;
						opPointer++;

						if (!requestedOnly || req->paramreqlist[*opPointer])
						{
							memcpy(op.value, opPointer, op.size);
							pvdata(req, &op);
						}
						opPointer += op.size;
					}
				}
			}

			MYBYTE *opPointer = cfig.options;

			if (opPointer)
			{
				MYBYTE requestedOnly = *opPointer;

				opPointer++;
				while (*opPointer && *opPointer != DHCP_OPTION_END)
				{
					op.opt_code = *opPointer;
					opPointer++;
					op.size = *opPointer;
					opPointer++;

					if (!requestedOnly || req->paramreqlist[*opPointer])
					{
						memcpy(op.value, opPointer, op.size);
						pvdata(req, &op);
					}
					opPointer += op.size;
				}
			}

			op.opt_code = DHCP_OPTION_SERVERID;
			op.size = 4;
			pIP(op.value, network.dhcpConn[req->sockInd].server);
			pvdata(req, &op);

			op.opt_code = DHCP_OPTION_DOMAINNAME;
			op.size = strlen(cfig.zone);
			memcpy(op.value, cfig.zone, op.size);
			pvdata(req, &op);

			if (!req->opAdded[DHCP_OPTION_IPADDRLEASE])
			{
				op.opt_code = DHCP_OPTION_IPADDRLEASE;
				op.size = 4;
				pULong(op.value, cfig.lease);
				pvdata(req, &op);
			}

			if (!req->opAdded[DHCP_OPTION_NETMASK])
			{
				op.opt_code = DHCP_OPTION_NETMASK;
				op.size = 4;

				if (req->dhcpEntry->rangeInd >= 0)
					pIP(op.value, cfig.dhcpRanges[req->dhcpEntry->rangeInd].mask);
				else
					pIP(op.value, network.dhcpConn[req->sockInd].mask);

				pvdata(req, &op);
			}

/*
			if (!req->opAdded[DHCP_OPTION_ROUTER])
			{
				op.opt_code = DHCP_OPTION_ROUTER;
				op.size = 4;
				pIP(op.value, network.dhcpConn[req->sockInd].server);
				pvdata(req, &op);
			}
*/
			if (!req->opAdded[DHCP_OPTION_DNS])
			{
				if (dnsService)
				{
					op.opt_code = DHCP_OPTION_DNS;
					if (cfig.replication == 1)
					{
						op.size = 8;
						pIP(op.value, network.dhcpConn[req->sockInd].server);
						pIP(op.value + 4, cfig.zoneServers[1]);
						pvdata(req, &op);
					}
					else if (cfig.replication == 2)
					{
						op.size = 8;
						pIP(op.value, network.dhcpConn[req->sockInd].server);
						pIP(op.value + 4, cfig.zoneServers[0]);
						pvdata(req, &op);
					}
					else
					{
						op.size = 4;
						pIP(op.value, network.dhcpConn[req->sockInd].server);
						pvdata(req, &op);
					}
				}
				else
				{
					for (i = 0; i < MAX_SERVERS; i++)
						if (!network.dns[i])
							break;

					if (i > 0)
					{
						op.opt_code = DHCP_OPTION_DNS;
						op.size = i * 4;
						memcpy(op.value, network.dns, op.size);
						pvdata(req, &op);
					}
				}
			}
/*
			if (!req->opAdded[DHCP_OPTION_HOSTNAME])
			{
				op.opt_code = DHCP_OPTION_HOSTNAME;
				op.size = strlen(req->hostname);
				memcpy(op.value, req->hostname, op.size);
				pvdata(req, &op);
			}

			if (req->clientId.opt_code == DHCP_OPTION_CLIENTID)
				pvdata(req, &req->clientId);
*/
			if (req->agentOption.opt_code == DHCP_OPTION_RELAYAGENTINFO)
				pvdata(req, &req->agentOption);
		}
	}

	*(req->vp) = DHCP_OPTION_END;
}

void CDualServer::pvdata(data9 *req, data3 *op)
{
	if (!req->opAdded[op->opt_code] && ((req->vp - (MYBYTE*)&req->dhcpp) + op->size < req->messsize))
	{
		if (op->opt_code == DHCP_OPTION_NEXTSERVER)
			req->dhcpp.header.bp_siaddr = fIP(op->value);
		else if(op->size)
		{
			if (op->opt_code == DHCP_OPTION_BOOTFILE && op->size <= 128)
				memcpy(req->dhcpp.header.bp_file, op->value, op->size);
			else if (op->opt_code == DHCP_OPTION_IPADDRLEASE)
			{
				if (!req->lease || req->lease > fULong(op->value))
					req->lease = fULong(op->value);

				if (req->dhcpEntry->no_route || req->lease >= INT_MAX)
					req->lease = UINT_MAX;

				pULong(op->value, req->lease);
			}
			else if (op->opt_code == DHCP_OPTION_HOSTNAME)
			{
				memcpy(req->hostname, op->value, op->size);
				req->hostname[op->size] = 0;
			}

			memcpy(req->vp, op, (op->size + 2));
			(req->vp) += 2;
			(req->vp) += op->size;
		}
		req->opAdded[op->opt_code] = true;
	}
}

MYDWORD CDualServer::updateDHCP(data9 *req)
{
	time_t t = time(NULL);
	data8 *dhcpData = (data8*)calloc(1, sizeof(data8));
	strcpy(dhcpData->hostname, req->hostname);
	memcpy(dhcpData->bp_chaddr, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen);
	dhcpData->bp_hlen = req->dhcpp.header.bp_hlen;
	dhcpData->ip = req->dhcpEntry->ip;

	if (!req->dhcpEntry->fixed)
		dhcpData->source = req->dhcpEntry->source;

	if (!req->req_type)
	{
		dhcpData->expiry = INT_MAX;
		setLeaseExpiry(req->dhcpEntry, INT_MAX, req->req_type == DHCP_MESS_REQUEST);
	}
	else if (req->lease > (MYDWORD)(INT_MAX - t))
	{
		dhcpData->expiry = INT_MAX;
		setLeaseExpiry(req->dhcpEntry, INT_MAX, req->req_type == DHCP_MESS_REQUEST);
	}
	else
	{
		dhcpData->expiry = t + req->lease;
		setLeaseExpiry(req->dhcpEntry, req->lease, req->req_type == DHCP_MESS_REQUEST);
	}

	addToCache(cacheInd, req->hostname, req->dhcpEntry->ip, dhcpData->expiry, LOCAL_A, NONE, cfig.serial1);

	if (makeLocal(req->dhcpEntry->ip))
		addToCache(cacheInd, req->hostname, req->dhcpEntry->ip, dhcpData->expiry, NONE, LOCAL_PTR_AUTH, cfig.serial2);
	else
		addToCache(cacheInd, req->hostname, req->dhcpEntry->ip, dhcpData->expiry, NONE, LOCAL_PTR_NAUTH, cfig.serial2);

	if (req->dhcpEntry->dhcpInd)
		dhcpData->dhcpInd = req->dhcpEntry->dhcpInd;
	else
	{
		cfig.dhcpIndex++;
		req->dhcpEntry->dhcpInd = cfig.dhcpIndex;
	}

	dhcpData->local = req->dhcpEntry->local;

	updateStateFile((void*)dhcpData);
//	if (_beginthread(&CDualServer::updateStateFile, 0, (void*)dhcpData) == 0)

	return req->dhcpEntry->ip;
}

void CDualServer::setLeaseExpiry(data7 *dhcpEntry, time_t expiry, bool local)
{
	//printf("%u\n", local);

	time_t t = time(NULL);

	if (dhcpService && dhcpEntry && dhcpEntry->ip)
	{
		if (INT_MAX - t < expiry)
			dhcpEntry->expiry = INT_MAX;
		else
			dhcpEntry->expiry = t + expiry;

		if (local && !dhcpEntry->local)
		{
			dhcpAge.insert(pair<time_t, data7*>(dhcpEntry->expiry, dhcpEntry));
			dhcpEntry->local = 1;
		}

		if (dhcpEntry->rangeInd >= 0 && dhcpEntry->rangeInd < MAX_RANGES)
		{
			MYDWORD iip = htonl(dhcpEntry->ip);

			if (iip >= cfig.dhcpRanges[dhcpEntry->rangeInd].rangeStart && iip <= cfig.dhcpRanges[dhcpEntry->rangeInd].rangeEnd)
			{
				int ind = iip - cfig.dhcpRanges[dhcpEntry->rangeInd].rangeStart;

				if (cfig.dhcpRanges[dhcpEntry->rangeInd].expiry[ind] != INT_MAX)
					cfig.dhcpRanges[dhcpEntry->rangeInd].expiry[ind] = dhcpEntry->expiry;

				cfig.dhcpRanges[dhcpEntry->rangeInd].dhcpEntry[ind] = dhcpEntry;
			}
		}
	}
}

void CDualServer::setLeaseExpiry(MYDWORD ip, time_t expiry)
{
	time_t t = time(NULL);

	if (dhcpService && ip)
	{
		MYDWORD iip = htonl(ip);

		for (char rangeInd = 0; rangeInd < MAX_RANGES && cfig.dhcpRanges[rangeInd].rangeStart; rangeInd++)
		{
			if (iip >= cfig.dhcpRanges[rangeInd].rangeStart && iip <= cfig.dhcpRanges[rangeInd].rangeEnd)
			{
				int ind = iip - cfig.dhcpRanges[rangeInd].rangeStart;

				if (cfig.dhcpRanges[rangeInd].expiry[ind] != INT_MAX)
				{
					if (INT_MAX - t < expiry)
						cfig.dhcpRanges[rangeInd].expiry[ind] = INT_MAX;
					else
						cfig.dhcpRanges[rangeInd].expiry[ind] = t + expiry;
				}

				break;
			}
		}
	}
}

MYDWORD CDualServer::sendRepl(data9 *req)
{
	data3 op;

	if (cfig.replication == 1)
		req->remote.sin_addr.s_addr = cfig.zoneServers[1];
	else if (cfig.replication == 2)
		req->remote.sin_addr.s_addr = cfig.zoneServers[0];
	else
		return 0;

	req->remote.sin_port = htons(IPPORT_DHCPS);
	req->remote.sin_family = AF_INET;

	MYBYTE *opPointer = req->dhcpp.vend_data;

	while ((*opPointer) != DHCP_OPTION_END && opPointer < req->vp)
	{
		if ((*opPointer) == DHCP_OPTION_MESSAGETYPE)
		{
			*(opPointer + 2) = DHCP_MESS_INFORM;
			break;
		}
		opPointer = opPointer + *(opPointer + 1) + 2;
	}

	if (!req->opAdded[DHCP_OPTION_MESSAGETYPE])
	{
		op.opt_code = DHCP_OPTION_MESSAGETYPE;
		op.size = 1;
		op.value[0] = DHCP_MESS_INFORM;
		pvdata(req, &op);
	}

	if (req->hostname[0] && !req->opAdded[DHCP_OPTION_HOSTNAME])
	{
		op.opt_code = DHCP_OPTION_HOSTNAME;
		op.size = strlen(req->hostname);
		memcpy(op.value, req->hostname, op.size);
		pvdata(req, &op);
	}

	if (!req->opAdded[DHCP_OPTION_IPADDRLEASE])
	{
		op.opt_code = DHCP_OPTION_IPADDRLEASE;
		op.size = 4;
		pULong(op.value, req->lease);
		pvdata(req, &op);
	}

	op.opt_code = DHCP_OPTION_SERIAL;
	op.size = 4;
	pULong(op.value, cfig.serial1);
	pvdata(req, &op);

	//printf("Here1 %u\n",req->vp);

	*(req->vp) = DHCP_OPTION_END;
	req->bytes = req->vp - (MYBYTE*)req->raw;
	req->bytes++;

	//printf("Here2\n");

	req->dhcpp.header.bp_op = BOOTP_REQUEST;
	errno = 0;

	//printf("%i\n", req->bytes);

	req->bytes = sendto(network.dhcpReplConn.sock,
	                    req->raw,
	                    req->bytes,
	                    0,
	                    (sockaddr*)&req->remote,
	                    sizeof(req->remote));

	//printf("Here3\n");

	errno = WSAGetLastError();

	if (errno || req->bytes <= 0)
	{
		if (cfig.dhcpLogLevel)
		{
			if (cfig.replication == 1)
				sprintf(logBuff, "Error %u Sending DHCP Update to Secondary Server", errno);
			else
				sprintf(logBuff, "Error %u Sending DHCP Update to Primary Server", errno);

			logDHCPMess(logBuff, 2);
		}

		return 0;
	}
	else if (cfig.dhcpLogLevel == 2)
	{
		if (cfig.replication == 1)
			sprintf(logBuff, "DHCP Update for host %s (%s) sent to Secondary Server", hex2String(extbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen, ':'), IP2String(tempbuff, req->dhcpEntry->ip));
		else
			sprintf(logBuff, "DHCP Update for host %s (%s) sent to Primary Server", hex2String(extbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen, ':'), IP2String(tempbuff, req->dhcpEntry->ip));

		logDHCPMess(logBuff, 2);
	}

	return req->dhcpp.header.bp_yiaddr;
}

MYDWORD CDualServer::sendRepl(data7 *dhcpEntry)
{
	data9 req;
	memset(&req, 0, sizeof(data9));
	req.vp = req.dhcpp.vend_data;
	req.messsize = sizeof(dhcp_packet);
	req.dhcpEntry = dhcpEntry;

	req.dhcpp.header.bp_op = BOOTP_REQUEST;
	req.dhcpp.header.bp_xid = static_cast<unsigned int>(time(NULL));
	req.dhcpp.header.bp_ciaddr = dhcpEntry->ip;
	req.dhcpp.header.bp_yiaddr = dhcpEntry->ip;;
	req.dhcpp.header.bp_hlen = fromUUE(req.dhcpp.header.bp_chaddr, dhcpEntry->mapname);
	req.dhcpp.header.bp_magic_num[0] = 99;
	req.dhcpp.header.bp_magic_num[1] = 130;
	req.dhcpp.header.bp_magic_num[2] = 83;
	req.dhcpp.header.bp_magic_num[3] = 99;

	data7 *cache = findEntry(cacheInd, IP2String(tempbuff, htonl(dhcpEntry->ip)));

	if (cache)
		strcpy(req.hostname, cache->hostname);

	return sendRepl(&req);
}

void CDualServer::recvRepl(data9 *req)
{
	time_t t = time(NULL);

	//printf("Here 2\n");

	MYDWORD ip = req->dhcpp.header.bp_yiaddr ? req->dhcpp.header.bp_yiaddr : req->dhcpp.header.bp_ciaddr;

	if (!cfig.replication || !ip || !req->serial)
		return;
	else if (cfig.replication == 1 && req->remote.sin_addr.s_addr != cfig.zoneServers[1])
		return;
	else if (cfig.replication == 2 && req->remote.sin_addr.s_addr != cfig.zoneServers[0])
		return;

	//printf("Here 3\n");
	char rInd = getRangeInd(ip);

	req->dhcpEntry = dhcpCache[toUUE(tempbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen)];

	if (!req->dhcpEntry && rInd >= 0)
	{
		req->dhcpEntry = (data7*)calloc(1, sizeof(data7));

		if (!req->dhcpEntry)
		{
			sprintf(logBuff, "Memory Allocation Error");
			logDHCPMess(logBuff, 1);
			return;
		}

		req->dhcpEntry->mapname = cloneString(toUUE(tempbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen));

		if (!req->dhcpEntry->mapname)
		{
			sprintf(logBuff, "Memory Allocation Error");
			free(req->dhcpEntry);
			logDHCPMess(logBuff, 1);
			return;
		}

		//req->dhcpEntry->dataType = DHCP;
		dhcpCache[req->dhcpEntry->mapname] = req->dhcpEntry;
	}

	if (req->dhcpEntry)
	{
		req->dhcpEntry->ip = ip;
		req->dhcpEntry->rangeInd = rInd;
		//printf("Here 4\n");

		if (updateDHCP(req))
		{
			cfig.serial1 = req->serial;
			cfig.serial2 = req->serial;

			if (cfig.replication == 2)
			{
				if (cfig.expire > (MYDWORD)(INT_MAX - t))
					cfig.expireTime = INT_MAX;
				else
					cfig.expireTime = t + cfig.expire;
			}
			else
				cfig.expireTime = INT_MAX;
		}
	}
	else
	{
		cfig.serial1 = req->serial;
		cfig.serial2 = req->serial;

		if (cfig.replication == 2)
		{
			if (cfig.expire > (MYDWORD)(INT_MAX - t))
				cfig.expireTime = INT_MAX;
			else
				cfig.expireTime = t + cfig.expire;
		}
		else
			cfig.expireTime = INT_MAX;

		time_t expiry = 0;

		if (req->lease > (MYDWORD)(INT_MAX - t))
			expiry = INT_MAX;
		else
			expiry = t + req->lease;

		addToCache(cacheInd, req->hostname, ip, expiry, LOCAL_A, NONE, cfig.serial1);
		//printf("%u,%s,%u,%u,%u,%u,%u\n", cacheInd, req->hostname, ip, expiry, LOCAL_A, NONE, cfig.serial1);

		if (makeLocal(ip))
			addToCache(cacheInd, req->hostname, ip, expiry, NONE, LOCAL_PTR_AUTH, cfig.serial2);
		else
			addToCache(cacheInd, req->hostname, ip, expiry, NONE, LOCAL_PTR_NAUTH, cfig.serial2);
	}

	if (cfig.dhcpLogLevel == 2)
	{
		if (cfig.replication == 1)
			sprintf(logBuff, "DHCP Update received for %s (%s) from Secondary Server", hex2String(extbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen, ':'), IP2String(tempbuff, ip));
		else
			sprintf(logBuff, "DHCP Update received for %s (%s) from Primary Server", hex2String(extbuff, req->dhcpp.header.bp_chaddr, req->dhcpp.header.bp_hlen, ':'), IP2String(tempbuff, ip));

		logDHCPMess(logBuff, 2);
	}
}

char CDualServer::getRangeInd(MYDWORD ip)
{
	if (dhcpService && ip)
	{
		MYDWORD iip = htonl(ip);
		for (char k = 0; k < MAX_RANGES && cfig.dhcpRanges[k].rangeStart; k++)
			if (iip >= cfig.dhcpRanges[k].rangeStart && iip <= cfig.dhcpRanges[k].rangeEnd)
				return k;
	}
	return -1;
}

int CDualServer::getIndex(char rangeInd, MYDWORD ip)
{
	if (dhcpService && ip && rangeInd >= 0 && rangeInd < MAX_RANGES)
	{
		MYDWORD iip = htonl(ip);
		if (iip >= cfig.dhcpRanges[rangeInd].rangeStart && iip <= cfig.dhcpRanges[rangeInd].rangeEnd)
			return (iip - cfig.dhcpRanges[rangeInd].rangeStart);
	}
	return -1;
}

void CDualServer::parseDhcpOptions(char *sectionBody, const char *sectionName, data20 *optionData)
{
	if (sectionBody == NULL)
	{
		return;
	}

	std::stringstream ss(sectionBody);
	ss.seekg(0);

	optionData->options = 0;
	optionData->ip = 0;
	optionData->mask = 0;
	MYBYTE maxInd = sizeof(opData) / sizeof(data4);
	MYBYTE options[sizeof(dhcp_packet) - sizeof(dhcp_header)];
	MYWORD buffsize = sizeof(options);
	MYBYTE *dp = options;
	MYBYTE op_specified[256];

	memset(op_specified, 0, 256);
	*dp = 0;
	dp++;

	char raw[512];
	char name[512];
	char value[512];

	while (readSection(raw, &ss))
	{
		MYBYTE *ddp = dp;
		MYBYTE hoption[256];
		MYBYTE valSize = sizeof(hoption) - 1;
		MYBYTE opTag = 0;
		MYBYTE opType = 0;
		MYBYTE valType = 0;
		bool tagFound = false;

		mySplit(name, value, raw, '=');

		if (!name[0])
		{
			sprintf(logBuff, "Warning: section [%s] invalid option %s ignored", sectionName, raw);
			logDHCPMess(logBuff, 1);
			continue;
		}

		if (!strcasecmp(name, "DHCP_Range"))
		{
			if (!strcasecmp(sectionName, "DHCP-RANGE"))
				addDHCPRange(value);
			else
			{
				sprintf(logBuff, "Warning: section [%s] option %s not allowed in this section, option ignored", sectionName, raw);
				logDHCPMess(logBuff, 1);
			}
			continue;
		}
		else if (!strcasecmp(name, "IP_Addr"))
		{
			if (!strcasecmp(sectionName, "DHCP-OPTIONS") || !strcasecmp(sectionName, "DHCP-RANGE"))
			{
				sprintf(logBuff, "Warning: section [%s] option %s not allowed in this section, option ignored", sectionName, raw);
				logDHCPMess(logBuff, 1);
			}
			else if (!isIP(value))
			{
				sprintf(logBuff, "Warning: section [%s] option %s Invalid IP Addr %s option ignored", sectionName, value);
				logDHCPMess(logBuff, 1);
			}
			else
				optionData->ip = my_inet_addr(value);

			continue;
		}
		else if (!strcasecmp(name, "Filter_Mac_Range"))
		{
			if (!strcasecmp(sectionName, "DHCP-RANGE"))
				addMacRange(optionData->rangeSetInd, value);
			else
			{
				sprintf(logBuff, "Warning: section [%s] option %s not allowed in this section, option ignored", sectionName, raw);
				logDHCPMess(logBuff, 1);
			}
			continue;
		}

		if (isInt(name))
		{
			if (atoi(name) >= 254)
			{
				sprintf(logBuff, "Warning: section [%s] invalid option %s, ignored", sectionName, raw);
				logDHCPMess(logBuff, 1);
				continue;
			}

			opTag = atoi(name);
			opType = 2;
		}

		for (MYBYTE i = 0; i < maxInd; i++)
			if (!strcasecmp(name, opData[i].opName) || (opTag && opTag == opData[i].opTag))
			{
				opTag = opData[i].opTag;
				opType = opData[i].opType;
				tagFound = true;
				break;
			}

		if (!opTag)
		{
			sprintf(logBuff, "Warning: section [%s] invalid option %s, ignored", sectionName, raw);
			logDHCPMess(logBuff, 1);
			continue;
		}

		if (!value[0])
			valType = 9;
		else if (value[0] == '"' && value[strlen(value)-1] == '"')
		{
			valType = 1;
			value[0] = 32;
			value[strlen(value)-1] = 32;
			myTrim(value, value);

			if (strlen(value) <= UCHAR_MAX)
				valSize = strlen(value);
			else
			{
				sprintf(logBuff, "Warning: section [%s] option %s value too big, option ignored", sectionName, raw);
				logDHCPMess(logBuff, 1);
				continue;
			}
		}
		else if (strchr(value, ':'))
		{
			valType = 2;
			valSize = sizeof(hoption) - 1;
			char *errorPos = getHexValue(hoption, value, &valSize);

			if (errorPos)
			{
				sprintf(logBuff, "Warning: section [%s] option %s position %u, Invalid char %c, option ignored", sectionName, name, (errorPos - value) + 1, *(errorPos));
				logDHCPMess(logBuff, 1);
				continue;
			}
			else
				memcpy(value, hoption, valSize);
		}
		else if (isInt(value) && atol(value) > USHRT_MAX)
			valType = 4;
		else if (isInt(value) && atoi(value) > UCHAR_MAX)
			valType = 5;
		else if (isInt(value))
			valType = 6;
		else if ((strchr(value, '.') && (opType == 2 || opType == 3 || opType == 8)) || (!strchr(value, '.') && strchr(value, ',')))
		{
			valType = 2;
			myTokenize(tempbuff, value, "/,.", true);
			char *ptr = tempbuff;
			valSize = 0;

			for (; *ptr; ptr = myGetToken(ptr, 1))
			{
				//printf("%s:", ptr);
				if (valSize >= UCHAR_MAX)
				{
					sprintf(logBuff, "Warning: section [%s] option %s, too many values, entry ignored", sectionName, raw);
					logDHCPMess(logBuff, 1);
					break;
				}
				else if (isInt(ptr) && atoi(ptr) <= UCHAR_MAX)
				{
					hoption[valSize] = atoi(ptr);
					valSize++;
				}
				else
				{
					sprintf(logBuff, "Warning: section [%s] option %s invalid byte/octate '%s', entry ignored", sectionName, name, ptr);
					logDHCPMess(logBuff, 1);
					break;
				}
			}

			if (*ptr)
				continue;

			memcpy(value, hoption, valSize);
		}
		else
		{
			if (strlen(value) <= UCHAR_MAX)
				valSize = strlen(value);
			else
			{
				sprintf(logBuff, "Warning: section [%s] option %s value too long, option ignored", sectionName, raw);
				logDHCPMess(logBuff, 1);
				continue;
			}
			valType = 1;
		}

		//sprintf(logBuff, "Tag %i ValType %i opType %i value=%s size=%u", opTag, valType, opType, value, valSize);
		//logDHCPMess(logBuff, 1);

		if (!strcasecmp(name, "Filter_Vendor_Class"))
		{
			if (!strcasecmp(sectionName, "DHCP-RANGE"))
				addVendClass(optionData->rangeSetInd, value, valSize);
			else
			{
				sprintf(logBuff, "Warning: section [%s] option %s not allowed in this section, option ignored", sectionName, raw);
				logDHCPMess(logBuff, 1);
			}
			continue;
		}
		else if (!strcasecmp(name, "Filter_User_Class"))
		{
			if (!strcasecmp(sectionName, "DHCP-RANGE"))
				addUserClass(optionData->rangeSetInd, value, valSize);
			else
			{
				sprintf(logBuff, "Warning: section [%s] option %s not allowed in this section, option ignored", sectionName, raw);
				logDHCPMess(logBuff, 1);
			}
			continue;
		}

		if (op_specified[opTag])
		{
			sprintf(logBuff, "Warning: section [%s] duplicate option %s, ignored", sectionName, raw);
			logDHCPMess(logBuff, 1);
			continue;
		}

		//printf("Option=%u opType=%u valueType=%u valSize=%u\n", opTag, opType, valType, valSize);

		op_specified[opTag] = true;

		if (valType == 9)
		{
			if (buffsize > 2)
			{
				*dp = opTag;
				dp++;
				*dp = 0;
				dp++;
				buffsize -= 2;
			}
			else
			{
				sprintf(logBuff, "Warning: section [%s] option %s, no more space for options", sectionName, raw);
				logDHCPMess(logBuff, 1);
			}
			continue;
		}

		switch (opType)
		{
			case 1:
			{
				if (valType == 6)
				{
					valType = 2;
					valSize = 1;
					*value = atoi(value);
				}

				if (valType != 1 && valType != 2)
				{
					sprintf(logBuff, "Warning: section [%s] option %s, need string value, option ignored", sectionName, raw);
					logDHCPMess(logBuff, 1);
				}
				else if (opTag == DHCP_OPTION_DOMAINNAME)
				{
					sprintf(logBuff, "Warning: section [%s] option %u should be under [DOMAIN-NAME], ignored", sectionName, opTag);
					logDHCPMess(logBuff, 1);
					continue;
				}
				else if (buffsize > strlen(value) + 2)
				{
					*dp = opTag;
					dp++;
					*dp = strlen(value);
					dp++;
					memcpy(dp, value, valSize);
					dp += (strlen(value));
					buffsize -= (valSize + 2);
				}
				else
				{
					sprintf(logBuff, "Warning: section [%s] option %s, no more space for options", sectionName, raw);
					logDHCPMess(logBuff, 1);
				}
			}
			break;

			case 3:
			case 8:
			{
				if (valType == 2)
				{
					if (opType == 3 && valSize % 4)
					{
						sprintf(logBuff, "Warning: section [%s] option %s, missing bytes of IP, option ignored", sectionName, raw);
						logDHCPMess(logBuff, 1);
						continue;
					}
					else if (opType == 8 && valSize % 8)
					{
						sprintf(logBuff, "Warning: section [%s] option %s, some values not in IP/Mask form, option ignored", sectionName, raw);
						logDHCPMess(logBuff, 1);
						continue;
					}

					if (opTag == DHCP_OPTION_NETMASK)
					{
						if (valSize != 4 || !checkMask(*((MYDWORD*)value)))
						{
							sprintf(logBuff, "Warning: section [%s] Invalid subnetmask %s, option ignored", sectionName, raw);
							logDHCPMess(logBuff, 1);
							continue;
						}
						else
							optionData->mask = *((MYDWORD*)value);
					}

					if (buffsize > valSize + 2)
					{
						*dp = opTag;
						dp++;
						*dp = valSize;
						dp++;
						memcpy(dp, value, valSize);
						dp += valSize;
						buffsize -= (valSize + 2);
					}
					else
					{
						sprintf(logBuff, "Warning: section [%s] option %s, no more space for options", sectionName, raw);
						logDHCPMess(logBuff, 1);
					}
				}
				else
				{
					sprintf(logBuff, "Warning: section [%s] option %s, Invalid value, should be one or more IP/4 Bytes", sectionName, raw);
					logDHCPMess(logBuff, 1);
				}
			}
			break;

			case 4:
			{
				MYDWORD j;

				if (valType == 2 && valSize == 4)
					j = fULong(value);
				else if (valType >= 4 && valType <= 6)
					j = atol(value);
				else
				{
					sprintf(logBuff, "Warning: section [%s] option %s, value should be integer between 0 & %u or 4 bytes, option ignored", sectionName, name, UINT_MAX);
					logDHCPMess(logBuff, 1);
					continue;
				}

				if (opTag == DHCP_OPTION_IPADDRLEASE)
				{
					if (!strcasecmp(sectionName, "DHCP-OPTIONS"))
					{
						sprintf(logBuff, "Warning: section [%s] option %s not allowed in this section, please set it in [TIMINGS] section", sectionName, raw);
						logDHCPMess(logBuff, 1);
						continue;
					}

					if (j == 0)
						j = UINT_MAX;

					if (j > cfig.lease)
					{
						sprintf(logBuff, "Warning: section [%s] option %s value should be less then %u (max lease), ignored", sectionName, name, cfig.lease);
						logDHCPMess(logBuff, 1);
						continue;
					}
				}

				if (buffsize > 6)
				{
					*dp = opTag;
					dp++;
					*dp = 4;
					dp++;
					dp += pULong(dp, j);
					buffsize -= 6;
					//printf("%s=%u=%u\n",opData[op_index].opName,opData[op_index].opType,htonl(j));
				}
				else
				{
					sprintf(logBuff, "Warning: section [%s] option %s, no more space for options", sectionName, raw);
					logDHCPMess(logBuff, 1);
				}
			}
			break;

			case 5:
			{
				MYWORD j;

				if (valType == 2 && valSize == 2)
					j = fUShort(value);
				else if (valType == 5 || valType == 6)
					j = static_cast<unsigned char>(atol(value));
				else
				{
					sprintf(logBuff, "Warning: section [%s] option %s, value should be between 0 & %u or 2 bytes, option ignored", sectionName, name, USHRT_MAX);
					logDHCPMess(logBuff, 1);
					continue;
				}

				if (buffsize > 4)
				{
					*dp = opTag;
					dp++;
					*dp = 2;
					dp++;
					dp += pUShort(dp, j);
					buffsize -= 4;
				}
				else
				{
					sprintf(logBuff, "Warning: section [%s] option %s, no more space for options", sectionName, raw);
					logDHCPMess(logBuff, 1);
				}
			}
			break;

			case 6:
			{
				MYBYTE j;

				if (valType == 2 && valSize == 1)
					j = *value;
				else if (valType == 6)
					j = static_cast<unsigned char>(atol(value));
				else
				{
					sprintf(logBuff, "Warning: section [%s] option %s, value should be between 0 & %u or single byte, option ignored", sectionName, name, UCHAR_MAX);
					logDHCPMess(logBuff, 1);
					continue;
				}

				if (buffsize > 3)
				{
					*dp = opTag;
					dp++;
					*dp = 1;
					dp++;
					*dp = j;
					dp++;
					buffsize -= 3;
				}
				else
				{
					sprintf(logBuff, "Warning: section [%s] option %s, no more space for options", sectionName, raw);
					logDHCPMess(logBuff, 1);
				}
			}
			break;

			case 7:
			{
				MYBYTE j;

				if (valType == 2 && valSize == 1 && *value < 2)
					j = *value;
				else if (valType == 1 && !strcasecmp(value, "yes"))
					j = 1;
				else if (valType == 1 && !strcasecmp(value, "no"))
					j = 0;
				else
				{
					sprintf(logBuff, "Warning: section [%s] option %s, value should be yes or no, option ignored", sectionName, raw);
					logDHCPMess(logBuff, 1);
					continue;
				}

				if (buffsize > 3)
				{
					*dp = opTag;
					dp++;
					*dp = 1;
					dp++;
					*dp = j;
					dp++;
					buffsize -= 3;
				}
				else
				{
					sprintf(logBuff, "Warning: section [%s] option %s, no more space for options", sectionName, raw);
					logDHCPMess(logBuff, 1);
				}
			}
			break;

			default:
			{
				if (valType == 6)
				{
					valType = 2;
					valSize = 1;
					*value = atoi(value);
				}

				if (opType == 2 && valType != 2)
				{
					sprintf(logBuff, "Warning: section [%s] option %s, value should be comma separated bytes or hex string, option ignored", sectionName, raw);
					logDHCPMess(logBuff, 1);
					continue;
				}
				else if (buffsize > valSize + 2)
				{
					*dp = opTag;
					dp++;
					*dp = valSize;
					dp++;
					memcpy(dp, value, valSize);
					dp += valSize;
					buffsize -= (valSize + 2);
				}
				else
				{
					sprintf(logBuff, "Warning: section [%s] option %s, no more space for options", sectionName, raw);
					logDHCPMess(logBuff, 1);
				}
			}
			break;
		}

		//printf("%s Option=%u opType=%u valType=%u  valSize=%u\n", raw, opTag, opType, valType, valSize);
		//printf("%s %s\n", name, hex2String(tempbuff, ddp, valSize+2, ':'));
	}

	//printf("%s=%s\n", sectionName, optionData->vendClass);

	*dp = DHCP_OPTION_END;
	dp++;
	MYWORD optionSize = (dp - options);

	if (optionSize > 2)
	{
		optionData->options = (MYBYTE*)calloc(1, optionSize);

		if (!optionData->options)
		{
			sprintf(logBuff, "DHCP Option Load, Memory Allocation Error");
			logDHCPMess(logBuff, 1);
		}

		memcpy(optionData->options, options, optionSize);
	}
}

void CDualServer::lockOptions(std::stringstream *pss)
{
	char raw[512];
	char name[512];
	char value[512];

	while (readSection(raw, pss))
	{
		mySplit(name, value, raw, '=');

		if (!name[0] || !value[0])
			continue;

		int op_index;
		MYBYTE n = sizeof(opData) / sizeof(data4);

		for (op_index = 0; op_index < n; op_index++)
			if (!strcasecmp(name, opData[op_index].opName) || (opData[op_index].opTag && atoi(name) == opData[op_index].opTag))
				break;

		if (op_index >= n)
			continue;

		if (opData[op_index].opType == 3)
		{
			if (myTokenize(value, value, "/,.", true))
			{
				char *ptr = value;
				char hoption[256];
				MYBYTE valueSize = 0;

				for (; *ptr; ptr = myGetToken(ptr, 1))
				{
					if (valueSize >= UCHAR_MAX)
						break;
					else if (isInt(ptr) && atoi(ptr) <= UCHAR_MAX)
					{
						hoption[valueSize] = atoi(ptr);
						valueSize++;
					}
					else
						break;
				}

				if (*ptr)
					continue;

				if (valueSize % 4)
					continue;

				for (MYBYTE i = 0; i < valueSize; i += 4)
				{
					MYDWORD inetAddr = *((MYDWORD*)&(hoption[i]));

					if (inetAddr != INADDR_ANY && inetAddr != INADDR_NONE)
						setLeaseExpiry(inetAddr, INT_MAX);
				}
			}
		}
	}
}

void CDualServer::addDHCPRange(char *dp)
{
	MYDWORD rs = 0;
	MYDWORD re = 0;
	char name[512];
	char value[512];
	mySplit(name, value, dp, '-');

	if (name[0] && value[0] && isIP(name) && isIP(value))
	{
		rs = htonl(my_inet_addr(name));
		re = htonl(my_inet_addr(value));

		if (rs && rs != INADDR_NONE && re && re != INADDR_NONE && rs <= re)
		{
			if (cfig.authorized && (rs < cfig.rangeStart || re > cfig.rangeEnd))
			{
				sprintf(logBuff, "Section [DHCP-RANGE], range %s-%s is out of zone %s, ignored", name, value, cfig.authority);
				logDHCPMess(logBuff, 1);
				return;
			}

			data13 *range;
			MYBYTE m = 0;

			for (; m < MAX_RANGES && cfig.dhcpRanges[m].rangeStart; m++)
			{
				range = &cfig.dhcpRanges[m];
				if ((rs >= range->rangeStart && rs <= range->rangeEnd)
						|| (re >= range->rangeStart && re <= range->rangeEnd)
						|| (range->rangeStart >= rs && range->rangeStart <= re)
						|| (range->rangeEnd >= rs && range->rangeEnd <= re))
				{
					sprintf(logBuff, "Warning: DHCP Range %s-%s overlaps with another range, ignored", name, value);
					logDHCPMess(logBuff, 1);
					return;
				}
			}

			if (m < MAX_RANGES)
			{
				cfig.dhcpSize += (re - rs + 1);
				range = &cfig.dhcpRanges[m];
				range->rangeStart = rs;
				range->rangeEnd = re;
				range->expiry = (time_t*)calloc((re - rs + 1), sizeof(time_t));
				range->dhcpEntry = (data7**)calloc((re - rs + 1), sizeof(data7*));

				if (!range->expiry || !range->dhcpEntry)
				{
					if (range->expiry)
						free(range->expiry);

					if (range->dhcpEntry)
						free(range->dhcpEntry);

					sprintf(logBuff, "DHCP Ranges Load, Memory Allocation Error");
					logDHCPMess(logBuff, 1);
					return ;
				}
			}
		}
		else
		{
			sprintf(logBuff, "Section [DHCP-RANGE] Invalid DHCP range %s in ini file, ignored", name);
			logDHCPMess(logBuff, 1);
		}
	}
	else
	{
		sprintf(logBuff, "Section [DHCP-RANGE] Invalid DHCP range %s in ini file, ignored", name);
		logDHCPMess(logBuff, 1);
	}
}

void CDualServer::addVendClass(MYBYTE rangeSetInd, char *vendClass, MYBYTE vendClassSize)
{
	data14 *rangeSet = &cfig.rangeSet[rangeSetInd];

	MYBYTE i = 0;

	for (; i <= 32 && rangeSet->vendClassSize[i]; i++);

	if (i >= 32 || !vendClassSize)
		return;

	rangeSet->vendClass[i] = (MYBYTE*)calloc(vendClassSize, 1);

	if(!rangeSet->vendClass[i])
	{
		sprintf(logBuff, "Vendor Class Load, Memory Allocation Error");
		logDHCPMess(logBuff, 1);
	}
	else
	{
		cfig.hasFilter = true;
		rangeSet->vendClassSize[i] = vendClassSize;
		memcpy(rangeSet->vendClass[i], vendClass, vendClassSize);
		//printf("Loaded Vendor Class %s Size=%i rangeSetInd=%i Ind=%i\n", rangeSet->vendClass[i], rangeSet->vendClassSize[i], rangeSetInd, i);
		//printf("Loaded Vendor Class %s Size=%i rangeSetInd=%i Ind=%i\n", hex2String(tempbuff, rangeSet->vendClass[i], rangeSet->vendClassSize[i], ':'), rangeSet->vendClassSize[i], rangeSetInd, i);
	}
}

void CDualServer::addUserClass(MYBYTE rangeSetInd, char *userClass, MYBYTE userClassSize)
{
	data14 *rangeSet = &cfig.rangeSet[rangeSetInd];

	MYBYTE i = 0;

	for (; i <= 32 && rangeSet->userClassSize[i]; i++);

	if (i >= 32 || !userClassSize)
		return;

	rangeSet->userClass[i] = (MYBYTE*)calloc(userClassSize, 1);

	if(!rangeSet->userClass[i])
	{
		sprintf(logBuff, "Vendor Class Load, Memory Allocation Error");
		logDHCPMess(logBuff, 1);
	}
	else
	{
		cfig.hasFilter = true;
		rangeSet->userClassSize[i] = userClassSize;
		memcpy(rangeSet->userClass[i], userClass, userClassSize);
		//printf("Loaded User Class %s Size=%i rangeSetInd=%i Ind=%i\n", hex2String(tempbuff, rangeSet->userClass[i], rangeSet->userClassSize[i], ':'), rangeSet->vendClassSize[i], rangeSetInd, i);
	}
}

void CDualServer::addMacRange(MYBYTE rangeSetInd, char *macRange)
{
	if (macRange[0])
	{
		data14 *rangeSet = &cfig.rangeSet[rangeSetInd];

		MYBYTE i = 0;

		for (; i <= 32 && rangeSet->macSize[i]; i++);

		if (i >= 32)
			return;

		char name[256];
		char value[256];

		mySplit(name, value, macRange, '-');

		if(!name[0] || !value[0])
		{
			sprintf(logBuff, "Section [RANGE_SET], invalid Filter_Mac_Range %s, ignored", macRange);
			logDHCPMess(logBuff, 1);
		}
		else
		{
			MYBYTE macSize1 = 16;
			MYBYTE macSize2 = 16;
			MYBYTE *macStart = (MYBYTE*)calloc(1, macSize1);
			MYBYTE *macEnd = (MYBYTE*)calloc(1, macSize2);

			if(!macStart || !macEnd)
			{
				sprintf(logBuff, "DHCP Range Load, Memory Allocation Error");
				logDHCPMess(logBuff, 1);
			}
			else if (getHexValue(macStart, name, &macSize1) || getHexValue(macEnd, value, &macSize2))
			{
				sprintf(logBuff, "Section [RANGE_SET], Invalid character in Filter_Mac_Range %s", macRange);
				logDHCPMess(logBuff, 1);
				free(macStart);
				free(macEnd);
			}
			else if (memcmp(macStart, macEnd, 16) > 0)
			{
				sprintf(logBuff, "Section [RANGE_SET], Invalid Filter_Mac_Range %s, (higher bound specified on left), ignored", macRange);
				logDHCPMess(logBuff, 1);
				free(macStart);
				free(macEnd);
			}
			else if (macSize1 != macSize2)
			{
				sprintf(logBuff, "Section [RANGE_SET], Invalid Filter_Mac_Range %s, (start/end size mismatched), ignored", macRange);
				logDHCPMess(logBuff, 1);
				free(macStart);
				free(macEnd);
			}
			else
			{
				cfig.hasFilter = true;
				rangeSet->macSize[i] = macSize1;
				rangeSet->macStart[i] = macStart;
				rangeSet->macEnd[i] = macEnd;
				//printf("Mac Loaded, Size=%i Start=%s rangeSetInd=%i Ind=%i\n", rangeSet->macSize[i], hex2String(tempbuff, rangeSet->macStart[i], rangeSet->macSize[i], ':'), rangeSetInd, i);
			}
		}
	}
}

bool CDualServer::getSection(const char *sectionName, char *buffer, MYBYTE serial, char *fileName)
{
	//printf("%s=%s\n",fileName,sectionName);
	char section[128];
	sprintf(section, "[%s]", sectionName);
	myUpper(section);
	FILE *f = fopen(fileName, "rt");
	char buff[512];
	BYTE found = 0;

	if (f)
	{
		while (fgets(buff, 511, f))
		{
			myUpper(buff);
			myTrim(buff, buff);

			if (strstr(buff, section) == buff)
			{
				found++;
				if (found == serial)
				{
					//printf("%s=%s\n",fileName,sectionName);
					while (fgets(buff, 511, f))
					{
						myTrim(buff, buff);

						if (strstr(buff, "[") == buff)
							break;

						if ((*buff) >= '0' && (*buff) <= '9' || (*buff) >= 'A' && (*buff) <= 'Z' || (*buff) >= 'a' && (*buff) <= 'z' || ((*buff) && strchr("/\\?*", (*buff))))
						{
							buffer += sprintf(buffer, "%s", buff);
							buffer++;
						}
					}
					break;
				}
			}
		}
		fclose(f);
	}

	*buffer = 0;
	*(buffer + 1) = 0;
	return (found == serial);
}
/*
FILE* CDualServer::openSection(const char *sectionName, MYBYTE serial, char *fileName)
{
	return NULL;
}
*/

BOOL CDualServer::openSection(const char *sectionName, MYBYTE serial, std::stringstream *pss)
{
	//printf("%s=%s\n",fileName,sectionName);
	BOOL bRetVal = FALSE;
	char section[128];
	sprintf(section, "[%s]", sectionName);
	myUpper(section);
//	FILE *f = fopen(fileName, "rt");
	char buff[512];
	MYBYTE found = 0;

	pss->seekg(0);

//	if (f)
	if (TRUE)
	{
//		while (fgets(buff, 511, f))
		while (pss->getline(buff, 511))
		{
			myUpper(buff);
			myTrim(buff, buff);

			if (strstr(buff, section) == buff)
			{
				found++;

				if (found == serial)
					return TRUE;
			}
		}
//		fclose(f);
	}
	return bRetVal;
}

char* CDualServer::readSection(char* buff, std::stringstream *pss)
{
	while(pss->getline(buff, 511))
//	while (fgets(buff, 511, f))
	{
		myTrim(buff, buff);

		if (*buff == '[')
			break;

		if ((*buff) >= '0' && (*buff) <= '9' || (*buff) >= 'A' && (*buff) <= 'Z' || (*buff) >= 'a' && (*buff) <= 'z' || ((*buff) && strchr("/\\?*", (*buff))))
			return buff;
	}

//	fclose(f);
	return NULL;
}

char* CDualServer::myGetToken(char* buff, MYBYTE index)
{
	while (*buff)
	{
		if (index)
			index--;
		else
			break;

		buff += strlen(buff) + 1;
	}

	return buff;
}

MYWORD CDualServer::myTokenize(char *target, char *source, const char *sep, bool whiteSep)
{
	bool found = true;
	char *dp = target;
	MYWORD kount = 0;

	while (*source)
	{
		if (sep && sep[0] && strchr(sep, (*source)))
		{
			found = true;
			source++;
			continue;
		}
		else if (whiteSep && (*source) <= 32)
		{
			found = true;
			source++;
			continue;
		}

		if (found)
		{
			if (target != dp)
			{
				*dp = 0;
				dp++;
			}
			kount++;
		}

		found = false;
		*dp = *source;
		dp++;
		source++;
	}

	*dp = 0;
	dp++;
	*dp = 0;

	//printf("%s\n", target);

	return kount;
}

char* CDualServer::myTrim(char *target, char *source)
{
	while ((*source) && (*source) <= 32)
		source++;

	int i = 0;

	for (; i < 511 && source[i]; i++)
		target[i] = source[i];

	target[i] = source[i];
	i--;

	for (; i >= 0 && target[i] <= 32; i--)
		target[i] = 0;

	return target;
}

void CDualServer::mySplit(char *name, char *value, char *source, char splitChar)
{
	int i = 0;
	int j = 0;
	int k = 0;

	for (; source[i] && j <= 510 && source[i] != splitChar; i++, j++)
	{
		name[j] = source[i];
	}

	if (source[i])
	{
		i++;
		for (; k <= 510 && source[i]; i++, k++)
		{
			value[k] = source[i];
		}
	}

	name[j] = 0;
	value[k] = 0;

	myTrim(name, name);
	myTrim(value, value);
	//printf("%s %s\n", name, value);
}


char* CDualServer::strqtype(char *buff, MYBYTE qtype)
{
	switch (qtype)
	{
		case 1:
			strcpy(buff, "A");
			break;
		case 2:
			strcpy(buff, "NS");
			break;
		case 3:
			strcpy(buff, "MD");
			break;
		case 4:
			strcpy(buff, "MF");
			break;
		case 5:
			strcpy(buff, "CNAME");
			break;
		case 6:
			strcpy(buff, "SOA");
			break;
		case 7:
			strcpy(buff, "MB");
			break;
		case 8:
			strcpy(buff, "MG");
			break;
		case 9:
			strcpy(buff, "MR");
			break;
		case 10:
			strcpy(buff, "NULL");
			break;
		case 11:
			strcpy(buff, "WKS");
			break;
		case 12:
			strcpy(buff, "PTR");
			break;
		case 13:
			strcpy(buff, "HINFO");
			break;
		case 14:
			strcpy(buff, "MINFO");
			break;
		case 15:
			strcpy(buff, "MX");
			break;
		case 16:
			strcpy(buff, "TXT");
			break;
		case 28:
			strcpy(buff, "AAAA");
			break;
		case 251:
			strcpy(buff, "IXFR");
			break;
		case 252:
			strcpy(buff, "AXFR");
			break;
		case 253:
			strcpy(buff, "MAILB");
			break;
		case 254:
			strcpy(buff, "MAILA");
			break;
		default:
			strcpy(buff, "ANY");
			break;
	}
	return buff;
}

MYDWORD CDualServer::getClassNetwork(MYDWORD ip)
{
	data15 data;
	data.ip = ip;
	data.octate[3] = 0;

	if (data.octate[0] < 192)
		data.octate[2] = 0;

	if (data.octate[0] < 128)
		data.octate[1] = 0;

	return data.ip;
}

/*
char *IP2Auth(MYDWORD ip)
{
data15 data;
data.ip = ip;

if (data.octate[0] >= 192)
sprintf(tempbuff, "%u.%u.%u", data.octate[2], data.octate[1], data.octate[0]);
else if (data.octate[0] >= 128)
sprintf(tempbuff, "%u.%u", data.octate[1], data.octate[0]);
else
sprintf(tempbuff, "%u", data.octate[0]);

strcat(tempbuff, arpa);
return tempbuff;
}
*/

char* CDualServer::IP2String(char *target, MYDWORD ip)
{
	data15 inaddr;
	inaddr.ip = ip;
	sprintf(target, "%u.%u.%u.%u", inaddr.octate[0], inaddr.octate[1], inaddr.octate[2], inaddr.octate[3]);
	//MYBYTE *octate = (MYBYTE*)&ip;
	//sprintf(target, "%u.%u.%u.%u", octate[0], octate[1], octate[2], octate[3]);
	return target;
}

bool CDualServer::addServer(MYDWORD *array, MYDWORD ip)
{
	for (MYBYTE i = 0; i < MAX_SERVERS; i++)
	{
		if (!ip || array[i] == ip)
			return 0;
		else if (!array[i])
		{
			array[i] = ip;
			return 1;
		}
	}
	return 0;
}

MYDWORD* CDualServer::findServer(MYDWORD *array, MYDWORD ip)
{
	if (ip)
	{
		for (MYBYTE i = 0; i < MAX_SERVERS && array[i]; i++)
		{
			if (array[i] == ip)
				return &(array[i]);
		}
	}
	return 0;
}

bool CDualServer::isInt(char *string)
{
	if (!(*string))
		return false;

	for(; *string; string++)
		if (*string <  '0' || *string > '9')
			return false;

	return true;
}

bool CDualServer::isIP(char *string)
{
	int j = 0;

	for (; *string; string++)
	{
		if (*string == '.' && *(string + 1) != '.')
			j++;
		else if (*string < '0' || *string > '9')
			return 0;
	}

	if (j == 3)
		return 1;
	else
		return 0;
}

char* CDualServer::toBase64(MYBYTE *source, MYBYTE length)
{
	MYBYTE a = 0, b = 0, i = 0;
	char *dp = tempbuff;

	for (; length; length--, source++)
	{
		i += 2;
		a = (*source) >> i;
		*dp = base64[a + b];
		dp++;
		b = (*source) << (8 - i);
		b >>= 2;
		if (i == 6)
		{
			*dp = base64[b];
			dp++;
			i = b = 0;
		}
	}
	if (i)
	{
		*dp = base64[b];
		dp++;
	}
	*dp = 0;
	//printf("%s\n",tempbuff);
	return tempbuff;
}

MYBYTE CDualServer::getBaseValue(MYBYTE a)
{
	if (a >= 'A' && a <= 'Z')
		a -= 'A';
	else if (a >= 'a' && a <= 'z')
		a = a - 'a' + 26;
	else if (a >= '0' && a <= '9')
		a = a - '0' + 52;
	else if (a == '+')
		a = 62;
	else if (a == '/')
		a = 63;
	else
		a = UCHAR_MAX;

	return a;
}

MYBYTE CDualServer::fromBase64(MYBYTE *target, char *source)
{
	//printf("SOURCE=%s\n", source);
	MYBYTE b = 0;
	MYBYTE shift = 4;
	MYBYTE bp_hlen = (3 * strlen(source))/4;
	*target = 0;

	if (*source)
	{
		b = getBaseValue(*source);
		*target = b << 2;
		source++;

		while (*source)
		{
			b = getBaseValue(*source);
			(*target) += (b >> (8 - shift));
			target++;
			(*target) = (b << shift);
			shift += 2;

			if (shift > 8)
			{
				source++;

				if (*source)
				{
					b = getBaseValue(*source);
					*target = b << 2;
					shift = 4;
				}
				else
					break;
			}

			source++;
		}
	}
	//printf("SIZE=%u\n", bp_hlen);
	return bp_hlen;
}

char* CDualServer::toUUE(char *tempbuff, MYBYTE *source, MYBYTE length)
{
	MYBYTE a = 0, b = 0, i = 0;
	char *dp = tempbuff;

	for (; length; length--, source++)
	{
		i += 2;
		a = (*source) >> i;
		*dp = a + b + 32;
		dp++;
		b = (*source) << (8 - i);
		b >>= 2;
		if (i == 6)
		{
			*dp = b + 32;
			dp++;
			i = b = 0;
		}
	}
	if (i)
	{
		*dp = b + 32;
		dp++;
	}
	*dp = 0;
	//printf("%s\n",tempbuff);
	return tempbuff;
}

MYBYTE CDualServer::fromUUE(MYBYTE *target, char *source)
{
	//printf("SOURCE=%s\n", source);
	MYBYTE b = 0;
	MYBYTE shift = 4;
	MYBYTE bp_hlen = (3 * strlen(source))/4;
	*target = 0;

	if (*source)
	{
		b = *source - 32;
		*target = b << 2;
		source++;

		while (*source)
		{
			b = *source - 32;
			(*target) += (b >> (8 - shift));
			target++;
			(*target) = (b << shift);
			shift += 2;

			if (shift > 8)
			{
				source++;

				if (*source)
				{
					b = *source - 32;
					*target = b << 2;
					shift = 4;
				}
				else
					break;
			}

			source++;
		}
	}
	//printf("SIZE=%u\n", bp_hlen);
	return bp_hlen;
}

char* CDualServer::hex2String(char *target, MYBYTE *hex, MYBYTE bytes, char sepChar)
{
	char *dp = target;

	if (bytes)
		dp += sprintf(target, "%02x", *hex);
	else
		*target = 0;

	for (MYBYTE i = 1; i < bytes; i++)
		if (sepChar)
			dp += sprintf(dp, "%c%02x", sepChar, *(hex + i));
		else
			dp += sprintf(dp, "%02x", *(hex + i));

	return target;
}

/*
char *IP62String(char *target, MYBYTE *source)
{
	MYWORD *dw = (MYWORD*)source;
	char *dp = target;
	MYBYTE markbyte;

	for (markbyte = 4; markbyte > 0 && !dw[markbyte - 1]; markbyte--);

	for (MYBYTE i = 0; i < markbyte; i++)
		dp += sprintf(dp, "%x:", ntohs(dw[i]));

	for (markbyte = 4; markbyte < 8 && !dw[markbyte]; markbyte++);

	for (MYBYTE i = markbyte; i < 8; i++)
		dp += sprintf(dp, ":%x", htons(dw[i]));

	return target;
}
*/

char* CDualServer::IP62String(char *target, MYBYTE *source)
{
	char *dp = target;
	bool zerostarted = false;
	bool zeroended = false;

	for (MYBYTE i = 0; i < 16; i += 2, source += 2)
	{
		if (source[0])
		{
			if (zerostarted)
				zeroended = true;

			if (zerostarted && zeroended)
			{
				dp += sprintf(dp, "::");
				zerostarted = false;
			}
			else if (dp != target)
				dp += sprintf(dp, ":");

			dp += sprintf(dp, "%x", source[0]);
			dp += sprintf(dp, "%02x", source[1]);
		}
		else if (source[1])
		{
			if (zerostarted)
				zeroended = true;

			if (zerostarted && zeroended)
			{
				dp += sprintf(dp, "::");
				zerostarted = false;
			}
			else if (dp != target)
				dp += sprintf(dp, ":");

			dp += sprintf(dp, "%0x", source[1]);
		}
		else if (!zeroended)
			zerostarted = true;
	}

	return target;
}

char* CDualServer::getHexValue(MYBYTE *target, char *source, MYBYTE *size)
{
	if (*size)
		memset(target, 0, (*size));

	if (*source == ':')
		source++;

	for ((*size) = 0; (*source) && (*size) < UCHAR_MAX; (*size)++, target++)
	{
		if ((*source) >= '0' && (*source) <= '9')
		{
			(*target) = (*source) - '0';
		}
		else if ((*source) >= 'a' && (*source) <= 'f')
		{
			(*target) = (*source) - 'a' + 10;
		}
		else if ((*source) >= 'A' && (*source) <= 'F')
		{
			(*target) = (*source) - 'A' + 10;
		}
		else
		{
			return source;
		}

		source++;

		if ((*source) >= '0' && (*source) <= '9')
		{
			(*target) *= 16;
			(*target) += ((*source) - '0');
		}
		else if ((*source) >= 'a' && (*source) <= 'f')
		{
			(*target) *= 16;
			(*target) += (((*source) - 'a') + 10);
		}
		else if ((*source) >= 'A' && (*source) <= 'F')
		{
			(*target) *= 16;
			(*target) += (((*source) - 'A') + 10);
		}
		else if ((*source) == ':')
		{
			source++;
			continue;
		}
		else if (*source)
			return source;
		else
			continue;

		source++;

		if ((*source) == ':')
			source++;
		else if (*source)
			return source;
	}

	if (*source)
		return source;

	return NULL;
}

char* CDualServer::myUpper(char *string)
{
	char diff = 'a' - 'A';
	MYWORD len = strlen(string);
	for (int i = 0; i < len; i++)
		if (string[i] >= 'a' && string[i] <= 'z')
			string[i] -= diff;
	return string;
}

char* CDualServer::myLower(char *string)
{
	char diff = 'a' - 'A';
	MYWORD len = strlen(string);
	for (int i = 0; i < len; i++)
		if (string[i] >= 'A' && string[i] <= 'Z')
			string[i] += diff;
	return string;
}

bool CDualServer::wildcmp(char *string, char *wild)
{
	// Written by Jack Handy - jakkhandy@hotmail.com
	// slightly modified
	char *cp = NULL;
	char *mp = NULL;

	while ((*string) && (*wild != '*'))
	{
		if ((*wild != *string) && (*wild != '?'))
		{
			return 0;
		}
		wild++;
		string++;
	}

	while (*string)
	{
		if (*wild == '*')
		{
			if (!*++wild)
				return 1;

			mp = wild;
			cp = string + 1;
		}
		else if ((*wild == *string) || (*wild == '?'))
		{
			wild++;
			string++;
		}
		else
		{
			wild = mp;
			string = cp++;
		}
	}

	while (*wild == '*')
		wild++;

	return !(*wild);
}

MYBYTE CDualServer::makeLocal(MYDWORD ip)
{
	if (cfig.rangeStart && htonl(ip) >= cfig.rangeStart && htonl(ip) <= cfig.rangeEnd)
		return 1;
	else if (getRangeInd(ip) >= 0)
		return 1;
	else
		return 0;
}

MYBYTE CDualServer::makeLocal(char *query)
{
	if (!strcasecmp(query, cfig.zone))
		return DNTYPE_A_ZONE;
	if (!strcasecmp(query, cfig.authority))
		return DNTYPE_P_ZONE;
	else
	{
		char *dp = strchr(query, '.');

		if (dp)
		{
			if (!strcasecmp(dp + 1, cfig.zone))
			{
				*dp = 0;
				return DNTYPE_A_LOCAL;
			}
			else
			{
				dp = strstr(query, arpa);

				if (dp)
				{
					if (strstr(query, cfig.authority))
					{
						*dp = 0;
						return DNTYPE_P_LOCAL;
					}
					else
					{
						*dp = 0;
						return DNTYPE_P_EXT;
					}
				}
			}
		}
		else
			return DNTYPE_A_BARE;
	}
	return DNTYPE_A_EXT;
}

void CDualServer::checkSize(MYBYTE ind)
{
	//printf("Start %u=%u\n",dnsCache[ind].size(),dnsAge[ind].size());
	time_t t = time(NULL);
	data7 *cache = NULL;
	//MYBYTE maxDelete = 3;
	expiryMap::iterator p = dnsAge[ind].begin();
	expiryMap::iterator q;

	//while (p != dnsAge[ind].end() && p->first < t && maxDelete > 0)
	while (p != dnsAge[ind].end() && p->first < t)
	{
		cache = p->second;
		//printf("processing %s=%i\n", cache->mapname, p->first - t);

		if (cache->expiry < t)
		{
			q = p;
			p++;
			dnsAge[ind].erase(q);

			if (cache->dataType == QUEUE && cache->expiry)
			{
				if (cache->dnsIndex < MAX_SERVERS)
				{
					if (network.currentDNS == cache->dnsIndex)
					{
						if (network.dns[1])
						{
							network.currentDNS++;

							if (network.currentDNS >= MAX_SERVERS || !network.dns[network.currentDNS])
								network.currentDNS = 0;
						}
					}
				}
				else if (cache->dnsIndex >= 128 && cache->dnsIndex < 192)
				{
					data6 *dnsRoute = &cfig.dnsRoutes[(cache->dnsIndex - 128) / 2];
					MYBYTE currentDNS = cache->dnsIndex % 2;

					if (dnsRoute->currentDNS == currentDNS && dnsRoute->dns[1])
						dnsRoute->currentDNS = 1 - dnsRoute->currentDNS;
				}
			}

			//sprintf(logBuff, "t=%u expiry=%u DataType=%u Cache Size=%u, Age Size=%u, Entry %s being deleted", t, cache->expiry, cache->dataType, dnsCache[ind].size(), dnsAge[ind].size(), cache->mapname);
			//logMess(logBuff, 1);
			delDnsEntry(ind, cache);
			//maxDelete--;
		}
		else if (cache->expiry > p->first)
		{
			q = p;
			p++;
			dnsAge[ind].erase(q);
			dnsAge[ind].insert(pair<time_t, data7*>(cache->expiry, cache));
		}
		else
			p++;
	}

	if (ind == cacheInd && dhcpService)
	{
		//printf("dhcpAge=%u\n", dhcpAge.size());

		p = dhcpAge.begin();

		while (p != dhcpAge.end() && p->first < t)
		{
			cache = p->second;
			//printf("processing %s=%i\n", cache->mapname, p->first - t);

			if (cache->local && cache->expiry < t)
			{
				q = p;
				p++;
				cache->local = 0;
				dhcpAge.erase(q);
				cfig.serial1 = static_cast<unsigned int>(time(NULL));
				cfig.serial2 = static_cast<unsigned int>(time(NULL));

				if (cfig.expire > (MYDWORD)(INT_MAX - t))
					cfig.expireTime = INT_MAX;
				else
					cfig.expireTime = t + cfig.expire;

				sendRepl(cache);
				//printf("Lease released\n");
			}
			else if (cache->expiry > p->first)
			{
				q = p;
				p++;
				dhcpAge.erase(q);
				dhcpAge.insert(pair<time_t, data7*>(cache->expiry, cache));
			}
			else
				p++;
		}
	}
	//printf("Done %u=%u\n",dnsCache[ind].size(),dnsAge[ind].size());
}

void CDualServer::calcRangeLimits(MYDWORD ip, MYDWORD mask, MYDWORD *rangeStart, MYDWORD *rangeEnd)
{
	*rangeStart = htonl(ip & mask) + 1;
	*rangeEnd = htonl(ip | (~mask)) - 1;
}

bool CDualServer::checkMask(MYDWORD mask)
{
	mask = htonl(mask);
	while (mask)
	{
		if (mask < (mask << 1))
			return false;

		mask <<= 1;
	}
	return true;
}

MYDWORD CDualServer::calcMask(MYDWORD rangeStart, MYDWORD rangeEnd)
{
	data15 ip1, ip2, mask;

	ip1.ip = htonl(rangeStart);
	ip2.ip = htonl(rangeEnd);

	for (MYBYTE i = 0; i < 4; i++)
	{
		mask.octate[i] = ip1.octate[i] ^ ip2.octate[i];

		if (i && mask.octate[i - 1] < 255)
			mask.octate[i] = 0;
		else if (mask.octate[i] == 0)
			mask.octate[i] = 255;
		else if (mask.octate[i] < 2)
			mask.octate[i] = 254;
		else if (mask.octate[i] < 4)
			mask.octate[i] = 252;
		else if (mask.octate[i] < 8)
			mask.octate[i] = 248;
		else if (mask.octate[i] < 16)
			mask.octate[i] = 240;
		else if (mask.octate[i] < 32)
			mask.octate[i] = 224;
		else if (mask.octate[i] < 64)
			mask.octate[i] = 192;
		else if (mask.octate[i] < 128)
			mask.octate[i] = 128;
		else
			mask.octate[i] = 0;
	}
	return mask.ip;
}

data7* CDualServer::findEntry(MYBYTE ind, char *key, MYBYTE entryType)
{
	myLower(key);
	hostMap::iterator it = dnsCache[ind].find(key);

	while (it != dnsCache[ind].end() && !strcasecmp(it->second->mapname, key))
		if (it->second->dataType == entryType)
			return it->second;
		else
			it++;

	return NULL;
}

data7* CDualServer::findEntry(MYBYTE ind, char *key)
{
	//printf("finding %u=%s\n",ind,key);
	myLower(key);
	hostMap::iterator it = dnsCache[ind].find(key);

	if (it != dnsCache[ind].end())
		return it->second;
	else
		return NULL;
}

void CDualServer::addEntry(MYBYTE ind, data7 *entry)
{
	myLower(entry->mapname);
	dnsCache[ind].insert(pair<string, data7*>(entry->mapname, entry));

	if (entry->expiry && entry->expiry < INT_MAX)
		dnsAge[ind].insert(pair<time_t, data7*>(entry->expiry, entry));
}

void CDualServer::delDnsEntry(MYBYTE ind, data7* cache)
{
	if (cache)
	{
		//printf("DataType=%u Size=%u, Entry %s being deleted\n", cache->dataType, dnsCache[ind].size(), cache->mapname);

		if (ind <= 1)
		{
			hostMap::iterator r = dnsCache[ind].find(cache->mapname);

			for (; r != dnsCache[ind].end(); r++)
				if (strcasecmp(r->second->mapname, cache->mapname))
					break;
				else if (r->second == cache)
				{
					dnsCache[ind].erase(r);
					break;
				}

			switch (cache->dataType)
			{
				case QUEUE:

					if (cache->addr)
						free(cache->addr);

					if (cache->query)
						free(cache->query);

					if (cache->mapname)
						free(cache->mapname);

					break;

				case LOCAL_PTR_AUTH:
				case LOCAL_PTR_NAUTH:
				case LOCALHOST_PTR:
				case SERVER_PTR_AUTH:
				case SERVER_PTR_NAUTH:
				case STATIC_PTR_AUTH:
				case STATIC_PTR_NAUTH:
				case LOCAL_CNAME:
				case EXT_CNAME:
				case CACHED:

					if (cache->hostname)
						free(cache->hostname);

					if (cache->mapname)
						free(cache->mapname);

					break;

				default:

					if (cache->mapname)
						free(cache->mapname);

					break;
			}

			free(cache);
		}
	}
	return ;
}

char* CDualServer::cloneString(char *string)
{
	char *s = (char*)calloc(1, strlen(string) + 1);

	if (s)
	{
		strcpy(s, string);
	}
	return s;
}

MYDWORD CDualServer::getSerial(char *zone)
{
	time_t t = time(NULL);
	char tempbuff[256];
	char logBuff[256];
	MYDWORD serial1 = 0;
	data5 req;
	memset(&req, 0, sizeof(data5));
	req.remote.sin_family = AF_INET;
	req.remote.sin_port = htons(DnsServerPort);
	timeval tv1;
	fd_set readfds1;

	if (cfig.replication == 2)
		req.remote.sin_addr.s_addr = cfig.zoneServers[0];
	else
		req.remote.sin_addr.s_addr = cfig.zoneServers[1];

	req.sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	req.dnsp = (dnsPacket*)req.raw;
	req.dnsp->header.qdcount = htons(1);
	req.dnsp->header.xid = (t % USHRT_MAX);
	req.dp = &req.dnsp->data;
	req.dp += pQu(req.dp, zone);
	req.dp += pUShort(req.dp, DNS_TYPE_SOA);
	req.dp += pUShort(req.dp, DNS_CLASS_IN);
	req.bytes = req.dp - req.raw;
	//pUShort(req.raw, req.bytes - 2);

	if ((req.bytes = sendto(req.sock, req.raw, req.bytes, 0, (sockaddr*)&req.remote, sizeof(req.remote))) <= 0)
	{
		closesocket(req.sock);
		sprintf(logBuff, "Failed to send request to Primary Server %s", IP2String(tempbuff, req.remote.sin_addr.s_addr));
		logDNSMess(logBuff, 1);
		return 0;
	}

	FD_ZERO(&readfds1);
	tv1.tv_sec = 3;
	tv1.tv_usec = 0;
	FD_SET(req.sock, &readfds1);
	select(USHRT_MAX, &readfds1, NULL, NULL, &tv1);

	if (FD_ISSET(req.sock, &readfds1))
	{
		req.sockLen = sizeof(req.remote);
		req.bytes = recvfrom(req.sock, req.raw, sizeof(req.raw), 0, (sockaddr*)&req.remote, &req.sockLen);

		if (req.bytes > 0 && req.dnsp->header.qr && !req.dnsp->header.rcode && ntohs(req.dnsp->header.ancount))
		{
			req.dp = &req.dnsp->data;
			for (int j = 1; j <= ntohs(req.dnsp->header.qdcount); j++)
			{
				req.dp += fQu(tempbuff, req.dnsp, req.dp);
				req.dp += 4;
			}

			for (int i = 1; i <= ntohs(req.dnsp->header.ancount); i++)
			{
				req.dp += fQu(tempbuff, req.dnsp, req.dp);
				req.qtype = fUShort(req.dp);
				req.dp += 2; //type
				req.qclass = fUShort(req.dp);
				req.dp += 2; //class
				fULong(req.dp);
				req.dp += 4; //ttl
				req.dp += 2; //datalength

				if (req.qtype == DNS_TYPE_SOA)
				{
					req.dp += fQu(tempbuff, req.dnsp, req.dp);
					req.dp += fQu(tempbuff, req.dnsp, req.dp);
					serial1 = fULong(req.dp);
				}
			}
		}
	}

	closesocket(req.sock);
	return serial1;
}

void CDualServer::checkDNS(MYBYTE dnsIndex)
{
	time_t t = time(NULL);
	//printf("DNS Index %u\n", dnsIndex);
	data5 req;
	memset(&req, 0, sizeof(data5));
	strcpy(req.query, "1.0.0.127.in-addr.arpa");
	req.remote.sin_family = AF_INET;
	req.remote.sin_port = htons(DnsServerPort);

	if (dnsIndex < MAX_SERVERS)
	{
		if (network.currentDNS == dnsIndex)
			req.remote.sin_addr.s_addr = network.dns[dnsIndex];
		else
			return ;
	}
	else if (dnsIndex >= 128 && dnsIndex < 192)
	{
		MYBYTE childIndex = (dnsIndex - 128) / 2;
		data6 *dnsRoute = &cfig.dnsRoutes[childIndex];
		MYBYTE currentDNS = dnsIndex % 2;

		if (dnsRoute->currentDNS == currentDNS)
			req.remote.sin_addr.s_addr = dnsRoute->dns[currentDNS];
		else
			return ;
	}
	else
		return ;

	req.dnsp = (dnsPacket*)req.raw;
	req.dnsp->header.qdcount = htons(1);
	req.dnsp->header.xid = (t % USHRT_MAX);
	req.dp = &req.dnsp->data;
	req.dp += pQu(req.dp, req.query);
	req.dp += pUShort(req.dp, DNS_TYPE_PTR);
	req.dp += pUShort(req.dp, DNS_CLASS_IN);
	req.bytes = req.dp - req.raw;

	char mapname[8];
	sprintf(mapname, "%u", req.dnsp->header.xid);
	data7 *queue = findEntry(cacheInd, mapname, DNS_CHECK);

	if (!queue)
	{
		if ((req.bytes = sendto(network.forwConn.sock, req.raw, req.bytes, 0, (sockaddr*)&req.remote, sizeof(req.remote))) > 0)
		{
			queue = (data7*)calloc(1, sizeof(data7));
			if (queue)
			{
				queue->mapname = cloneString(mapname);

				if (!queue->mapname)
				{
					free(queue);
					return ;
				}

				queue->expiry = 2 + t;
				queue->dataType = DNS_CHECK;
				queue->sockInd = UCHAR_MAX;
				queue->dnsIndex = dnsIndex;
				addEntry(cacheInd, queue);
			}
			else
				return ;

			if (cfig.dnsLogLevel)
			{
				if (dnsIndex < MAX_SERVERS)
					sprintf(logBuff, "Verification sent to Forwarding DNS Server %s", IP2String(tempbuff, req.remote.sin_addr.s_addr));
				else
					sprintf(logBuff, "Verification sent to Child DNS Server %s", IP2String(tempbuff, req.remote.sin_addr.s_addr));

				logDNSMess(logBuff, 1);
			}
		}
	}
}

char* CDualServer::getServerName(char *target, MYDWORD ip)
{
	time_t t = time(NULL);
	data5 req;
	memset(&req, 0, sizeof(data5));
	req.remote.sin_family = AF_INET;
	req.remote.sin_port = htons(DnsServerPort);
	req.remote.sin_addr.s_addr = ip;

	timeval tv1;
	fd_set readfds1;

	req.sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	req.dnsp = (dnsPacket*)req.raw;
	req.dnsp->header.qdcount = htons(1);
	req.dnsp->header.xid = (t % USHRT_MAX);
	req.dp = &req.dnsp->data;
	IP2String(target, htonl(ip));
	strcat(target, arpa);
	req.dp += pQu(req.dp, target);
	req.dp += pUShort(req.dp, DNS_TYPE_PTR);
	req.dp += pUShort(req.dp, DNS_CLASS_IN);
	req.bytes = req.dp - req.raw;
	//pUShort(req.raw, req.bytes - 2);

	if ((req.bytes = sendto(req.sock, req.raw, req.bytes, 0, (sockaddr*)&req.remote, sizeof(req.remote))) <= 0)
	{
		closesocket(req.sock);
		return NULL;
	}

	FD_ZERO(&readfds1);
	tv1.tv_sec = 5;
	tv1.tv_usec = 0;
	FD_SET(req.sock, &readfds1);
	select(USHRT_MAX, &readfds1, NULL, NULL, &tv1);

	if (FD_ISSET(req.sock, &readfds1))
	{
		req.sockLen = sizeof(req.remote);
		req.bytes = recvfrom(req.sock, req.raw, sizeof(req.raw), 0, (sockaddr*)&req.remote, &req.sockLen);
		if (req.bytes > 0 && req.dnsp->header.qr && !req.dnsp->header.rcode && ntohs(req.dnsp->header.ancount))
		{
			closesocket(req.sock);
			return getResult(&req);
		}
	}

	closesocket(req.sock);
	return NULL;
}

MYWORD CDualServer::recvTcpDnsMess(SOCKET sock, char *target, bool ready)
{
	timeval tv1;
	fd_set readfds1;

	if (!ready)
	{
		FD_ZERO(&readfds1);
		tv1.tv_sec = 5;
		tv1.tv_usec = 0;
		FD_SET(sock, &readfds1);

		if (!select(sock + 1, &readfds1, NULL, NULL, &tv1))
			return 0;
	}

	errno = 0;
	MYWORD rcd = recv(sock, target, 2, 0);
	errno = WSAGetLastError();

	if (errno)
		return 0;

	if (rcd == 2)
	{
		MYWORD bytes = fUShort(target) + rcd;
		char *ptr;

		while (rcd < bytes)
		{
			FD_ZERO(&readfds1);
			tv1.tv_sec = 5;
			tv1.tv_usec = 0;
			FD_SET(sock, &readfds1);
			ptr = target + rcd;

			if (select(sock + 1, &readfds1, NULL, NULL, &tv1))
			{
				errno = 0;
				rcd += recv(sock, ptr, bytes - rcd, 0);
				errno = WSAGetLastError();

				if (errno)
					return 0;
			}
			else
				return 0;
		}
		return rcd;
	}
	return 0;
}

void CDualServer::checkZone(void *lpParam)
{
	char tempbuff[256];
	char logBuff[256];

	while (kRunning)
	{
		time_t t = time(NULL);
		sprintf(logBuff, "Checking Serial from Primary Server %s", IP2String(tempbuff, cfig.zoneServers[0]));
		logDNSMess(logBuff, 2);

		MYDWORD serial1 = getSerial(cfig.zone);
		MYDWORD serial2 = getSerial(cfig.authority);

		if (!serial1 || !serial2)
		{
			sprintf(logBuff, "Failed to get Serial from %s, waiting %i seconds to retry", IP2String(tempbuff, cfig.zoneServers[0]), cfig.retry);
			logDNSMess(logBuff, 1);
			Sleep(1000*(cfig.retry));
			continue;
		}
		else if (cfig.serial1 && cfig.serial1 == serial1 && cfig.serial2 && cfig.serial2 == serial2)
		{
			sprintf(logBuff, "Zone Refresh not required");
			logDNSMess(logBuff, 2);

			if (cfig.expire > (MYDWORD)(INT_MAX - t))
				cfig.expireTime = INT_MAX;
			else
				cfig.expireTime = t + cfig.expire;

			Sleep(1000*(cfig.refresh));
		}
		else
		{
			serial1 = getZone(1 - cacheInd, cfig.zone);
			serial2 = getZone(1 - cacheInd, cfig.authority);

			if (!serial1 || !serial2)
			{
				sprintf(logBuff, "Waiting %u seconds to retry", cfig.retry);
				logDNSMess(logBuff, 1);
				Sleep(1000*(cfig.retry));
			}
			else
			{
				newInd = 1 - cacheInd;
				cfig.serial1 = serial1;
				cfig.serial2 = serial2;

				if (cfig.expire > (MYDWORD)(INT_MAX - t))
					cfig.expireTime = INT_MAX;
				else
					cfig.expireTime = t + cfig.expire;

				if (!lpParam)
					break;

				Sleep(1000*(cfig.refresh));
			}
		}
	}
	return;
}

MYDWORD CDualServer::getZone(MYBYTE updateCache, char *zone)
{
	char tempbuff[256];
	char logBuff[256];
	char hostname[256];
	char cname[256];
	MYDWORD serial1 = 0;
	MYDWORD serial2 = 0;
	MYDWORD hostExpiry = 0;
	MYDWORD refresh = 0;
	MYDWORD retry = 0;
	MYDWORD expire = 0;
	MYDWORD expiry;
	MYDWORD minimum = 0;
	int added = 0;
	char *data;
	char *dp;
	MYDWORD ip;
	data5 req;

	memset(&req, 0, sizeof(data5));
	req.sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (req.sock == INVALID_SOCKET)
	{
		sprintf(logBuff, "Failed to Create Socket, Zone Transfer Failed");
		logDNSMess(logBuff, 1);
		return 0;
	}

	req.addr.sin_family = AF_INET;
	req.addr.sin_addr.s_addr = cfig.zoneServers[1];
	req.addr.sin_port = 0;

	int nRet = ::bind(req.sock, (sockaddr*)&req.addr, sizeof(req.addr));

	if (nRet == SOCKET_ERROR)
	{
		closesocket(req.sock);
		sprintf(logBuff, "Error: Interface %s not ready, Zone Transfer Failed", IP2String(tempbuff, req.addr.sin_addr.s_addr));
		logDNSMess(logBuff, 1);
		return 0;
	}

	req.remote.sin_family = AF_INET;
	req.remote.sin_port = htons(DnsServerPort);
	req.remote.sin_addr.s_addr = cfig.zoneServers[0];

	req.sockLen = sizeof(req.remote);
	time_t t = time(NULL);

	if (connect(req.sock, (sockaddr*)&req.remote, req.sockLen) >= 0)
	{
		req.dp = req.raw;
		req.dp += 2;
		req.dnsp = (dnsPacket*)req.dp;
		req.dnsp->header.qdcount = htons(1);
		req.dnsp->header.xid = (t % USHRT_MAX);
		req.dp = &req.dnsp->data;
		req.dp += pQu(req.dp, zone);
		req.dp += pUShort(req.dp, DNS_TYPE_AXFR);
		req.dp += pUShort(req.dp, DNS_CLASS_IN);
		req.bytes = req.dp - req.raw;
		pUShort(req.raw, req.bytes - 2);

		if (send(req.sock, req.raw, req.bytes, 0) < req.bytes)
		{
			closesocket(req.sock);
			sprintf(logBuff, "Failed to send request to Replication Server %s, Zone Transfer Failed", IP2String(tempbuff, req.remote.sin_addr.s_addr));
			logDNSMess(logBuff, 1);
			return 0;
		}

		if (!strcasecmp(zone, cfig.zone))
		{
			for (; cfig.mxCount[updateCache]; cfig.mxCount[updateCache]--)
				strcpy(cfig.mxServers[updateCache][cfig.mxCount[updateCache]].hostname, "");
		}

		while (kRunning)
		{
			req.bytes = recvTcpDnsMess(req.sock, req.raw, false);
			//printf("bytes = %u\n", req.bytes);

			if (req.bytes < 2)
				break;

			MYWORD pktSize = fUShort(req.raw);

			if ((MYWORD)req.bytes < pktSize + 2)
				break;

			req.dnsp = (dnsPacket*)(req.raw + 2);
			req.dp = &req.dnsp->data;

			if (!req.dnsp->header.qr || req.dnsp->header.rcode || !ntohs(req.dnsp->header.ancount))
				break;

			for (int j = 1; j <= ntohs(req.dnsp->header.qdcount); j++)
			{
				req.dp += fQu(hostname, req.dnsp, req.dp);
				req.dp += 4;
			}

			for (int i = 1; i <= ntohs(req.dnsp->header.ancount); i++)
			{
				//char *dp = req.dp;
				req.dp += fQu(hostname, req.dnsp, req.dp);
				//printf("%s\n", hostname);
				req.qtype = fUShort(req.dp);
				req.dp += 2; //type
				req.qclass = fUShort(req.dp);
				req.dp += 2; //class
				expiry = fULong(req.dp);
				req.dp += 4; //ttl
				int dataSize = fUShort(req.dp);
				req.dp += 2; //datalength
				data = req.dp;
				req.dp += dataSize;

				switch (req.qtype)
				{
					case DNS_TYPE_SOA:

						data += fQu(hostname, req.dnsp, data);
						data += fQu(cname, req.dnsp, data);

						if (!serial1)
						{
							hostExpiry = expiry;
							serial1 = fULong(data);
							data += 4;
							refresh = fULong(data);
							data += 4;
							retry = fULong(data);
							data += 4;
							expire = fULong(data);
							data += 4;
							minimum = fULong(data);
							data += 4;
							added++;
						}
						else if (!serial2)
							serial2 = fULong(data);

						break;

					case DNS_TYPE_A:

						ip = fIP(data);
						makeLocal(hostname);

						if (dhcpService && expiry < hostExpiry)
						{
							char rInd = getRangeInd(ip);
							int ind = getIndex(rInd, ip);

							if (expiry > INT_MAX)
								expiry = INT_MAX;

							if (ind >= 0 && cfig.dhcpRanges[rInd].dhcpEntry[ind])
								setLeaseExpiry(cfig.dhcpRanges[rInd].dhcpEntry[ind], expiry, false);
							else
								setLeaseExpiry(ip, 0);

							if (expiry < (MYDWORD)(INT_MAX - t))
								expiry += static_cast<unsigned int>(t);

							addToCache(updateCache, hostname, ip, expiry, LOCAL_A, NONE, serial1);
							added++;
						}
						else
						{
							setLeaseExpiry(ip, INT_MAX);
							addToCache(updateCache, hostname, ip, INT_MAX, STATIC_A_AUTH, NONE, serial1);
							added++;
						}
						break;

					case DNS_TYPE_PTR:

						myLower(hostname);
						dp = strstr(hostname, arpa);

						if (dp)
						{
							*dp = 0;
							ip = ntohl(my_inet_addr(hostname));
							fQu(hostname, req.dnsp, data);
							makeLocal(hostname);

							if (dhcpService && expiry < hostExpiry)
							{
								char rInd = getRangeInd(ip);
								int ind = getIndex(rInd, ip);

								if (expiry > INT_MAX)
									expiry = INT_MAX;

								if (ind >= 0 && cfig.dhcpRanges[rInd].dhcpEntry[ind])
									setLeaseExpiry(cfig.dhcpRanges[rInd].dhcpEntry[ind], expiry, false);
								else
									setLeaseExpiry(ip, 0);

								if (expiry < (MYDWORD)(INT_MAX - t))
									expiry += static_cast<unsigned int>(t);

								addToCache(updateCache, hostname, ip, expiry, NONE, LOCAL_PTR_AUTH, serial1);
								added++;
							}
							else
							{
								setLeaseExpiry(ip, INT_MAX);
								addToCache(updateCache, hostname, ip, INT_MAX, NONE, STATIC_PTR_AUTH, serial1);
								added++;
							}
						}
						break;

					case DNS_TYPE_MX:

						if (cfig.replication == 2 && makeLocal(hostname) == DNTYPE_A_ZONE)
						{
							cfig.mxServers[updateCache][cfig.mxCount[updateCache]].pref = fUShort(data);
							data += sizeof(MYWORD);
							fQu(cname, req.dnsp, data);
							strcpy(cfig.mxServers[updateCache][cfig.mxCount[updateCache]].hostname, cname);
							cfig.mxCount[updateCache]++;
							added++;
						}
						break;

					case DNS_TYPE_NS:
						if (cfig.replication == 2)
							fQu(cfig.ns, req.dnsp, data);
						break;

					case DNS_TYPE_CNAME:
						if (cfig.replication == 2)
						{
							fQu(cname, req.dnsp, data);
							MYBYTE cname_type = 0;

							if (makeLocal(cname) == DNTYPE_A_EXT)
								cname_type = EXT_CNAME;
							else
								cname_type = LOCAL_CNAME;

							makeLocal(hostname);

							data7 *cache = findEntry(updateCache, hostname, cname_type);

							if (!cache)
							{
								cache = (data7*)calloc(1, sizeof(data7));

								if (cache)
								{
									cache->mapname = cloneString(hostname);
									cache->dataType = cname_type;
									cache->hostname = cloneString(cname);

									if (cache->mapname && cache->hostname)
									{
										addEntry(updateCache, cache);
									}
									else
									{
										sprintf(logBuff, "Memory Error");
										logDNSMess(logBuff, 1);
										continue;
									}
								}
								else
								{
									sprintf(logBuff, "Memory Error");
									logDNSMess(logBuff, 1);
									continue;
								}
							}
							else
							{
								if (strcasecmp(cname, cache->hostname))
								{
									free(cache->hostname);
									cache->hostname = cloneString(cname);
								}
							}

							cache->expiry = INT_MAX;
							cache->serial = serial1;
							added++;
						}
						break;
				}
				//printf("serial=%u %u %u\n", serial1, serial2, hostExpiry);
			}
		}


		strcpy(hostname, cfig.ns);
		makeLocal(hostname);

		data7 *cache = findEntry(updateCache, hostname, STATIC_A_AUTH);

		if (!cache)
			cache = findEntry(updateCache, hostname, STATIC_A_NAUTH);

		if (cache)
			cfig.nsIP = cache->ip;

		closesocket(req.sock);

		if (serial1 && serial1 == serial2 && hostExpiry)
		{
			if (cfig.replication == 2)
			{
				//printf("Here %u %u %u %u %u \n",hostExpiry,refresh,retry,expire,minimum);
				cfig.lease = hostExpiry;
				cfig.refresh = refresh;
				cfig.retry = retry;
				cfig.expire = expire;
				cfig.minimum = minimum;
			}

			if (cacheInd != updateCache)
			{
				if (!strcasecmp(zone, cfig.zone))
				{
					hostMap::iterator q = dnsCache[updateCache].begin();

					while (q != dnsCache[updateCache].end())
					{
						//printf("%s=%u\n", q->second->mapname,q->second->serial);
						switch (q->second->dataType)
						{
							case LOCAL_A:
							case SERVER_A:
							case STATIC_A_AUTH:
							case LOCAL_CNAME:
							case EXT_CNAME:
								if (q->second->serial < serial1)
									q->second->expiry = 0;
						}
						q++;
					}
				}
				else if (!strcasecmp(zone, cfig.authority))
				{
					hostMap::iterator q = dnsCache[updateCache].begin();

					while (q != dnsCache[updateCache].end())
					{
						//printf("%s=%u\n", q->second->mapname,q->second->serial);
						switch (q->second->dataType)
						{
							case LOCAL_PTR_AUTH:
							case STATIC_PTR_AUTH:
							case SERVER_PTR_AUTH:
								if (q->second->serial < serial1)
									q->second->expiry = 0;
						}
						q++;
					}
				}

				checkSize(updateCache);

			}

			//printf("Refresh ind %i serial %u size %i\n", updateCache, serial1, dnsCache[updateCache].size());
			sprintf(logBuff, "Zone %s Transferred from Primary Server, %u RRs imported", zone, added);
			logDNSMess(logBuff, 2);
			return serial1;
		}
		else if (!serial1)
		{
			sprintf(logBuff, "Replication Server %s, Missing Serial", IP2String(tempbuff, req.remote.sin_addr.s_addr));
			logDNSMess(logBuff, 1);
		}
		else if (serial1 != serial2)
		{
			sprintf(logBuff, "Replication Server %s, Serial Changed %u %u", IP2String(tempbuff, req.remote.sin_addr.s_addr), serial1, serial2);
			logDNSMess(logBuff, 1);
		}
		else
		{
			sprintf(logBuff, "Replication Server %s, Invalid AXFR data", IP2String(tempbuff, req.remote.sin_addr.s_addr));
			logDNSMess(logBuff, 1);
		}
	}
	else
	{
		errno = WSAGetLastError();
		sprintf(logBuff, "Server %s, Winsock Error %u", IP2String(tempbuff, req.remote.sin_addr.s_addr), errno);
		logDNSMess(logBuff, 1);
		closesocket(req.sock);
	}

	return 0;
}

void CDualServer::getSecondary()
{
	char hostname[256];
	MYDWORD ip;
	MYDWORD hostExpiry = 0;
	MYDWORD expiry = 0;
	char *data = NULL;
	char *dp = NULL;
	MYWORD rr = 0;
	data5 req;

	memset(&req, 0, sizeof(data5));
	req.sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (req.sock == INVALID_SOCKET)
		return;

	req.addr.sin_family = AF_INET;
	req.addr.sin_addr.s_addr = cfig.zoneServers[0];
	req.addr.sin_port = 0;

	int nRet = ::bind(req.sock, (sockaddr*)&req.addr, sizeof(req.addr));

	if (nRet == SOCKET_ERROR)
	{
		closesocket(req.sock);
		return;
	}

	req.remote.sin_family = AF_INET;
	req.remote.sin_port = htons(DnsServerPort);

	if (dhcpService && cfig.replication == 1)
		req.remote.sin_addr.s_addr = cfig.zoneServers[1];
	else
		return;

	req.sockLen = sizeof(req.remote);
	time_t t = time(NULL);

	if (connect(req.sock, (sockaddr*)&req.remote, req.sockLen) >= 0)
	{
		req.dp = req.raw;
		req.dp += 2;
		req.dnsp = (dnsPacket*)req.dp;
		req.dnsp->header.qdcount = htons(1);
		req.dnsp->header.xid = (t % USHRT_MAX);
		req.dp = &req.dnsp->data;
		req.dp += pQu(req.dp, cfig.authority);
		req.dp += pUShort(req.dp, DNS_TYPE_AXFR);
		req.dp += pUShort(req.dp, DNS_CLASS_IN);
		req.bytes = req.dp - req.raw;
		pUShort(req.raw, req.bytes - 2);

		if (send(req.sock, req.raw, req.bytes, 0) < req.bytes)
		{
			closesocket(req.sock);
			return;
		}

		while (kRunning)
		{
			req.bytes = recvTcpDnsMess(req.sock, req.raw, false);
			//printf("bytes = %u\n", req.bytes);

			if (req.bytes < 2)
				break;

			MYWORD pktSize = fUShort(req.raw);

			if ((MYWORD)req.bytes < pktSize + 2)
				break;

			req.dnsp = (dnsPacket*)(req.raw + 2);
			req.dp = &req.dnsp->data;

			if (!req.dnsp->header.qr || req.dnsp->header.rcode || !ntohs(req.dnsp->header.ancount))
				break;

			for (int j = 1; j <= ntohs(req.dnsp->header.qdcount); j++)
			{
				req.dp += fQu(hostname, req.dnsp, req.dp);
				req.dp += 4;
			}

			for (int i = 1; i <= ntohs(req.dnsp->header.ancount); i++)
			{
				//char *dp = req.dp;
				req.dp += fQu(hostname, req.dnsp, req.dp);
				//printf("%s\n", hostname);
				req.qtype = fUShort(req.dp);
				req.dp += 2; //type
				req.qclass = fUShort(req.dp);
				req.dp += 2; //class
				expiry = fULong(req.dp);
				req.dp += 4; //ttl
				int dataSize = fUShort(req.dp);
				req.dp += 2; //datalength
				data = req.dp;
				req.dp += dataSize;

				switch (req.qtype)
				{
					case DNS_TYPE_SOA:

						data += fQu(tempbuff, req.dnsp, data);
						data += fQu(tempbuff, req.dnsp, data);
						cfig.serial1 = fULong(data);
						cfig.serial2 = cfig.serial1;
						//printf("%u\n", cfig.serial1);
						break;

					case DNS_TYPE_PTR:

						myLower(hostname);
						dp = strstr(hostname, arpa);

						if (dp)
						{
							*dp = 0;
							ip = ntohl(my_inet_addr(hostname));
							fQu(hostname, req.dnsp, data);
							makeLocal(hostname);

							if (dhcpService && expiry < hostExpiry)
							{
								char rInd = getRangeInd(ip);
								int ind = getIndex(rInd, ip);

								if (expiry > INT_MAX)
									expiry = INT_MAX;

								if (ind >= 0 && cfig.dhcpRanges[rInd].dhcpEntry[ind])
									setLeaseExpiry(cfig.dhcpRanges[rInd].dhcpEntry[ind], expiry, false);
								else
									setLeaseExpiry(ip, 0);

								if (expiry < (MYDWORD)(INT_MAX - t))
									expiry += static_cast<unsigned int>(t);

								addToCache(cacheInd, hostname, ip, expiry, LOCAL_A, NONE, cfig.serial1);
								addToCache(cacheInd, hostname, ip, expiry, NONE, LOCAL_PTR_AUTH, cfig.serial2);
								rr++;
							}
						}
						break;
				}
			}
		}

		sprintf(logBuff, "%u RRs imported from Secondary Server", rr);
		logDNSMess(logBuff, 2);
	}
	else
	{
		errno = WSAGetLastError();
		sprintf(logBuff, "Failed to connect to Secondary Server %s, Winsock Error %u", IP2String(tempbuff, req.remote.sin_addr.s_addr), errno);
		logDNSMess(logBuff, 1);
	}
	closesocket(req.sock);
}

void CDualServer::init(void *lparam, int *nostatic)
{
	time_t t = time(NULL);

	memset(&cfig, 0, sizeof(cfig));
	memset(&network, 0, sizeof(network));

	cfig.dnsLogLevel = default_loglevel;
	cfig.dhcpLogLevel = default_loglevel;

	if (m_pszFlagFolder != NULL)
	{
		_tcsncpy_s(leaFile, MAX_PATH, m_pszFlagFolder, _TRUNCATE);
		_tcsncat_s(leaFile, MAX_PATH, _T("dhcp.lea"), _TRUNCATE);
		logMess("leafile", 1);
	}

	TCHAR tsz[MAX_PATH];
	GetModuleFileName(NULL, tsz, MAX_PATH);

	memset(extbuff, 0, MAX_PATH);

#ifdef _UNICODE
	WideCharToMultiByte(CP_ACP, NULL, tsz, -1, extbuff, _MAX_PATH, NULL, NULL);
#else
	strncpy_s(extbuff, MAX_PATH, tsz, _TRUNCATE);
#endif
/*
	char *fileExt = strrchr(extbuff, '.');
	*fileExt = 0;
	sprintf(leaFile, "%s.state", extbuff);
	sprintf(iniFile, "%s.ini", extbuff);
	sprintf(lnkFile, "%s.url", extbuff);
	sprintf(htmFile, "%s.htm", extbuff);
	fileExt = strrchr(extbuff, '\\');
	*fileExt = 0;
	fileExt++;
	sprintf(logFile, "%s\\log\\%s%%Y%%m%%d.log", extbuff, fileExt);
*/

	MYWORD wVersionRequested = MAKEWORD(1, 1);
	WSAStartup(wVersionRequested, &cfig.wsaData);

	if (cfig.wsaData.wVersion != wVersionRequested)
	{
		sprintf(logBuff, "WSAStartup Error");
		logMess(logBuff, 1);
	}

//	std::stringstream ss;

/*
  ifstream is; 
  is.open ("s:\\home\\DualServer\\dualserver.ini", ios::binary ); 
 
  // get length of file: 
  is.seekg (0, std::ios::end); 
  long length = is.tellg(); 
  is.seekg (0, std::ios::beg); 
 
  // allocate memory: 
  char *buffer = new char [length]; 
 
  // read data as a block: 
  is.read (buffer,length); 
 
  // create string stream of memory contents 
  // NOTE: this ends up copying the buffer!!! 
	ss << buffer;

  // delete temporary buffer 
  delete [] buffer; 
 
  // close filestream 
  is.close();
*/

/*
	if (openSection("SERVICES", 1, &ss))
	{
		dhcpService = false;
		dnsService = false;

		while(readSection(raw, &ss))
			if (!strcasecmp(raw, "DNS"))
				dnsService = true;
			else if (!strcasecmp(raw, "DHCP"))
				dhcpService = true;
			else
			{
				sprintf(logBuff, "Section [SERVICES] invalid entry %s ignored", raw);
				logMess(logBuff, 1);
			}

		if (!dhcpService && !dnsService)
		{
			dhcpService = true;
			dnsService = true;
		}
	}
*/
	if (dnsService)
	{
		sprintf(logBuff, "Starting DNS Service...");
		logDNSMess(logBuff, 3);
	}

	if (dhcpService)
	{
		sprintf(logBuff, "Starting DHCP Service...");
		logDHCPMess(logBuff, 3);
	}

	// See "TIMINGS" section

	cfig.lease = 360000;
	cfig.refresh = 3600;
	cfig.retry = 100;
	cfig.expire = 360000;
	cfig.minimum = 100;
	cfig.minCache = 100;
	cfig.maxCache = 1000;

/*
	if (openSection("DOMAIN-NAME", 1, &ss))
	{
		while (readSection(raw, &ss))
		{
			mySplit(name, value, raw, '=');

			if (name[0] && value[0])
			{
				data15 mask;
				data15 network;
				char left[64];

				cfig.authority[0] = 0;
				myLower(value);
				mask.ip = 0;
				network.ip = 0;

				for (MYBYTE octateNum = 0; octateNum < 3; octateNum++)
				{
					mySplit(left, value, value, '.');
					if (left[0] == '0' || (atoi(left) && atoi(left) < 256))
					{
						for (int j = 2; j >= 0; j--)
						{
							network.octate[j + 1] = network.octate[j];
							mask.octate[j + 1] = mask.octate[j];
						}

						mask.octate[0] = UCHAR_MAX;
						network.octate[0] = atoi(left);
						strcat(cfig.authority, left);
						strcat(cfig.authority, ".");
					}
					else
						break;

					if (!strcasecmp(value, arpa + 1))
						break;
				}

				if (!strcasecmp(value, arpa + 1))
				{
					strcat(cfig.authority, arpa + 1);
					cfig.aLen = strlen(cfig.authority);
					calcRangeLimits(network.ip, mask.ip, &cfig.rangeStart, &cfig.rangeEnd);
					cfig.authorized = 1;
				}
				else
				{
					sprintf(logBuff, "Warning: Invalid Domain Name (Part %s), ignored", cfig.authority);
					cfig.aLen = 0;
					cfig.authority[0] = 0;
					logDNSMess(logBuff, 1);
				}
			}

			if (chkQu(name))
			{
				strcpy(cfig.zone, name);
				cfig.zLen = strlen(cfig.zone);
			}
			else
			{
				cfig.aLen = 0;
				cfig.authority[0] = 0;
				sprintf(logBuff, "Warning: Invalid Domain Name %s, ignored", raw);
				logDNSMess(logBuff, 1);
			}
		}
	}
*/

	getInterfaces(&network);
	sprintf(cfig.servername_fqn, "%s.%s", cfig.servername, cfig.zone);
	strcpy(cfig.ns, cfig.servername_fqn);
	cfig.zLen = strlen(cfig.zone);
/*
	if (openSection("ZONE-REPLICATION", 1, &ss))
	{
		int i = 2;
		while (readSection(raw, &ss))
		{
			if(i < MAX_SERVERS)
			{
				if (dnsService && !cfig.authorized)
				{
					sprintf(logBuff, "Section [REPLICATION-SERVERS], Server is not authorized, entry %s ignored", raw);
					logMess(logBuff, 1);
					continue;
				}

				mySplit(name, value, raw, '=');

				if (name[0] && value[0])
				{
					if (chkQu(name) && !isIP(name) && isIP(value))
					{
						if (!strcasecmp(name, "Primary"))
							cfig.zoneServers[0] = my_inet_addr(value);
						else if (!strcasecmp(name, "Secondary"))
							cfig.zoneServers[1] = my_inet_addr(value);
						else if (dnsService && !strcasecmp(name, "AXFRClient"))
						{
							cfig.zoneServers[i] = my_inet_addr(value);
							i++;
						}
						else
						{
							sprintf(logBuff, "Section [REPLICATION-SERVERS] Invalid Entry: %s ignored", raw);
							logMess(logBuff, 1);
						}
					}
					else
					{
						sprintf(logBuff, "Section [REPLICATION-SERVERS] Invalid Entry: %s ignored", raw);
						logMess(logBuff, 1);
					}
				}
				else
				{
					sprintf(logBuff, "Section [REPLICATION-SERVERS], Missing value, entry %s ignored", raw);
					logMess(logBuff, 1);
				}
			}
		}
	}
*/
	if (!cfig.zoneServers[0] && cfig.zoneServers[1])
	{
		sprintf(logBuff, "Section [REPLICATION-SERVERS] Missing Primary Server");
		logMess(logBuff, 1);
	}
	else if (cfig.zoneServers[0] && !cfig.zoneServers[1])
	{
		sprintf(logBuff, "Section [REPLICATION-SERVERS] Missing Secondary Server");
		logMess(logBuff, 1);
	}
	else if (cfig.zoneServers[0] && cfig.zoneServers[1])
	{
		if (findServer(network.staticServers, cfig.zoneServers[0]) && findServer(network.staticServers, cfig.zoneServers[1]))
		{
			sprintf(logBuff, "Section [REPLICATION-SERVERS] Primary & Secondary should be Different Boxes");
			logMess(logBuff, 1);
		}
		else if (findServer(network.staticServers, cfig.zoneServers[0]))
			cfig.replication = 1;
		else if (findServer(network.staticServers, cfig.zoneServers[1]))
			cfig.replication = 2;
		else
		{
			sprintf(logBuff, "Section [REPLICATION-SERVERS] No Server IP not found on this Machine");
			logMess(logBuff, 1);
		}
	}

	if (dhcpService)
	{
//		if (openSection("DHCP-OPTIONS", 1, &ss))
		if (TRUE)
		{
			data20 optionData;
			parseDhcpOptions(pAlexDhcpOptions, "DHCP-OPTIONS", &optionData);
			cfig.options = optionData.options;
			cfig.mask = optionData.mask;
		}

//		for (MYBYTE i = 1; i <= MAX_RANGES ; i++)
		for (MYBYTE i = 1; i <= 1 ; i++)
		{
//			if (openSection("DHCP-RANGE", i, &ss))
			if (TRUE)
			{
				MYBYTE m = 0;
				for (; m < MAX_RANGES && cfig.dhcpRanges[m].rangeStart; m++);

				data20 optionData;
				optionData.rangeSetInd = i - 1;
				parseDhcpOptions(pAlexDhcpRange, "DHCP-RANGE", &optionData);
				cfig.rangeSet[optionData.rangeSetInd].active = true;

				for (; m < MAX_RANGES && cfig.dhcpRanges[m].rangeStart; m++)
				{
					cfig.dhcpRanges[m].rangeSetInd = optionData.rangeSetInd;
					cfig.dhcpRanges[m].options = optionData.options;
					cfig.dhcpRanges[m].mask = optionData.mask;
				}
			}
			else
				break;
		}

		if (!cfig.mask)
			cfig.mask = my_inet_addr("255.255.255.0");

		for (char rangeInd = 0; rangeInd < MAX_RANGES && cfig.dhcpRanges[rangeInd].rangeStart; rangeInd++)
		{
			if (!cfig.dhcpRanges[rangeInd].mask)
				cfig.dhcpRanges[rangeInd].mask = cfig.mask;

			for (MYDWORD iip = cfig.dhcpRanges[rangeInd].rangeStart; iip <= cfig.dhcpRanges[rangeInd].rangeEnd; iip++)
			{
				MYDWORD ip = htonl(iip);

				if ((cfig.dhcpRanges[rangeInd].mask | (~ip)) == UINT_MAX || (cfig.dhcpRanges[rangeInd].mask | ip) == UINT_MAX)
					cfig.dhcpRanges[rangeInd].expiry[iip - cfig.dhcpRanges[rangeInd].rangeStart] = INT_MAX;
			}
		}

		statrec_iterator iter;
		for (iter = AlexDhcpStaticHosts.begin(); iter != AlexDhcpStaticHosts.end(); iter++)
		{
			memset(tempbuff, 0, sizeof(tempbuff));

			data7 *dhcpEntry;
			data20 optionData;
			memset(&optionData, 0, sizeof(optionData));

			char szIp[64];
			sprintf_s(szIp, 64, "IP_Addr=%s", (*iter).ip.to_string().c_str());
			parseDhcpOptions(szIp, "", &optionData);

			if (optionData.ip)
			{
				dhcpMap::iterator p = dhcpCache.begin();

				for (; p != dhcpCache.end(); p++)
				{
					if (p->second && p->second->ip == optionData.ip)
						break;
				}

				if (p == dhcpCache.end())
				{
					dhcpEntry = (data7*)calloc(1, sizeof(data7));

					if (!dhcpEntry)
					{
						sprintf(logBuff, "Client Options Load, Memory Allocation Error");
						logDHCPMess(logBuff, 1);
						return;
					}

					unsigned char* mac = (unsigned char*)(*iter).mac.get();
/*
					__int64 m = (*iter).first;
					unsigned char mac[6];
					mac[5] = (BYTE)m & 0x00000000000000FF;
					mac[4] = (BYTE)((m & 0x000000000000FF00) >> 8);
					mac[3] = (BYTE)((m & 0x0000000000FF0000) >> 16);
					mac[2] = (BYTE)((m & 0x00000000FF000000) >> 24);
					mac[1] = (BYTE)((m & 0x000000FF00000000) >> 32);
					mac[0] = (BYTE)((m & 0x0000FF0000000000) >> 40);
*/
					dhcpEntry->mapname = cloneString(toUUE(tempbuff, mac, 6));

					if (!dhcpEntry->mapname)
					{
						sprintf(logBuff, "Client Data Load, Memory Allocation Error");
						logDHCPMess(logBuff, 1);
						return;
					}

					MYDWORD tmask = htonl(optionData.mask);

					while (tmask)
					{
						(dhcpEntry->bitmask)++;
						tmask = tmask << 1;
					}

					dhcpEntry->ip = optionData.ip;
					dhcpEntry->options = optionData.options;
					dhcpEntry->rangeInd = getRangeInd(optionData.ip);
					dhcpEntry->fixed = 1;
					setLeaseExpiry(optionData.ip, INT_MAX);
					dhcpCache[dhcpEntry->mapname] = dhcpEntry;
				}
			}
		}

/*
		// alex
		if (openSection("DHCP-OPTIONS", 1, &ss))
			lockOptions(&ss);

		for (MYBYTE i = 1; i <= 32 ;i++)
		{
			if (openSection("DHCP-RANGE", i, &ss))
				lockOptions(&ss);
		}
*/
/*
		// alex
		f = fopen(iniFile, "rt");

		if (f)
		{
			char sectionName[512];

			while (fgets(sectionName, 511, f))
			{
				if (*sectionName == '[')
				{
					char *secend = strchr(sectionName, ']');

					if (secend)
					{
						*secend = 0;
						sectionName[0] = 32;
						myTrim(sectionName, sectionName);
					}
					else
						continue;
				}
				else
					continue;


				MYBYTE hexValue[255];
				MYBYTE hexValueSize = sizeof(hexValue);
				data20 optionData;

				if (!getHexValue(hexValue, sectionName, &hexValueSize))
				{
					data7 *dhcpEntry = dhcpCache[toUUE(tempbuff, hexValue, hexValueSize)];

					if (!dhcpEntry)
					{

						FILE *ff = openSection(sectionName, 1, iniFile);
						loadOptions(ff, sectionName, &optionData);
						ff = openSection(sectionName, 1, iniFile);
						lockOptions(ff);

						if (optionData.ip)
						{
							dhcpMap::iterator p = dhcpCache.begin();

							for (; p != dhcpCache.end(); p++)
							{
								if (p->second && p->second->ip == optionData.ip)
									break;
							}

							if (p == dhcpCache.end())
							{
								dhcpEntry = (data7*)calloc(1, sizeof(data7));

								if (!dhcpEntry)
								{
									sprintf(logBuff, "Client Options Load, Memory Allocation Error");
									logDHCPMess(logBuff, 1);
									return;
								}

								dhcpEntry->mapname = cloneString(toUUE(tempbuff, hexValue, hexValueSize));

								if (!dhcpEntry->mapname)
								{
									sprintf(logBuff, "Client Data Load, Memory Allocation Error");
									logDHCPMess(logBuff, 1);
									return;
								}

								MYDWORD tmask = htonl(optionData.mask);

								while (tmask)
								{
									(dhcpEntry->bitmask)++;
									tmask = tmask << 1;
								}

								dhcpEntry->ip = optionData.ip;
								dhcpEntry->options = optionData.options;
								dhcpEntry->rangeInd = getRangeInd(optionData.ip);
								dhcpEntry->fixed = 1;
								setLeaseExpiry(optionData.ip, INT_MAX);
								dhcpCache[dhcpEntry->mapname] = dhcpEntry;
								//printf("%s=%s=%s\n", sectionName, dhcpEntry->mapname, IP2String(tempbuff, optionData.ip));
							}
							else
							{
								sprintf(logBuff, "Static DHCP Host [%s] Duplicate IP Address %s, Entry ignored", sectionName, IP2String(tempbuff, optionData.ip));
								logDHCPMess(logBuff, 1);
							}
						}
						else
						{
							sprintf(logBuff, "IP Address is missing in Static DHCP Host [%s], Entry ignored", sectionName);
							logDHCPMess(logBuff, 1);
						}

					}
					else
					{
						sprintf(logBuff, "Duplicate Static DHCP Host [%s] ignored", sectionName);
						logDHCPMess(logBuff, 1);
					}
				}
				else if (strchr(sectionName, ':'))
				{
					sprintf(logBuff, "Invalid Static DHCP Host MAC Addr [%s] ignored", sectionName);
					logDHCPMess(logBuff, 1);
				}
			}

			fclose(f);
		}
*/


		// alex

		FILE *f = NULL;
		_tfopen_s(&f, leaFile, _T("rb"));

		if (f)
		{
			data8 dhcpData;

			while (fread(&dhcpData, sizeof(data8), 1, f))
			{
				char rangeInd = -1;
				int ind = -1;

				if (!findServer(network.staticServers, dhcpData.ip) && !findServer(network.dns, dhcpData.ip))
				{
					data7 *dhcpEntry = dhcpCache[toUUE(tempbuff, dhcpData.bp_chaddr, dhcpData.bp_hlen)];

					if (dhcpEntry)
					{
						if (dhcpEntry->ip != dhcpData.ip)
							continue;
					}
					else
					{
						dhcpMap::iterator p = dhcpCache.begin();

						for (; p != dhcpCache.end(); p++)
						{
							if (p->second && p->second->ip == dhcpData.ip)
								break;
						}

						if (p != dhcpCache.end())
						{
							if (p->second->fixed || p->second->expiry > dhcpData.expiry)
								continue;
							else
								dhcpCache.erase(p);
						}
					}

					rangeInd = getRangeInd(dhcpData.ip);

					if(rangeInd >= 0)
					{
						ind = getIndex(rangeInd, dhcpData.ip);

						if (ind >= 0 && !dhcpEntry)
						{
							dhcpEntry = (data7*)calloc(1, sizeof(data7));

							if (!dhcpEntry)
							{
								sprintf(logBuff, "Loading Existing Leases, Memory Allocation Error");
								logDHCPMess(logBuff, 1);
								return;
							}

							dhcpEntry->mapname = cloneString(toUUE(tempbuff, dhcpData.bp_chaddr, dhcpData.bp_hlen));

							if (!dhcpEntry->mapname)
							{
								sprintf(logBuff, "Loading Existing Leases, Memory Allocation Error");
								free(dhcpEntry);
								logDHCPMess(logBuff, 1);
								return ;
							}

							//dhcpEntry->dataType = DHCP;
							dhcpCache[dhcpEntry->mapname] = dhcpEntry;
						}
					}

					if (dhcpEntry)
					{
						dhcpEntry->ip = dhcpData.ip;
						dhcpEntry->rangeInd = rangeInd;

						if (!dhcpEntry->fixed)
							dhcpEntry->source = dhcpData.source;

						if (dhcpData.expiry > t)
						{
							setLeaseExpiry(dhcpEntry, dhcpData.expiry - t, dhcpData.local != 0);

							addToCache(cacheInd, dhcpData.hostname, dhcpEntry->ip, dhcpData.expiry, LOCAL_A, NONE, cfig.serial1);

							if (makeLocal(dhcpEntry->ip))
								addToCache(cacheInd, dhcpData.hostname, dhcpEntry->ip, dhcpData.expiry, NONE, LOCAL_PTR_AUTH, cfig.serial2);
							else
								addToCache(cacheInd, dhcpData.hostname, dhcpEntry->ip, dhcpData.expiry, NONE, LOCAL_PTR_NAUTH, cfig.serial2);
						}
						else
							setLeaseExpiry(dhcpEntry, 0, false);
					}
				}
			}

			fclose(f);

			_tfopen_s(&f, leaFile, _T("wb"));
			cfig.dhcpIndex = 0;

			if (f)
			{
				dhcpMap::iterator p = dhcpCache.begin();

				for (; p != dhcpCache.end(); p++)
				{
					if (p->second && p->second->expiry)
					{
						memset(&dhcpData, 0, sizeof(data8));
						dhcpData.bp_hlen = fromUUE(dhcpData.bp_chaddr, p->second->mapname);
						dhcpData.ip = p->second->ip;
						dhcpData.expiry = p->second->expiry;
						dhcpData.local = p->second->local;

						if (!p->second->fixed)
							dhcpData.source = p->second->source;

						data7 *cache = findEntry(cacheInd, IP2String(tempbuff, htonl(p->second->ip)));

						if (cache && cache->hostname)
							strcpy(dhcpData.hostname, cache->hostname);

						cfig.dhcpIndex++;
						dhcpData.dhcpInd = cfig.dhcpIndex;
						p->second->dhcpInd = cfig.dhcpIndex;

						fwrite(&dhcpData, sizeof(data8), 1, f);
					}
				}
				fclose(f);
			}
		}
/*
		fEvent = CreateEvent(
			NULL,                  // default security descriptor
			FALSE,                 // ManualReset
			TRUE,                  // Signalled
			TEXT("AchalDualServerFileEvent"));  // object name

		SetEvent(fEvent);
*/
		for (int i = 0; i < MAX_RANGES && cfig.dhcpRanges[i].rangeStart;i++)
		{
			char *logPtr = logBuff;
			logPtr += sprintf(logPtr, "DHCP Range: ");
			logPtr += sprintf(logPtr, "%s", IP2String(tempbuff, htonl(cfig.dhcpRanges[i].rangeStart)));
			logPtr += sprintf(logPtr, "-%s", IP2String(tempbuff, htonl(cfig.dhcpRanges[i].rangeEnd)));
			logPtr += sprintf(logPtr, "/%s", IP2String(tempbuff, cfig.dhcpRanges[i].mask));
			logDHCPMess(logBuff, 3);
		}
	}

	if (dnsService)
	{
		GetDnsAllowedHosts();

/*
		if (f = openSection("DNS-ALLOWED-HOSTS", 1, &ss))
		{
			int i = 0;
			while (readSection(raw, f))
			{
				if(i < 32)
				{
					MYDWORD rs = 0;
					MYDWORD re = 0;
					mySplit(name, value, raw, '-');

					if (value[0] && isIP(value))
					{
						rs = htonl(my_inet_addr(name));
						re = htonl(my_inet_addr(value));
					}
					else
					{
						rs = htonl(my_inet_addr(name));
						re = rs;
					}

					//printf("%u=%u\n", rs, re);

					if (rs && rs != INADDR_NONE && re && re != INADDR_NONE && rs <= re)
					{
						cfig.dnsRanges[i].rangeStart = rs;
						cfig.dnsRanges[i].rangeEnd = re;
						i++;
					}
					else
					{
						sprintf(logBuff, "Section [DNS-ALLOWED-HOSTS] Invalid entry %s in ini file, ignored", raw);
						logDNSMess(logBuff, 1);
					}
				}
			}
		}
*/
/*
		if (cfig.replication != 2 && (openSection("HOSTS", 1, &ss)))
		{
			while (readSection(raw, &ss))
			{
				mySplit(name, value, raw, '=');
				if (name[0] && value[0])
				{
					if (chkQu(name) && !isIP(name) && isIP(value))
					{
						MYDWORD inetAddr = my_inet_addr(value);

						MYBYTE nameType = makeLocal(name);
						MYBYTE ipType = makeLocal(inetAddr);

						 if (!inetAddr)
						{
							sprintf(logBuff, "Section [HOSTS] Invalid Entry: %s ignored", raw);
							logDNSMess(logBuff, 1);
						}
						else if (nameType == DNTYPE_A_ZONE && ipType)
						{
							addToCache(cacheInd, name, inetAddr, INT_MAX, STATIC_A_AUTH, STATIC_PTR_AUTH, cfig.serial1);
							setLeaseExpiry(inetAddr, INT_MAX);
						}
						else if (nameType == DNTYPE_A_ZONE)
						{
							addToCache(cacheInd, name, inetAddr, INT_MAX, STATIC_A_AUTH, STATIC_PTR_NAUTH, cfig.serial1);
							setLeaseExpiry(inetAddr, INT_MAX);
						}
						else if ((nameType == DNTYPE_A_BARE || nameType == DNTYPE_A_LOCAL || nameType == DNTYPE_A_ZONE) && ipType)
						{
							addToCache(cacheInd, name, inetAddr, INT_MAX, STATIC_A_AUTH, STATIC_PTR_AUTH, cfig.serial1);
							setLeaseExpiry(inetAddr, 0);
						}
						else if ((nameType == DNTYPE_A_BARE || nameType == DNTYPE_A_LOCAL || nameType == DNTYPE_A_ZONE))
						{
							addToCache(cacheInd, name, inetAddr, INT_MAX, STATIC_A_AUTH, STATIC_PTR_NAUTH, cfig.serial1);
							setLeaseExpiry(inetAddr, 0);
						}
						else if (ipType)
						{
							addToCache(cacheInd, name, inetAddr, INT_MAX, STATIC_A_NAUTH, STATIC_PTR_AUTH, cfig.serial1);
							setLeaseExpiry(inetAddr, 0);
						}
						else
						{
							addToCache(cacheInd, name, inetAddr, INT_MAX, STATIC_A_NAUTH, STATIC_PTR_NAUTH, cfig.serial1);
							setLeaseExpiry(inetAddr, 0);
						}
					}
					else
					{
						sprintf(logBuff, "Section [HOSTS] Invalid Entry: %s ignored", raw);
						logDNSMess(logBuff, 1);
					}
				}
				else
				{
					sprintf(logBuff, "Section [HOSTS], Missing value, entry %s ignored", raw);
					logDNSMess(logBuff, 1);
				}
			}
		}
*/
/*
		if (cfig.replication != 2 && (openSection("ALIASES", 1, &ss)))
		{
			int i = 0;

			while (readSection(raw, &ss))
			{
				mySplit(name, value, raw, '=');
				if (name[0] && value[0])
				{
					MYBYTE nameType = makeLocal(name);
					MYBYTE aliasType = makeLocal(value);

					if (chkQu(name) && chkQu(value) && strcasecmp(value, cfig.zone))
					{
						if ((nameType == DNTYPE_A_BARE || nameType == DNTYPE_A_LOCAL || nameType == DNTYPE_A_ZONE))
						{
							data7 *cache = findEntry(cacheInd, name);

							if (!cache)
							{
								cache = (data7*)calloc(1, sizeof(data7));

								if (cache)
								{
									if ((aliasType == DNTYPE_A_BARE || aliasType == DNTYPE_A_LOCAL || aliasType == DNTYPE_A_ZONE))
										cache->dataType = LOCAL_CNAME;
									else
										cache->dataType = EXT_CNAME;

									cache->mapname = cloneString(name);
									cache->hostname = cloneString(value);

									if (!cache->mapname || !cache->hostname)
									{
										sprintf(logBuff, "Section [ALIASES] entry %s memory error", raw);
										logDNSMess(logBuff, 1);
									}
									else
									{
										cache->expiry = INT_MAX;
										cache->serial = cfig.serial1;
										addEntry(cacheInd, cache);

										if (aliasType == DNTYPE_A_ZONE)
											cache->isZone = 1;

										i++;
									}
								}
								else
								{
									sprintf(logBuff, "Section [ALIASES] entry %s memory error", raw);
									logDNSMess(logBuff, 1);
								}
							}
							else
							{
								sprintf(logBuff, "Section [ALIASES] duplicate entry %s ignored", raw);
								logDNSMess(logBuff, 1);
							}
						}
						else
						{
							sprintf(logBuff, "Section [ALIASES] alias %s should be bare/local name, entry ignored", name);
							logDNSMess(logBuff, 1);
						}
					}
					else
					{
						sprintf(logBuff, "Section [ALIASES] Invalid Entry: %s ignored", raw);
						logDNSMess(logBuff, 1);
					}
				}
				else
				{
					sprintf(logBuff, "Section [ALIASES], Missing value, entry %s ignored", raw);
					logDNSMess(logBuff, 1);
				}
			}
		}
*/
/*
		if (cfig.replication != 2 && (openSection("MAIL-SERVERS", 1, &ss)))
		{
			cfig.mxCount[cacheInd] = 0;

			while (readSection(raw, &ss))
			{
				if (cfig.mxCount[cacheInd] < MAX_SERVERS)
				{
					mySplit(name, value, raw, '=');
					if (name[0] && value[0])
					{
						if (chkQu(name) && atoi(value))
						{
							cfig.mxServers[0][cfig.mxCount[cacheInd]].pref = atoi(value);
							cfig.mxServers[1][cfig.mxCount[cacheInd]].pref = atoi(value);
							if (!strchr(name, '.'))
							{
								strcat(name, ".");
								strcat(name, cfig.zone);
							}
							strcpy(cfig.mxServers[0][cfig.mxCount[cacheInd]].hostname, name);
							strcpy(cfig.mxServers[1][cfig.mxCount[cacheInd]].hostname, name);
							cfig.mxCount[cacheInd]++;
						}
						else
						{
							sprintf(logBuff, "Section [MAIL-SERVERS] Invalid Entry: %s ignored", raw);
							logDNSMess(logBuff, 1);
						}
					}
					else
					{
						sprintf(logBuff, "Section [MAIL-SERVERS], Missing value, entry %s ignored", raw);
						logDNSMess(logBuff, 1);
					}
				}
				//cfig.mxCount[1] = cfig.mxCount[0];
			}
		}
*/
/*
		if (openSection("CHILD-ZONES", 1, &ss))
		{
			int i = 0;

			while (readSection(raw, &ss))
			{
				if (i < 32)
				{
					mySplit(name, value, raw, '=');
					if (name[0] && value[0])
					{
						int j = 0;
						for (; j < 32 && cfig.dnsRoutes[j].zone[0]; j++)
						{
							if (!strcasecmp(cfig.dnsRoutes[j].zone, name))
							{
								sprintf(logBuff, "Section [CHILD-ZONES], Duplicate Entry for Child Zone %s ignored", raw);
								logDNSMess(logBuff, 1);
								break;
							}
						}

						if (j < 32 && !cfig.dnsRoutes[j].zone[0])
						{
							if (name[0] && chkQu(name) && value[0])
							{
								char *value1 = strchr(value, ',');
								if (value1)
								{
									*value1 = 0;
									value1++;

									MYDWORD ip = my_inet_addr(myTrim(value, value));
									MYDWORD ip1 = my_inet_addr(myTrim(value1, value1));

									if (isIP(value) && ip && isIP(value1) && ip1)
									{
										strcpy(cfig.dnsRoutes[i].zone, name);
										cfig.dnsRoutes[i].zLen = strlen(cfig.dnsRoutes[i].zone);
										cfig.dnsRoutes[i].dns[0] = ip;
										cfig.dnsRoutes[i].dns[1] = ip1;
										i++;
									}
									else
									{
										sprintf(logBuff, "Section [CHILD-ZONES] Invalid Entry: %s ignored", raw);
										logDNSMess(logBuff, 1);
									}
								}
								else
								{
									MYDWORD ip = my_inet_addr(value);
									if (isIP(value) && ip)
									{
										strcpy(cfig.dnsRoutes[i].zone, name);
										cfig.dnsRoutes[i].zLen = strlen(cfig.dnsRoutes[i].zone);
										cfig.dnsRoutes[i].dns[0] = ip;
										i++;
									}
									else
									{
										sprintf(logBuff, "Section [CHILD-ZONES] Invalid Entry: %s ignored", raw);
										logDNSMess(logBuff, 1);
									}
								}
							}
							else
							{
								sprintf(logBuff, "Section [CHILD-ZONES] Invalid Entry: %s ignored", raw);
								logDNSMess(logBuff, 1);
							}
						}
					}
					else
					{
						sprintf(logBuff, "Section [CHILD-ZONES], Missing value, entry %s ignored", raw);
						logDNSMess(logBuff, 1);
					}
				}
			}
		}
*/
/*
		if (f = openSection("WILD-HOSTS", 1, &ss))
		{
			int i = 0;

			while (readSection(raw, f))
			{
				if (i < 32)
				{
					mySplit(name, value, raw, '=');
					if (name[0] && value[0])
					{
						if (chkQu(name) && isIP(value))
						{
							MYDWORD ip = my_inet_addr(value);
							strcpy(cfig.wildHosts[i].wildcard, name);
							myLower(cfig.wildHosts[i].wildcard);
							cfig.wildHosts[i].ip = ip;
							i++;
						}
						else
						{
							sprintf(logBuff, "Section [WILD-HOSTS] Invalid Entry: %s ignored", raw);
							logDNSMess(logBuff, 1);
						}
					}
					else
					{
						sprintf(logBuff, "Section [WILD-HOSTS], Missing value, entry %s ignored", raw);
						logDNSMess(logBuff, 1);
					}
				}
			}
		}
*/
		GetDnsWildHosts();

		cfig.serial1 = static_cast<unsigned int>(t);
		cfig.serial2 = static_cast<unsigned int>(t);
		cfig.expireTime = INT_MAX;

		if (dhcpService)
		{
			if (cfig.replication == 1)
				getSecondary();
			else if (cfig.replication == 2)
				checkZone(0);
		}
		else if (cfig.replication == 2)
		{
			checkZone(0);
//			_beginthread(checkZone, 0, (void*)&cfig);
		}
	}

	if (dhcpService)
	{
		if (cfig.lease >= INT_MAX)
			sprintf(logBuff, "Max Lease: Infinity");
		else
			sprintf(logBuff, "Max Lease: %u (sec)", cfig.lease);

		logDHCPMess(logBuff, 1);
	}

	if (cfig.replication == 1)
		sprintf(logBuff, "Server Name: %s (Primary)", cfig.servername);
	else if (cfig.replication == 2)
		sprintf(logBuff, "Server Name: %s (Secondary)", cfig.servername);
	else
		sprintf(logBuff, "Server Name: %s", cfig.servername);

	logDNSMess(logBuff, 1);

	if (dnsService)
	{
		if (cfig.authorized)
			sprintf(logBuff, "Authority for Zone: %s (%s)", cfig.zone, cfig.authority);
		else
			sprintf(logBuff, "Domain Name: %s", cfig.zone);

		logDNSMess(logBuff, 1);

		if (cfig.replication)
		{
			sprintf(logBuff, "Refresh: %u (sec)", cfig.refresh);
			logDNSMess(logBuff, 1);
			sprintf(logBuff, "Retry: %u (sec)", cfig.retry);
			logDNSMess(logBuff, 1);

			if (cfig.expire == UINT_MAX)
				sprintf(logBuff, "Expire: Infinity");
			else
				sprintf(logBuff, "Expire: %u (sec)", cfig.expire);

			logDNSMess(logBuff, 1);
			sprintf(logBuff, "Min: %u (sec)", cfig.minimum);
			logDNSMess(logBuff, 1);
		}

		if (cfig.lease >= INT_MAX)
			sprintf(logBuff, "Default Host Expiry: Infinity");
		else
			sprintf(logBuff, "Default Host Expiry: %u (sec)", cfig.lease);

		logDNSMess(logBuff, 1);

		for (int i = 0; i < 32 && cfig.dnsRoutes[i].dns[0]; i++)
		{
			char temp[256];

			if (!cfig.dnsRoutes[i].dns[1])
				sprintf(logBuff, "Child DNS Server: %s for Zone %s", IP2String(tempbuff, cfig.dnsRoutes[i].dns[0]), cfig.dnsRoutes[i].zone);
			else
				sprintf(logBuff, "Child DNS Servers: %s, %s for Zone %s", IP2String(temp, cfig.dnsRoutes[i].dns[0]), IP2String(tempbuff, cfig.dnsRoutes[i].dns[1]), cfig.dnsRoutes[i].zone);

			logDNSMess(logBuff, 1);
		}

		for (int i = 0; i < MAX_SERVERS && network.dns[i]; i++)
		{
			sprintf(logBuff, "Forwarding DNS Server: %s", IP2String(tempbuff, network.dns[i]));
			logDNSMess(logBuff, 1);
		}

		for (int i = 0; i <= MAX_RANGES && cfig.dnsRanges[i].rangeStart; i++)
		{
			char *logPtr = logBuff;
			logPtr += sprintf(logPtr, "%s", "DNS Service Permitted Hosts: ");
			logPtr += sprintf(logPtr, "%s-", IP2String(tempbuff, htonl(cfig.dnsRanges[i].rangeStart)));
			logPtr += sprintf(logPtr, "%s", IP2String(tempbuff, htonl(cfig.dnsRanges[i].rangeEnd)));
			logDNSMess(logBuff, 1);
		}
	}
	else
	{
		sprintf(logBuff, "Domain Name: %s", cfig.zone);
		logDNSMess(logBuff, 1);
	}

	if (!verbatim)
	{
		if (dnsService)
		{
			if (cfig.dnsLogLevel > 1)
				sprintf(logBuff, "DNS Logging: All");
			else if (cfig.dnsLogLevel)
				sprintf(logBuff, "DNS Logging: Errors");
			else
				sprintf(logBuff, "DNS Logging: None");

			logMess(logBuff, 1);
		}

		if (dhcpService)
		{
			if (cfig.dhcpLogLevel > 1)
				sprintf(logBuff, "DHCP Logging: All");
			else if (cfig.dhcpLogLevel)
				sprintf(logBuff, "DHCP Logging: Errors");
			else
				sprintf(logBuff, "DHCP Logging: None");

			logMess(logBuff, 1);
		}
	}

//	sprintf(logBuff, "Detecting Static Interfaces..");
//	logMess(logBuff, 1);

	do
	{
		memset(&newNetwork, 0, sizeof(data1));
		getInterfaces(&newNetwork);

		if (dnsService)
		{
			char localhost[] = "localhost";
			addToCache(0, localhost, my_inet_addr("127.0.0.1"), INT_MAX, LOCALHOST_A, LOCALHOST_PTR, INT_MAX);
			addToCache(1, localhost, my_inet_addr("127.0.0.1"), INT_MAX, LOCALHOST_A, LOCALHOST_PTR, INT_MAX);

			for (int i = 0; cfig.replication != 2 && i < MAX_SERVERS && newNetwork.allServers[i]; i++)
			{
				if (cfig.authorized && makeLocal(newNetwork.allServers[i]))
				{
					addToCache(0, cfig.servername, newNetwork.allServers[i], INT_MAX, SERVER_A, SERVER_PTR_AUTH, INT_MAX);
					addToCache(1, cfig.servername, newNetwork.allServers[i], INT_MAX, SERVER_A, SERVER_PTR_AUTH, INT_MAX);
				}
				else
				{
					addToCache(0, cfig.servername, newNetwork.allServers[i], INT_MAX, SERVER_A, SERVER_PTR_NAUTH, INT_MAX);
					addToCache(1, cfig.servername, newNetwork.allServers[i], INT_MAX, SERVER_A, SERVER_PTR_NAUTH, INT_MAX);
				}
			}
		}

		bool ifSpecified = false;
		bool bindfailed = false;
/*
		if (openSection("LISTEN-ON", 1, &ss))
		{
			//char *name = myGetToken(iniStr, 0)
			int i = 0;

			while (readSection(raw, &ss))
			{
				if(i < MAX_SERVERS)
				{
					ifSpecified = true;
					MYDWORD addr = my_inet_addr(raw);

					if (isIP(raw) && addr)
					{
						for (MYBYTE m = 0; ; m++)
						{
							if (m >= MAX_SERVERS || !newNetwork.staticServers[m])
							{
								if (findServer(newNetwork.allServers, addr))
								{
									sprintf(logBuff, "Warning: Section [LISTEN-ON], Interface %s is not Static, ignored", raw);
									logMess(logBuff, 1);
								}
								else
								{
									bindfailed = true;
									sprintf(logBuff, "Warning: Section [LISTEN-ON], Interface %s not available, ignored", raw);
									logMess(logBuff, 1);
								}
								break;
							}
							else if (newNetwork.staticServers[m] == addr)
							{
								for (MYBYTE n = 0; n < MAX_SERVERS; n++)
								{
									if (newNetwork.listenServers[n] == addr)
										break;
									else if (!newNetwork.listenServers[n])
									{
										newNetwork.listenServers[n] = newNetwork.staticServers[m];
										newNetwork.listenMasks[n] = newNetwork.staticMasks[m];
										break;
									}
								}
								break;
							}
						}
					}
					else
					{
						sprintf(logBuff, "Warning: Section [LISTEN-ON], Invalid Interface Address %s, ignored", raw);
						logMess(logBuff, 1);
					}
				}
			}
		}
*/
		if (!ifSpecified)
		{
			MYBYTE k = 0;

			for (MYBYTE m = 0; m < MAX_SERVERS && newNetwork.allServers[m]; m++)
			{
				for (MYBYTE n = 0; n < MAX_SERVERS; n++)
				{
					if (newNetwork.allServers[m] == newNetwork.staticServers[n])
					{
						BOOL bSkipIp = FALSE;

						if (pIpNoListenOn != NULL)
						{
							DWORD *p = pIpNoListenOn;
							while (*p != 0)
							{
								DWORD pl = htonl(*p);
								if (newNetwork.allServers[m] == pl)
								{
									bSkipIp = TRUE;
									break;
								}

								p++;
							}
						}

						if (!bSkipIp)
						{
							newNetwork.listenServers[k] = newNetwork.staticServers[n];
							newNetwork.listenMasks[k] = newNetwork.staticMasks[n];
							k++;
							break;
						}
					}
					else if (!newNetwork.staticServers[n])
					{
						sprintf(logBuff, "Warning: Interface %s is not Static, not used", IP2String(tempbuff, newNetwork.allServers[m]));
						logMess(logBuff, 2);
						break;
					}
				}
			}
		}

		if (dhcpService)
		{
			int i = 0;

			for (int j = 0; j < MAX_SERVERS && newNetwork.listenServers[j]; j++)
			{
				int k = 0;

				for (; k < MAX_SERVERS && network.dhcpConn[i].loaded; k++)
				{
					if (network.dhcpConn[k].ready && network.dhcpConn[k].server == newNetwork.listenServers[j])
						break;
				}

				if (network.dhcpConn[k].ready && network.dhcpConn[k].server == newNetwork.listenServers[j])
				{
					memcpy(&(newNetwork.dhcpConn[i]), &(network.dhcpConn[k]), sizeof(DhcpConnType));

					if (newNetwork.maxFD < newNetwork.dhcpConn[i].sock)
						newNetwork.maxFD = newNetwork.dhcpConn[i].sock;

					network.dhcpConn[k].ready = false;
					i++;
					continue;
				}
				else
				{
					newNetwork.dhcpConn[i].sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

					if (newNetwork.dhcpConn[i].sock == INVALID_SOCKET)
					{
						bindfailed = true;
						sprintf(logBuff, "Failed to Create Socket");
						logDNSMess(logBuff, 1);
						continue;
					}

					//printf("Socket %u\n", newNetwork.dhcpConn[i].sock);

					newNetwork.dhcpConn[i].addr.sin_family = AF_INET;
					newNetwork.dhcpConn[i].addr.sin_addr.s_addr = newNetwork.listenServers[j];
					newNetwork.dhcpConn[i].addr.sin_port = htons(IPPORT_DHCPS);

					newNetwork.dhcpConn[i].broadCastVal = TRUE;
					newNetwork.dhcpConn[i].broadCastSize = sizeof(newNetwork.dhcpConn[i].broadCastVal);
					setsockopt(newNetwork.dhcpConn[i].sock, SOL_SOCKET, SO_BROADCAST, (char*)(&newNetwork.dhcpConn[i].broadCastVal), newNetwork.dhcpConn[i].broadCastSize);

					int nRet = ::bind(newNetwork.dhcpConn[i].sock,
									(sockaddr*)&newNetwork.dhcpConn[i].addr,
									sizeof(struct sockaddr_in)
								   );

					if (nRet == SOCKET_ERROR)
					{
						bindfailed = true;
						closesocket(newNetwork.dhcpConn[i].sock);
						sprintf(logBuff, "Warning: %s UDP Port 53 already in use", IP2String(tempbuff, newNetwork.listenServers[j]));
						logDNSMess(logBuff, 1);
						continue;
					}

					newNetwork.dhcpConn[i].loaded = true;
					newNetwork.dhcpConn[i].ready = true;

					if (newNetwork.maxFD < newNetwork.dhcpConn[i].sock)
						newNetwork.maxFD = newNetwork.dhcpConn[i].sock;

					newNetwork.dhcpConn[i].server = newNetwork.listenServers[j];
					newNetwork.dhcpConn[i].port = IPPORT_DHCPS;

					i++;
				}
			}

			if (cfig.replication)
			{
				if (network.dhcpReplConn.ready)
				{
					memcpy(&(newNetwork.dhcpReplConn), &(network.dhcpReplConn), sizeof(ConnType));

					if (newNetwork.maxFD < newNetwork.dhcpReplConn.sock)
						newNetwork.maxFD = newNetwork.dhcpReplConn.sock;

					network.dhcpReplConn.ready = false;
				}
				else
				{
					newNetwork.dhcpReplConn.sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

					if (newNetwork.dhcpReplConn.sock == INVALID_SOCKET)
					{
						bindfailed = true;
						sprintf(logBuff, "Failed to Create DHCP Replication Socket");
						logDHCPMess(logBuff, 1);
					}
					else
					{
						//printf("Socket %u\n", newNetwork.dhcpReplConn.sock);

						if (cfig.replication == 1)
							newNetwork.dhcpReplConn.server = cfig.zoneServers[0];
						else
							newNetwork.dhcpReplConn.server = cfig.zoneServers[1];

						newNetwork.dhcpReplConn.addr.sin_family = AF_INET;
						newNetwork.dhcpReplConn.addr.sin_addr.s_addr = newNetwork.dhcpReplConn.server;
						newNetwork.dhcpReplConn.addr.sin_port = 0;

						int nRet = ::bind(newNetwork.dhcpReplConn.sock, (sockaddr*)&newNetwork.dhcpReplConn.addr, sizeof(struct sockaddr_in));

						if (nRet == SOCKET_ERROR)
						{
							bindfailed = true;
							newNetwork.dhcpReplConn.ready = false;
							sprintf(logBuff, "DHCP Replication Server, Bind Failed");
							logDHCPMess(logBuff, 1);
						}
						else
						{
							newNetwork.dhcpReplConn.port = IPPORT_DHCPS;
							newNetwork.dhcpReplConn.loaded = true;
							newNetwork.dhcpReplConn.ready = true;

							if (newNetwork.maxFD < newNetwork.dhcpReplConn.sock)
								newNetwork.maxFD = newNetwork.dhcpReplConn.sock;
						}
					}
				}
			}

#ifdef _DEBUG
			newNetwork.httpConn.port = 6789;
			newNetwork.httpConn.server = inet_addr("127.0.0.1");
			newNetwork.httpConn.loaded = true;

			if (newNetwork.httpConn.loaded)
			{
				if (network.httpConn.ready && network.httpConn.server == newNetwork.httpConn.server)
				{
					memcpy(&(newNetwork.httpConn), &(network.httpConn), sizeof(ConnType));

					if (newNetwork.httpConn.sock > newNetwork.maxFD)
						newNetwork.maxFD = newNetwork.httpConn.sock;

					network.httpConn.ready = false;
				}
				else
				{
					newNetwork.httpConn.sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

					if (newNetwork.httpConn.sock == INVALID_SOCKET)
					{
						bindfailed = true;
						sprintf(logBuff, "Failed to Create Socket");
						logDHCPMess(logBuff, 1);
					}
					else
					{
						//printf("Socket %u\n", newNetwork.httpConn.sock);

						newNetwork.httpConn.addr.sin_family = AF_INET;
						newNetwork.httpConn.addr.sin_addr.s_addr = newNetwork.httpConn.server;
						newNetwork.httpConn.addr.sin_port = htons(newNetwork.httpConn.port);

						int nRet = ::bind(newNetwork.httpConn.sock,
										(sockaddr*)&newNetwork.httpConn.addr,
										sizeof(struct sockaddr_in));

						if (nRet == SOCKET_ERROR)
						{
							bindfailed = true;
							sprintf(logBuff, "Http Interface %s TCP Port %u not available", IP2String(tempbuff, newNetwork.httpConn.server), newNetwork.httpConn.port);
							logDHCPMess(logBuff, 1);
							closesocket(newNetwork.httpConn.sock);
						}
						else
						{
							nRet = listen(newNetwork.httpConn.sock, SOMAXCONN);

							if (nRet == SOCKET_ERROR)
							{
								bindfailed = true;
								sprintf(logBuff, "%s TCP Port %u Error on Listen", IP2String(tempbuff, newNetwork.httpConn.server), newNetwork.httpConn.port);
								logDHCPMess(logBuff, 1);
								closesocket(newNetwork.httpConn.sock);
							}
							else
							{
								newNetwork.httpConn.loaded = true;
								newNetwork.httpConn.ready = true;

								if (newNetwork.httpConn.sock > newNetwork.maxFD)
									newNetwork.maxFD = newNetwork.httpConn.sock;
							}
						}
					}
				}
			}
#endif
		}

		if (dnsService)
		{
			int i = 0;

			for (int j = 0; j < MAX_SERVERS && newNetwork.listenServers[j]; j++)
			{
				int k = 0;

				for (; k < MAX_SERVERS && network.dnsUdpConn[i].loaded; k++)
				{
					if (network.dnsUdpConn[k].ready && network.dnsUdpConn[k].server == newNetwork.listenServers[j])
						break;
				}

				if (network.dnsUdpConn[k].ready && network.dnsUdpConn[k].server == newNetwork.listenServers[j])
				{
					memcpy(&(newNetwork.dnsUdpConn[i]), &(network.dnsUdpConn[k]), sizeof(ConnType));

					if (newNetwork.maxFD < newNetwork.dnsUdpConn[i].sock)
						newNetwork.maxFD = newNetwork.dnsUdpConn[i].sock;

					network.dnsUdpConn[k].ready = false;
					i++;
					continue;
				}
				else
				{
					newNetwork.dnsUdpConn[i].sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

					if (newNetwork.dnsUdpConn[i].sock == INVALID_SOCKET)
					{
						bindfailed = true;
						sprintf(logBuff, "Failed to Create Socket");
						logDNSMess(logBuff, 1);
						continue;
					}

					//printf("Socket %u\n", newNetwork.dnsUdpConn[i].sock);

					newNetwork.dnsUdpConn[i].addr.sin_family = AF_INET;
					newNetwork.dnsUdpConn[i].addr.sin_addr.s_addr = newNetwork.listenServers[j];
					newNetwork.dnsUdpConn[i].addr.sin_port = htons(DnsServerPort);

					int nRet = ::bind(newNetwork.dnsUdpConn[i].sock,
									(sockaddr*)&newNetwork.dnsUdpConn[i].addr,
									sizeof(struct sockaddr_in)
								   );

					if (nRet == SOCKET_ERROR)
					{
						bindfailed = true;
						closesocket(newNetwork.dnsUdpConn[i].sock);
						sprintf(logBuff, "Warning: %s UDP Port 53 already in use", IP2String(tempbuff, newNetwork.listenServers[j]));
						logDNSMess(logBuff, 1);
						continue;
					}

					newNetwork.dnsUdpConn[i].loaded = true;
					newNetwork.dnsUdpConn[i].ready = true;

					if (newNetwork.maxFD < newNetwork.dnsUdpConn[i].sock)
						newNetwork.maxFD = newNetwork.dnsUdpConn[i].sock;

					newNetwork.dnsUdpConn[i].server = newNetwork.listenServers[j];
					newNetwork.dnsUdpConn[i].port = DnsServerPort;

					if (!cfig.nsIP)
						cfig.nsIP = newNetwork.listenServers[j];

					i++;
				}
			}

			if (network.forwConn.ready)
			{
				memcpy(&(newNetwork.forwConn), &(network.forwConn), sizeof(ConnType));

				if (newNetwork.maxFD < newNetwork.forwConn.sock)
					newNetwork.maxFD = newNetwork.forwConn.sock;

				network.forwConn.ready = false;
			}
			else
			{
				newNetwork.forwConn.sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

				if (newNetwork.forwConn.sock == INVALID_SOCKET)
				{
					bindfailed = true;
					sprintf(logBuff, "Failed to Create Socket");
					logDNSMess(logBuff, 1);
				}
				else
				{
					newNetwork.forwConn.addr.sin_family = AF_INET;
					newNetwork.forwConn.server = network.dns[0];
					newNetwork.forwConn.port = DnsServerPort;
					//bind(newNetwork.forwConn.sock, (sockaddr*)&newNetwork.forwConn.addr, sizeof(struct sockaddr_in));

					newNetwork.forwConn.loaded = true;
					newNetwork.forwConn.ready = true;

					if (newNetwork.maxFD < newNetwork.forwConn.sock)
						newNetwork.maxFD = newNetwork.forwConn.sock;
				}
			}

			i = 0;

			for (int j = 0; j < MAX_SERVERS && newNetwork.listenServers[j]; j++)
			{
				int k = 0;

				for (; k < MAX_SERVERS && network.dnsTcpConn[i].loaded; k++)
				{
					if (network.dnsTcpConn[k].ready && network.dnsTcpConn[k].server == newNetwork.listenServers[j])
						break;
				}

				if (network.dnsTcpConn[k].ready && network.dnsTcpConn[k].server == newNetwork.listenServers[j])
				{
					memcpy(&(newNetwork.dnsTcpConn[i]), &(network.dnsTcpConn[k]), sizeof(ConnType));

					if (newNetwork.maxFD < newNetwork.dnsTcpConn[i].sock)
						newNetwork.maxFD = newNetwork.dnsTcpConn[i].sock;

					network.dnsTcpConn[k].ready = false;
					i++;
					continue;
				}
				else
				{
					newNetwork.dnsTcpConn[i].sock = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP);

					if (newNetwork.dnsTcpConn[i].sock == INVALID_SOCKET)
					{
						bindfailed = true;
						sprintf(logBuff, "Failed to Create Socket");
						logDNSMess(logBuff, 1);
					}
					else
					{
						//printf("Socket %u\n", newNetwork.dnsTcpConn[i].sock);
						newNetwork.dnsTcpConn[i].addr.sin_family = AF_INET;
						newNetwork.dnsTcpConn[i].addr.sin_addr.s_addr = newNetwork.listenServers[j];
						newNetwork.dnsTcpConn[i].addr.sin_port = htons(DnsServerPort);

						int nRet = ::bind(newNetwork.dnsTcpConn[i].sock,
										(sockaddr*)&newNetwork.dnsTcpConn[i].addr,
										sizeof(struct sockaddr_in));

						if (nRet == SOCKET_ERROR)
						{
							bindfailed = true;
							closesocket(newNetwork.dnsTcpConn[i].sock);
							sprintf(logBuff, "Warning: %s TCP Port 53 already in use", IP2String(tempbuff, newNetwork.listenServers[j]));
							logDNSMess(logBuff, 1);
						}
						else
						{
							nRet = listen(newNetwork.dnsTcpConn[i].sock, SOMAXCONN);

							if (nRet == SOCKET_ERROR)
							{
								closesocket(newNetwork.dnsTcpConn[i].sock);
								sprintf(logBuff, "TCP Port 53 Error on Listen");
								logDNSMess(logBuff, 1);
							}
							else
							{
								newNetwork.dnsTcpConn[i].server = newNetwork.listenServers[j];
								newNetwork.dnsTcpConn[i].port = DnsServerPort;

								newNetwork.dnsTcpConn[i].loaded = true;
								newNetwork.dnsTcpConn[i].ready = true;

								if (newNetwork.maxFD < newNetwork.dnsTcpConn[i].sock)
									newNetwork.maxFD = newNetwork.dnsTcpConn[i].sock;

								i++;
							}
						}
					}
				}
			}
		}

		newNetwork.maxFD++;

		if (dhcpService)
		{
			for (MYBYTE m = 0; m < MAX_SERVERS && newNetwork.allServers[m]; m++)
				setLeaseExpiry(newNetwork.allServers[m], INT_MAX);

			for (MYBYTE m = 0; m < MAX_SERVERS && newNetwork.dns[m]; m++)
				setLeaseExpiry(newNetwork.dns[m], INT_MAX);
		}

		if (bindfailed)
			cfig.failureCount++;
		else
			cfig.failureCount = 0;

		closeConn();
		memcpy(&network, &newNetwork, sizeof(data1));

		//printf("%i %i %i\n", network.dhcpConn[0].ready, network.dnsUdpConn[0].ready, network.dnsTcpConn[0].ready);

		if ((dhcpService && !network.dhcpConn[0].ready) || (dnsService && !(network.dnsUdpConn[0].ready && network.dnsTcpConn[0].ready)))
		{
			sprintf(logBuff, "No Static Interface ready, Waiting...");
			logMess(logBuff, 1);
			Sleep(500);

			*nostatic = *nostatic + 1;

			if (*nostatic < 2)
			{
				continue;
			}
		}
/*
		if (dhcpService && network.httpConn.ready)
		{
			sprintf(logBuff, "Lease Status URL: http://%s:%u", IP2String(tempbuff, network.httpConn.server), network.httpConn.port);
			logDHCPMess(logBuff, 1);
			FILE *f = fopen(htmFile, "wt");

			if (f)
			{
				fprintf(f, "<html><head><meta http-equiv=\"refresh\" content=\"0;url=http://%s:%u\"</head></html>", IP2String(tempbuff, network.httpConn.server), network.httpConn.port);
				fclose(f);
			}
		}
		else
		{
			FILE *f = fopen(htmFile, "wt");

			if (f)
			{
				fprintf(f, "<html><body><h2>DHCP/HTTP Service is not running</h2></body></html>");
				fclose(f);
			}
		}
*/
		for (int i = 0; i < MAX_SERVERS && network.staticServers[i]; i++)
		{
			for (MYBYTE j = 0; j < MAX_SERVERS; j++)
			{
				if (network.dhcpConn[j].server == network.staticServers[i] || network.dnsUdpConn[j].server == network.staticServers[i])
				{
					sprintf(logBuff, "Listening On: %s", IP2String(tempbuff, network.staticServers[i]));
					logMess(logBuff, 1);
					break;
				}
			}
		}

		network.ready = true;
// alexbegin
//	} while (detectChange());

		break;

	} while (true);

// alexend

	return;
}

bool CDualServer::detectChange()
{
	network.ready = true;

	MYDWORD eventWait = UINT_MAX;

	float f2 = 2.0;
	int c = (int)cfig.failureCount;

	if (cfig.failureCount)
		eventWait = 10000 * (unsigned int)pow(f2, c);

	OVERLAPPED overlap;
	MYDWORD ret;
	HANDLE hand = NULL;
	overlap.hEvent = WSACreateEvent();

	ret = NotifyAddrChange(&hand, &overlap);

	if (ret != NO_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			printf("NotifyAddrChange error...%d\n", WSAGetLastError());
			return true;
		}
	}

	if ( WaitForSingleObject(overlap.hEvent, eventWait) == WAIT_OBJECT_0 )
		WSACloseEvent(overlap.hEvent);

	network.ready = false;

	while (network.busy)
		Sleep(1000);

	if (cfig.failureCount)
	{
		sprintf(logBuff, "Retrying failed Listening Interfaces..");
		logDHCPMess(logBuff, 1);
	}
	else
	{
		sprintf(logBuff, "Network changed, re-detecting Static Interfaces..");
		logMess(logBuff, 1);
	}

	return true;
}

void CDualServer::getInterfaces(data1 *network)
{
	// alexbegin

	char iniStr[MAX_INI_STRING];
	int len = 0;

	if (pAlexDnsServers != NULL)
	{
		strncpy_s(iniStr, MAX_INI_STRING, pAlexDnsServers, _TRUNCATE);

		for (int j = 0; j < MAX_INI_STRING; j++)
		{
			if (iniStr[j] == 0)
				break;

			if (iniStr[j] == ' ')
				iniStr[j] = 0;
		}

		len = strlen(pAlexDnsServers);
	}

	if (len < MAX_INI_STRING)
	{
		memset(iniStr + len, 0, sizeof(iniStr) - len);
	}

	// alexend

	memset(network, 0, sizeof(data1));

	SOCKET sd = WSASocket(PF_INET, SOCK_DGRAM, 0, 0, 0, 0);

	if (sd == INVALID_SOCKET)
		return;

	INTERFACE_INFO InterfaceList[MAX_SERVERS];
	unsigned long nBytesReturned;

	if (WSAIoctl(sd, SIO_GET_INTERFACE_LIST, 0, 0, &InterfaceList,
	             sizeof(InterfaceList), &nBytesReturned, 0, 0) == SOCKET_ERROR)
		return ;

	int nNumInterfaces = nBytesReturned / sizeof(INTERFACE_INFO);

	int u = 0;
	for (int i = 0; i < nNumInterfaces; ++i)
	{
		sockaddr_in *pAddress = (sockaddr_in*)&(InterfaceList[i].iiAddress);
		u_long nFlags = InterfaceList[i].iiFlags;
		if (!((nFlags & IFF_POINTTOPOINT)))
//		if (!((nFlags & IFF_POINTTOPOINT) || (nFlags & IFF_LOOPBACK)))
		{
			//printf("%s\n", IP2String(tempbuff, pAddress->sin_addr.S_un.S_addr));
			addServer(network->allServers, pAddress->sin_addr.s_addr);

			if (nFlags & IFF_LOOPBACK)
			{
				sockaddr_in *pMask = (sockaddr_in*)&(InterfaceList[i].iiNetmask);
				network->staticServers[u] = pAddress->sin_addr.s_addr;
				network->staticMasks[u] = pMask->sin_addr.s_addr;
				u++;
			}
		}
	}

	closesocket(sd);

	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter;

	pAdapterInfo = (IP_ADAPTER_INFO*) calloc(1, sizeof(IP_ADAPTER_INFO));
	DWORD ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO*)calloc(1, ulOutBufLen);
	}

	network->staticServers[0] = my_inet_addr("127.0.0.1");
	network->staticMasks[0] = my_inet_addr("255.0.0.0");

	if ((GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
	{
		pAdapter = pAdapterInfo;
		while (pAdapter)
		{
			if (!pAdapter->DhcpEnabled)
			{
				IP_ADDR_STRING *sList = &pAdapter->IpAddressList;
				while (sList)
				{
					DWORD iaddr = my_inet_addr(sList->IpAddress.String);

					if (iaddr)
					{
						for (MYBYTE k = 0; k < MAX_SERVERS; k++)
						{
							if (network->staticServers[k] == iaddr)
								break;
							else if (!network->staticServers[k])
							{
								network->staticServers[k] = iaddr;
								network->staticMasks[k] = my_inet_addr(sList->IpMask.String);
								break;
							}
						}
					}
					sList = sList->Next;
				}

//				IP_ADDR_STRING *rList = &pAdapter->GatewayList;
//				while (rList)
//				{
//					DWORD trouter = my_inet_addr(rList->IpAddress.String);
//					addServer(cfig.routers, trouter);
//					rList = rList->Next;
//				}
			}
			pAdapter = pAdapter->Next;
		}
		free(pAdapterInfo);
	}

	MYDWORD dservers[MAX_SERVERS];

	for (int i = 0; i < MAX_SERVERS; i++)
	{
		network->dns[i] = 0;
		dservers[i] = 0;
	}

//	if (getSection("DNS-SERVERS", iniStr, 1, &ss))
	if (iniStr[0] != 0)
	{
		char *iniStrPtr = myCleanToken(iniStr, 0);
		for (int i = 0; i < MAX_SERVERS && iniStrPtr[0]; iniStrPtr = myCleanToken(iniStrPtr, 1))
		{
			if (isIP(iniStrPtr))
			{
				DWORD addr = my_inet_addr(iniStrPtr);
				if (addServer(dservers, addr))
					i++;
			}
		}
	}

	FIXED_INFO *FixedInfo;
	IP_ADDR_STRING *pIPAddr;

	FixedInfo = (FIXED_INFO*)GlobalAlloc(GPTR, sizeof(FIXED_INFO));
	ulOutBufLen = sizeof(FIXED_INFO);

	if (ERROR_BUFFER_OVERFLOW == GetNetworkParams(FixedInfo, &ulOutBufLen))
	{
		GlobalFree(FixedInfo);
		FixedInfo = (FIXED_INFO*)GlobalAlloc(GPTR, ulOutBufLen);
	}

	if (!GetNetworkParams(FixedInfo, &ulOutBufLen))
	{
		if (!cfig.servername[0])
			strcpy(cfig.servername, FixedInfo->HostName);

		//printf("d=%u=%s", strlen(FixedInfo->DomainName), FixedInfo->DomainName);

		if (!cfig.zone[0])
		{
			strcpy(cfig.zone, FixedInfo->DomainName);
			cfig.zLen = strlen(cfig.zone);
		}

		if (!cfig.zone[0] || cfig.zone[0] == 32)
		{
			strcpy(cfig.zone, "workgroup");
			cfig.zLen = strlen(cfig.zone);
		}

		if (!dservers[0])
		{
			pIPAddr = &FixedInfo->DnsServerList;
			while (pIPAddr)
			{
				DWORD addr = my_inet_addr(pIPAddr->IpAddress.String);

				addServer(dservers, addr);
				pIPAddr = pIPAddr->Next;
			}
		}
		GlobalFree(FixedInfo);
	}

	for (int i = 0; i < MAX_SERVERS && dservers[i]; i++)
	{
		if (dnsService)
		{
			if (findServer(network->allServers, dservers[i]))
				continue;

			addServer(network->dns, dservers[i]);
		}
		else
			addServer(network->dns, dservers[i]);
	}
	return;
}

void CDualServer::updateStateFile(void *lpParam)
{
	data8 *dhcpData = (data8*)lpParam;

	time_t t;
	time(&t);

	char msg[MAX_PATH], sz[MAX_PATH];

	addrip_v4 ip(dhcpData->ip);
	sprintf_s(sz, MAX_PATH, "Hostname=%s IP=%s", dhcpData->hostname, ip.to_string().c_str());  
//	sprintf_s(sz, MAX_PATH, "Hostname=%s IP=%d.%d.%d.%d", dhcpData->hostname, OCTET4(dhcpData->ip), OCTET3(dhcpData->ip), OCTET2(dhcpData->ip), OCTET1(dhcpData->ip));  

	if (dhcpData->expiry <= t)
	{
		sprintf_s(msg, MAX_PATH, "DHCP release: %s", sz);
	}
	else
	{
		sprintf_s(msg, MAX_PATH, "DHCP renew: %s", sz);
	}

	WCHAR wsz[MAX_PATH];
	memset(wsz, 0, sizeof(wsz));
	MultiByteToWideChar(CP_OEMCP, NULL, msg, -1, wsz, MAX_PATH);

	memset(msg, 0, sizeof(msg));
	WideCharToMultiByte(CP_ACP, NULL, wsz, -1, msg, MAX_PATH, NULL, NULL);

	if (dhcpData->dhcpInd)
	{
		// Удаление аренды

		FILE *f = NULL;
		_tfopen_s(&f, leaFile, _T("rb+"));

		if (f)
		{
			if (fseek(f, (dhcpData->dhcpInd - 1)*sizeof(data8), SEEK_SET) >= 0)
				fwrite(dhcpData, sizeof(data8), 1, f);

			fclose(f);
		}
	}
	else
	{
		// Новая аренда

		dhcpData->dhcpInd = cfig.dhcpIndex;

		FILE *f = NULL;
		_tfopen_s(&f, leaFile, _T("ab"));

		if (f)
		{
			fwrite(dhcpData, sizeof(data8), 1, f);
			fclose(f);
		}
	}

	free(dhcpData);

	logMess(msg, 3);
}

MYWORD CDualServer::gdmess(data9 *req, MYBYTE sockInd)
{
	memset(req, 0, sizeof(data9));
	req->sockInd = sockInd;
	req->sockLen = sizeof(req->remote);
	errno = 0;

	req->bytes = recvfrom(network.dhcpConn[req->sockInd].sock,
	                      req->raw,
	                      sizeof(req->raw),
	                      0,
	                      (sockaddr*)&req->remote,
	                      &req->sockLen);

	//printf("start bytes=%u\n", req->bytes);

	errno = WSAGetLastError();

	//printf("errno=%u\n", errno);

	if (errno || req->bytes <= 0 || req->dhcpp.header.bp_op != BOOTP_REQUEST)
		return 0;

	data3 *op;
	MYBYTE *raw = req->dhcpp.vend_data;
	MYBYTE *rawEnd = raw + (req->bytes - sizeof(dhcp_header));

	for (; raw < rawEnd && *raw != DHCP_OPTION_END;)
	{
		op = (data3*)raw;
		//printf("OpCode=%u,MessType=%u\n", op->opt_code, op->value[0]);

		switch (op->opt_code)
		{
			case DHCP_OPTION_PAD:
				raw++;
				continue;

			case DHCP_OPTION_PARAMREQLIST:
				for (int ix = 0; ix < op->size; ix++)
					req->paramreqlist[op->value[ix]] = 1;
				break;

			case DHCP_OPTION_MESSAGETYPE:
				req->req_type = op->value[0];
				break;

			case DHCP_OPTION_SERVERID:
				req->server = fIP(op->value);
				break;

			case DHCP_OPTION_IPADDRLEASE:
				req->lease = fULong(op->value);
				break;

			case DHCP_OPTION_MAXDHCPMSGSIZE:
				req->messsize = fUShort(op->value);
				break;

			case DHCP_OPTION_REQUESTEDIPADDR:
				req->reqIP = fIP(op->value);
				break;

			case DHCP_OPTION_HOSTNAME:
				if (op->size && strcasecmp((char*)op->value, "(none)") && strcasecmp((char*)op->value, cfig.servername))
				{
					memcpy(req->hostname, op->value, op->size);
					req->hostname[op->size] = 0;

					if (char *ptr = strchr(req->hostname, '.'))
						*ptr = 0;
				}
				break;

			case DHCP_OPTION_VENDORCLASSID:
				memcpy(req->vendClass, op->value, op->size);
				req->vendClassSize = op->size;
				break;

			case DHCP_OPTION_USERCLASS:
				memcpy(req->userClass, op->value, op->size);
				req->userClassSize = op->size;
				break;

			case DHCP_OPTION_RELAYAGENTINFO:
				memcpy(&req->agentOption, op, op->size + 2);
				break;

			case DHCP_OPTION_CLIENTID:
				memcpy(&req->clientId, op, op->size + 2);
				break;

			case DHCP_OPTION_SERIAL:
				req->serial = fULong(op->value);
				break;
		}
		raw += 2;
		raw += op->size;
	}

	req->vp = req->dhcpp.vend_data;
	memset(req->vp, 0, sizeof(dhcp_packet) - sizeof(dhcp_header));
	//printf("end bytes=%u\n", req->bytes);
	return 1;
}

void CDualServer::logMess(char *logBuff, MYBYTE logLevel)
{
    if (logBuff == NULL)
    {
        return;
    }

	if (pclog != NULL)
	{
		int level = MAKELONG(MAKEWORD(logLevel, 0), 0);

#ifdef _DEBUG
		(*pclog)(logBuff, level);
#else
		if (level > 2)
		{
			(*pclog)(logBuff, level);
		}
#endif
	}
}

void CDualServer::logDHCPMess(char *logBuff, MYBYTE dhcpLogLevel)
{
#ifdef _DEBUG
	if (logBuff == NULL)
    {
        return;
    }

	if (pclog != NULL)
	{
		int level = MAKELONG(MAKEWORD(dhcpLogLevel, 0), 0);
		(*pclog)(logBuff, level);
	}
#endif
}

void CDualServer::logDNSMess(char *logBuff, MYBYTE dnsLogLevel)
{
#ifdef _DEBUG
	if (logBuff == NULL)
    {
        return;
    }

	if (pclog != NULL)
	{
		int level = MAKELONG(MAKEWORD(dnsLogLevel, 0), 0);
		(*pclog)(logBuff, level);
	}
#endif
}

void CDualServer::logDNSMess(data5 *req, char *logBuff, MYBYTE dnsLogLevel)
{
#ifdef _DEBUG
	if (logBuff == NULL)
    {
        return;
    }

	if (pclog != NULL)
	{
		int level = MAKELONG(MAKEWORD(dnsLogLevel, 0), 0);
		(*pclog)(logBuff, level);
	}
#endif
}

void CDualServer::logTCPMess(data5 *req, char *logBuff, MYBYTE dnsLogLevel)
{
#ifdef _DEBUG
	if (logBuff == NULL)
    {
        return;
    }

	if (pclog != NULL)
	{
		int level = MAKELONG(MAKEWORD(dnsLogLevel, 0), 0);
		(*pclog)(logBuff, level);
	}
#endif
}

void CDualServer::GetDnsForwardingServers()
{
}

void CDualServer::GetDnsAllowedHosts()
{
//	if (getSection("DNS-ALLOWED-HOSTS", iniStr, 1, &ss))
	char name[512];
	char value[512];

    for (int i = 0; i < 32; i++)
    {
        cfig.dnsRanges[i].rangeStart = 0;
        cfig.dnsRanges[i].rangeEnd = 0;
    }

    char iniStr[MAX_INI_STRING];
	int len = 0;

	if (pAlexDnsAllowedHosts != NULL)
	{
		strncpy_s(iniStr, MAX_INI_STRING, pAlexDnsAllowedHosts, _TRUNCATE);

		for (int j = 0; j < MAX_INI_STRING; j++)
		{
			if (iniStr[j] == 0)
				break;

    		if (iniStr[j] == ' ')
	    		iniStr[j] = 0;
		}

		len = strlen(pAlexDnsAllowedHosts);
	}

	if (len < MAX_INI_STRING)
	{
		memset(iniStr + len, 0, sizeof(iniStr) - len);
	}

    if (iniStr[0] != 0)
	{
		char *iniStrPtr = myCleanToken(iniStr, 0);
		for (int i = 0; i < 32 && iniStrPtr[0]; iniStrPtr = myCleanToken(iniStrPtr, 1))
		{
			DWORD rs = 0;
			DWORD re = 0;
			mySplit(name, value, iniStrPtr, '-');

			if (value[0])
			{
				rs = htonl(my_inet_addr(name));
				re = htonl(my_inet_addr(value));
			}
			else
			{
				rs = htonl(my_inet_addr(name));
				re = rs;
			}

			if (rs && rs != INADDR_NONE && re && re != INADDR_NONE && rs <= re)
			{
				cfig.dnsRanges[i].rangeStart = rs;
				cfig.dnsRanges[i].rangeEnd = re;
				i++;
			}
			else
			{
//					sprintf(logBuff, "Section [DNS-ALLOWED-HOSTS] Invalid entry %s in ini file, ignored", iniStrPtr);
//					logMess(logBuff, 1);
			}
		}
	}
}
/*
void CDualServer::GetDnsHostsRecords()
{
    char iniStr[MAX_CONFIG_SECTION];
	char name[512];
	char value[512];

    strncpy_s(iniStr, MAX_CONFIG_SECTION, default_dnsHostsRecords, _TRUNCATE);
	int len = strlen(iniStr);
	if (len < MAX_CONFIG_SECTION)
	{
		memset(iniStr + len, 0, sizeof(iniStr) - len);
	}

    for (int j = 0; j < MAX_CONFIG_SECTION; j++)
    {
	    if (iniStr[j] == 0)
			break;

    	if (iniStr[j] == ' ')
	    	iniStr[j] = 0;
    }

    if (iniStr[0] != 0)
		{
			char *iniStrPtr = myCleanToken(iniStr, 0);
			for (; iniStrPtr[0]; iniStrPtr = myCleanToken(iniStrPtr, 1))
			{
				mySplit(name, value, iniStrPtr, '=');
				if (name[0] && value[0])
				{
					if (chkQu(name) && !isIP(name) && isIP(value))
					{
						DWORD inetAddr = my_inet_addr(value);

						 if (!inetAddr)
						{
							sprintf(logBuff, "Section [HOSTS] Invalid Entry: %s ignored", iniStrPtr);
							logMess(logBuff, 1);
						}
						else if (cfig.authorized && makeLocal(name) && makeLocal(inetAddr))
						{
							addToCache(cacheInd, name, inetAddr, LONG_MAX, STATIC_A_AUTH, STATIC_PTR_AUTH, cfig.serial1);
							setLeaseExpiry(inetAddr, 0);
						}
						else if (cfig.authorized && makeLocal(name))
						{
							addToCache(cacheInd, name, inetAddr, LONG_MAX, STATIC_A_AUTH, STATIC_PTR_NAUTH, cfig.serial1);
							setLeaseExpiry(inetAddr, 0);
						}
						else if (cfig.authorized && makeLocal(inetAddr))
						{
							addToCache(cacheInd, name, inetAddr, LONG_MAX, STATIC_A_NAUTH, STATIC_PTR_AUTH, cfig.serial1);
							setLeaseExpiry(inetAddr, 0);
						}
						else
						{
							addToCache(cacheInd, name, inetAddr, LONG_MAX, STATIC_A_NAUTH, STATIC_PTR_NAUTH, cfig.serial1);
							setLeaseExpiry(inetAddr, 0);
						}
					}
					else
					{
						sprintf(logBuff, "Section [HOSTS] Invalid Entry: %s ignored", iniStrPtr);
						logMess(logBuff, 1);
					}
				}
				else
				{
					sprintf(logBuff, "Section [HOSTS], Missing value, entry %s ignored", iniStrPtr);
					logMess(logBuff, 1);
				}
			}
		}
}
*/

void CDualServer::GetDnsWildHosts()
{
	char name[512];
	char value[512];

    char iniStr[MAX_CONFIG_SECTION];
	int len = 0;

	if (pAlexDnsWildHosts != NULL)
	{
		strncpy_s(iniStr, MAX_CONFIG_SECTION, pAlexDnsWildHosts, _TRUNCATE);

		for (int j = 0; j < MAX_CONFIG_SECTION; j++)
		{
			if (iniStr[j] == 0)
				break;

    		if (iniStr[j] == ' ')
	    		iniStr[j] = 0;
		}

		len = strlen(pAlexDnsWildHosts);
	}

	if (len < MAX_CONFIG_SECTION)
	{
		memset(iniStr + len, 0, sizeof(iniStr) - len);
	};

    for (int i = 0; i < 32; i++)
    {
        cfig.wildHosts[i].ip = 0;
        strcpy(cfig.wildHosts[i].wildcard, "");
    }

    if (iniStr[0] != 0)
	{
		char *iniStrPtr = myCleanToken(iniStr, 0);
		for (int i = 0; i < 32 && iniStrPtr[0]; iniStrPtr = myCleanToken(iniStrPtr, 1))
		{
			mySplit(name, value, iniStrPtr, '=');
			if (name[0] && value[0])
			{
				if (chkQu(name) && isIP(value))
				{
					DWORD ip = my_inet_addr(value);
					if (ip)
					{
						strcpy(cfig.wildHosts[i].wildcard, name);
						myLower(cfig.wildHosts[i].wildcard);
						cfig.wildHosts[i].ip = ip;
						i++;
					}
					else
					{
#ifdef _DUALSERVERDEBUG
						sprintf(logBuff, "Section [WILD-HOSTS] Invalid IP: %s ignored", value);
						logMess(logBuff, 1);
#endif
					}
				}
				else
				{
#ifdef _DUALSERVERDEBUG
					sprintf(logBuff, "Section [WILD-HOSTS] Invalid Entry: %s ignored", iniStrPtr);
					logMess(logBuff, 1);
#endif
				}
			}
			else
			{
#ifdef _DUALSERVERDEBUG
				sprintf(logBuff, "Section [WILD-HOSTS], Missing value, entry %s ignored", iniStrPtr);
				logMess(logBuff, 1);
#endif
			}
		}
	}
}

char* CDualServer::myCleanToken(char* buff, BYTE index)
{
	while (*buff)
	{
		if ((*buff) <= 32)
		{
			buff++;
			continue;
		}
		else if ((*buff) >= '0' && (*buff) <= '9' || (*buff) >= 'A' && (*buff) <= 'Z' || (*buff) >= 'a' && (*buff) <= 'z' || strchr("/\\?*", (*buff)))
		{
			if (index)
				index--;
			else
				break;
		}
		buff += strlen(buff) + 1;
	}

	return buff;
}

}