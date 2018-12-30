#ifndef __COOLWORK_MATHLIB_SSE_VECTOR__
#define __COOLWORK_MATHLIB_SSE_VECTOR__

#pragma pack(push,1)

////////////////////////////////////////////////////////////////////////////////////////////
///Адаптер для SSE-ориентированных векторных баз
////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __MATHLIB_SSE_ALIGN__
template <> 
struct vec_base<float,4>
{
             vec_base  () {}
             vec_base  (const __m128&);

  vec_base& operator = (const __m128&);
  vec_base& operator = (const vec_base&);

             operator float*        ()       { return &x; }
             operator const float*  () const { return &x; }
             operator const __m128& () const { return p;  }
             operator       __m128& ()       { return p;  }

  union {
    __m128 p;
    struct {
      float  x,y,z,w;
    };  
  };
};

#else

template <> 
struct vec_base<float,4>
{
             vec_base  () {}
             vec_base  (const __m128&);

  vec_base& operator = (const __m128&);

             operator float*       ()       { return &x; }
             operator const float* () const { return &x; }
             operator const __m128 () const;

  float  x,y,z,w;
};

#endif

#pragma pack(pop)

#endif
