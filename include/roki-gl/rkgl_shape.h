/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_shape - visualization of shapes
 */

#ifndef __RKGL_SHAPE_H__
#define __RKGL_SHAPE_H__

#include <roki-gl/rkgl_optic.h>
#include <zeo/zeo_mshape.h>

__BEGIN_DECLS

typedef enum{
  RKGL_FACE=0, RKGL_WIREFRAME, RKGL_STICK, RKGL_COM, RKGL_ELLIPS, RKGL_BB,
} rkglDisplayType;

/* display list */

int rkglBeginList(void);

/* 3D object drawing */

void rkglTranslate(zVec3D *v);
void rkglXfer(zFrame3D *f);

#define rkglVertex(v)  glVertex3dv( (v)->e )
#define rkglNormal(n)  glNormal3dv( (n)->e )

void rkglPoint(zVec3D *p);
void rkglEdge(zEdge3D *e);
void rkglTri(zTri3D *t);
void rkglPolygon(zVec3D v[], int n, ...);

void rkglPH(zPH3D *ph, rkglDisplayType disptype);

void rkglShape(zShape3D *s, zOpticalInfo *oi_alt, rkglDisplayType disptype);
int rkglShapeEntry(zShape3D *s, zOpticalInfo *oi_alt, rkglDisplayType disptype);

void rkglMShape(zMShape3D *s, rkglDisplayType disptype);
int rkglMShapeEntry(zMShape3D *s, rkglDisplayType disptype);

void rkglBox(zBox3D *box);
void rkglSphere(zSphere3D *sphere);
void rkglEllips(zEllips3D *ellips);
void rkglCyl(zCyl3D *cyl);
void rkglECyl(zECyl3D *ecyl);
void rkglCone(zCone3D *cone);

void rkglTorus(zVec3D *c, zVec3D *n, double r1, double r2, int div1, int div2, GLfloat color[]);

int rkglPointCloud(zVec3DList *pc, zVec3D *center, short size);

void rkglArrow(zVec3D *bot, zVec3D *vec, double mag, GLfloat color[]);
int rkglAxis(zDir axis, double d, double w, GLfloat color[]);
int rkglGauge(zDir axis1, double d1, zDir axis2, double d2, double w, double step, GLfloat color[]);

__END_DECLS

#endif /* __RKGL_SHAPE_H__ */
