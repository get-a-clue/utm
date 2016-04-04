#include "stdafx.h"
#include "proc_list.h"

#include <filtercnt.h>
#include <ubase_test.h>

#ifdef UTM_WIN

#include <TlHelp32.h>
#include <Psapi.h>

namespace utm {

const char proc_list::this_class_name[] = "proc_list";
	
proc_list::proc_list(void)
{
}

proc_list::~proc_list(void)
{
}

void proc_list::init(proc_list_mode mode)
{
	if (proc_ptr.get() != NULL)
		return;

	this->mode = mode;
	proc_ptr = std::unique_ptr<proc_info[]>(new proc_info[UTM_MAX_PROC_COUNT]);
	if (mode == PROC_LIST_FULL_MODE)
	{
		proc2_ptr = std::unique_ptr<proc_info2[]>(new proc_info2[UTM_MAX_PROC_COUNT]);
	}

	OpenProcessDesiredAccess = PROCESS_QUERY_INFORMATION;

	OSVERSIONINFO osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (GetVersionEx(&osvi))
	{
		if (osvi.dwMajorVersion >= 6)
		{
			// Windows Vista and later
			OpenProcessDesiredAccess = PROCESS_QUERY_LIMITED_INFORMATION;
		}
	}
}

void proc_list::reset_counters()
{
	proc_info* ptr = proc_ptr.get();
	if (ptr == NULL)
		return;

	for (int i = 0; i < UTM_MAX_PROC_COUNT; i++)
	{
		ptr->reset_counters();
		ptr++;
	}
}

int proc_list::get_active_proc_size() const
{
	int retval = 0;

	proc_info* ptr = proc_ptr.get();
	if (ptr == NULL)
		return retval;

	for (int i = 0; i < UTM_MAX_PROC_COUNT; i++)
	{
		if (ptr->flag & 0x0001)
			retval++;

		ptr++;
	}

	return retval;
}

// Получаем список логических дисков в системе и их отображение в физические диски
// Например, "\Device\HarddiskVolume1" - "C:"
void proc_list::detect_drives(drivemap_container& drives)
{
	static const int MAX_DRIVE_LENGTH = 4;

	TCHAR szTemp[MAX_PATH];
	if (GetLogicalDriveStrings(MAX_PATH - 1, szTemp) > 0)
	{
		TCHAR szName[MAX_PATH];
		int drives_count = 0;
		while ((drives_count * 4) < (MAX_PATH - 2))
		{
			if ((szTemp[drives_count * MAX_DRIVE_LENGTH] == 0))
				break;

			szTemp[(drives_count * MAX_DRIVE_LENGTH) + 2] = 0;

			gstring drive_letter(&szTemp[drives_count * MAX_DRIVE_LENGTH]);
			if (QueryDosDevice(drive_letter.c_str(), szName, MAX_PATH))
			{
				gstring drive_path(szName);
				drives.insert(std::make_pair(drive_path.getmb(), drive_letter.getmb()));
			}

			drives_count++;
		};
	}
}

void proc_list::refresh(const procnicknamelist& procnicks, const userlist& users)
{
	if (proc_ptr.get() == NULL)
		return;

	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (h == NULL)
	{
		return;
	}

	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(h, &processEntry))
	{   
		CloseHandle(h);
        return;
    };

	drivemap_container drives;
	detect_drives(drives);

	{
		proc_info* pi = proc_ptr.get();
		for (int i = 0; i < UTM_MAX_PROC_COUNT; i++, pi++)
		{
			pi->flag = pi->flag & 0xFFFB;
		}
	}

	// Теперь пробегаемся по каждому процессу и пытаемся получить
	// полный путь исполняемого файла.
	// Ищем ник процесса по полному пути исполняемого файла.
	// Если ник процесса найден в таблице pProcNickNameStruct ppn,
	// то делаем соотвествующую отметку о найденном нике в таблице процессов (флаг 0x00000007).
	// Если процесс существует без ника, то флаг будет 0x00000005.

	TCHAR *p;
	DWORD pid, dwPidIndex;
	HANDLE hProcess;
	HANDLE hToken;

