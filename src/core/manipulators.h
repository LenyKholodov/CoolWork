#ifndef __COOLWORK_CORE_MANIPULATORS__
#define __COOLWORK_CORE_MANIPULATORS__

#include <cursor.h>

/*
    Создание манипуляторов
*/

Manipulator*  manipCreateSelect            (RenderCursor&,const char*);

Manipulator*  manipCreateMove              (RenderCursor&,const char*);
Manipulator*  manipCreateScale             (RenderCursor&,const char*);
Manipulator*  manipCreateRotate            (RenderCursor&,const char*);
Manipulator*  manipCreateShear   	   (RenderCursor&,const char*);

Manipulator*  manipCreatePencilExtrude     (RenderCursor&,const char*);
Manipulator*  manipCreateBrushExtrude      (RenderCursor&,const char*);

Manipulator*  manipCreateBox               (RenderCursor&,const char*);
Manipulator*  manipCreateSphere            (RenderCursor&,const char*);
Manipulator*  manipCreateHalfSphere        (RenderCursor&,const char*);
Manipulator*  manipCreateCone              (RenderCursor&,const char*);
Manipulator*  manipCreateHalfCone          (RenderCursor&,const char*);
Manipulator*  manipCreateCylinder          (RenderCursor&,const char*);
Manipulator*  manipCreateHalfCylinder      (RenderCursor&,const char*);
Manipulator*  manipCreateTetrahedron       (RenderCursor&,const char*);
Manipulator*  manipCreateHexahedron   	   (RenderCursor&,const char*);
Manipulator*  manipCreateOctahedron   	   (RenderCursor&,const char*);
Manipulator*  manipCreateDodecahedron      (RenderCursor&,const char*);
Manipulator*  manipCreateIcosahedron       (RenderCursor&,const char*);
Manipulator*  manipCreateTorus             (RenderCursor&,const char*);
Manipulator*  manipCreateHalfTorus         (RenderCursor&,const char*);
Manipulator*  manipCreateLandscape         (RenderCursor&,const char*);

Manipulator*  manipCreateBind              (RenderCursor&,const char*);

Manipulator*  manipCreatePointLight        (RenderCursor&,const char*);
Manipulator*  manipCreateSpotFreeLight     (RenderCursor&,const char*);
Manipulator*  manipCreateSpotTargetLight   (RenderCursor&,const char*);
Manipulator*  manipCreateDirectFreeLight   (RenderCursor&,const char*);
Manipulator*  manipCreateDirectTargetLight (RenderCursor&,const char*);

Manipulator*  manipCameraMoveRL_UD         (RenderCursor&,const char*);
Manipulator*  manipCameraMoveRL_FB         (RenderCursor&,const char*);
Manipulator*  manipCameraMoveFB_UD         (RenderCursor&,const char*);
Manipulator*  manipCameraSelfRotate        (RenderCursor&,const char*);
Manipulator*  manipCameraRotate            (RenderCursor&,const char*);
Manipulator*  manipCameraZoom              (RenderCursor&,const char*);

#endif
