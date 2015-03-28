#include "stdafx.h"
#include "capture_status.h"

namespace utm {

capture_status::capture_status() : status(NON_CAPTURE)
{
}

capture_status::~capture_status()
{
}

bool capture_status::operator==(const capture_status& rhs) const
{
	return status == rhs.status;
}

const char *capture_status::get_status_str() const
{
	if (is_running())
	{
		return "In capture";
	}
	else if (is_stopped())
	{
		return "No capture";
	}

	return "Pause capture";
}

bool capture_status::is_running() const
{
	return status == IN_CAPTURE;
}

bool capture_status::is_pause() const
{
	return status == PAUSE_CAPTURE;
}

bool capture_status::is_stopped() const
{
	return status == NON_CAPTURE;
}

void capture_status::set_running()
{
	status = IN_CAPTURE;
}

void capture_status::set_pause()
{
	status = PAUSE_CAPTURE;
}

void capture_status::set_stopped()
{
	status = NON_CAPTURE;
}

std::istream& operator>>(std::istream& s, capture_status& obj)
{
	char ch;
	char tmp[2] = { 0, 0 };

	while (!s.eof() && !s.fail())
	{
		s.get(ch);
		if (s.rdstate() & (std::ios::eofbit | std::ios::badbit | std::ios::failbit))
			break;

		if (isdigit(ch))
			tmp[0] = ch;
	}

	int i = atoi(tmp);

	if (i == IN_CAPTURE)
	{
		obj.set_running();
	}
	else if (i == PAUSE_CAPTURE)
	{
		obj.set_pause();
	}
	else
	{
		obj.set_stopped();
	}

	return s;
}

}