	TCHAR szExeFile[MAX_PATH];
	TCHAR szUserName[MAX_PATH];
	TCHAR szDevice[MAX_PATH];
	TCHAR szTemp[MAX_PATH];
	DWORD dwSize;

	TCHAR szNameTmp[MAX_PATH];
	TCHAR szDomainTmp[MAX_PATH];
	DWORD d1, d2;

	// Вычисляем полный путь к процессу svchost
	// чтобы в цикле сравнивать его с полученными полными путями процессов

	TCHAR szSvcHost[MAX_PATH];
	GetSystemDirectory(szSvcHost, MAX_PATH);
	_tcsncat_s(szSvcHost, MAX_PATH, _T("\\svchost.exe"), _TRUNCATE);

    do 
	{
		pid = processEntry.th32ProcessID;
		dwPidIndex = pid / 4;

		if (dwPidIndex >= UTM_MAX_PROC_COUNT)
			break;

		proc_info* pi = proc_ptr.get() + dwPidIndex;
		proc_info2* pi2 = proc2_ptr.get() + dwPidIndex;

		p = NULL;
		hProcess = OpenProcess(OpenProcessDesiredAccess, FALSE, pid);

		if (hProcess != NULL)
		{
			// Пытаемся получить полный путь файла исполняемого процесса

			szExeFile[0] = 0;
			dwSize = GetProcessImageFileName(hProcess, szExeFile, MAX_PATH);

			if (dwSize == 0)
			{
				p = &processEntry.szExeFile[0];
			}
			else
			{
				// Полный путь файла получен в виде
				// \Device\Harddisk1\Windows\System32\Winlogon.exe
				// Теперь преобразуем \Device\Harddisk1 в букву диска.

				p = &szExeFile[0];
				
				const TCHAR* t1 = _tcsstr(p + 1, _T("\\"));
				if (t1 != NULL)
				{
					const TCHAR* t2 = _tcsstr(t1 + 1, _T("\\"));
					if (t2 != NULL)
					{
						_tcsncpy_s(szDevice, p, (t2 - p));
						gstring gdevice(szDevice);
						std::string device = gdevice.getmb();
						
						drivemap_container::const_iterator iter = drives.find(device);
						if (iter != drives.end())
						{
							_tcscpy_s(szTemp, gstring(iter->second).c_str());
							_tcscat_s(szTemp, t2);
							_tcscpy_s(szExeFile, szTemp);
						}
					}
				}
			};

			// Пытаемся получить SID пользователя и его имя, которому принадлежит процесс

			if (OpenProcessToken(hProcess, TOKEN_QUERY, &hToken))
			{
				PTOKEN_USER ptu = NULL;
				DWORD dtu = 0;
				DWORD dw = 0;

				GetTokenInformation(hToken, TokenUser, ptu, dtu, &dw);

				if (dw > 0)
				{
					ptu = (PTOKEN_USER)malloc(dw);
					dtu = dw;

					GetTokenInformation(hToken, TokenUser, ptu,	dtu, &dw);

					PSID psid = ptu->User.Sid;
					{
						memset(&szNameTmp, 0, sizeof(szNameTmp));
						memset(&szDomainTmp, 0, sizeof(szDomainTmp));
						d1 = sizeof(szNameTmp);
						d2 = sizeof(szDomainTmp);

						SID_NAME_USE snu;

						if (LookupAccountSid(NULL, psid, szNameTmp, &d1, szDomainTmp, &d2, &snu))
						{
							_tcsncpy_s(szUserName, MAX_PATH, szDomainTmp, _TRUNCATE);
							_tcsncat_s(szUserName, MAX_PATH, _T("\\"), _TRUNCATE);
							_tcsncat_s(szUserName, MAX_PATH, szNameTmp, _TRUNCATE);

							gstring gusername(szUserName);

							if (mode == PROC_LIST_FULL_MODE)
							{
								pi2->username.reset();
								pi2->username = std::unique_ptr<gstring>(new gstring(gusername));
							}

							ubase* u = users.findptr_by_name(gusername, true);
							if (u != NULL)
							{
								utm::fsuser *user = dynamic_cast<utm::fsuser *>(u);
								if (user != NULL)
								{
									pi->filterset_uid = user->get_id();
								}
							}
						}
					}

					free(ptu);
				}

				CloseHandle(hToken);
			}

			CloseHandle(hProcess);
		}
		else
		{
			p = &processEntry.szExeFile[0];
		}

		// Указатель p - указатель на полный путь исполняемого процесса
		gstring exepath(p);

		{
			// Делаем отметку, что существует процесс с найденным pid

			pi->flag = 0x0005;

			if (mode == PROC_LIST_FULL_MODE)
			{
				pi2->exefile.reset();
				pi2->exefile = std::unique_ptr<gstring>(new gstring(exepath));
			}

			if (pid == 0)						// SystemIdleProcess
			{
				pi->nick_id = 1;
				pi->flag = 0x0007;
			} 
			else 
			if (pid == 4)						// System
			{
				pi->nick_id = 2;
				pi->flag = 0x0007;
			} 
			else
			if (_tcsicmp(p, szSvcHost) == 0)	// svchost
			{
				pi->nick_id = 3;
				pi->flag = 0x0007;
			}

			if (pi->flag != 0x0007)
			{
				// Теперь ищем никнейм процесса в массиве procnicks

				procnickname result_procnick;
				if (procnicks.find_by_pathexe(exepath, result_procnick))
				{
					pi->nick_id = static_cast<unsigned short>(result_procnick.get_id() & 0xffff);
					pi->flag = 0x0007;
				}
			}
		}

    } while(Process32Next(h, &processEntry));   

