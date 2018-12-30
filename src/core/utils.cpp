#include <mathlib.h>
#include "misc.h"

//#define MY_AFFINE_DECOMP

#ifndef MY_AFFINE_DECOMP

void affine_decomp (const matrix<float,4>& mat,vec<float,3>& pos,vec<float,3>& rot,vec<float,3>& scale)
{
  matrix<float,4> locmat;
  matrix<float,4> pmat = 1, invpmat = 1, tinvpmat = 1;
  /* Vector4 type and functions need to be added to the common set. */
  vec<float,4> prhs = 0, psol = 0;
  vec<float,4> row[3], pdum3 = 0;
  vec<float,3> shear = 0;
  
  row [0] = row [1] = row [2] = 0;

  locmat = mat;
  
  /* Normalize the matrix. */
  
  if ( locmat [3][3] == 0 )
    return ;

  for (int i=0;i<4;i++)
    for (int j=0;j<4;j++)
      locmat [i][j] = locmat [i][j] / locmat [3][3];

  /* pmat is used to solve for perspective, but it also provides
   * an easy way to test for singularity of the upper 3x3 component.
   */

  pmat = locmat;

  for (int i=0;i<3;i++) pmat [3][i] = 0;
  
  pmat [3][3] = 1;

  if (det (pmat) == 0.0 )
    return ;

  //подразумеваем, что перспективные преобразования отсутствуют

  /* Next take care of translation (easy). */

  for (int i=0;i<3;i++) 
  {
    pos [i]       = locmat [i][3];
    locmat [i][3] = 0;
  }

  //scale & shear

  for (int i=0;i<3;i++) 
  {
    row [i].x = locmat [0][i];
    row [i].y = locmat [1][i];
    row [i].z = locmat [2][i];
  }

  /* Compute X scale factor and normalize first row. */
  scale.x = length (row[0]);

  row [0] = normalize (row [0]);

  /* Compute XY shear factor and make 2nd row orthogonal to 1st. */

  shear.z = dot (row[0],row[1]);

//  row[1].linearCombo(1.0, row[0], -tran[U_SHEARXY]); //????????
  row[1] -= row[0] * shear.z; //????????

  /* Now, compute Y scale and normalize 2nd row. */
  scale.y = length (row[1]);
  row[1] = normalize (row [1]);

  shear.z /= scale.y;

  /* Compute XZ and YZ shears, orthogonalize 3rd row. */
  shear.y = dot (row[0],row[2]);
//  row[2].linearCombo(1.0, row[0], -tran[U_SHEARXZ]); //?????
  row[2] -= row[0]* shear.y; //?????

  shear.x = dot (row[1],row[2]);
//  row[2].linearCombo(1.0, row[1], -tran[U_SHEARYZ]); //?????
  row[2] -= row[1]*shear.x; //?????

  /* Next, get Z scale and normalize 3rd row. */
  scale.z = length (row[2]);
  row[2] = normalize (row [2]);
  shear.y /= scale.z;
  shear.x /= scale.z;

  /* At this point, the matrix (in rows[]) is orthonormal.
   * Check for a coordinate system flip.  If the determinant
   * is -1, then negate the matrix and the scaling factors.
   */  
//  row[1].cross(row[2], pdum3);
  pdum3 = cross ((vec<float,3>&)row [1],(vec<float,3>&)row [2]);

  if (dot (row[0],pdum3) < 0 )
    for (int i=0;i<3;i++) 
    {
      scale [i] *= -1;
//      row[i].scale(-1); ///?????
      row [i] *= -1;
    }

  /* Now, get the rotations out, as described in the gem. */

  rot.y = asin(-row[0].z);

  if ( cos(rot.y) != 0 ) 
  {
    rot.x = atan2(row[1].z, row[2].z);
    rot.z = atan2(row[0].y, row[0].x);
  } 
  else 
  {
    rot.x = atan2(-row[2].x,row[1].y);
    rot.z = 0;
  }
  
  for (int i=0;i<3;i++)
    rot [i] = rad2deg (rot [i]);

}

#else

void affine_decomp (const mat4f& m,vec3f& pos,vec3f& rot,vec3f& scale)
{
  mat4f tm = m;

  for (int i=0;i<3;i++)
  {
    pos [i]   = tm [i][3];
    tm [i][3] = 0.0f;
  }
  
  vec3f row [3];  

  row [0] = 0.0f;
  row [1] = 0.0f;
  row [2] = 0.0f;
      
  for (int i=0;i<3;i++) 
  {
    row [i].x = tm [0][i];
    row [i].y = tm [1][i];
    row [i].z = tm [2][i];
  }  

  for (int i=0;i<3;i++)
    scale [i] = length (row [i]);
    
  tm *= scalef (1.0f/scale.x,1.0f/scale.y,1.0f/scale.z);
  
  quat2EulerAngle (quatf (tm),rot.x,rot.y,rot.z);
}

#endif

void print (const mat4f& m)
{
  for (int i=0;i<4;i++)
    logPrintf ("(%g,%g,%g,%g)",m [i][0],m [i][1],m [i][2],m [i][3]);
}

bool maskcmp (const char* s,const char* mask)
{
  while (*s && *mask) 
    switch (*mask)
    {
      case '*':
        do mask++; while (*mask == '*' || *mask == '?');

        if (!*mask)
          return true;

        for (s=strchr(s,*mask);s;s=strchr (s+1,*mask))
          if (maskcmp (s,mask))
            return true;

        return false;
      default:
        if (*mask != *s) 
          return false;
        mask++;
        s++;
        break;
      case '?':
        mask++;       
        return maskcmp (s,mask)?true:maskcmp(s+1,mask);
    }

  if (!*s)
  {
    while (*mask)
      switch (*mask)
      {
        case '*':
        case '?':  
          mask++;
          break;             
        default:
          return false;          
      }

    return true;
  }
  else return false;
}
