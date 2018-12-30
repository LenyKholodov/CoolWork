#ifndef __COMMONLIB_STRING__
#define __COMMONLIB_STRING__

#include <stl/string>
#include <stl/vector>

/*
    ������� ������ � ������� �����
*/

stl::string basename (const char*);
stl::string suffix   (const char*);
stl::string dir      (const char*);
stl::string notdir   (const char*);
stl::string basename (const stl::string&);
stl::string suffix   (const stl::string&);
stl::string dir      (const stl::string&);
stl::string notdir   (const stl::string&);

/*
    ��������������� ����� � ������
*/

stl::string  format  (const char*,...);
stl::string  vformat (const char*,va_list);
stl::wstring format  (const wchar_t*,...);
stl::wstring vformat (const wchar_t*,va_list);

/*
    ������ ����������� �������� �� ������������� (\n, \r, etc.)
*/

stl::string compress   (const char*);
stl::string decompress (const char* str,const char* exception = NULL);

/*
    �����������
*/

unsigned int strhash  (const char*);
unsigned int istrhash (const char*);

/*
    ��������� ������
*/

//strip/trim/...

stl::vector<stl::string> split (const char* str,const char* delimiters=" ",const char* spaces=" \t");
stl::vector<stl::string> split (const stl::string& str,const char* delimiters=" ",const char* spaces=" \t");

/*
    ������ � ����������� �����������
*/

stl::vector<stl::string> parse (const char* string,const char* re_pattern);
stl::string replace (const char* string,const char* re_pattern,const char* replacement);

//bool strWildcardMatch (const char* pattern);
//bool strREMatch (const char* pattern);

#endif
