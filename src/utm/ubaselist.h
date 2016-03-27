#ifndef _UTM_UBASELIST_H
#define _UTM_UBASELIST_H

#pragma once
#include <utm.h>

#include <list>

#include <stringtools.h>
#include <ubase.h>

#define WRITE_PROTECTED_ELEMENT_MAX_ID 10

#define UBASELISTORDER_ERROR 0
#define UBASELISTORDER_EQUAL 1
#define UBASELISTORDER_FIRST 2
#define UBASELISTORDER_SECOND 3

#define INSERT_TYPE_TAIL 0
#define INSERT_TYPE_BEFORE 1
#define INSERT_TYPE_AFTER 2

namespace utm {

	template<class T>
	class ubaselist : public ubase
	{
	public:
		ubaselist(void) { };
		~ubaselist(void) { };

		virtual const char *get_this_class_name() const { return "ubaselist"; };

		typedef std::list<T> ubaselist_container;

		ubaselist_container items;
		T temp_item;

		virtual void clear()
		{
			items.clear();
		}

		void clear_temp()
		{
			temp_item.clear();
		}

		T* get_temp_item()
		{
			temp_item.clear();
			return &temp_item;
		}

		void commit_temp_item()
		{
			T ethalon;
			if (!(ethalon == temp_item))
				add_element(temp_item);

			temp_item.clear();
		}

		size_t size() const
		{
			return items.size();
		}

		bool empty() const
		{
			return items.empty();
		}

		virtual bool has_write_protection() const
		{
			return false;
		}

		bool is_write_protected(unsigned int id) const
		{
			return ((has_write_protection()) && (id < WRITE_PROTECTED_ELEMENT_MAX_ID));
		}

	private:
		unsigned int insert_element(int insert_type, const T& e, unsigned int element_id, unsigned int position_id)
		{
			if (e.get_id() > 0)
			{
				element_id = e.get_id();
				if (is_write_protected(element_id) && is_element_exist(element_id))
				{
					return element_id;
				}
			}
			else if (element_id == 0)
			{
				element_id = get_next_id();
			}

			T e_copy(e);
			e_copy.set_id(element_id);

			if (insert_type == INSERT_TYPE_TAIL)
			{
				items.push_back(e_copy);
			}

			if ((insert_type == INSERT_TYPE_AFTER) || (insert_type == INSERT_TYPE_BEFORE))
			{
				bool found = false;
				ubaselist_container::iterator iter;
				for (iter = items.begin(); iter != items.end(); ++iter)
				{
					if (iter->get_id() == position_id)
					{
						found = true;
						break;
					}
				}

				if (insert_type == INSERT_TYPE_AFTER)
				{
					if (iter == items.end())
					{
						found = false;
					}
					else
					{
						++iter;
						if (iter == items.end())
						{
							found = false;
						}
					}
				}

				if (found)
				{
					items.insert(iter, e);
				}
				else
				{
					items.push_back(e_copy);
				}
			}

			return element_id;
		}

	public:
		unsigned int add_element(const T& e, unsigned int element_id = 0)
		{
			return insert_element(INSERT_TYPE_TAIL, e, element_id, 0);
		}

		unsigned int insert_element_before(const T& e, unsigned int element_id, unsigned int position_id)
		{
			return insert_element(INSERT_TYPE_BEFORE, e, element_id, position_id);
		}

		unsigned int insert_element_after(const T& e, unsigned int element_id, unsigned int position_id)
		{
			return insert_element(INSERT_TYPE_AFTER, e, element_id, position_id);
		}

		bool modify_element(const T& e)
		{
			if ((has_write_protection()) && (e.get_id() < WRITE_PROTECTED_ELEMENT_MAX_ID))
				return false;

			bool retval = false;
			ubaselist_container::iterator iter;

			for (iter = items.begin(); iter != items.end(); ++iter)
			{
				if (iter->get_id() == e.get_id())
				{
					*iter = e;
					retval = true;
					break;
				}
			}

			return retval;
		}

		bool delete_element(unsigned int id)
		{
			if ((has_write_protection()) && (id < WRITE_PROTECTED_ELEMENT_MAX_ID))
				return false;

			bool retval = false;
			ubaselist_container::iterator iter;

			for (iter = items.begin(); iter != items.end(); ++iter)
			{
				if (iter->get_id() == id)
				{
					items.erase(iter);
					retval = true;
					break;
				}
			}

			return retval;
		}

		void truncate(size_t maxitems) 
		{
			ubaselist_container::iterator iter;
			for (iter = items.begin(); iter != items.end(); )
			{
				if (maxitems == 0)
				{
					iter = items.erase(iter);
				}
				else
				{
					maxitems--;
					++iter;
				}
			}
		}

		int get_element_index(unsigned int id) const
		{
			int retval = 0;
			bool found = false;

			ubaselist_container::const_iterator iter;
			for (iter = items.begin(); iter != items.end(); ++iter)
			{
				if (iter->get_id() == id)
				{
					found = true;
					break;
				}
				retval++;
			}

			return found ? retval : -1;
		}

