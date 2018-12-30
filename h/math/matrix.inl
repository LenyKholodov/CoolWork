template <class T,int size>
inline matrix<T,size>::matrix (const T& a)
{
  for (int i=0;i<size;i++)
    for (int j=0;j<size;j++)
      if (i==j) (*this)[i][j] = a;
      else      (*this)[i][j] = 0;
}

template <class T,int size>
inline matrix<T,size>::matrix (const T* a)
{
  for (int i=0;i<size;i++)
    for (int j=0;j<size;j++)
      (*this)[i][j] = *a++;                
}

/*template <class T,int size>
matrix<T,size>::matrix (const matrix<T,size>& src)
{
  matrix_copy (*this,src);
}

template <class T,int size>
void trick_quat2matrix (const quat<T>& q,matrix<T,size>&);

template <class T>
void trick_quat2matrix (const quat<T>& q,matrix<T,3>& m)
{
  quat2matrix (q,m);
}*/

template <class T>
void trick_quat2matrix (const quat<T>& q,matrix<T,4>& m)
{
  quat2matrix (q,m);
}

template <class T,int size> 
matrix<T,size>::matrix (const quat<T>& q)
{
  trick_quat2matrix (q,*this);
}

template <class T,int size> template <class T1>
inline matrix<T,size>::matrix (const T1& a,void (*eval)(matrix&,const T1&))
{
  eval (*this,a);
}

template <class T,int size> template <class T1,class T2>
inline matrix<T,size>::matrix (const T1& a,const T2& b,void (*eval)(matrix&,const T1&,const T2&))
{
  eval (*this,a,b);
}

template <class T,int size>
typename matrix<T,size>::vector& matrix<T,size>::row (int i) 
{
  return x [i];
}

template <class T,int size>
const typename matrix<T,size>::vector& matrix<T,size>::row (int i) const
{
  return x [i];
}

template <class T,int size>
void matrix_copy_column (vec<T,size>& res,const matrix<T,size>& m,const int& column)
{
  for (int i=0;i<size;i++)
    res [i] = m [i][column];
}

template <class T,int size>
const typename matrix<T,size>::vector matrix<T,size>::column (int i) const
{
  return vector (*this,i,matrix_copy_column<T,size>);
}

/*template <class T,int size>
matrix<T,size>& matrix<T,size>::operator = (const matrix<T,size>& src)
{
  matrix_copy (*this,src);
  return *this;
} */

template <class T,int size>
matrix<T,size>& matrix<T,size>::operator = (const quat<T>& q)
{
  trick_quat2matrix (q,*this);
  return *this;
} 

template <class T,int size> 
matrix<T,size>& matrix<T,size>::operator = (const T& a)
{
  matrix_assign_scalar (*this,a);
  return *this;
} 

template <class T,int size>
const matrix<T,size> matrix<T,size>::operator - () const
{
  return matrix (*this,matrix_neg<T,size>);
}

template <class T,int size> 
inline matrix<T,size>& matrix<T,size>::operator += (const matrix<T,size>& a)
{
  matrix_add (*this,*this,a);
  return *this;
}

template <class T,int size>   
inline matrix<T,size>& matrix<T,size>::operator -= (const matrix<T,size>& a)
{
  matrix_sub (*this,*this,a);
  return *this;
}

template <class T,int size>   
inline matrix<T,size>& matrix<T,size>::operator *= (const matrix<T,size>& a)
{
  matrix_mul (*this,a);
  return *this;
}

template <class T,int size>
inline matrix<T,size>& matrix<T,size>::operator *= (const T& a)
{
  matrix_mul_scalar (*this,*this,a);
  return *this;
}

template <class T,int size>
inline matrix<T,size>& matrix<T,size>::operator /= (const T& a)
{
  matrix_div_scalar (*this,*this,a);
  return *this;
}

template <class T,int size>   
inline const matrix<T,size> matrix<T,size>::operator + (const matrix<T,size>& m) const
{
  return matrix (*this,m,matrix_add<T,size>);
}

template <class T,int size>   
inline const matrix<T,size> matrix<T,size>::operator - (const matrix<T,size>& m) const
{
  return matrix (*this,m,matrix_sub<T,size>);
}

