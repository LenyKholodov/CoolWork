template <class T,int size>  class vec;
template <class T,int size>  class matrix;
template <class T>           class quat;

/*
        �������
*/

///�������� ��������
template <class T,int size> void vec_add (vec<T,size>& res,const vec<T,size>& a,const vec<T,size>& b);
template <class T,int size> void vec_sub (vec<T,size>& res,const vec<T,size>& a,const vec<T,size>& b);
template <class T,int size> void vec_mul (vec<T,size>& res,const vec<T,size>& a,const vec<T,size>& b);
template <class T,int size> void vec_div (vec<T,size>& res,const vec<T,size>& a,const vec<T,size>& b);

template <class T,int size> void vec_mul_scalar (vec<T,size>& res,const vec<T,size>& a,const T& b);
template <class T,int size> void vec_div_scalar (vec<T,size>& res,const vec<T,size>& a,const T& b);

///������������ / �����������
template <class T,int size,int size1> void vec_copy          (vec<T,size>& res,const vec<T,size1>& src);
template <class T,int size>           void vec_assign_scalar (vec<T,size>& res,const T& value);

///��������� 
template <class T,int size> bool vec_equal  (const vec<T,size>& a,const vec<T,size>& b);
template <class T,int size> bool vec_equal  (const vec<T,size>& a,const vec<T,size>& b,const T& eps);
template <class T,int size> bool vec_nequal (const vec<T,size>& a,const vec<T,size>& b);

///��������� �����
template <class T,int size> void vec_neg (vec<T,size>& res,const vec<T,size>& src);

///�������
template <class T,int size> void  vec_abs       (vec<T,size>& res,const vec<T,size>& src);
template <class T,int size> void  vec_min       (vec<T,size>& res,const vec<T,size>& a,const vec<T,size>& b);
template <class T,int size> void  vec_max       (vec<T,size>& res,const vec<T,size>& a,const vec<T,size>& b);
template <class T,int size> T     vec_length    (const vec<T,size>&);
template <class T,int size> T     vec_qlength   (const vec<T,size>&);
template <class T,int size> void  vec_normalize (vec<T,size>& res,const vec<T,size>& src);

///��������� ������������
template <class T,int size> T vec_dot_product (const vec<T,size>& a,const vec<T,size>& b);

//��������� ������������
template <class T>  void  vec_cross_product (vec<T,3>& res,const vec<T,3>& a,const vec<T,3>& b);
template <class T>  void  vec_cross_product (vec<T,4>& res,const vec<T,4>& a,const vec<T,4>& b); //???

/*
        �������
*/

///�������� ��������
template <class T,int size> void matrix_add (matrix<T,size>& res,const matrix<T,size>& a,const matrix<T,size>& b);
template <class T,int size> void matrix_sub (matrix<T,size>& res,const matrix<T,size>& a,const matrix<T,size>& b);

/*
  ��������� ������� �� �������
    rr - ������ �� ������
    cc - ������� �� �������
    rc - ������ �� ������� 
    cr - ������� �� ������
*/
template <class T,int size> void matrix_mul_rr (matrix<T,size>& res,const matrix<T,size>& a,const matrix<T,size>& b);
template <class T,int size> void matrix_mul_cc (matrix<T,size>& res,const matrix<T,size>& a,const matrix<T,size>& b);
template <class T,int size> void matrix_mul_rc (matrix<T,size>& res,const matrix<T,size>& a,const matrix<T,size>& b);
template <class T,int size> void matrix_mul_cr (matrix<T,size>& res,const matrix<T,size>& a,const matrix<T,size>& b);

template <class T,int size> void matrix_mul_rr (matrix<T,size>& a,const matrix<T,size>& b); //*=
template <class T,int size> void matrix_mul_cc (matrix<T,size>& a,const matrix<T,size>& b); //*=
template <class T,int size> void matrix_mul_rc (matrix<T,size>& a,const matrix<T,size>& b); //*=
template <class T,int size> void matrix_mul_cr (matrix<T,size>& a,const matrix<T,size>& b); //*=

//�� ��������� ������ �� �������
template <class T,int size> void matrix_mul (matrix<T,size>& res,const matrix<T,size>& a,const matrix<T,size>& b);
template <class T,int size> void matrix_mul (matrix<T,size>& a,const matrix<T,size>& b); //*=

