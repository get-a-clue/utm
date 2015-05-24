#include "stdafx.h"
#include "filteragent.h"

namespace utm {

filteragent::filteragent()
{
}


filteragent::~filteragent()
{
}

void filteragent::create(unsigned int total_points, unsigned int next_point, unsigned int array_elements_count, const filter2& f, std::string& xml)
{
	filter_id = f.m_id;
	filter_name = f.m_szFilterName;
	disabled = f.m_bDisable;
	speed = f.m_nSpeed;
	traffic_limit_mb = f.m_nTrafficLimitMb;
	traffic_limit_type = f.m_nTrafficLimitType;
	bytes_sent = f.cnt_sent.get_cnt();
	bytes_recv = f.cnt_recv.get_cnt();
	f.cnt_sent.select_speedarray_base64(total_points, next_point, array_elements_count, speed_array_sent);
	f.cnt_recv.select_speedarray_base64(total_points, next_point, array_elements_count, speed_array_recv);

	xml_create();
	xml_get_string(xml, false);
}

}