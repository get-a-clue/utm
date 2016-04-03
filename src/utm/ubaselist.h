#ifndef _UTM_UBASELIST_H
#define _UTM_UBASELIST_H

#pragma once
#include <utm.h>

#include <list>
#include <memory>

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
		ubaselist(const ubaselist<T>& rhs)
		{
			operator=(rhs);
		}

		~ubaselist(void) { };

		virtual const char *get_this_class_name() const { return "ubaselist"; };

		typedef std::list<std::unique_ptr<ubase>> ubaselist_container;

		ubaselist_container items;

		ubaselist<T>& operator=(const ubaselist<T>& rhs)
		{
			copyitems(rhs);
			return *this;
		}

		void copyitems(const ubaselist<T>& rhs)
		{
			items.clear();
			for (auto iter = rhs.items.begin(); iter != rhs.items.end(); ++iter)
			{
				ubase* u = iter->get();
				T* t = (T *)u;
				ubase* ucopy = copyInstance(*t);
				items.push_back(std::unique_ptr<ubase>(ucopy));
			}
		}

		bool operator==(const ubaselist<T>& rhs) const
		{
			size_t rhs_size = rhs.size();
			if (size() != rhs_size)
			{
				return false;
			}

			auto iter2 = rhs.items.begin();
			for (auto iter = items.begin(); iter != items.end(); ++iter, ++iter2)
			{
				ubase* ub = iter->get();
				ubase* ub2 = iter2->get();

				if (!ub->equals(ub2))
				{
					return false;
				}
			}

			return true;
		}

		virtual bool equals(const ubase* rhs) const
		{
			const ubaselist<T>* p = dynamic_cast<const ubaselist<T> *>(rhs);
			if (p == NULL)
			{
				return false;
			}

			return operator==(*p);
		}

	protected:
		std::unique_ptr<ubase> temp_item;

		ubase* copyInstance(const T& e)
		{
			return new T(e);
		}

		virtual ubase* createInstance()
		{
			return NULL;
		}

	public:
		virtual void clear()
		{
			items.clear();
		}

		void delete_temp_item()
		{
			temp_item.reset();
		}

		ubase* init_and_get_temp_item(ubase* u)
		{
			temp_item.reset(u);
			return temp_item.get();
		}

		void commit_temp_item()
		{
			T t;
			ubase* u = temp_item.get();
			if (u != NULL)
			{
				if (!(t.equals(u)))
				{
					temp_item.release();
					add_element(u);
				}
			}

			delete_temp_item();
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
		unsigned int insert_element(int insert_type, ubase* e, unsigned int element_id, unsigned int position_id)
		{
			if (e->get_id() > 0)
			{
				element_id = e->get_id();
				if (is_write_protected(element_id) && is_element_exist(element_id))
				{
					return element_id;
				}
			}
			else if (element_id == 0)
			{
				element_id = get_next_id();
			}

			e->set_id(element_id);

			if (insert_type == INSERT_TYPE_TAIL)
			{
				items.push_back(std::unique_ptr<ubase>(e));
			}

			if ((insert_type == INSERT_TYPE_AFTER) || (insert_type == INSERT_TYPE_BEFORE))
			{
				bool found = false;
				ubaselist_container::iterator iter;
				for (iter = items.begin(); iter != items.end(); ++iter)
				{
					if (iter->get()->get_id() == position_id)
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
					items.insert(iter, std::unique_ptr<ubase>(e));
				}
				else
				{
					items.push_back(std::unique_ptr<ubase>(e));
				}
			}

			return element_id;
		}

	public:
		unsigned int add_element(const T& e, unsigned int element_id = 0)
		{
			return insert_element(INSERT_TYPE_TAIL, copyInstance(e), element_id, 0);
		}

		unsigned int add_element(const ubase* e, unsigned int element_id = 0)
		{
			return insert_element(INSERT_TYPE_TAIL, const_cast<ubase *>(e), element_id, 0);
		}

		unsigned int insert_element_before(const T& e, unsigned int element_id, unsigned int position_id)
		{
			return insert_element(INSERT_TYPE_BEFORE, copyInstance(e), element_id, position_id);
		}

		unsigned int insert_element_before(const ubase* e, unsigned int element_id, unsigned int position_id)
		{
			return insert_element(INSERT_TYPE_BEFORE, const_cast<ubase *>(e), element_id, position_id);
		}

		unsigned int insert_element_after(const T& e, unsigned int element_id, unsigned int position_id)
		{
			return insert_element(INSERT_TYPE_AFTER, copyInstance(e), element_id, position_id);
		}

		unsigned int insert_element_after(const ubase* e, unsigned int element_id, unsigned int position_id)
		{
			return insert_element(INSERT_TYPE_AFTER, const_cast<ubase *>(e), element_id, position_id);
		}

		bool modify_element(const ubase* e)
		{
			if ((has_write_protection()) && (e->get_id() < WRITE_PROTECTED_ELEMENT_MAX_ID))
				return false;

			bool retval = false;
			for (auto iter = items.begin(); iter != items.end(); ++iter)
			{
				if (iter->get()->get_id() == e->get_id())
				{
					ubase* e2 = const_cast<ubase *>(e);
					iter->reset(e2);
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

			for (auto iter = items.begin(); iter != items.end(); ++iter)
			{
				if (iter->get()->get_id() == id)
				{
					items.erase(iter);
					return true;
				}
			}

			return false;
		}

		void truncate(size_t maxitems) 
		{
			for (auto iter = items.begin(); iter != items.end(); )
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

			for (auto iter = items.begin(); iter != items.end(); ++iter)
			{
				if (iter->get()->get_id() == id)
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

			for (auto iter = items.begin(); iter != items.end(); ++iter)
			{
				if (iter->get()->get_id() == id)
				{
					ret = UBASELISTORDER_FIRST;
					break;
				};

				if (iter->get()->get_id() == id2)
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
			for (auto iter = items.begin(); iter != items.end(); ++iter)
			{
				if (iter->get()->get_id() > retval)
				{
					retval = iter->get()->get_id();
				};
			};

			if ((has_write_protection()) && (retval < WRITE_PROTECTED_ELEMENT_MAX_ID))
				retval = WRITE_PROTECTED_ELEMENT_MAX_ID;

			return (retval + 1);
		}

		bool is_element_exist(unsigned int id) const
		{
			bool retval = false;
			for (auto iter = items.begin(); iter != items.end(); ++iter)
			{
				if (iter->get()->get_id() == id)
				{
					return true;
				};
			};

			return false;
		}

		ubase* findptr_by_index(unsigned int index) const
		{
			int i = 0;
			for (auto iter = items.begin(); iter != items.end(); ++iter, i++)
			{
				if (i == index)
				{
					return iter->get();
				};
			};

			return NULL;
		}

		ubase* findptr_next_by_id(unsigned int id) const
		{
			for (auto iter = items.begin(); iter != items.end(); ++iter)
			{
				if (iter->get()->get_id() == id)
				{
					++iter;
					if (iter != items.end())
					{
						return iter->get();
					}
					break;
				};
			};

			return NULL;
		}

		ubase* findptr_by_id(unsigned int id) const
		{
			for (auto iter = items.begin(); iter != items.end(); ++iter)
			{
				if (iter->get()->get_id() == id)
				{
					return iter->get();
				};
			};

			return NULL;
		}

		ubase* findptr_by_name(const gstring& name, bool case_insensetive = false) const
		{
			for (auto iter = items.begin(); iter != items.end(); ++iter)
			{
				if (!case_insensetive)
				{
					if (iter->get()->get_name() == name)
					{
						return iter->get();
					};
				}
				else if (name.is_ci_equal(iter->get()->get_name()))
				{
					return iter->get();
				}
			};

			return NULL;
		}

		bool move_up(unsigned int id)
		{
			bool retval = false;
			ubaselist_container::iterator iter, iter2;

			for (iter = items.begin(); iter != items.end(); ++iter)
			{
				if (iter->get()->get_id() == id)
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
				if (iter->get()->get_id() == id)
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

		void sort_by_name(bool ascending = true)
		{

		}
	};

#ifdef UTM_DEBUG
	class testitem : public ubase
	{
	public:
		static int destructor_calls;

		testitem() : id(0) { };
		testitem(unsigned int _id) : id(_id) { };
		~testitem() { destructor_calls++; };

		const char *get_this_class_name() const { return "testitem"; };

		unsigned int id;
		gstring name;

		bool equals(const ubase* rhs) const
		{
			if (id != rhs->get_id()) return false;
			if (name != rhs->get_name()) return false;
			return true;
		}

		unsigned int get_id() const { return id; };
		void set_id(unsigned int _id) { id = _id; name << id << "A"; };
		const gstring& get_name() const { return name; }
		void set_name(const gstring& _name) { name = _name; };

		void xml_create() { };
		void xml_catch_value(const char *name, const char *value) { };
		ubase* xml_catch_subnode(const char *tag_name, const char *class_name) { return NULL; };
	};
	
	class ubaselist_test : public ubaselist<testitem>
	{
	public:
		ubaselist_test() { };
		~ubaselist_test() { };

		bool operator==(const ubaselist_test& rhs) const
		{
			return equals(&rhs);
		}

		void xml_create() { };
		void xml_catch_value(const char *keyname, const char *keyvalue) { };
		ubase* xml_catch_subnode(const char *tag_name, const char *class_name) { return NULL; };

		
	};

	class ubaselist_main_test : public ubase
	{
	public:
		ubaselist_main_test() { };
		~ubaselist_main_test() { };

		ubaselist_test test_items;

		bool equals(const ubase* rhs) const
		{
			const ubaselist_main_test* p = dynamic_cast<const ubaselist_main_test *>(rhs);
			if (p == NULL)
			{
				return false;			
			}
			return test_items == p->test_items;
		}

		bool operator==(const ubaselist_main_test& rhs) const
		{
			return test_items == rhs.test_items;
		}

		void xml_create() { };
		void xml_catch_value(const char *keyname, const char *keyvalue) { };
		ubase* xml_catch_subnode(const char *tag_name, const char *class_name) { return NULL; };
	};
#endif

}

#endif //_UTM_UBASELIST_H