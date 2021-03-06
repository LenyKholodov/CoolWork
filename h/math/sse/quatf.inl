template <>
__forceinline void quat_assign_scalar (quat<float>& res,const float& a)
{
  res = _mm_set_ps1 (a);
}

template <>
__forceinline void quat_add (quat<float>& res,const quat<float>& a,const quat<float>& b) 
{ 
  res = _mm_add_ps (a,b);
}

template <>
__forceinline void quat_sub (quat<float>& res,const quat<float>& a,const quat<float>& b) 
{ 
  res = _mm_sub_ps (a,b);
}

template <>
__forceinline void quat_neg (quat<float>& res,const quat<float>& src)
{
  static union {
    int    i;
    float  f;  
  } mask = {0x80000000};

  res = _mm_xor_ps (src,_mm_set_ps1 (mask.f));
}

#ifdef __MATHLIB_SSE_ALIGN__

template <>
__forceinline void quat_copy (quat<float>& res,const quat<float>& src)
{
  res.p = src.p;
}

__forceinline quat_base<float>::quat_base<float> (const __m128& _p)
{
  p = _p;
}

__forceinline quat_base<float>& quat_base<float>::operator = (const __m128& _p)
{
  p = _p; 
  return *this;
}

__forceinline quat_base<float>& quat_base<float>::operator = (const quat_base<float>& v)
{
  p = v.p;
  return *this;
} 

#else

template <>
__forceinline void quat_copy (quat<float>& res,const quat<float>& src)
{
  _mm_storeu_ps (res,src); 
}

__forceinline quat_base<float>::operator const __m128 () const
{
  return _mm_loadu_ps (&x);
}

__forceinline quat_base<float>::quat_base<float> (const __m128& p)
{
  _mm_storeu_ps (&x,p);  
}

__forceinline quat_base<float>& quat_base<float>::operator = (const __m128& p)
{
  _mm_storeu_ps (&x,p);  
  return *this;
}

#endif