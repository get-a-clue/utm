#ifndef _UTM_EVENT_HOLDER_H
#define _UTM_EVENT_HOLDER_H

#pragma once
#include <utm.h>

#include <loggingparams.h>
#include <ubase.h>
#include <event_defs.h>

#include <list>

#include <boost/thread/mutex.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#define EVENTHOLDER_XMLTAG_EVENTS "Events"
#define LOGFILE_ROTATE_CHECK_INTERVAL 25

namespace utm {

template<class T>
class event_holder : public loggingparamsbase, public ubase
{
	typedef std::list<T> event_holder_container;

public:
	event_holder(size_t _capacity = 50) { next_event_id = 0; capacity = _capacity; rotate_check_counter = 0; };
	virtual ~event_holder() { };

	event_holder(const event_holder<T>& r)
	{
		operator=(r);
	}

	event_holder& operator=(const event_holder<T>& rhs)
	{
		capacity = rhs.capacity;
		next_event_id = rhs.next_event_id;
		events = rhs.events;

		return *this;
	}

	virtual bool equals(const ubase* rhs) const
	{
		const event_holder<T>* p = dynamic_cast<const event_holder<T> *>(rhs);
		if (p == NULL)
		{
			return false;
		}

		return operator==(*p);
	}

	bool operator==(const event_holder<T>& rhs) const
	{
		return events == rhs.events;
	}

	virtual const char *get_this_class_name() const { return "event_holder"; };

	void safe_copy_container(event_holder_container& dest_holder, event_id from_event_id, bool clear_source = false)
	{
		boost::mutex::scoped_lock lock(guard);

		dest_holder.clear();
		event_holder_container::iterator iter;
		
		for (iter = events.begin(); iter != events.end(); ++iter)
		{
			if (iter->id >= from_event_id)
				dest_holder.push_back(*iter);
		}

		if (clear_source)
			events.clear();
	}

	size_t get_capacity() const	{ return capacity; }
	void set_capacity(size_t maxcapacity) { capacity = maxcapacity;	}
	size_t size() const { boost::mutex::scoped_lock lock(guard); return events.size(); }
	void clear() { boost::mutex::scoped_lock lock(guard); events.clear(); }

	void truncate_front(size_t maxitems)
	{
		boost::mutex::scoped_lock lock(guard);
		size_t s = events.size();
		while (s > maxitems)
		{
			events.pop_front();
			s--;
		}
	}

	T* get_last_event_ptr()
	{
		if (events.empty())
			return NULL;

		event_holder_container::reverse_iterator riter = events.rbegin();
		event_holder_container::iterator iter;

		// convert reverse iterator to forward iterator
		if (riter == events.rend())
			iter = events.begin();
		else
			iter = (--riter.base());

		return &(*iter);
	}

	void xml_create()
	{
		std::string str;
		xml_create(str, true);
	}

	void xml_create(std::string& xml, bool include_xml_declaration = true) 
	{ 
		xml_create(0, xml, include_xml_declaration); 
	}

	void xml_create(event_id threshold_id, std::string& xml, bool include_xml_declaration = true)
	{
		boost::mutex::scoped_lock lock(guard);

		xml_append_root(EVENTHOLDER_XMLTAG_EVENTS);

		if (threshold_id == 0)
		{
			event_holder_container::iterator iter;
			for (iter = events.begin(); iter != events.end(); ++iter)
			{
				iter->xml_create();
				xml_append_child(iter->get_first_child());
			}
		}
		else
		{
			event_holder_container::iterator iter;
			event_holder_container::reverse_iterator riter;

			size_t items_found = 0;
			for (riter = events.rbegin(); riter != events.rend(); ++riter)
			{
				if (riter->id < threshold_id)
				{
					break;
				}
				items_found++;
			}

			if (items_found > 0)
			{
				// convert reverse iterator to forward iterator
				if (riter == events.rend())
					iter = events.begin();
				else
					iter = (--riter.base());

				for (; iter != events.end(); ++iter)
				{
					if (iter->id >= threshold_id)
					{
						iter->xml_create();
						xml_append_child(iter->get_first_child());
					}
				}
			}
		}

		xml_get_string(xml, include_xml_declaration);
	}

	void xml_catch_value(const char *keyname, const char *keyvalue)
	{
		if (strcmp(keyname, T::get_xmltag()) == 0)
		{
			T item;
			item.xml_catch_value(keyname, keyvalue);
			add_item(item, true);
		};
	}

	ubase* xml_catch_subnode(const char *tag_name, const char *class_name) { return NULL; };

	void make_diff_dump(const event_id remote_next_id, std::string& xml_dump, bool include_xml_declaration = true)
	{
		xml_create(remote_next_id, xml_dump, include_xml_declaration);
	}

