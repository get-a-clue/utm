#include "stdafx.h"

#include "addrtable_v4.h"
#include "stringtools.h"
#include <boost/test/unit_test.hpp>

#include <boost/algorithm/string.hpp>

namespace utm {

const char addrtable_v4::this_class_name[] = "addrtable_v4";

addrtable_v4::addrtable_v4(void)
{
}

addrtable_v4::~addrtable_v4(void)
{
}

bool addrtable_v4::check_addr_network(const addrip_v4& addr) const
{
	boost::mutex::scoped_lock lock(guard);

	bool retval = false;
	if (!items.empty())
	{
		AddrTableConstIter found = items.lower_bound(addr);

		if (found == items.end())
		{
			const std::pair<addrip_v4, addrip_v4>& rec = *(items.rbegin());
			unsigned int net1 = rec.first.m_addr & rec.second.m_addr;
			unsigned int net2 = addr.m_addr & rec.second.m_addr;
			retval = (net1 == net2);
		}
		else
		{
			unsigned int net1 = found->first.m_addr & found->second.m_addr;
			unsigned int net2 = addr.m_addr & found->second.m_addr;
			retval = (net1 == net2);

			if (!retval)
			{
				if (found != items.begin())
					--found;

				net1 = found->first.m_addr & found->second.m_addr;
				net2 = addr.m_addr & found->second.m_addr;
				retval = (net1 == net2);
			}
		}
	}

	return retval;
}

bool addrtable_v4::check_addr_network(const addrip_v4& addr, addrip_v4& net, addrip_v4& mask) const
{
	boost::mutex::scoped_lock lock(guard);

	bool retval = false;
	if (!items.empty())
	{
		AddrTableConstIter found = items.lower_bound(addr);

		if (found == items.end())
		{
			const std::pair<addrip_v4, addrip_v4>& rec = *items.rbegin();
			unsigned int net1 = rec.first.m_addr & rec.second.m_addr;
			unsigned int net2 = addr.m_addr & rec.second.m_addr;
			retval = (net1 == net2);

			if (retval)
			{
				net = rec.first;
				mask = rec.second;
			}
		}
		else
		{
			unsigned int net1 = found->first.m_addr & found->second.m_addr;
			unsigned int net2 = addr.m_addr & found->second.m_addr;
			retval = (net1 == net2);

			if (!retval)
			{
				if (found != items.begin())
					--found;

				net1 = found->first.m_addr & found->second.m_addr;
				net2 = addr.m_addr & found->second.m_addr;
				retval = (net1 == net2);
			}

			if (retval)
			{
				net = found->first;
				mask = found->second;
			}
		}
	}

	return retval;
}


bool addrtable_v4::import_from_txt(const gstring& input_filename)
{
	clear();

#ifdef UTM_WIN
	WIN32_FIND_DATA fd;
	memset(&fd, 0, sizeof(fd));

	HANDLE h = FindFirstFile(input_filename.c_str(), &fd);
	if (h == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	FindClose(h);
	last_file_write_time.from_filetime(fd.ftLastWriteTime);
#endif

	std::ifstream infile(input_filename.c_str(), std::ios_base::in);

	char tmp[200];
	std::string str_start, str_end;
	addrip_v4 addr_start, addr_end;

	std::string report;
	int lines_count = 0;

	if (infile)
	{
		std::string line;
		bool start_ok, end_ok;

		while (getline(infile, line, '\n'))
		{
			lines_count++;
			boost::trim(line);

			if (line.empty())
				continue;

			if (line.find_first_of("/#;") == 0)
				continue;

			size_t pos = line.find_first_of(",- \t");
			if (pos == std::string::npos)
				continue;

			str_start = line.substr(0, pos);
			str_end = line.substr(pos + 1);
			boost::trim(str_end);

			start_ok = addr_start.from_string(str_start, true);
			end_ok = addr_end.from_string(str_end, true);

			if ((start_ok) && (end_ok))
			{
				if (!this->AddAddrPair(addr_start, addr_end, true))
				{
#ifdef UTM_WIN
					sprintf_s(tmp, 200, "// Problem in parsing line %d (address is already present)\n", lines_count);
#else
					sprintf(tmp, "// Problem in parsing line %d (address is already present)\n", lines_count);
#endif
					report.append(tmp);
				}
			}
			else
			{
#ifdef UTM_WIN
				sprintf_s(tmp, 200, "// Problem in parsing line %d\n", lines_count);
#else
				sprintf_s(tmp, "// Problem in parsing line %d\n", lines_count);
#endif
				report.append(tmp);
			}
		};

		infile.close();
	}

	if (!report.empty())
	{
		report.append("//\n");
	}

	gstring backup_filename(input_filename);
	backup_filename.append(".backup");
	export_to_txt(backup_filename, report.c_str());

	return true;
}

BOOST_AUTO_TEST_CASE(addrtable_v4_test_all)
{
	

	addrtable_v4 a1;
	a1.AddAddrPair("192.168.0.0", "192.168.0.19", true);
	a1.AddAddrPair("192.168.0.20", "192.168.0.255", true);
	a1.AddAddrPair("192.168.0.255", "192.168.1.255", true);
	a1.AddAddrPair("192.168.2.0", "192.168.2.255", true);
	a1.AddAddrPair("192.168.4.0", "192.168.4.19", true);
	a1.AddAddrPair("192.168.4.21", "192.168.5.2", true);

	BOOST_REQUIRE_EQUAL(a1.GetAddrCount(), size_t(5));

	for (int i = 0; i < 3; i++)
	{
		bool result0 = false;
		bool result1 = true;

		if (i == 1)
		{
			result0 = false;
			result1 = false;
			a1.clear();
			a1.AddAddrPair("0.0.0.1", "0.0.0.1", true);
		}

		if (i == 2)
		{
			result0 = true;
			result1 = true;
			a1.clear();
			a1.AddAddrPair("0.0.0.0", "255.255.255.255", true);
		}

		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("0.0.0.0")), result0);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("10.10.10.10")), result0);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("191.167.255.255")), result0);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("192.168.0.0")), result1);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("192.168.0.1")), result1);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("192.168.0.19")), result1);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("192.168.0.20")), result1);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("192.168.0.21")), result1);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("192.168.0.255")), result1);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("192.168.1.0")), result0);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("192.168.1.255")), result0);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("192.168.2.0")), result1);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("192.168.2.2")), result1);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("192.168.2.255")), result1);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("192.168.3.255")), result0);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("192.168.4.0")), result1);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("192.168.4.10")), result1);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("192.168.4.19")), result1);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("192.168.4.20")), result0);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("192.168.4.21")), result1);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("192.168.4.22")), result1);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("192.168.4.255")), result1);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("192.168.5.0")), result1);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("192.168.255.255")), result0);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("217.89.12.88")), result0);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrRange(addrip_v4("255.255.255.255")), result0);

		if (i == 2)
		{
			result0 = false;
			result1 = false;
		}

		BOOST_REQUIRE_EQUAL(a1.CheckAddrSingle(addrip_v4("192.168.0.0")), result1);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrSingle(addrip_v4("192.168.0.1")), result0);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrSingle(addrip_v4("192.168.0.19")), result0);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrSingle(addrip_v4("192.168.0.20")), result1);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrSingle(addrip_v4("192.168.0.255")), result0);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrSingle(addrip_v4("192.168.2.0")), result1);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrSingle(addrip_v4("192.168.2.1")), result0);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrSingle(addrip_v4("192.168.4.0")), result1);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrSingle(addrip_v4("192.168.4.1")), result0);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrSingle(addrip_v4("192.168.4.19")), result0);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrSingle(addrip_v4("192.168.4.20")), result0);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrSingle(addrip_v4("192.168.4.21")), result1);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrSingle(addrip_v4("192.168.4.22")), result0);
		BOOST_REQUIRE_EQUAL(a1.CheckAddrSingle(addrip_v4("192.168.4.255")), result0);
	}

	addrtable_v4 a2;
	a2.AddAddrPair("192.168.0.0", "255.255.255.0", false);
	a2.AddAddrPair("192.168.2.0", "255.255.255.0", false);
	a2.AddAddrPair("192.168.3.0", "255.255.255.0", false);
	a2.AddAddrPair("192.168.8.0", "255.255.252.0", false);
	a2.AddAddrPair("10.10.10.0", "255.0.0.0", false);

	for (int j = 0; j < 3; j++)
	{
		bool result0 = false;
		bool result1 = true;

		if (j == 1)
		{
			result0 = false;
			result1 = false;
			a2.clear();
			a2.AddAddrPair("0.0.1.0", "255.255.255.0", false);
		}

		if (j == 2)
		{
			result0 = true;
			result1 = true;
			a2.clear();
			a2.AddAddrPair("0.0.0.0", "0.0.0.0", false);
		}

		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("0.0.0.0")), result0);
		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("10.0.0.0")), result1);
		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("192.168.0.0")), result1);
		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("192.168.0.1")), result1);
		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("192.168.0.255")), result1);
		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("192.168.1.0")), result0);
		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("192.168.1.255")), result0);
		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("192.168.2.0")), result1);
		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("192.168.2.1")), result1);
		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("192.168.2.255")), result1);
		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("192.168.3.0")), result1);
		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("192.168.3.255")), result1);
		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("192.168.4.0")), result0);
		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("192.168.4.1")), result0);
		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("192.168.4.255")), result0);
		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("192.168.7.255")), result0);
		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("192.168.8.0")), result1);
		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("192.168.8.1")), result1);
		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("192.168.8.2")), result1);
		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("192.168.10.0")), result1);
		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("192.168.11.255")), result1);
		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("192.168.12.0")), result0);
		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("192.168.12.1")), result0);
		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("192.168.255.255")), result0);
		BOOST_REQUIRE_EQUAL(a2.check_addr_network(addrip_v4("255.255.255.255")), result0);
	}

}

}
