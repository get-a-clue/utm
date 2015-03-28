#ifndef _UTM_PROTO_H
#define _UTM_PROTO_H

#pragma once

namespace utm {

struct uchar
{
	static const char umap[256][4];
};

struct uproto
{
	static const char* get_name(unsigned char proto);
};

}

#endif // _UTM_PROTO_H