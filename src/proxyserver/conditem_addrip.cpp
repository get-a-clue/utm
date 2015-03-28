#include "stdafx.h"
#include "conditem_addrip.h"

namespace utm {

const char conditem_addrip::this_class_name[] = "conditem_addrip";

conditem_addrip::conditem_addrip(void)
{
	clear();
}

conditem_addrip::conditem_addrip(addrtypes _addrtype, const addrip_v4& _addr1, const addrip_v4& _addr2)
{
	set_conditem_addrip(_addrtype, _addr1, _addr2);
}

conditem_addrip::~conditem_addrip(void)
{
}

void conditem_addrip::copy_properties(const conditem& rhs)
{
	conditem_addrip *p = (conditem_addrip *)&rhs;

	addrtype = p->addrtype;
	addr1 = p->addr1;
	addr2 = p->addr2;

	conditem::copy_properties(rhs);
}

void conditem_addrip::clear()
{
	addrtype = addr_none;
	addr1.clear();
	addr2.clear();
	conditem::clear();
}

void conditem_addrip::set_conditem_addrip(addrtypes _addrtype, const addrip_v4& _addr1, const addrip_v4& _addr2)
{
	addrtype = _addrtype;
	addr1 = _addr1;
	addr2 = _addr2;

	std::string str = to_string();
	condvalue = str;
}

bool conditem_addrip::parse(const std::string& str)
{
	if (str == "any")
	{
		addrtype = addr_any;
		return true;
	}

	size_t slashpos = str.find('/');
	size_t hypenpos = str.find('-');

	if ((slashpos != std::string::npos) && (hypenpos != std::string::npos))
	{
		return false;
	};

	if ((slashpos == std::string::npos) && (hypenpos == std::string::npos))
	{
		addrip_v4 a1;
		
		if (!a1.from_string(str.c_str(), true))
			return false;

		addr1 = a1;
		addr2 = addr1;
		addrtype = addr_single;

		return true;
	}

	size_t pos = slashpos;
	addrtype = addr_network;

	if (pos == std::string::npos)
	{
		pos = hypenpos;
		addrtype = addr_range;
	}

	addrip_v4 ip1;
	if (!ip1.from_string(str.substr(0, pos).c_str(), true))
		return false;

	addrip_v4 ip2;
	if (!ip2.from_string(str.c_str() + pos + 1, true))
		return false;

	addr1 = ip1;
	addr2 = ip2;

	return true;
}

std::string conditem_addrip::to_string() const
{
	std::ostringstream oss;

	switch(addrtype)
	{
		case addr_any:
			oss << "any";
			break;

		case addr_single:
			oss << addr1;
			break;

		case addr_network:
			oss << addr1 << "/" << addr2;
			break;

		case addr_range:
			oss << addr1 << "-" << addr2;
			break;
	}

	return oss.str();
}

bool conditem_addrip::do_check(proxyrequest_params *params)
{
	if (addrtype == addr_any)
		return true;

	if (params->request->client_addr.is_v4())
	{
		unsigned long ac = params->request->client_addr.to_v4().to_ulong();
		unsigned long a1 = addr1.m_addr;
		unsigned long a2 = addr2.m_addr;

		if (addrtype == addr_single)
		{
			if (ac == a1)
				return true;
			else
				return false;
		}

		if (addrtype == addr_network)
		{
			if (a1 == (ac & a2))
				return true;
			else
				return false;
		}

		if (addrtype == addr_range)
		{
			if ((a1 <= ac) && (ac <= a2))
				return true;
			else
				return false;
		}
	}

	return false;
};

}
