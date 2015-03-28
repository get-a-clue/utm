#ifndef _UTM_PROC_ROW_H
#define _UTM_PROC_ROW_H

#pragma once
#include <utm.h>

#include <proc_row_base.h>

namespace utm {

class proc_row : public proc_row_base
{
public:
	proc_row();
	~proc_row();
};

}

#endif // _UTM_PROC_ROW_H