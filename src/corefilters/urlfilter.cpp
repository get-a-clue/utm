#include "StdAfx.h"
#include "urlfilter.h"

#include <stringtools.h>
#include <ubase_test.h>

namespace utm {

const char urlfilter::this_class_name[] = "urlfilter";

urlfilter::urlfilter(void)
{
}

urlfilter::~urlfilter(void)
{
}

//void urlfilter::xml_catch_value(const char *keyname, const char *keyvalue)
//{
//	urlfilter_base::xml_catch_value(keyname, keyvalue);
//}

std::string urlfilter::get_urlrules_str() const
{
	const static std::string delim(" ");
	return get_urlrules_str(delim);
}

std::string urlfilter::get_urlrules_str(const std::string& delimiter) const
{
	std::string retval;
	retval.reserve(urlrules.size() * 12);

	urlrules_container::const_iterator iter;
	for (iter = urlrules.begin(); iter != urlrules.end(); ++iter)
	{
		if (iter != urlrules.begin())
		{
			retval.append(delimiter);
		}

		retval.append(*iter);
	}

	return retval;
}

void urlfilter::parse_urlrules_str(const char *p)
{
	parse_urlrules_str(p, ' ');
}

void urlfilter::parse_urlrules_str(const char *p, char delimiter)
{
	std::vector<std::string> tmp_urlrules;
	stringtools::split(tmp_urlrules, p, delimiter);
	for (std::vector<std::string>::iterator iter = tmp_urlrules.begin(); iter != tmp_urlrules.end(); ++iter)
	{
		urlrules.push_back(*iter);
	}
}

bool urlfilter::check_fid(unsigned int filter_id) const
{
	return fids.check_id(filter_id);
}

int urlfilter::match(const char *host, const char *uri, const char *hosturi) const
{
	int result = URLFILTERING_NO_MATCH;
	const char *p;
	int n;

	if (!is_disabled)
	{
		urlrules_container::const_iterator iter;
		for (iter = urlrules.begin(); iter != urlrules.end(); ++iter)
		{
			p = NULL;

			if ((addr_part & URLFILTER_ADDRPART_HOST) && (addr_part & URLFILTER_ADDRPART_URI))
			{
				p = hosturi;
			}
			else
			{
				if (addr_part & URLFILTER_ADDRPART_HOST) p = host;
				if (addr_part & URLFILTER_ADDRPART_URI) p = uri;
			}

			if (p != NULL)
			{
				n = match_rule((*iter).c_str(), p);
				if (n != URLFILTERING_NO_MATCH)
				{
					result = n;
					break;
				}
			}
		}
	}

	return result;
}

int urlfilter::match_rule(const char* rule_str, const char* urladdr)
{
	size_t outpos = 0;
	return match_rule(rule_str, urladdr, &outpos);
}

//  Проверка правила URL Filter на срабатывание
//  urladdr - указатель на фрагмент URL-адреса (имя хоста или URI)
int urlfilter::match_rule(const char* rule_str, const char *urladdr, size_t* poutpos)
{
	*poutpos = 0;
	int retval = URLFILTERING_NO_MATCH;

	bool is_positive = false;
	bool is_matchbegin = false;
	bool is_matchend = false;

	size_t rule_str_len = strlen(rule_str);
	if (rule_str_len == 0)
	{
		return retval;
	}

	// Если URL Rule начинается с '#',
	// то это считается комментарием и такое правило не срабатывает

	if (rule_str[0] == '#')
	{
		return retval;
	}

	// Копируем правило в строку tmp, поскольку правило может быть
	// модифицировано перед использованием.

	char* rule_tmp = new char[rule_str_len + 1];

#ifdef _MSC_VER
	strncpy_s(rule_tmp, rule_str_len + 1, rule_str, rule_str_len);
#else
	strcpy(rule_tmp, rule_str);
#endif

	// Если правило оканчивается на символ '$', это означает
	// "Match the end of the string", однако в строке tmp символ '$'
	// фигурировать не должен, поскольку в поиске не участвует.

	if (rule_tmp[rule_str_len - 1] == '$')
	{
		rule_tmp[rule_str_len - 1] = 0;
		is_matchend = true;
	}

	// r - указатель на правило, которое будет использоваться
	// при поиске с помощью функции strstr.

	char *r = rule_tmp;

	if (*r == '+')
	{
		r++;
		is_positive = true;
	}

	// Символ - "Match the beginning of the string"

	if (*r == '^')
	{
		r++;
		is_matchbegin = true;
	}

	size_t urladdr_len = strlen(urladdr);

	if (strcmp(r, "*") == 0)
	{
		retval = URLFILTERING_BLOCK_MATCH;
	}
	else
	{
		const char *a = strchr(r, '*');
		if (a != NULL)
		{
			size_t leftpart_len = a - r;
			size_t rightpart_len = rule_str_len - leftpart_len;

			r[leftpart_len] = 0;

			const char* leftpart = r;
			const char* rightpart = a + 1;

			int leftpart_match = URLFILTERING_NO_MATCH;
			int rightpart_match = URLFILTERING_NO_MATCH;

			size_t leftpart_pos = 0;
			size_t rightpart_pos = 0;

			if (leftpart_len == 0)
			{
				leftpart_match = URLFILTERING_BLOCK_MATCH;
			}
			else
			{
				leftpart_match = match_rule(leftpart, urladdr, &leftpart_pos);
				if ((is_matchbegin) && (leftpart_match == URLFILTERING_BLOCK_MATCH))
				{
					if (leftpart_pos != 0)
					{
						leftpart_match = URLFILTERING_NO_MATCH;
					}
				}
			}

			if (rightpart_len == 0)
			{
				rightpart_match = URLFILTERING_BLOCK_MATCH;
			}
			else
			{
				rightpart_match = match_rule(rightpart, urladdr, &rightpart_pos);
				if ((is_matchend) && (rightpart_match == URLFILTERING_BLOCK_MATCH))
				{
					if (urladdr_len < rightpart_len)
					{
						rightpart_match = URLFILTERING_NO_MATCH;
					}
					else
					{
						if (strncmp(urladdr + urladdr_len - rightpart_len, rightpart, rightpart_len) != 0)
						{
							rightpart_match = URLFILTERING_NO_MATCH;
						}					
					}
				}
			}

//			if (leftpart_pos < rightpart_pos)
			{
				if ((leftpart_match == URLFILTERING_BLOCK_MATCH) && (rightpart_match == URLFILTERING_BLOCK_MATCH))
				{
					retval = URLFILTERING_BLOCK_MATCH;
				}
			}
		}
		else
		{
			// Поиск вхождения входной строки в правило URL
			
			const char *p = strstr(urladdr, r);
			while((is_matchend) && (p != NULL))
			{
				// Если найдено вхождение и данное правило должно
				// "Match the end of the string", то будем повторять
				// поиск до последнего вхождения.

				if (strstr(p + 1, r) != NULL)
				{
					p = strstr(p + 1, r);
				}
				else
				{
					break;
				};
			}

			bool flag = false;

			if (p != NULL)
			{
				*poutpos = p - urladdr;

				// Вхождение найдено, но это еще не означает
				// полный успех. Для полного успеха нужно еще выполнить
				// два пограничных условия ('^' и '$')

				if (is_matchbegin)
				{
					if (p != urladdr)
					{
						flag = true;
					}
				}

				if (is_matchend)
				{
					if ((*(p + strlen(r))) != 0)
					{
						flag = true;
					}
				}

				if (!flag)
				{
					retval = URLFILTERING_BLOCK_MATCH;
				}
			}
		}
	}

	// Если впереди правила стоит символ '+',
	// это будет означать 'positive match'

	if ((is_positive) && (retval == URLFILTERING_BLOCK_MATCH))
	{
		retval = URLFILTERING_POSITIVE_MATCH;
	}

	delete[] rule_tmp;

	return retval;
}

#ifdef UTM_DEBUG
void urlfilter::test_all()
{
	test_case::classname.assign(this_class_name);

	TEST_CASE_CHECK(urlfilter::match_rule("#mail", "mail.ru"), URLFILTERING_NO_MATCH);
	TEST_CASE_CHECK(urlfilter::match_rule("mail", "mail.ru"), URLFILTERING_BLOCK_MATCH);
	TEST_CASE_CHECK(urlfilter::match_rule("+mail", "mail.ru"), URLFILTERING_POSITIVE_MATCH);
	TEST_CASE_CHECK(urlfilter::match_rule("mail", "yandex.ru"), URLFILTERING_NO_MATCH);
	TEST_CASE_CHECK(urlfilter::match_rule("mail.ru$", "www.mail.ru"), URLFILTERING_BLOCK_MATCH);
	TEST_CASE_CHECK(urlfilter::match_rule("mail.ru$", "www.mail.ru.ru"), URLFILTERING_NO_MATCH);
	TEST_CASE_CHECK(urlfilter::match_rule("mail*ru", "mail.ru"), URLFILTERING_BLOCK_MATCH);
	TEST_CASE_CHECK(urlfilter::match_rule("mail*ru", "mail.yandex.ru"), URLFILTERING_BLOCK_MATCH);
	TEST_CASE_CHECK(urlfilter::match_rule("mail*ya*ru", "mail.yandex.ru"), URLFILTERING_BLOCK_MATCH);
	TEST_CASE_CHECK(urlfilter::match_rule("mail*ya*ru", "www.mail.yandex.ru"), URLFILTERING_BLOCK_MATCH);
	TEST_CASE_CHECK(urlfilter::match_rule("^mail.ru", "www.mail.ru"), URLFILTERING_NO_MATCH);
	TEST_CASE_CHECK(urlfilter::match_rule("^mail.ru.ru", "mail.ru"), URLFILTERING_NO_MATCH);
	TEST_CASE_CHECK(urlfilter::match_rule("^mail.ru", "mail.ru.ru"), URLFILTERING_BLOCK_MATCH);

	return;
}
#endif

}