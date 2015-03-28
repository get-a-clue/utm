#pragma once

#define NICALIAS_DEFAULT 0
#define NICALIAS_NDISWAN 1
#define NICALIAS_PUBLIC 2
#define NICALIAS_PRIVATE 3
#define NICALIAS_NETFLOW 4

#define	RULE_MYIP		0		// IP, принадлежащие этому компьютеру
#define	RULE_IP			1		// точный IP
#define	RULE_LAN		2		// адреса, определенные в LAT
#define	RULE_WAN		3		// адреса, не определенные в LAT
#define	RULE_MAC		4		// MAC-адрес
#define	RULE_RANGE		5		// диапазон IP-адресов
//#define RULE_RANGE_EX	6		// Диапазон IP-адресов с индивидуальными счетчиками для каждого адреса
#define RULE_HOST		7		// Имя хоста
#define RULE_ADDRGRP	8		// Группа адресов, определенная в MAT
#define RULE_ADDRGRP_NO 9		// Группа адресов, не определенная в MAT
#define RULE_USER       10      // Пользователь, аутентифицированный средствами TMeter
#define RULE_USER_ANY	11		// Любой пользователь, аутентифированный средствами TMeter
#define RULE_PROCNAME	12		// Процесс (по имени процесса)
#define RULE_PROCUSER	13		// Процесс (по имени пользователя-владельца процесса)
#define RULE_INCOMING	20		// Все входящий пакеты с сетевого адаптера
#define RULE_OUTGOING	21		// Все исходящие пакеты с сетевого адаптера

#define	PORT_ANY		0
#define	PORT_EQUAL		1
#define	PORT_GREATER	2
#define	PORT_LESS		3
#define	PORT_NOTEQUAL	4
#define	PORT_BETWEEN	5
#define	PORT_NOTBETWEEN	6
#define PORT_EQUAL_OR	7

#define	ACTION_COUNT	0
#define	ACTION_PASS		1
#define	ACTION_COUNTPASS 2
#define	ACTION_DENY		3
#define ACTION_FWD_DST	4
#define ACTION_FWD_SRC	5

#define	COND_ALWAYS		0
#define	COND_SENT_LESS	1
#define	COND_SENT_GREATER 2
#define	COND_RECV_LESS	3
#define	COND_RECV_GREATER 4
#define	COND_SENT_OR_RECV_LESS 5                // Sent OR Recv less
#define	COND_SENT_OR_RECV_GREATER 6				// Sent OR Recv greater
#define	COND_SENT_AND_RECV_LESS 7				// Recv AND Recv less
#define	COND_SENT_AND_RECV_GREATER 8			// Recv AND Recv greater
#define	COND_SENT_PLUS_RECV_LESS 9
#define	COND_SENT_PLUS_RECV_GREATER 10

#define COND_MAC_ANY 0
#define COND_MAC_SRC_EQUAL 1
#define COND_MAC_SRC_NOTEQUAL 2
#define COND_MAC_DST_EQUAL 3
#define COND_MAC_DST_NOTEQUAL 4

#define PREVFILTER_ANY 0
#define PREVFILTER_NOTMATCHED 1
#define PREVFILTER_MATCHED 2

#define NATUSE_ANY 0
#define NATUSE_YES 1
#define NATUSE_NO 2

#define PKTOPT_NONE 0
#define PKTOPT_TCPSYN 1
#define PKTOPT_ICMPECHOREQUEST 2
#define PKTOPT_FTP 4
#define PKTOPT_ICMPTTLEXCEEDED 8

#define DIRECTION_ONEWAY 0
#define DIRECTION_TWOWAY 1
#define DIRECTION_STATEFUL 2

#define	WDAY_MON 1
#define	WDAY_TUE 2
#define	WDAY_WED 4
#define WDAY_THU 8
#define WDAY_FRI 16
#define WDAY_SAT 32
#define WDAY_SUN 64
#define WDAY_ALL 127
