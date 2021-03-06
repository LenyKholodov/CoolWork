#ifndef __COMMONLIB_PARSER__
#define __COMMONLIB_PARSER__

#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ��������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ParseNode
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ParseNode* first () const;
    ParseNode* next  () const;
    ParseNode* first (const char*) const;
    ParseNode* next  (const char*) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* tag   () const;
    bool        test  (const char* name) const; //��������� ���������� �����

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t       argc () const;
    const char** argv () const;
    const char*  arg  (size_t i) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void error    (const char*,...);
    void warning  (const char*,...);
    void verror   (const char*,va_list);
    void vwarning (const char*,va_list);

  private:
    ParseNode ();
    ~ParseNode ();
    
  private:  
    char data [1];
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////          
enum ParseResult
{
  PARSE_OK = 0,   //������ � �������������� ���
  PARSE_WARNINGS, //���� ���������������
  PARSE_ERRORS    //���� ������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////          
class Parser
{
  public:
    Parser  (const char* file_name,const char* format=NULL);  //NULL - auto detect
    Parser  (const char* buf,size_t len,const char* format=NULL);
    ~Parser ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ParseNode* root () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////      
    ParseResult status () const; //��������� �������
    const char* log    () const; //���������� ������ � ��������� ������ (����������� '/n')

  private:
    struct ParseTree* tree;
};

#endif
