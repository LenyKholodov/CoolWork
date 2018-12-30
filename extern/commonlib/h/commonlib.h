#ifndef __COMMONLIB__
#define __COMMONLIB__

#include <stdarg.h>
#include <stddef.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///Компонент библиотеки
///////////////////////////////////////////////////////////////////////////////////////////////////
enum CommonLibComponent
{
  COMMONLIB_ALL    = 0, //настройки устанавливаются для всех компонентов
  COMMONLIB_PARSER = 1, //парсер
  COMMONLIB_STRLIB = 2, //строковые операции
  
  COMMONLIB_COMPONENTS_NUM
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Интерфейс управления библиотекой
///////////////////////////////////////////////////////////////////////////////////////////////////
class CommonLib
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Интерфейсы обработчиков
///////////////////////////////////////////////////////////////////////////////////////////////////      
    typedef void* (*mem_alloc_proc)(size_t);
    typedef void  (*mem_free_proc)(void*);
    
    typedef void*  (*file_open_proc)(const char* file_name,bool write_flag);
    typedef void   (*file_close_proc)(void* context);    
    typedef size_t (*file_read_proc)(void* context,void* buf,size_t size);
    typedef size_t (*file_write_proc)(void* context,const void* buf,size_t size);
    typedef size_t (*file_size_proc)(void* context);
    typedef size_t (*file_seek_proc)(void* context,size_t position);
    typedef size_t (*file_tell_proc)(void* context);    
    
    typedef void   (*log_message_proc)(const char*,va_list); //обработчик вывода сообщений
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка обработчиков (NULL - default)
///////////////////////////////////////////////////////////////////////////////////////////////////                
    static void SetAllocHandler (CommonLibComponent component,mem_alloc_proc,mem_free_proc);
    static void SetLogHandler   (log_message_proc);    
    static void SetIOHandler    (file_open_proc,file_close_proc,file_read_proc,file_write_proc,
                                 file_size_proc,file_seek_proc,file_tell_proc);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Информация о библиотеке
///////////////////////////////////////////////////////////////////////////////////////////////////
    static const char* version    ();
    static const char* components ();      
};

#endif
