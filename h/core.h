#ifndef __COOLWORK_CORE__
#define __COOLWORK_CORE__

#ifdef __CORE_BUILD
  #define CoreAPI extern "C" __declspec(dllimport)
#else  
  #define CoreAPI extern "C" __declspec(dllexport)
#endif  

/*
    Инициализация / завершение ядра
*/

CoreAPI void CoreInit     ();
CoreAPI void CoreShutdown ();

/*
    Создание / удаление рендера
*/

CoreAPI void CoreInitRender     (void* wnd_handle);
CoreAPI void CoreShutdownRender ();
CoreAPI bool CoreIsRenderInit   ();
CoreAPI void CoreResize         (int left,int top,int width,int height);

/*
    Протоколирование
*/

typedef void (*core_log_func)(const char*);

CoreAPI void CoreLogMessage  (const char*);
CoreAPI void CoreLogCallback (core_log_func);

/*
    Работа с командным интерпритатором
*/

CoreAPI void CoreDoCommand (const char* command);

/*
    Работа с объектом
*/

//параметры объекта
enum CoreEntityParam
{
  CORE_ENTITY_NAME,            //имя объекта (string)
  CORE_ENTITY_PARENT_NAME,     //имя родителя (string, "" - no parent)
  CORE_ENTITY_TYPE,            //тип объекта (string)
  CORE_ENTITY_VISIBLE,         //видим ли объект (bool)
  CORE_ENTITY_SELECT,          //выделение объекта (bool)
  CORE_ENTITY_WIRE_COLOR,      //цвет каркасного представления (vec3f)
  CORE_ENTITY_LOCAL_POSITION,  //локальная позиция объекта (vec3f)
  CORE_ENTITY_LOCAL_ROTATION,  //локальный поворот объекта (vec3f)
  CORE_ENTITY_LOCAL_SCALE,     //локальный масштаб объекта (vec3f)
  CORE_ENTITY_LOCAL_DIR,       //локальное направление оси OZ объекта
  CORE_ENTITY_LOCAL_UP,        //локальное направление оси OY объекта
  CORE_ENTITY_WORLD_POSITION,  //мировая позиция объекта (vec3f)
  CORE_ENTITY_WORLD_ROTATION,  //мировый поворот объекта (vec3f)
  CORE_ENTITY_WORLD_SCALE,     //мировый масштаб объекта (vec3f)
  CORE_ENTITY_WORLD_DIR,       //мировое направление оси OZ объекта
  CORE_ENTITY_WORLD_UP,        //мировое направление оси OY объекта  
  
  CORE_LIGHT_ENABLE,           //влючён ли источник (bool)
  CORE_LIGHT_TYPE,             //тип источника (string: "point", "spot", "direct")
  CORE_LIGHT_AMBIENT_COLOR,    //поглощённая составляющая освещения (vec3f)
  CORE_LIGHT_DIFFUSE_COLOR,    //рассеянная составляющая освещения (vec3f)
  CORE_LIGHT_SPECULAR_COLOR,   //отражённая составляющая освещения (vec3f)
  CORE_LIGHT_SPOT_ANGLE,       //эффективный угол освещения (float [0..180])
  CORE_LIGHT_SPOT_EXPONENT,    //экспонета освещения (float [0..128])
  CORE_LIGHT_RADIUS,           //радиус освещения (float)
  
  CORE_MODEL_WIDTH,            //линейные параметры модели (float)
  CORE_MODEL_HEIGHT,
  CORE_MODEL_DEPTH,
  CORE_MODEL_RADIUS,
  CORE_MODEL_SECOND_RADIUS,    //радиус верхнего основания усеченного конуса
  CORE_MODEL_SLICES,           //количество разбиений в конусе и цилиндре (int)
  CORE_MODEL_PARALLELS,        //количество параллелей (для сферы) (int)
  CORE_MODEL_MERIDIANS,        //количество меридианов (для сферы) (int)
  CORE_MODEL_SHEAR_XY,         //скос по оси X относительно Y
  CORE_MODEL_SHEAR_ZY,         //скос по оси Z относительно Y
  
