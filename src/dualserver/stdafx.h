#pragma once

#if defined(_MSC_VER)
#ifndef UTM_WIN
#define UTM_WIN
#endif

//#define _WINSOCKAPI_
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>
#include <winsock2.h>
#include <WS2tcpip.h>

#include "configdhcp.h"
#include "configdhcp_statrecord.h"
#include "configdns.h"
#include "DnsWildRecord.h"
#include "DualServer.h"

#endif
