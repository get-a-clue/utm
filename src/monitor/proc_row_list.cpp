#include "stdafx.h"
#include "proc_row_list.h"

namespace utm {

proc_row_list::proc_row_list()
{
}


proc_row_list::~proc_row_list()
{
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
			proc_row* pr = new proc_row();

			pr->id = c;
			pr->pid = i * 4;
			pr->nick_id = ptr->nick_id;
			pr->last_activity = ptr->last_activity;
			pr->bytes_sent = ptr->bytes_sent;
			pr->bytes_recv = ptr->bytes_recv;
			pr->speed_sent = ptr->speed_sent;
			pr->speed_recv = ptr->speed_recv;

			if (ptr2->username.get() != NULL)
			{
				pr->username = *ptr2->username.get();
			}

			if (ptr2->exefile.get() != NULL)
			{
				pr->exename = *ptr2->exefile.get();
			}

			items.push_back(std::unique_ptr<ubase>(pr));

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
		proc_row *pr = dynamic_cast<proc_row *>(iter->get());
		if (pr != NULL)
		{
			pid_index = pr->pid / 4;
			p = ptr + pid_index;
			p->bytes_recv = pr->bytes_recv;
			p->bytes_sent = pr->bytes_sent;
			p->speed_recv = pr->speed_recv;
			p->speed_sent = pr->speed_sent;
			p->nick_id = pr->nick_id;
			p->last_activity = pr->last_activity;
			p->flag = p->flag | 0x0005;

			p2 = ptr2 + pid_index;
			if ((p2->username.get() == NULL) || (*p2->username.get() == pr->username))
			{
				p2->username.reset(new gstring(pr->username));
			}

			if ((p2->exefile.get() == NULL) || (*p2->exefile.get() == pr->exename))
			{
				p2->exefile.reset(new gstring(pr->exename));
			}
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

ubase* proc_row_list::xml_catch_subnode(const char *tag_name, const char *classname)
{
	ubase *u = NULL;

	if (strcmp(tag_name, PROCROW_XMLTAG_PROCROW) == 0)
	{
		u = init_and_get_temp_item(new proc_row());
		add_element(u);
	}

	return u;
}

}