	CloseHandle(h);

	// Делаем пометку в m_proc для тех pid, которые не нашлись в памяти.

	{
		proc_info* pif = proc_ptr.get();
		proc_info2* pif2 = proc2_ptr.get();
		for (int i = 0; i < UTM_MAX_PROC_COUNT; i++, pif++, pif2++)
		{
			if ((pif->flag & 0x0004) == 0)
			{
				pif->clear();
				if (mode == PROC_LIST_FULL_MODE)
				{
					if (pif2->exefile.get() != NULL) pif2->exefile.reset();
					if (pif2->username.get() != NULL) pif2->username.reset();
				}
			}
		}
	}
}

void proc_list::refresh_speed(unsigned int interval, const utimestamp& current_timestamp)
{
	proc_info* ptr = proc_ptr.get();
	if (ptr == NULL)
		return;

	std::uint64_t delta_sent, delta_recv;
	unsigned int speed;

	for (int i = 0; i < UTM_MAX_PROC_COUNT; i++)
	{
		delta_sent = ptr->bytes_sent - ptr->bytes_sent_prev;
		speed = static_cast<unsigned int>(delta_sent / interval);
		ptr->speed_sent = filtercnt::pack_speed(speed);
		ptr->bytes_sent_prev = ptr->bytes_sent_prev + delta_sent;

		delta_recv = ptr->bytes_recv - ptr->bytes_recv_prev;
		speed = static_cast<unsigned int>(delta_recv / interval);
		ptr->speed_recv = filtercnt::pack_speed(speed);
		ptr->bytes_recv_prev = ptr->bytes_recv_prev + delta_recv;

		if ((delta_sent > 0) || (delta_recv > 0))
		{
			ptr->last_activity = current_timestamp;
		}

		ptr++;
	}
}

bool proc_list::check_proc_exist(unsigned int pid) const
{
	unsigned int pid_index = pid / 4;

	if (pid_index < UTM_MAX_PROC_COUNT)
	{
		proc_info* pif = proc_ptr.get();
		if (pif[pid_index].flag & 0x0001)
			return true;
	}

	return false;
}

proc_info* proc_list::get_procptr_by_pid(unsigned int pid) const
{
	unsigned int pid_index = pid / 4;

	if (pid_index < UTM_MAX_PROC_COUNT)
	{
		proc_info* pif = proc_ptr.get() + pid_index;
		return pif;
	}

	return NULL;
}

