#pragma once
#ifndef AP_PL

#include <string>
#include <vector>

class PlayListNode
{
public:
	PlayListNode();
	~PlayListNode();

	std::wstring m_name;


};

class PlayList
{
	mgf::ListBox* m_listBox = 0;
public:
	PlayList(mgf::ListBox*);
	~PlayList();
};

#endif // !AP_PL
