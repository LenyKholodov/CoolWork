#ifndef __COOLWORK_MATHLIB__
#define __COOLWORK_MATHLIB__

#define _USE_MATH_DEFINES
#include <math.h>

//#pragma inline_depth (255)
//#pragma inline_recursion( on )
//#pragma auto_inline (on)

#include <math/base.h>
#include <math/vector.h>
#include <math/matrix.h>
#include <math/quat.h>
#include <math/utils.h>

#ifdef __MATHLIB_SSE__
  #include <xmmintrin.h>
  #include <math/sse/vec4f.h>
  #include <math/sse/vec4f.inl>
  #include <math/sse/matrix4f.inl>
  #include <math/sse/quatf.h>
  #include <math/sse/quatf.inl>
#endif

#include <math/vec_base.inl>
#include <math/vector.inl>
#include <math/matrix_base.inl>
#include <math/matrix.inl>
#include <math/quat_base.inl>
#include <math/quat.inl>
#include <math/utils.inl>
#include <math/affine.inl>

typedef vec<float,2>              vec2f;
typedef vec<double,2>             vec2d;
typedef vec<int,2>                vec2i;
typedef vec<unsigned int,2>       vec2ui;
typedef vec<short,2>              vec2s;
typedef vec<unsigned short,2>     vec2us;
typedef vec<char,2>               vec2b;
typedef vec<unsigned char,2>      vec2ub;

typedef vec<float,3>              vec3f;
typedef vec<double,3>             vec3d;
typedef vec<int,3>                vec3i;
typedef vec<unsigned int,3>       vec3ui;
typedef vec<short,3>              vec3s;
typedef vec<unsigned short,3>     vec3us;
typedef vec<char,3>               vec3b;
typedef vec<unsigned char,3>      vec3ub;

typedef vec<float,4>              vec4f;
typedef vec<double,4>             vec4d;
typedef vec<int,4>                vec4i;
typedef vec<unsigned int,4>       vec4ui;
typedef vec<short,4>              vec4s;
typedef vec<unsigned short,4>     vec4us;
typedef vec<char,4>               vec4b;
typedef vec<unsigned char,4>      vec4ub;

typedef matrix<float,2>           mat2f;
typedef matrix<float,3>           mat3f;
typedef matrix<float,4>           mat4f;
typedef matrix<double,2>          mat2d;
typedef matrix<double,3>          mat3d;
typedef matrix<double,4>          mat4d;
typedef matrix<int,2>             mat2i;
typedef matrix<int,3>             mat3i;
typedef matrix<int,4>             mat4i;

typedef quat<float>              quatf;
typedef quat<double>             quatd;
typedef quat<int>                quati;
typedef quat<unsigned int>       quatui;
typedef quat<short>              quats;
typedef quat<unsigned short>     quatus;
typedef quat<char>               quatb;
typedef quat<unsigned char>      quatub;

/*
#ifdef __ICL
  typedef class __declspec(align(16))  vec4<float>     vec4fa;
  typedef class __declspec(align(16))  matrix<float,4> matrix4fa;
#else
  typedef __declspec(align(16)) vec4f    vec4fa; 
  typedef __declspec(align(16)) matrix4f matrix4fa;
#endif
*/

#endif
