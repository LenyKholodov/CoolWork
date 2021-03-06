/*
        �������������� ��������� ��������
*/

template <class T,int size> 
void vec_add (vec<T,size>& res,const vec<T,size>& a,const vec<T,size>& b)
{
  for (int i=0;i<size;i++) res [i] = a [i] + b [i];
}

template <class T,int size> 
void vec_sub (vec<T,size>& res,const vec<T,size>& a,const vec<T,size>& b)
{
  for (int i=0;i<size;i++) res [i] = a [i] - b [i];
}

template <class T,int size> 
void vec_mul (vec<T,size>& res,const vec<T,size>& a,const vec<T,size>& b)
{
  for (int i=0;i<size;i++) res [i] = a [i] * b [i];
}

template <class T,int size> 
void vec_div (vec<T,size>& res,const vec<T,size>& a,const vec<T,size>& b)
{
  for (int i=0;i<size;i++) res [i] = a [i] / b [i];
}

template <class T,int size> 
void vec_mul_scalar (vec<T,size>& res,const vec<T,size>& a,const T& b)
{
  for (int i=0;i<size;i++) res [i] = a [i] * b;
}

template <class T,int size> 
void vec_div_scalar (vec<T,size>& res,const vec<T,size>& a,const T& b)
{
  vec_mul_scalar (res,a,T(1)/b);
}

/*
        ��������� ������������
*/

template <class T,int size,int size1> 
void vec_copy (vec<T,size>& res,const vec<T,size1>& src)
{
  int s1 = size<size1?size:size1, s2 = (size>size1?size:size1)-s1, i;

  for (i=0;i<s1;i++) res [i] = src [i];
  for (;i<s2;i++) res [i] = T(0);
}

template <class T,int size> 
void vec_assign_scalar (vec<T,size>& res,const T& a)
{
  for (int i=0;i<size;i++) res [i] = a;
}

/*
        �������
*/

template <class T,int size> 
T vec_dot_product (const vec<T,size>& a,const vec<T,size>& b)
{
  T result = T (0);

  for (int i=0;i<size;i++)
    result += a [i] * b [i];

  return result;  
}

template <class T,int size> 
void vec_neg (vec<T,size>& res,const vec<T,size>& src)
{
  for (int i=0;i<size;i++) res [i] = -src [i]; 
}

template <class T,int size> 
void vec_abs (vec<T,size>& res,const vec<T,size>& src)
{
  for (int i=0;i<size;i++) res [i] = src [i] < T (0) ? -src [i] : src [i];
}

template <class T,int size> 
void vec_min (vec<T,size>& res,const vec<T,size>& a,const vec<T,size>& b)
{
  for (int i=0;i<size;i++) res [i] = a [i] < b [i] ? a [i] : b [i];
}

template <class T,int size> 
void vec_max (vec<T,size>& res,const vec<T,size>& a,const vec<T,size>& b)
{
  for (int i=0;i<size;i++) res [i] = a [i] > b [i] ? a [i] : b [i];
}

template <class T,int size> 
bool vec_equal (const vec<T,size>& a,const vec<T,size>& b)
{
  for (int i=0;i<size;i++)
    if (a [i] != b [i])
      return false;

  return true;
}

template <class T,int size> 
bool vec_nequal (const vec<T,size>& a,const vec<T,size>& b)
{
  for (int i=0;i<size;i++)
    if (a [i] != b [i])
      return true;

  return false;
}

template <class T,int size> 
bool vec_equal (const vec<T,size>& a,const vec<T,size>& b,const T& eps)
{
  vec<T,size> tmp;

  vec_sub (tmp,a,b);
  vec_abs (tmp,tmp);

  for (int i=0;i<size;i++)
    if (tmp [i] > eps)
      return false;  

  return true;  
} 

/*
        ����� � ������������
*/

template <class T,int size> 
T vec_length (const vec<T,size>& v)
{
  return sqrt (vec_qlength (v));
}

template <int size>
int vec_length (const vec<int,size>& v)
{
  return (int)sqrtf ((float)vec_qlength (v));
}

template <int size>
float vec_length (const vec<float,size>& v)
{
  return sqrtf (vec_qlength (v));
}

template <int size>
long double vec_length (const vec<long double,size>& v)
{
  return sqrtl (vec_qlength (v));
}

template <class T,int size> 
T vec_qlength (const vec<T,size>& v)
{
  return vec_dot_product (v,v);
}

template <class T,int size> 
void vec_normalize (vec<T,size>& res,const vec<T,size>& src)
{
  vec_div_scalar (res,src,vec_length (src));
}

/*
    ��������� ������������
*/

template <class T>
void vec_cross_product (vec<T,3>& res,const vec<T,3>& a,const vec<T,3>& b)
{
  res [0] = a [1] * b [2] - b [1] * a [2];
  res [1] = b [0] * a [2] - b [2] * a [0];
  res [2] = a [0] * b [1] - b [0] * a [1];
}

template <class T>
void vec_cross_product (vec<T,4>& res,const vec<T,4>& a,const vec<T,4>& b)
{
  vec_cross_product ((vec<T,3>&)res,(const vec<T,3>&)a,(const vec<T,3>&)b);
  
  res [3] = T (0);
}
