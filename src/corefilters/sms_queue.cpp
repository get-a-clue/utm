#include "StdAfx.h"
#include "sms_queue.h"

#include <utime.h>
#include <ufs.h>
#include <enumhelper.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace utm {

const char* sms_queue::this_class_name = "sms_queue";

const char* sms_queue::sms_reason_str[] = { "QUEUED", "EXPRED", "DLVRED" };;
const int sms_queue::sms_reason_int[] = { (int)sms_reason_put_queue, (int)sms_reason_expired, (int)sms_reason_delivered };
const int sms_queue::sms_reason_qty = 3;

sms_queue::sms_queue(void)
{
}

sms_queue::sms_queue(const sms_queue& rhs)
{
	items = rhs.items;
}

sms_queue::~sms_queue(void)
{
}

bool sms_queue::operator==(const sms_queue& rhs) const
{
	return items == rhs.items;
}

void sms_queue::clear()
{
	boost::mutex::scoped_lock lock(guard);
	items.clear();
}

size_t sms_queue::size() const
{
	boost::mutex::scoped_lock lock(guard);
	return items.size();
}

void sms_queue::set_log_folder(const gstring& log_folder)
{
	ufs::make_strong_directory_path(log_folder, sms_log_folder);
	sms_log_filename = sms_log_folder;
	sms_log_filename.append("Sms_%Y_%m.txt");
}

void sms_queue::add_sms_existed(const sms& s)
{
	boost::mutex::scoped_lock lock(guard);
	items.push_back(s);
}

void sms_queue::add_sms_new_bulk(const sms_container& smslist)
{
	for (sms_container::const_iterator iter = smslist.begin(); iter != smslist.end(); ++iter)
	{
		add_sms_new(*iter);
	}
}

void sms_queue::add_sms_new(const sms& s)
{
	sms scopy(s);
	scopy.generate_msg_id();

	{
		boost::mutex::scoped_lock lock(guard);
		items.push_back(scopy);
	}

	log_sms(scopy, sms_reason_put_queue);
}

void sms_queue::add_sms_new_to_users(const sms& s, const std::map<unsigned int, gstring>& selected_users)
{
	sms scopy(s);
	for (auto iter = selected_users.begin(); iter != selected_users.end(); ++iter)
	{
		scopy.recp_uid = iter->first;
		scopy.recp_name = iter->second;
		add_sms_new(s);
	}
}

void sms_queue::select_sms_for_user(unsigned int userid, sms_container& smslist) const
{
	boost::mutex::scoped_lock lock(guard);
	for (sms_container::const_iterator iter = items.begin(); iter != items.end(); ++iter)
	{
		if ((userid == 0) || (iter->recp_uid == userid))
		{
			smslist.push_back(*iter);
		}
	}
}

