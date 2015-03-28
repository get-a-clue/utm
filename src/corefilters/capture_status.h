#ifndef _CAPTURE_STATUS_H
#define _CAPTURE_STATUS_H

#pragma once
#include <utm.h>

#define	NON_CAPTURE 0
#define	IN_CAPTURE 1
#define	PAUSE_CAPTURE 2

#include <iostream>

namespace utm {

class capture_status
{
public:
	capture_status();
	~capture_status();

	bool operator==(const capture_status& rhs) const;

	bool is_running() const;
	bool is_pause() const;
	bool is_stopped() const;

	void set_running();
	void set_pause();
	void set_stopped();

	int get_status() const { return status; };
	const char *get_status_str() const;

	friend std::ostream& operator<<(std::ostream& s, const capture_status& rhs) { s << rhs.get_status(); return s; };
	friend std::istream& operator>>(std::istream& s, capture_status& obj);

private:
	volatile int status;
};

}

#endif // _CAPTURE_STATUS_H

