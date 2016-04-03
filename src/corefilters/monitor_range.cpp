#include "StdAfx.h"
#include "monitor_range.h"

namespace utm {

monitor_range::monitor_range(void)
{
}


monitor_range::~monitor_range(void)
{
}

ubase* monitor_range::xml_catch_subnode(const char *keyname, const char *class_name)
{
	ubase *u = NULL;

	if (strcmp(keyname, MONDETREC_XMLTAG_ROOT) == 0)
	{
		return details.xml_catch_subnode(keyname, NULL);
	}

	return u;
}

const gchar* monitor_range::get_ping_status_str(int status)
{
	if (status == PING_STATUS_OK)
		return _U("OK");
	else if (status == PING_STATUS_OFFLINE)
		return _U("Offline");
	else if (status == PING_STATUS_QUALITYDROP)
		return _U("QualityDrop");

	return _U("?");
}

unsigned int monitor_range::get_ping_status_color(int status) const
{
	if (status == PING_STATUS_OK)
		return color1;
	else if (status == PING_STATUS_OFFLINE)
		return color2;
	else if (status == PING_STATUS_QUALITYDROP)
		return color3;

	return color0;
}

void monitor_range::set_ping_status_color(int status, unsigned int color)
{
	if (status == PING_STATUS_OK)
		color1 = color;
	else if (status == PING_STATUS_OFFLINE)
		color2 = color;
	else if (status == PING_STATUS_QUALITYDROP)
		color3 = color;
	else if (status == PING_STATUS_UNKNOWN)
		color0 = color;
}

monitor_detail_record* monitor_range::find_detail_by_mac(const addrmac& mac)
{
	if (!mac.is_zero())
	{
		for (auto iter = details.items.begin(); iter != details.items.end(); ++iter)
		{
			monitor_detail_record* mdr = dynamic_cast<monitor_detail_record *>(iter->get());
			if (mdr->mac == mac)
			{
				return mdr;
			}
		}
	}

	return NULL;
}

monitor_detail_record* monitor_range::find_detail_by_ip(const addrip_v4& ip)
{
	if (!ip.is_zero())
	{
		for (auto iter = details.items.begin(); iter != details.items.end(); ++iter)
		{
			monitor_detail_record* mdr = dynamic_cast<monitor_detail_record *>(iter->get());
			if (mdr->ip == ip)
			{
				return mdr;
			}
		}
	}

	return NULL;
}

void monitor_range::test_fillparams(int test_num)
{
	clear();

	attempts = 2;
	poll_interval = 1;

	comment.assign("test comment");
	reverse_lookup = false;
	start_addr.from_string("192.168.1.1");
	end_addr.from_string("192.168.1.255");

	monitor_detail_record r1;
	r1.comment.assign("Comment for ip address 1");
	r1.ip.from_string("192.168.1.2");
	details.add_element(r1);

	monitor_detail_record r2;
	r2.comment.assign("Comment mac address 2");
	r2.mac.from_string("cc:b2:55:9a:28:9a");
	details.add_element(r2);

	monitor_detail_record r3;
	r3.comment.assign("Comment mac address 3");
	r3.mac.from_string("00:01:02:03:04:05");
	details.add_element(r3);

	monitor_detail_record r4;
	r4.comment.assign("Broadcast hardware address");
	r4.mac.from_string("ff:ff:ff:ff:ff:ff");
	details.add_element(r4);

	details.add_element(r1);

	details.move_down(12);

	return;
}

}