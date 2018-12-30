#ifndef __COOLWORK_CORE__
#define __COOLWORK_CORE__

#ifdef __CORE_BUILD
  #define CoreAPI extern "C" __declspec(dllimport)
#else  
  #define CoreAPI extern "C" __declspec(dllexport)
#endif  

/*
    ������������� / ���������� ����
*/

CoreAPI void CoreInit     ();
CoreAPI void CoreShutdown ();

/*
    �������� / �������� �������
*/

CoreAPI void CoreInitRender     (void* wnd_handle);
CoreAPI void CoreShutdownRender ();
CoreAPI bool CoreIsRenderInit   ();
CoreAPI void CoreResize         (int left,int top,int width,int height);

/*
    ����������������
*/

typedef void (*core_log_func)(const char*);

CoreAPI void CoreLogMessage  (const char*);
CoreAPI void CoreLogCallback (core_log_func);

/*
    ������ � ��������� ���������������
*/

CoreAPI void CoreDoCommand (const char* command);

/*
    ������ � ��������
*/

//��������� �������
enum CoreEntityParam
{
  CORE_ENTITY_NAME,            //��� ������� (string)
  CORE_ENTITY_PARENT_NAME,     //��� �������� (string, "" - no parent)
  CORE_ENTITY_TYPE,            //��� ������� (string)
  CORE_ENTITY_VISIBLE,         //����� �� ������ (bool)
  CORE_ENTITY_SELECT,          //��������� ������� (bool)
  CORE_ENTITY_WIRE_COLOR,      //���� ���������� ������������� (vec3f)
  CORE_ENTITY_LOCAL_POSITION,  //��������� ������� ������� (vec3f)
  CORE_ENTITY_LOCAL_ROTATION,  //��������� ������� ������� (vec3f)
  CORE_ENTITY_LOCAL_SCALE,     //��������� ������� ������� (vec3f)
  CORE_ENTITY_LOCAL_DIR,       //��������� ����������� ��� OZ �������
  CORE_ENTITY_LOCAL_UP,        //��������� ����������� ��� OY �������
  CORE_ENTITY_WORLD_POSITION,  //������� ������� ������� (vec3f)
  CORE_ENTITY_WORLD_ROTATION,  //������� ������� ������� (vec3f)
  CORE_ENTITY_WORLD_SCALE,     //������� ������� ������� (vec3f)
  CORE_ENTITY_WORLD_DIR,       //������� ����������� ��� OZ �������
  CORE_ENTITY_WORLD_UP,        //������� ����������� ��� OY �������  
  
  CORE_LIGHT_ENABLE,           //������ �� �������� (bool)
  CORE_LIGHT_TYPE,             //��� ��������� (string: "point", "spot", "direct")
  CORE_LIGHT_AMBIENT_COLOR,    //����������� ������������ ��������� (vec3f)
  CORE_LIGHT_DIFFUSE_COLOR,    //���������� ������������ ��������� (vec3f)
  CORE_LIGHT_SPECULAR_COLOR,   //��������� ������������ ��������� (vec3f)
  CORE_LIGHT_SPOT_ANGLE,       //����������� ���� ��������� (float [0..180])
  CORE_LIGHT_SPOT_EXPONENT,    //��������� ��������� (float [0..128])
  CORE_LIGHT_RADIUS,           //������ ��������� (float)
  
  CORE_MODEL_WIDTH,            //�������� ��������� ������ (float)
  CORE_MODEL_HEIGHT,
  CORE_MODEL_DEPTH,
  CORE_MODEL_RADIUS,
  CORE_MODEL_SECOND_RADIUS,    //������ �������� ��������� ���������� ������
  CORE_MODEL_SLICES,           //���������� ��������� � ������ � �������� (int)
  CORE_MODEL_PARALLELS,        //���������� ���������� (��� �����) (int)
  CORE_MODEL_MERIDIANS,        //���������� ���������� (��� �����) (int)
  CORE_MODEL_SHEAR_XY,         //���� �� ��� X ������������ Y
  CORE_MODEL_SHEAR_ZY,         //���� �� ��� Z ������������ Y
  
