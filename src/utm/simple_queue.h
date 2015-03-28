#ifndef _UTM_SIMPLE_QUEUE_H
#define _UTM_SIMPLE_QUEUE_H

#pragma once
#include <utm.h>

#include <boost/thread/mutex.hpp>

namespace utm {

template<class T>
class simple_queue
{
	typedef std::list<T> queue_container;

public:
	simple_queue() : items_size(0) { };
	~simple_queue() { };

private:
	queue_container items;
	mutable boost::mutex guard;
	size_t items_size;

public:
	size_t size() const {
		boost::mutex::scoped_lock lock(guard);
		return items_size;
	}

	void push_item(const T& item)
	{
		boost::mutex::scoped_lock lock(guard);
		items.push_back(item);
		items_size++;
	}

	void push_items(const queue_container& manyitems)
	{
		boost::mutex::scoped_lock lock(guard);
		for (auto iter = manyitems.begin(); iter != manyitems.end(); ++iter)
		{
			items.push_back(*iter);
			items_size++;
		}
	}

	T* front_item()
	{
		boost::mutex::scoped_lock lock(guard);
		if (items.empty())
		{
			return NULL;
		}

		T& front = items.front();
		return &front;
	}

	T* pop_front_and_front_item()
	{
		boost::mutex::scoped_lock lock(guard);
		if (items.empty())
		{
			return NULL;
		}

		items.pop_front();
		items_size--;

		if (items.empty())
		{
			return NULL;
		}

		T& front = items.front();
		return &front;
	}
};

#ifdef UTM_DEBUG
class simple_queue_test : public simple_queue<int>
{
public:
	simple_queue_test() { };
	~simple_queue_test() { };

	static void test_all()
	{
		simple_queue_test sqt;
		sqt.push_item(1);
		int* v1 = sqt.front_item();
		TEST_CASE_CHECK(1, *v1);
		int* v2 = sqt.pop_front_and_front_item();
		TEST_CASE_CHECK(true, v2 == NULL);

		std::list<int> myitems;
		myitems.push_back(20);
		myitems.push_back(30);
		myitems.push_back(40);

		sqt.push_items(myitems);
		TEST_CASE_CHECK(size_t(3), sqt.size());

		int* t1 = sqt.front_item();
		TEST_CASE_CHECK(20, *t1);
		int* t2 = sqt.pop_front_and_front_item();
		TEST_CASE_CHECK(30, *t2);
		int* t3 = sqt.pop_front_and_front_item();
		TEST_CASE_CHECK(40, *t3);
		int* t4 = sqt.pop_front_and_front_item();
		TEST_CASE_CHECK(true, NULL == t4);

		return;
	}
};
#endif

}

#endif // _UTM_SIMPLE_QUEUE_H