		int compare_element_position(unsigned int id, unsigned int id2) const
		{
			if (id == id2)
				return UBASELISTORDER_EQUAL;

			int ret = UBASELISTORDER_ERROR;

			ubaselist_container::const_iterator iter;
			for (iter = items.begin(); iter != items.end(); ++iter)
			{
				if (iter->get_id() == id)
				{
					ret = UBASELISTORDER_FIRST;
					break;
				};

				if (iter->get_id() == id2)
				{
					ret = UBASELISTORDER_SECOND;
					break;
				};
			};

			return ret;
		}

		unsigned int get_next_id() const
		{
			unsigned int retval = 0;
			ubaselist_container::const_iterator iter;

			for (iter = items.begin(); iter != items.end(); ++iter)
			{
				if (iter->get_id() > retval)
				{
					retval = iter->get_id();
				};
			};

			if ((has_write_protection()) && (retval < WRITE_PROTECTED_ELEMENT_MAX_ID))
				retval = WRITE_PROTECTED_ELEMENT_MAX_ID;

			return (retval + 1);
		}

		bool is_element_exist(unsigned int id) const
		{
			bool retval = false;
			ubaselist_container::const_iterator iter;

			for (iter = items.begin(); iter != items.end(); ++iter)
			{
				if (iter->get_id() == id)
				{
					retval = true;
					break;
				};
			};

			return retval;
		}

		bool find_by_id(unsigned int id, T& result) const
		{
			bool retval = false;
			ubaselist_container::const_iterator iter;

			for (iter = items.begin(); iter != items.end(); ++iter)
			{
				if (iter->get_id() == id)
				{
					result = *iter;
					retval = true;
					break;
				};
			};

			return retval;
		}

		bool find_by_index(unsigned int index, T& result) const
		{
			bool retval = false;
			ubaselist_container::const_iterator iter;

			int i = 0;
			for (iter = items.begin(); iter != items.end(); ++iter, i++)
			{
				if (i == index)
				{
					result = *iter;
					retval = true;
					break;
				};
			};

			return retval;
		}

		bool find_next_by_id(unsigned int id, T& result) const
		{
			bool retval = false;
			ubaselist_container::const_iterator iter;

			for (iter = items.begin(); iter != items.end(); ++iter)
			{
				if (iter->get_id() == id)
				{
					++iter;
					if (iter != items.end())
					{
						result = *iter;
						retval = true;
					}
					break;
				};
			};

			return retval;
		}

		T* findptr_by_id(unsigned int id)
		{
			T* retval = NULL;
			ubaselist_container::iterator iter;

			for (iter = items.begin(); iter != items.end(); ++iter)
			{
				if (iter->get_id() == id)
				{
					retval = &(*iter);
					break;
				};
			};

			return retval;
		}

		bool find_by_name(const gstring& name, T& result, bool case_insensetive = false) const
		{
			bool retval = false;
			ubaselist_container::const_iterator iter;

			for (iter = items.begin(); iter != items.end(); ++iter)
			{
				if (!case_insensetive)
				{
					if (iter->get_name() == name)
					{
						result = *iter;
						retval = true;
						break;
					};
				}
				else if (name.is_ci_equal((*iter).get_name()))
				{
					result = *iter;
					retval = true;
					break;
				}
			};

			return retval;
		}

		bool move_up(unsigned int id)
		{
			bool retval = false;
			ubaselist_container::iterator iter, iter2;

			for (iter = items.begin(); iter != items.end(); ++iter)
			{
				if (iter->get_id() == id)
				{
					if (iter != items.begin())
					{
						iter2 = iter;
						--iter2;
						std::swap(*iter, *iter2);
						retval = true;
					};
					break;
				};
			};

			return retval;
		}

		bool move_down(unsigned int id)
		{
			bool retval = false;
			ubaselist_container::iterator iter, iter2;

			for (iter = items.begin(); iter != items.end(); ++iter)
			{
				if (iter->get_id() == id)
				{
					iter2 = iter;
					++iter2;
					if (iter2 != items.end())
					{
						std::swap(*iter, *iter2);
						retval = true;
					};
					break;
				};
			};

			return retval;
		}

		T* insert_and_get_inserted(T p)
		{
			T* lastinserted = NULL;

			items.push_back(p);

			std::list<T>::reverse_iterator riter = items.rbegin();
			if (riter != items.rend())
			{
				std::list<T>::iterator iter = --riter.base();
				lastinserted = &(*iter);
			}

			return lastinserted;
		}

		T* insert_and_get_inserted()
		{
			T p;
			T* lastinserted = insert_and_get_inserted(p);
			return lastinserted;
		}

		void sort_by_name(bool ascending = true)
		{

		}
	};

#ifdef UTM_DEBUG
	class testitem
	{
	public:
		testitem() : id(0) { };
		testitem(unsigned int _id) : id(_id) { };
		~testitem() { };

		unsigned int id;
		unsigned int get_id() const { return id; };
		void set_id(unsigned int _id) { id = _id; };
	};

	class ubaselist_test : public ubaselist<testitem>
	{
	public:
		ubaselist_test() { };
		~ubaselist_test() { };

		void xml_create() { };
		void xml_catch_value(const char *keyname, const char *keyvalue) { };
		ubase* xml_catch_subnode(const char *tag_name, const char *class_name) { return NULL; };

		
	};
#endif

}

#endif //_UTM_UBASELIST_H