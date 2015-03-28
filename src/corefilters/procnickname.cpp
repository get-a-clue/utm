#include "StdAfx.h"
#include "procnickname.h"

namespace utm {

procnickname::procnickname(void)
{
}

procnickname::procnickname(const gstring& _nickname, const gstring& _pathexe)
{
	nid = 0;
	nickname = _nickname;
	pathexe = _pathexe;
}

procnickname::~procnickname(void)
{
}

}