#ifndef __COOLWORK_MATHLIB_SSE_QUATERNION__
#define __COOLWORK_MATHLIB_SSE_QUATERNION__

#pragma pack(push,1)

#ifdef __MATHLIB_SSE_ALIGN__

template <> 
struct quat_base<float>
{
             quat_base  () {}
             quat_base  (const __m128&);

  quat_base& operator = (const __m128&);
  quat_base& operator = (const quat_base&);

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
struct quat_base<float>
{
             quat_base  () {}
             quat_base  (const __m128&);

  quat_base& operator = (const __m128&);

             operator float*       ()       { return &x; }
             operator const float* () const { return &x; }
             operator const __m128 () const;


  float  x,y,z,w;
};

#endif

#pragma pack(pop)

#endif