	void AddMessage(int pane, int type, const char *msg)
	{
		add_message(pane, type, msg);
	}

#ifdef UTM_WIN
#ifdef UNICODE
	void AddMessage(int pane, int type, const WCHAR *msg)
	{
		add_message(pane, type, msg);
	}
#endif
#endif

	void add_message(int pane, int type, const std::string& msg)
	{
		add_message(pane, type, msg.c_str());
	}

	void add_message(int pane, int type, const utm::gstring& msg)
	{
		add_message(pane, type, msg.c_str());
	}

	template<typename charT>
	void add_message(int pane, int type, const charT *msg)
	{
		boost::mutex::scoped_lock lock(guard);

		size_t events_size = events.size();
		while (events_size >= get_capacity())
		{
			events.pop_front();
			events_size--;
		}

		T item;
		item.id = next_event_id;
		item.pane = pane;
		item.type = type;
		item.time.now();
		item.assign(msg);

		next_event_id++;

		events.push_back(item);
		write_log(item);
	}

	bool pop_front_item(T& item)
	{
		boost::mutex::scoped_lock lock(guard);

		if (events.empty())
			return false;

		item = (*events.begin());
		events.pop_front();

		return true;
	}

	utm::event_id add_item(const T& item, bool dont_writelog = false)
	{
		boost::mutex::scoped_lock lock(guard);

		while (events.size() >= get_capacity())
		{
			events.pop_front();
		}

		T itemcopy(item);

		if (item.id == 0)
		{
			itemcopy.id = next_event_id;
			next_event_id++;
		}
		else
		{
			if (itemcopy.id >= next_event_id) 
				next_event_id = item.id + 1;
		}

		events.push_back(itemcopy);
		if (!dont_writelog)	write_log(itemcopy);
		return itemcopy.id;
	}

	bool find_item(event_id id, T& item) const
	{
		boost::mutex::scoped_lock lock(guard);

		event_holder_container::const_iterator iter;
		for (iter = events.begin(); iter != events.end(); ++iter)
		{
			if (iter->id == id)
			{
				item = *iter;
				return true;
			}
		}

		return false;
	}

	event_id get_next_event_id() const
	{
		return next_event_id;
	}

protected:

	void write_log(T& item)
	{
		if ((enable_file) || (enable_console))
		{
			std::string msg = item.to_string(true);

			if (enable_file)
			{
				gstring filename;
				if (ufs::derive_fullpath(utime(true), get_fullpath(), filename))
				{
					boost::filesystem::path path(filename.get());
					boost::filesystem::path dir = path.parent_path();

					if (!boost::filesystem::is_directory(dir))
						boost::filesystem::create_directories(dir);

					if (get_maxlogrotate() > 0)
					{
						rotate_check_counter++;
						if (rotate_check_counter >= LOGFILE_ROTATE_CHECK_INTERVAL)
						{
							rotate_check_counter = 0;
							size_t current_size = get_filesize(path);

							if (current_size >= get_maxlogsize())
								rotate(path, get_maxlogrotate());
						}
					}

					std::fstream f;
					f.open(filename.c_str(), std::fstream::out|std::fstream::app);
					if (f)
					{
						f << msg << std::endl;
						f.close();
					}
				}
			}

			if (enable_console)
			{
				std::cout << msg << std::endl;
			}
		}
	}


	size_t get_filesize(boost::filesystem::path& path)
	{
		if (!boost::filesystem::exists(path))
			return 0;

		if (!boost::filesystem::is_regular(path))
			return 0;

		uintmax_t filesize = boost::filesystem::file_size(path) / 1048576;

		return (size_t)filesize;
	}

	void rotate(boost::filesystem::path& path, size_t logfile_rotate)
	{
		for (size_t i = logfile_rotate - 1; ; i--)
		{
			std::ostringstream newoss;
			newoss << path.string() << "." << i;
			std::string newfilename = newoss.str();
			boost::filesystem::path newpath(newfilename);

			std::ostringstream oldoss;
			oldoss << path.string();
			if (i > 0)
				oldoss << "." << (i - 1);
			std::string oldfilename(oldoss.str());
			boost::filesystem::path oldpath(oldfilename);
			
			try
			{
				boost::filesystem::rename(oldpath, newpath);
			}
			catch(std::exception&/* e*/)
			{
//				std::string what = e.what();
			}

			if (i == 0)
				break;
		}
	}

	size_t capacity;
	event_id next_event_id;
	event_holder_container events;
	mutable boost::mutex guard;

protected:
	size_t rotate_check_counter;
};

}

#endif // _UTM_EVENT_HOLDER_H
