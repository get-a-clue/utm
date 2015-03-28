#ifndef _UTM_USINGLETON_H
#define _UTM_USINGLETON_H

#pragma once
#include <utm.h>

namespace utm {

template <class T>
class usingleton
{
	static T* _self;
protected:
	usingleton() {};
	virtual ~usingleton() { _self = NULL; };
public:
	static T* instance();
};

template <class T>
T*  usingleton<T>::_self = NULL;

template <class T>
T*  usingleton<T>::instance()
{
  if (!_self)
    _self = new T;

  return _self;
}

}

#endif // _UTM_USINGLETON_H