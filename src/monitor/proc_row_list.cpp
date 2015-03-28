#include "stdafx.h"
#include "proc_row_list.h"

namespace utm {

proc_row_list::proc_row_list()
{
}


proc_row_list::~proc_row_list()
{
}

bool proc_row_list::operator==(const proc_row_list& rhs) const
{
	return items == rhs.items;
}

void proc_row_list::create_from_proc_list(const proc_list& plist, std::string& xml, bool include_xml_declaration)
{
	proc_info* ptr = plist.proc_ptr.get();
	proc_info2* ptr2 = plist.proc2_ptr.get();

	if ((ptr == NULL) || (ptr2 == NULL))
		return;

	unsigned int c = 0;
	for (unsigned int i = 0; i < UTM_MAX_PROC_COUNT; i++, ptr++, ptr2++)
	{
		if (ptr->flag & 0x0001)
		{
			proc_row pr;

			pr.id = c;
			pr.pid = i * 4;
			pr.nick_id = ptr->nick_id;
			pr.last_activity = ptr->last_activity;
			pr.bytes_sent = ptr->bytes_sent;
			pr.bytes_recv = ptr->bytes_recv;
			pr.speed_sent = ptr->speed_sent;
			pr.speed_recv = ptr->speed_recv;

			if (ptr2->username.get() != NULL)
			{
				pr.username = *ptr2->username.get();
			}

			if (ptr2->exefile.get() != NULL)
			{
				pr.exename = *ptr2->exefile.get();
			}

			items.push_back(pr);

			c++;
		}
	}

	xml_create();
	xml_get_string(xml, include_xml_declaration);
}

void proc_row_list::update_proc_list(proc_list& plist)
{
	proc_info* ptr = plist.proc_ptr.get();
	proc_info2* ptr2 = plist.proc2_ptr.get();

	if ((ptr == NULL) || (ptr2 == NULL))
		return;

	proc_info* p = ptr;
	proc_info2* p2 = ptr2;

	for (unsigned int i = 0; i < UTM_MAX_PROC_COUNT; i++, p++)
	{
		p->flag = p->flag & 0xFFFB;
	}

	unsigned int pid_index;
	for (auto iter = items.begin(); iter != items.end(); ++iter)
	{
		pid_index = iter->pid / 4;
		p = ptr + pid_index;
		p->bytes_recv = iter->bytes_recv;
		p->bytes_sent = iter->bytes_sent;
		p->speed_recv = iter->speed_recv;
		p->speed_sent = iter->speed_sent;
		p->nick_id = iter->nick_id;
		p->last_activity = iter->last_activity;
		p->flag = p->flag | 0x0005;

		p2 = ptr2 + pid_index;
		if ((p2->username.get() == NULL) || (*p2->username.get() == iter->username))
		{
			p2->username.reset(new gstring(iter->username));
		}

		if ((p2->exefile.get() == NULL) || (*p2->exefile.get() == iter->exename))
		{
			p2->exefile.reset(new gstring(iter->exename));
		}
	}

	p = ptr;
	for (unsigned int i = 0; i < UTM_MAX_PROC_COUNT; i++, p++)
	{
		if (!(p->flag & 0x0004))
			p->flag = 0;
	}
}


void proc_row_list::clear()
{
	items.clear();
}

void proc_row_list::xml_create()
{
	xml_append_root("ProcList");
	xml_append_node(PROCROW_XMLTAG_PROCROW_TOTAL_ITEMS, items.size());
	xml_append_node(PROCROW_XMLTAG_PROCROW, items);
}

ubase* proc_row_list::xml_catch_subnode(const char *keyname)
{
	ubase *u = NULL;

	if (strcmp(keyname, PROCROW_XMLTAG_PROCROW) == 0)
	{
		proc_row* r = insert_and_get_inserted();
		u = dynamic_cast<ubase *>(r);
	}

	return u;
}

}