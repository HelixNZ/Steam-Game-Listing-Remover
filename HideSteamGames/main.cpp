#include <windows.h>
#include <iostream>
#include <conio.h>
#include <string>
#include <vector>

std::vector<std::string> QueryKey(HKEY hKey)
{
	char cKey[255];
	char cClass[MAX_PATH] = "";
	DWORD ulNameLen;
	DWORD ulClassNameLen = MAX_PATH;
	DWORD ulSubKeys = 0;
	std::vector<std::string> vecstrOutput;

	RegQueryInfoKeyA(hKey, cClass, &ulClassNameLen, NULL, &ulSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	for(unsigned long i = 0; i < ulSubKeys; i++)
	{
		ulNameLen = 255;

		if(RegEnumKeyExA(hKey, i, cKey, &ulNameLen, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
		{
			char strTest[10] = "\0";
			strncpy_s(strTest, cKey, 9);
			if(!strcmp(strTest, "Steam App")) vecstrOutput.push_back(cKey);
		}
	}

	return vecstrOutput;
}

int main()
{
	//Locations in Registry
	std::string str64bitLocation = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\";
	std::string str32bitLocation = "SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\";

	//Query 64bit uninstaller list
	HKEY h64bit;
	RegOpenKeyA(HKEY_LOCAL_MACHINE, str64bitLocation.c_str(), &h64bit);
	std::vector<std::string> vec64bit = QueryKey(h64bit);
	RegCloseKey(h64bit);

	//Query 32bit uninstaller list
	HKEY h32bit;
	RegOpenKeyA(HKEY_LOCAL_MACHINE, str32bitLocation.c_str(), &h32bit);
	std::vector<std::string> vec32bit = QueryKey(h32bit);
	RegCloseKey(h32bit);

	//Query user, don't care if there is nothing in the lists
	std::cout << "\n\n[USE AT YOUR OWN RISK, THIS PROGRAM EDITS YOUR REGISTRY]\n\n";
	std::cout << "I found " << vec64bit.size() + vec32bit.size() << " steam games existing in your programs and features list\n";
	std::cout << "Are you sure you want to remove them? y/n: ";

	//Input
	if(_getch() == 'y')
	{
		//User accepted
		std::cout << "y\n\nRemoving items...\n";

		//Remove 64bit keys
		for(unsigned int i = 0; i < vec64bit.size(); ++i)
		{
			std::cout << "\n\t[64bit] Removing: " << vec64bit[i];
			RegDeleteKeyA(HKEY_LOCAL_MACHINE, (str64bitLocation + vec64bit[i]).c_str());
		}

		//Remove 32bit keys
		for(unsigned int i = 0; i < vec32bit.size(); ++i)
		{
			std::cout << "\n\t[32bit] Removing: " << vec32bit[i];
			RegDeleteKeyA(HKEY_LOCAL_MACHINE, (str32bitLocation + vec32bit[i]).c_str());
		}

		std::cout << "\n\nRemoved " << vec64bit.size() + vec32bit.size() << " items...";
	}
	else
	{
		//User declined
		std::cout << "n\n\nNothing has been removed...";
	}

	//Hold console open
	_getch();
	return(0);
}