template <class T,int size>
inline const matrix<T,size> matrix<T,size>::operator * (const T& a) const
{
  return matrix (*this,a,matrix_mul_scalar<T,size>);
}

template <class T,int size>
inline const matrix<T,size> matrix<T,size>::operator / (const T& a) const
{
  return matrix (*this,a,matrix_div_scalar<T,size>);
}

template <class T,int size>   
inline const matrix<T,size> matrix<T,size>::operator * (const matrix<T,size>& a) const
{
  return matrix (*this,a,matrix_mul<T,size>);
} 

template <class T,int size>
void trick_matrix_mul_vec (vec<T,size>& res,const matrix<T,size>& m,const vec<T,size>& v)
{
  matrix_mul_vec (res,m,v);
}

template <class T,int size>
void trick_vec_mul_matrix (vec<T,size>& res,const vec<T,size>& v,const matrix<T,size>& m)
{
  vec_mul_matrix (res,v,m);
}

template <class T,int size>
void trick_matrix_mul_vec1 (vec<T,size-1>& res,const matrix<T,size>& m,const vec<T,size-1>& v)
{
  matrix_mul_vec<T,size-1> (res,m,v);
}

template <class T,int size>
void trick_vec1_mul_matrix (vec<T,size-1>& res,const vec<T,size-1>& v,const matrix<T,size>& m)
{
  vec_mul_matrix<T,size-1> (res,v,m);
} 

template <class T,int size> 
const vec<T,size> matrix<T,size>::operator * (const vec<T,size>& v) const
{
  return vec<T,size> (*this,v,trick_matrix_mul_vec<T,size>);
}

template <class T,int size> 
const vec<T,size-1> matrix<T,size>::operator * (const vec<T,size-1>& v) const
{
  return vec<T,size-1> (*this,v,trick_matrix_mul_vec1<T,size>);
} 

template <class T,int size>
const vec<T,size> vec<T,size>::operator * (const matrix<T,size>& m) const
{
  return vec (*this,m,trick_vec_mul_matrix<T,size>);
} 

template <class T,int size>
const vec<T,size> vec<T,size>::operator * (const matrix<T,size+1>& m) const
{
  return vec (*this,m,trick_vec1_mul_matrix<T,size+1>);
} 

template <class T,int size>
vec<T,size>& vec<T,size>::operator *= (const matrix<T,size>& m)
{
  vec_mul_matrix (*this,*this,m);
  return *this;
}

template <class T,int size>
vec<T,size>& vec<T,size>::operator *= (const matrix<T,size+1>& m)
{
  vec_mul_matrix<T,size> (*this,*this,m);
  return *this;
}

template <class T,int size> 
bool matrix<T,size>::operator == (const matrix<T,size>& a) const
{
  return matrix_equal (*this,a);
}

template <class T,int size> 
bool matrix<T,size>::operator != (const matrix<T,size>& a) const
{
  return matrix_nequal (*this,a);
}

template <class T,int size>
void matrix<T,size>::transpose ()
{
  matrix_transpose (*this);
}

template <class T,int size>
void matrix<T,size>::invert ()
{
  matrix_invert (*this);
}

template <class T,int size>
void matrix<T,size>::normalize ()
{
  matrix_mormalize (*this);
}

template <class T,int size> 
bool equal (const matrix<T,size>& a,const matrix<T,size>& b,const T& eps)
{
  return matrix_equal (a,b,eps);
} 

template <class T,int size>
matrix<T,size> transpose (const matrix<T,size>& m)
{
  return matrix<T,size> (m,matrix_transpose<T,size>);
} 

template <class T,int size>
void trick_matrix_invert (matrix<T,size>& res,const matrix<T,size>& src)
{
  matrix_invert (res,src);
}

template <class T,int size>
matrix<T,size> invert (const matrix<T,size>& m)
{
  return matrix<T,size> (m,trick_matrix_invert<T,size>); 
}

template <class T,int size>
matrix<T,size> normalize (const matrix<T,size>& m)
{
  return matrix<T,size> (m,matrix_normalize<T,size>);
} 

template <class T,int size>
T det (const matrix<T,size>& m)
{
  return matrix_det (m);
}

template <class T,int size> 
T minor (const matrix<T,size>& src,int m,int n) 
{
  return matrix_minor (src,m,n);
} 

