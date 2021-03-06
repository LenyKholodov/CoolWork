#ifndef __COOLWORK_MATHLIB_VECTOR__
#define __COOLWORK_MATHLIB_VECTOR__

#pragma pack (push,1)

////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���� �� ���������
////////////////////////////////////////////////////////////////////////////////////////////
template <class type,int size> struct vec_base
{
  operator type*       ()       { return x; }
  operator const type* () const { return x; }
 
  type x [size];
};

template <class type> struct vec_base<type,2>
{
  operator type*       ()       { return &x; }
  operator const type* () const { return &x; }
 
  type x,y;
};

template <class type> struct vec_base<type,3>
{
  operator type*       ()       { return &x; }
  operator const type* () const { return &x; }
 
  type x,y,z;
};

template <class type> struct vec_base<type,4>
{
  operator type*       ()       { return &x; }
  operator const type* () const { return &x; }
 
  type x,y,z,w;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//������ 
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class type,int size>
class vec: public vec_base<type,size>
{
  public:
    typedef vec_base<type,size> base;

////////////////////////////////////////////////////////////////////////////////////////////
///������������
////////////////////////////////////////////////////////////////////////////////////////////
    vec () {}
    vec (const type&);
    vec (const base&);

      //��� ������� - �� ������, �� ��� ���� ����������
    vec (const type&,const type&,const type& = 0,const type& = 0); 
    vec (const vec<type,size-1>&,const type&);

      //��� ������������� ����������� ������������� ��������
    template <class T>           vec (const T&,void (*eval)(vec&,const T&));
    template <class T1,class T2> vec (const T1&,const T2&,void (*eval)(vec&,const T1&,const T2&));

    template <int size1>         vec (const vec<type,size1>&); 

////////////////////////////////////////////////////////////////////////////////////////////
///������� +,-, ����� (~)
////////////////////////////////////////////////////////////////////////////////////////////
    const vec&   operator +  () const;
    const vec    operator -  () const;
          type   operator ~  () const;

////////////////////////////////////////////////////////////////////////////////////////////
///������������
////////////////////////////////////////////////////////////////////////////////////////////
    vec&   operator =  (const type&); 
    vec&   operator =  (const base&);

    template <int size1> vec& operator = (const vec<type,size1>&); 

////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� ��������
////////////////////////////////////////////////////////////////////////////////////////////
    vec&      operator += (const vec&);
    vec&      operator -= (const vec&);
    vec&      operator *= (const vec&);
    vec&      operator /= (const vec&);
    vec&      operator *= (const type&);
    vec&      operator /= (const type&);
    const vec operator +  (const vec&) const;
    const vec operator -  (const vec&) const;
    const vec operator *  (const vec&) const;
    const vec operator /  (const vec&) const;
    const vec operator *  (const type&) const;
    const vec operator /  (const type&) const;

    friend const vec operator *  (const type& a,const vec& v)    { return v * a; }

////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������ �� ������� �������
////////////////////////////////////////////////////////////////////////////////////////////
    vec&      operator *= (const matrix<type,size>&);
    vec&      operator *= (const matrix<type,size+1>&);

    const vec operator *  (const matrix<type,size>&) const;
    const vec operator *  (const matrix<type,size+1>&) const;

////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� ���������           
////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const vec&) const;      
    bool operator != (const vec&) const;

////////////////////////////////////////////////////////////////////////////////////////////
///C�������� ������������
////////////////////////////////////////////////////////////////////////////////////////////
    type operator &  (const vec&) const;

////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����� (��������� ����������� �������)
////////////////////////////////////////////////////////////////////////////////////////////
    operator const vec<type,size-1>& () const;
    operator       vec<type,size-1>& ();
};

template <class type> class vec<type,0> {};

#pragma pack(pop)

////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������
////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
///�������
////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������
////////////////////////////////////////////////////////////////////////////////////////////
template <class type,int size> 
type dot (const vec<type,size>&,const vec<type,size>&); 

////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� � �������� ������������
////////////////////////////////////////////////////////////////////////////////////////////    
template <class type,int size>
bool equal (const vec<type,size>&,const vec<type,size>&,const type& eps); 

////////////////////////////////////////////////////////////////////////////////////////////
///����� �������
////////////////////////////////////////////////////////////////////////////////////////////                
template <class type,int size>                          
type length (const vec<type,size>&); 

////////////////////////////////////////////////////////////////////////////////////////////
///������� �����
////////////////////////////////////////////////////////////////////////////////////////////                
template <class type,int size>
type qlen (const vec<type,size>&);

////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������� ������
////////////////////////////////////////////////////////////////////////////////////////////                
template <class type,int size>                          
const vec<type,size> normalize (const vec<type,size>&);

////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������������� ������
////////////////////////////////////////////////////////////////////////////////////////////                
template <class type,int size>                          
const vec<type,size> abs (const vec<type,size>&); 

////////////////////////////////////////////////////////////////////////////////////////////
///�������������� �������/��������
////////////////////////////////////////////////////////////////////////////////////////////                
template <class type,int size>
const vec<type,size> vmin (const vec<type,size>&,const vec<type,size>&); 

template <class type,int size>
const vec<type,size> vmax (const vec<type,size>&,const vec<type,size>&); 

////////////////////////////////////////////////////////////////////////////////////////////
///���� ����� ���������
////////////////////////////////////////////////////////////////////////////////////////////
template <class type,int size>
type angle (const vec<type,size>& a,const vec<type,size>& b);

////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������
////////////////////////////////////////////////////////////////////////////////////////////
template <class type>
vec<type,3> cross (const vec<type,3>& a,const vec<type,3>& b);

template <class type>
vec<type,4> cross (const vec<type,4>& a,const vec<type,4>& b);

template <class type>
vec<type,3> operator ^ (const vec<type,3>& a,const vec<type,3>& b);

template <class type>
vec<type,4> operator ^ (const vec<type,4>& a,const vec<type,4>& b);

#endif