template <class T,int size> void matrix_mul_scalar (matrix<T,size>& res,const matrix<T,size>& a,const T& b);
template <class T,int size> void matrix_div_scalar (matrix<T,size>& res,const matrix<T,size>& a,const T& b);

///������������ / �����������
template <class T,int size> void matrix_copy          (matrix<T,size>& res,const matrix<T,size>& src);
template <class T,int size> void matrix_assign_scalar (matrix<T,size>& res,const T& value);

///��������� 
template <class T,int size> bool matrix_equal  (const matrix<T,size>& a,const matrix<T,size>& b);
template <class T,int size> bool matrix_equal  (const matrix<T,size>& a,const matrix<T,size>& b,const T& eps);
template <class T,int size> bool matrix_nequal (const matrix<T,size>& a,const matrix<T,size>& b);

///��������� �����
template <class T,int size> void matrix_neg (matrix<T,size>& res,const matrix<T,size>& src);

///�������
template <class T,int size> T     matrix_det       (const matrix<T,size>&);
template <class T,int size> T     matrix_minor     (const matrix<T,size>&,int,int);
template <class T,int size> void  matrix_transpose (matrix<T,size>& res,const matrix<T,size>& src);
template <class T,int size> void  matrix_transpose (matrix<T,size>&);
template <class T,int size> void  matrix_invert    (matrix<T,size>& res,const matrix<T,size>& src);
template <class T,int size> void  matrix_invert    (matrix<T,size>&);
template <class T,int size> void  matrix_normalize (matrix<T,size>& res,const matrix<T,size>& src);
template <class T,int size> void  matrix_normalize (matrix<T,size>&);

/*
   ��������� ������� �� ������     
     1. ������� ������ �� ������ �������
     2. ������� ������� �� ������ ������
*/
template <class T,int size> void  matrix_mul_vec (vec<T,size>&,const matrix<T,size>&,const vec<T,size>&);
template <class T,int size> void  vec_mul_matrix (vec<T,size>&,const vec<T,size>&,const matrix<T,size>&);

//��� ����� ��������
template <class T,int size> void  matrix_mul_vec (vec<T,size>&,const matrix<T,size+1>&,const vec<T,size>&);
template <class T,int size> void  vec_mul_matrix (vec<T,size>&,const vec<T,size>&,const matrix<T,size+1>&);

/*
        �����������
*/

///�������� ��������
template <class T> void quat_add (quat<T>& res,const quat<T>& a,const quat<T>& b);
template <class T> void quat_sub (quat<T>& res,const quat<T>& a,const quat<T>& b);
template <class T> void quat_mul (quat<T>& res,const quat<T>& a,const quat<T>& b);
template <class T> void quat_mul (quat<T>& res,const quat<T>& q);

template <class T> void quat_mul_scalar (quat<T>& res,const quat<T>& a,const T& b);
template <class T> void quat_div_scalar (quat<T>& res,const quat<T>& a,const T& b);

///������������ / �����������
template <class T> void quat_copy          (quat<T>& res,const quat<T>& src);
template <class T> void quat_assign_scalar (quat<T>& res,const T& value);

///��������� 
template <class T> bool quat_equal  (const quat<T>& a,const quat<T>& b);
template <class T> bool quat_equal  (const quat<T>& a,const quat<T>& b,const T& eps);
template <class T> bool quat_nequal (const quat<T>& a,const quat<T>& b);

///��������� �����
template <class T> void quat_neg (quat<T>& res,const quat<T>& src);

///�������
template <class T> T     quat_length    (const quat<T>&);
template <class T> T     quat_norm      (const quat<T>&);
template <class T> void  quat_normalize (quat<T>& res,const quat<T>& src);

///��������� ������������
template <class T> T quat_inner (const quat<T>& a,const quat<T>& b);

///��������� ����������� �� ������ (�������)
template <class T> void  quat_mul_vec (vec<T,3>& res,const quat<T>& q,const vec<T,3>& v);
template <class T> void  quat_mul_vec (vec<T,4>& res,const quat<T>& q,const vec<T,4>& v);
