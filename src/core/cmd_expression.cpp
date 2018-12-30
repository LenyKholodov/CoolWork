#include "expression.h"
#include <string>
#include <vector>
#include <stack>

using namespace std;

struct Expression::Impl {      //���������� ������ ���������
  Var&           var;          //������ �� ���������� ��������� � ����������
  string         expression;   //����������� ���������
  string         source;       //�������� ��������� (�� �������)
  vector<string> deps;         //������ ������������
  
  Impl (Var& _var) : var (_var) {}
};

Expression::Expression (Var& var)
{
  impl = new Impl (var);    
}

Expression::~Expression ()
{
  set ("");
  delete impl;
}

const char* Expression::source ()
{
  return impl->source.c_str ();
}

vec3f Expression::compute ()
{
  stack <vec3f> args;
  std::string outstring;
  bool var = false;
  int k = 0;
  
  Etc& etc  = impl->var.etc ();
  
  const char* a = impl->expression.c_str ();
  
//  logPrintf ("compute expression: %s=%s (parse='%s')",impl->var.name (),impl->source.c_str (),a);  

  while (a [k] != '\0')
  {
                      /* ���� ���p����� ������ - ����� , �� */
    if(((a[k] >= 'a') && (a[k] <= 'z')) || ((a[k] >= 'A') && (a[k] <= 'Z')) || ((a[k] >= '0') && (a[k] <= '9')) || (var && (a[k] == '.' || a[k] == '_')))    
    {           /* ��p��������� � � �������� ��p��� */    
      outstring.push_back(a[k]);
      var = true;
    }
    else if (var)
         {           
            const char* name = "__value";
          
           if (isdigit (outstring [0])) args.push ((float)atof (outstring.c_str ()));
           else                         
           {
             vec3f value;

             value = etc.get3f (name=outstring.c_str ());
             args.push (value);
             if (!etc.present(name))
               logPrintf("Unknown variable %s used in expression, replaced by default value", name);
           }
           
//           vec3f v = args.top ();
           
//           logPrintf ("\tpush %s=(%f,%f,%f)",name,v.x,v.y,v.z);
                                            
           outstring.clear();
           var = false;
         }
       else
         {
           var = false;         
         }

    if(a[k]=='+'||a[k]=='-'||a[k]=='/'||a[k]=='*')
    {
       vec3f fArg1, fArg2, result = 0.0f; 
       
       if(args.empty())
         fArg2 = 0;
       else
       {
         fArg2 = args.top ();              
         args.pop ();
       }
       
       if(args.empty())
         fArg1 = 0;
       else
       {
         fArg1 = args.top ();
         args.pop ();       
       }
       
       switch (a[k])
       {
          case '+': result = fArg1 + fArg2; break;
          case '-': result = fArg1 - fArg2; break;
          case '/': result = fArg1 / fArg2; break;
          case '*': result = fArg1 * fArg2; break;
       }
       
//       logPrintf ("\t(%f,%f,%f) %c (%f,%f,%f) = (%f,%f,%f)",
//       fArg1.x,fArg1.y,fArg1.z,a[k],fArg2.x,fArg2.y,fArg2.z,result.x,result.y,result.z);
       
       args.push (result);              
    }

    k++;                                    
  }  
  
  vec3f result = args.top ();
  
//  logPrintf ("\tresult=(%f,%f,%f)",result.x,result.y,result.z);

  return args.top ();
}

static int priority(char a)
{
  switch(a)
  {
    case '*':
    case '/':  return 3;
    case '-':
    case '+':  return 2;
    case '(':  return 1;
    default:   return 0;
  }
}

