#include "stdafx.h"
#include "trafficreport_filter.h"

namespace utm {

const char trafficreport_filter::this_class_name[] = "trafficreport_filter";

trafficreport_filter::trafficreport_filter()
{
}

trafficreport_filter::~trafficreport_filter()
{
}

bool trafficreport_filter::xml_has_root_attr() const
{
	return true;
}

ubase* trafficreport_filter::xml_catch_subnode(const char *keyname, const char *class_name)
{
	ubase *u = NULL;

	if (strcmp(keyname, TR_DAYTICK_XMLTAG_ROOT) == 0)
	{
		u = dayticks.init_and_get_temp_item(new trafficreport_daytick());
		return u;
	}

	return NULL;
};

void trafficreport_filter::xml_catch_subnode_finished(const char *keyname)
{
	if (strcmp(keyname, TR_DAYTICK_XMLTAG_ROOT) == 0)
	{
		dayticks.commit_temp_item();
	}
}

void trafficreport_filter::xml_catch_subnode_attribute(const char *attrname, const char* attrvalue)
{
	xml_catch_rootnode_attribute(attrname, attrvalue);
}

void trafficreport_filter::xml_catch_rootnode_attribute(const char *attrname, const char* attrvalue)
{
	id = atol(attrvalue);
}

void trafficreport_filter::xml_get_root_attr(xmlattr_container& attr)
{
	attr.insert(std::make_pair<std::string, std::string>(std::string("Id"), std::to_string(id)));
};

void trafficreport_filter::set_id(unsigned int id)
{
	this->id = id;
}

unsigned int trafficreport_filter::get_id() const
{
	return id;
}

void trafficreport_filter::update_counters(const utime& ctm, __int64 sent, __int64 recv, const gstring& name)
{
	unsigned int cts_day = trafficreport_daytick::utime2id(ctm);

	this->sent += sent;
	this->recv += recv;
	this->name = name;

	bool found = false;
	for (auto iter = dayticks.items.begin(); iter != dayticks.items.end(); ++iter)
	{
		unsigned int ts_day = iter->get()->get_id();
		if (cts_day == ts_day)
		{
			trafficreport_daytick *tr = dynamic_cast<trafficreport_daytick *>(iter->get());
			tr->update_counters(ctm, sent, recv);
			found = true;
			break;
		}
	}

	if (!found)
	{
		utime mtm;
		trafficreport_daytick::id2utime(cts_day, mtm);
		trafficreport_daytick tick;
		tick.ut = mtm;
		tick.update_counters(ctm, sent, recv);
		dayticks.add_element(tick);
	}
}

#ifdef UTM_DEBUG
void trafficreport_filter::test_all()
{
	trafficreport_daytick trd = trafficreport_daytick::test_get_daytick();

	trafficreport_filter* trf = new trafficreport_filter();
	trf->set_id(1);
	trf->name.assign("Test filter name");
	trf->color = 255;
	trf->sent = 1001;
	trf->recv = 2002;
	trf->dayticks.add_element(trd);

	std::string x;
	trf->xml_create();
	trf->xml_get_string(x);

	return;
}
#endif

}