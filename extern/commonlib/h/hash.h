#ifndef __COMMONLIB_HASH__
#define __COMMONLIB_HASH__

#include <stddef.h>

/*
        CRC32
*/

unsigned int crc32 (const void* data,size_t size,unsigned int init_value=0xFFFFFFFF);

/*
        Adler32
*/

unsigned int adler32 (const void* data,size_t size,unsigned int init_value=1);

/*
        MD5
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///Контекст для последовательного хэширования
///////////////////////////////////////////////////////////////////////////////////////////////////
class MD5Context
{
  public:
     MD5Context  ();
     ~MD5Context ();
          
     void update (const void* buf,size_t len);
     void finish (unsigned char result_hash_value [16]); //после данного вызова контекст сбрасывается
     
  private:     
    struct MD5_CTX* context;
};

//хеширование блока памяти
void md5 (unsigned char result_hash_value [16],const void* buf,size_t len);

#endif