  CORE_MATERIAL_AMBIENT_COLOR, //поглощённая составляющая материала (vec3f)
  CORE_MATERIAL_DIFFUSE_COLOR, //рассеянная составляющая материала (vec3f)
  CORE_MATERIAL_SPECULAR_COLOR,//отражённая составляющая материала (vec3f)
  CORE_MATERIAL_EMISSION_COLOR,//цвет излучения (vec3f)
  CORE_MATERIAL_SHININESS,     //"металличность" (float [0..128])
  CORE_MATERIAL_TRANSPARENCY,  //прозрачность (float [0..1])
  CORE_MATERIAL_BLEND_MODE,    //режим смешивания цветов (string: "solid", "filter", "add")
  CORE_MATERIAL_WIREFRAME,     //отображать ли каркасную модель (bool)
  CORE_MATERIAL_TWO_SIDED,     //двусторонний материал (bool)
  
  CORE_CAMERA_PROJECTION,      //тип проекции (string: ortho, isometry, dimetry, trimetry, cabinet, free, perspective, perspective_special)
  CORE_CAMERA_LEFT,            //параметры плоскостей отсечения для камеры (float)
  CORE_CAMERA_RIGHT,
  CORE_CAMERA_TOP,
  CORE_CAMERA_BOTTOM,
  CORE_CAMERA_ZNEAR,
  CORE_CAMERA_ZFAR,  
  CORE_CAMERA_DISTANCE,        //расстояния до точек проекций по осям для перспективной преокции (vec3f) (устарело)
  CORE_CAMERA_ALPHA,           //углы для диметрии и триметрии (float)
  CORE_CAMERA_BETA,
  CORE_CAMERA_POINT1,          //точки для перспективной проекции (vec3f)
  CORE_CAMERA_POINT2,
  CORE_CAMERA_POINT3,
};

typedef size_t CoreEntityHandle; //дескриптор объекта

//обработчики событий сцены
struct CoreSceneListener 
{
  typedef void (*handler_fn)(CoreEntityHandle);
  typedef void (*select_handler_fn)();
  
  handler_fn        onCreate;  //вызывается после создания объекта
  handler_fn        onDelete;  //вызывается перед удалением объекта
  handler_fn        onModify;  //вызывается после изменения объекта
  select_handler_fn onSelect;  //вызывается на изменение группы выделения  
};

//существует ли такой объект
CoreAPI bool CoreIsValidEntity (CoreEntityHandle);

//дескриптор родительского объекта
CoreAPI CoreEntityHandle CoreGetParentEntity (CoreEntityHandle handle);

//установка параметров объекта
CoreAPI void CoreSetEntityParams   (CoreEntityHandle handle,CoreEntityParam param,const char* value);
CoreAPI void CoreSetEntityParamf   (CoreEntityHandle handle,CoreEntityParam param,float value);
CoreAPI void CoreSetEntityParami   (CoreEntityHandle handle,CoreEntityParam param,int value);
CoreAPI void CoreSetEntityParam3fv (CoreEntityHandle handle,CoreEntityParam param,const float* value);

//чтение параметров объекта
CoreAPI const char* CoreGetEntityParams   (CoreEntityHandle handle,CoreEntityParam param);
CoreAPI void        CoreGetEntityParam3fv (CoreEntityHandle handle,CoreEntityParam param,float* result);
CoreAPI float       CoreGetEntityParamf   (CoreEntityHandle handle,CoreEntityParam param);
CoreAPI int         CoreGetEntityParami   (CoreEntityHandle handle,CoreEntityParam param);

//исключающее выделение данного объекта (будет выделен только он)
CoreAPI void CoreEntityXSelect (CoreEntityHandle handle);

//регистрация слушателя сцены
CoreAPI void CoreSetSceneListener (CoreSceneListener* listener);

//перебор выделенных объектов
CoreAPI size_t            CoreGetSelectedEntitiesCount ();
CoreAPI CoreEntityHandle  CoreGetSelectedEntity        (size_t index);

//имя последнего текцщего файла сцены ("" - в случа отсутствия)
CoreAPI const char* CoreGetCurrentSceneFileName ();

/*
    Работа с манипуляторами
*/

typedef void (*core_change_manipulator_func)(const char* manipulator_name);

CoreAPI void CoreSetManipulatorCallback (core_change_manipulator_func);

#endif