void Expression::set (const char* a)
{
  if (!a)  
    a = "";
    
  impl->source = a;
  impl->expression = "";

  Etc& etc  = impl->var.etc ();    
  const char* var_name = impl->var.name ();
    
  for (vector<string>::iterator i=impl->deps.begin ();i!=impl->deps.end ();i++)
    etc.remove_dep (var_name,i->c_str ());

  impl->deps.clear ();   
  
  if (!*a)
    return;

  /* ���� ���p���� ���� */
  stack <char> ops;
  bool var = false;
  int k;
  
  std::string& outstring = impl->expression;

  k=0;
  
        /* �����p��� , ���� �� ������ �� '=' */
  while(a[k]!='\0')           
  {
                      /* ���� ���p����� ������ - ����� , �� */
    if(((a[k] >= 'a') && (a[k] <= 'z')) || ((a[k] >= 'A') && (a[k] <= 'Z')) || ((a[k] >= '0') && (a[k] <= '9')) || (var && (a[k] == '.' || a[k] == '_')))    
//    if (isalpha (a [k]) || a [k] == '_' || var && (isdigit (a [k]) || a [k] == '.'))
    {           /* ��p��������� � � �������� ��p��� */
      outstring.push_back(a[k]);      
      
      var = true;
    }
    else if (var)
         {
           outstring.push_back(' ');           
           var = false;
         }
       else
         {
           var = false;         
         }
      /* ���� ���p����� ������ - ')' */
    if(a[k]==')')
                   /* �� ����������� �� ����� � �������� ��p��� */
    {                                     
                /* ��� ����� ���p���� �� ��������� */
      while(ops.top()!='(')         
      {         /* ���p������� ������ */
        outstring.push_back(ops.top());  
        outstring.push_back(' ');
        ops.pop();
      }
              /* ������� �� ����� ���� ���p������� ������ */
      ops.pop();
    }
                    /* ���� ���p����� ������ - '(' , �� */
    if(a[k]=='(')                         
                /* ����������� � � ���� */
        ops.push('(');           
    if(a[k]=='+'||a[k]=='-'||a[k]=='/'||a[k]=='*')
        /* ���� ��������� ������ - ���� ���p���� , ��: */
    {                             
                  /* ���� ���� ���� */
      if(ops.empty())                     
      {  /* ���������� � ���� ���p���� */
          ops.push(a[k]);        
      }
           /* ���� �� ���� */
      else                                 
/* ���� �p��p���� ����������� ���p���� ������ 
             �p��p����� ���p���� �� ��p���� ����� */
      if(priority(ops.top()) < priority(a[k]))      
      /* ����������� ����������� ���p���� �� ���� */             
          ops.push(a[k]);      
               /* ���� �p��p���� ������ */
      else                              
      {
        while((!ops.empty()) && (priority(ops.top()) >= priority(a[k])))
        {/* ��p��������� � �������� ��p��� ��� ���p����
                 � ������� ��� p����� �p��p������ */
          outstring.push_back(ops.top()); 
          outstring.push_back(' ');
          ops.pop();
        }
              /* ���������� � ���� �����������  ���p���� */
        ops.push(a[k]);           
      } 
    }
        /* ��p���� � ���������� ������� ������� ��p��� */
    k++;                                    
  }
  if (var)
    outstring.push_back(' ');
         /* ����� p������p���� ����� ��p������ */
  while(!ops.empty())                      
  {  /* ��p��������� ��� ���p���� �� */
    outstring.push_back(ops.top());
    outstring.push_back(' ');        
    ops.pop();    
  }  
  
  string::iterator first = outstring.begin ();
  bool identifier = false;

  for (string::iterator i=first;i!=outstring.end ();i++)
  {
    if (isalpha (*i) || *i == '_' || isdigit (*i) || *i == '.')
    {
      if (!identifier && (isalpha (*i) || *i == '_'))
      {
        first      = i;
        identifier = true;
      }
      
      continue;
    }

    if (identifier)
    {
      string s (first,i);
      
      etc.add_dep (var_name,s.c_str ());
      impl->deps.push_back (s);
          
      identifier = false;      
    }
  }
}