void sms_queue::delivery_preparing_sms_for_user(unsigned int userid, std::ostringstream& res)
{
	sms_container smslist;

	{
		boost::mutex::scoped_lock lock(guard);
		for (sms_container::iterator iter = items.begin(); iter != items.end(); )
		{
			if (iter->recp_uid == userid)
			{
				smslist.push_back(*iter);
				iter = items.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}

	if (!smslist.empty())
	{
		log_sms_bulk(smslist, sms_reason_delivered);

		utm::sms_queue local_queue;
		local_queue.items = smslist;
		local_queue.xml_create();

		std::string s, xml;
		local_queue.xml_get_string(s, false);
		utm::stringtools::remove_root_tag(s, xml);
		res << xml;
	}
}

void sms_queue::delete_sms_bulk(const sms_container& smslist)
{
	for (sms_container::const_iterator iter = smslist.begin(); iter != smslist.end(); ++iter)
	{
		delete_sms(*iter);
	}

	log_sms_bulk(smslist, sms_reason_delivered);
}

void sms_queue::delete_sms(const sms& s)
{
	boost::mutex::scoped_lock lock(guard);
	for (sms_container::iterator iter = items.begin(); iter != items.end(); ++iter)
	{
		if (iter->msg_id == s.msg_id)
		{
			items.erase(iter);
			break;
		}
	}
}

void sms_queue::purge()
{
	time_t now;
	time(&now);

	sms_container expired_sms;

	{
		boost::mutex::scoped_lock lock(guard);
		for (sms_container::iterator iter = items.begin(); iter != items.end(); )
		{
			if (iter->time_expire < now)
			{
				expired_sms.push_back(*iter);
				iter = items.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}

	log_sms_bulk(expired_sms, sms_reason_expired);
}

void sms_queue::log_sms_bulk(const sms_container& smslist, sms_reason reason)
{
	for (sms_container::const_iterator iter = smslist.begin(); iter != smslist.end(); ++iter)
	{
		log_sms(*iter, reason);
	}
}

void sms_queue::log_sms(const sms& s, sms_reason reason)
{
	if (sms_log_folder.empty())
	{
		return;
	}

	utime now(true);

	gstring filename;
	if (ufs::derive_fullpath(now, sms_log_filename, filename))
	{
		boost::filesystem::path path(filename.get());
		boost::filesystem::path dir = path.parent_path();

		if (!boost::filesystem::is_directory(dir))
			boost::filesystem::create_directories(dir);

		std::string tab("\t");
		std::string quote("\"");

		std::fstream f;
		f.open(filename.c_str(), std::fstream::out|std::fstream::app);
		if (f)
		{
			f << now.to_string(utime::format_log) << tab;
			f << enumhelper::to_str(reason, sms_reason_str, sms_reason_int, sms_queue::sms_reason_qty) << tab;
			f << quote << s.sender_name.getmb() << quote << tab;
			f << quote << s.recp_name.getmb() << quote << tab;
			f << s.msg_text.getmb() << std::endl;
			f.close();
		}
	}

}

void sms_queue::make_sms_queue_filename(const gstring& filterset_folder, const gstring& filterset_file)
{
	gstring part1;
	ufs::make_strong_directory_path(filterset_folder, part1);

	gstring part2(filterset_file);
	size_t p = part2.find(_U("."), 0);
	if (p != std::string::npos)
	{
		size_t total_size = part2.size();
		part2.replace(p, total_size - p, _U(".sms"), 4);
	}

	sms_filename = part1;
	sms_filename.append(part2);
}

void sms_queue::xml_create()
{
	boost::mutex::scoped_lock lock(guard);

	xml_append_root(SMS_QUEUE_XMLTAG_ROOT);

	for(sms_container::iterator iter = items.begin(); iter != items.end(); ++iter)
	{
		iter->xml_create();
		xml_append_child(iter->get_first_child());
	}
}

ubase* sms_queue::xml_catch_subnode(const char *keyname)
{
	if (strcmp(keyname, SMS_XMLTAG_ROOT) == 0)
	{
		sms *lastinserted = NULL;

		{
			boost::mutex::scoped_lock lock(guard);
			sms s;
			items.push_back(s);

			std::list<sms>::reverse_iterator riter = items.rbegin();
			if (riter != items.rend())
			{
				std::list<sms>::iterator iter = --riter.base();
				lastinserted = &(*iter);
			}
		}

		return (ubase *)lastinserted;
	}

	return NULL;
}

#ifdef UTM_DEBUG
void sms_queue::test_fillparams(int test_num)
{
	test_case::classname.assign(this_class_name);

	make_sms_queue_filename(gstring("c:\\log"), gstring("default.tmf"));
	TEST_CASE_CHECK(get_sms_queue_filename().getmb(), std::string("c:\\log\\default.sms"));

	clear();
	set_log_folder("c:\\tmp");

	{
		sms s1;
		s1.msg_text = "You have exceeded the traffic limit";
		s1.sender_uid = 1;
		s1.sender_name = "Admin";
		s1.recp_uid = 20;
		s1.recp_name = "Roger Waters";

		add_sms_new(s1);
		add_sms_new(s1);
	}

	return;
}
#endif

}