  CORE_MATERIAL_AMBIENT_COLOR, //����������� ������������ ��������� (vec3f)
  CORE_MATERIAL_DIFFUSE_COLOR, //���������� ������������ ��������� (vec3f)
  CORE_MATERIAL_SPECULAR_COLOR,//��������� ������������ ��������� (vec3f)
  CORE_MATERIAL_EMISSION_COLOR,//���� ��������� (vec3f)
  CORE_MATERIAL_SHININESS,     //"�������������" (float [0..128])
  CORE_MATERIAL_TRANSPARENCY,  //������������ (float [0..1])
  CORE_MATERIAL_BLEND_MODE,    //����� ���������� ������ (string: "solid", "filter", "add")
  CORE_MATERIAL_WIREFRAME,     //���������� �� ��������� ������ (bool)
  CORE_MATERIAL_TWO_SIDED,     //������������ �������� (bool)
  
  CORE_CAMERA_PROJECTION,      //��� �������� (string: ortho, isometry, dimetry, trimetry, cabinet, free, perspective, perspective_special)
  CORE_CAMERA_LEFT,            //��������� ���������� ��������� ��� ������ (float)
  CORE_CAMERA_RIGHT,
  CORE_CAMERA_TOP,
  CORE_CAMERA_BOTTOM,
  CORE_CAMERA_ZNEAR,
  CORE_CAMERA_ZFAR,  
  CORE_CAMERA_DISTANCE,        //���������� �� ����� �������� �� ���� ��� ������������� �������� (vec3f) (��������)
  CORE_CAMERA_ALPHA,           //���� ��� �������� � ��������� (float)
  CORE_CAMERA_BETA,
  CORE_CAMERA_POINT1,          //����� ��� ������������� �������� (vec3f)
  CORE_CAMERA_POINT2,
  CORE_CAMERA_POINT3,
};

typedef size_t CoreEntityHandle; //���������� �������

//����������� ������� �����
struct CoreSceneListener 
{
  typedef void (*handler_fn)(CoreEntityHandle);
  typedef void (*select_handler_fn)();
  
  handler_fn        onCreate;  //���������� ����� �������� �������
  handler_fn        onDelete;  //���������� ����� ��������� �������
  handler_fn        onModify;  //���������� ����� ��������� �������
  select_handler_fn onSelect;  //���������� �� ��������� ������ ���������  
};

//���������� �� ����� ������
CoreAPI bool CoreIsValidEntity (CoreEntityHandle);

//���������� ������������� �������
CoreAPI CoreEntityHandle CoreGetParentEntity (CoreEntityHandle handle);

//��������� ���������� �������
CoreAPI void CoreSetEntityParams   (CoreEntityHandle handle,CoreEntityParam param,const char* value);
CoreAPI void CoreSetEntityParamf   (CoreEntityHandle handle,CoreEntityParam param,float value);
CoreAPI void CoreSetEntityParami   (CoreEntityHandle handle,CoreEntityParam param,int value);
CoreAPI void CoreSetEntityParam3fv (CoreEntityHandle handle,CoreEntityParam param,const float* value);

//������ ���������� �������
CoreAPI const char* CoreGetEntityParams   (CoreEntityHandle handle,CoreEntityParam param);
CoreAPI void        CoreGetEntityParam3fv (CoreEntityHandle handle,CoreEntityParam param,float* result);
CoreAPI float       CoreGetEntityParamf   (CoreEntityHandle handle,CoreEntityParam param);
CoreAPI int         CoreGetEntityParami   (CoreEntityHandle handle,CoreEntityParam param);

//����������� ��������� ������� ������� (����� ������� ������ ��)
CoreAPI void CoreEntityXSelect (CoreEntityHandle handle);

//����������� ��������� �����
CoreAPI void CoreSetSceneListener (CoreSceneListener* listener);

//������� ���������� ��������
CoreAPI size_t            CoreGetSelectedEntitiesCount ();
CoreAPI CoreEntityHandle  CoreGetSelectedEntity        (size_t index);

//��� ���������� �������� ����� ����� ("" - � ����� ����������)
CoreAPI const char* CoreGetCurrentSceneFileName ();

/*
    ������ � ��������������
*/

typedef void (*core_change_manipulator_func)(const char* manipulator_name);

CoreAPI void CoreSetManipulatorCallback (core_change_manipulator_func);

#endif