#ifdef UTM_DEBUG
void proc_list::test_print()
{
	unsigned int c = 0;

	proc_info* pif = proc_ptr.get();
	proc_info2* pif2 = proc2_ptr.get();
	for (int i = 0; i < UTM_MAX_PROC_COUNT; i++, pif++, pif2++)
	{
		if (pif->flag & 0x0001)
		{
			std::string username;
			if (pif2->username.get() != NULL)
			{
				username = pif2->username.get()->getmb();
			}

			std::string exepath;
			if (pif2->exefile.get() != NULL)
			{
				exepath = pif2->exefile.get()->getmb();
			}

			std::cout << c << "\t" << i*4 << "\t" << username.c_str() << "\t" << exepath.c_str() << std::endl;
			c++;
		}
	}
}

void proc_list::test_all()
{
	const char* ntauth = "NT AUTHORITY\\system";
	const char* ntfake = "NT AUTHORITY\\Fake User";

	utm::proc_list p;
	p.init(utm::proc_list::PROC_LIST_FULL_MODE);

	procnicknamelist plist;
	procnickname pnn1(gstring("Winlogon"), gstring("c:\\Windows\\System32\\winlogon.exe"));
	unsigned int pnid1 = plist.add_element(pnn1);
	procnickname pnn2(gstring("Smss"), gstring("c:\\Windows\\System32\\smss.exe"));
	unsigned int pnid2 = plist.add_element(pnn2);

	utm::userlist ulist;
	fsuser u1(gstring("NT AUTHORITY\\system"));
	unsigned int puid1 = ulist.add_element(u1);

	p.refresh(plist, ulist);

	bool process[4] = { 0, 0, 0, 0 };
	unsigned int nid[4] = { 0, 0, 0, 0 };
	unsigned int uid[4] = { 0, 0, 0, 0 };

	proc_info* pif = p.proc_ptr.get();
	proc_info2* pif2 = p.proc2_ptr.get();
	for (int i = 0; i < UTM_MAX_PROC_COUNT; i++, pif++, pif2++)
	{
		if (pif->flag & 0x0001)
		{
			gstring username;
			if (pif2->username.get() != NULL) username = *pif2->username.get();

			gstring exepath;
			if (pif2->exefile.get() != NULL) exepath = *pif2->exefile.get();

			if ((exepath.is_ci_equal("c:\\Windows\\System32\\winlogon.exe")) && (username.is_ci_equal(ntauth)))
			{
				process[0] = true;
				nid[0] = pif->nick_id;
				uid[0] = pif->filterset_uid;
			}

			if ((exepath.is_ci_equal("c:\\Windows\\System32\\smss.exe")) && (username.is_ci_equal(ntauth)))
			{
				process[1] = true;
				nid[1] = pif->nick_id;
				uid[1] = pif->filterset_uid;
			}

			if ((exepath.is_ci_equal("c:\\Windows\\System32\\svchost.exe")) && (username.is_ci_equal(ntauth)))
			{
				process[2] = true;
				nid[2] = pif->nick_id;
				uid[2] = pif->filterset_uid;
			}

			if ((exepath.is_ci_equal("c:\\Windows\\System32\\svchost.exe")) && (username.is_ci_equal(ntfake)))
			{
				process[3] = true;
				nid[3] = pif->nick_id;
				uid[3] = pif->filterset_uid;
			}
		}
	}

	test_case::classname.assign(this_class_name);

	TEST_CASE_CHECK(process[0], bool(true));
	TEST_CASE_CHECK(nid[0], pnid1);
	TEST_CASE_CHECK(uid[0], puid1);

	TEST_CASE_CHECK(process[1], bool(true));
	TEST_CASE_CHECK(nid[1], pnid2);
	TEST_CASE_CHECK(uid[1], puid1);

	TEST_CASE_CHECK(process[2], bool(true));
	TEST_CASE_CHECK(nid[2], unsigned int(3));
	TEST_CASE_CHECK(uid[2], puid1);

	TEST_CASE_CHECK(process[3], bool(false));
	TEST_CASE_CHECK(nid[3], unsigned int(0));
	TEST_CASE_CHECK(uid[3], unsigned int(0));
}
#endif

}

#endif
