/* RoKi-GL - Robot Kinetics library: visualization using OpenGL
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * rkgl_shape - visualization of shapes
 */

#include <stdarg.h>
#include <roki-gl/rkgl_shape.h>

/* display list */

int rkglBeginList(void)
{
  int result;

  result = glGenLists( 1 );
  glNewList( result, GL_COMPILE );
  return result;
}

/* 3D object drawing */

void rkglTranslate(zVec3D *v)
{
  glTranslated( v->e[zX], v->e[zY], v->e[zZ] );
}

void rkglXfer(zFrame3D *f)
{
  GLdouble m[16];

  zMat3DCol( zFrame3DAtt(f), 0, (zVec3D*)&m[0] );
  zMat3DCol( zFrame3DAtt(f), 1, (zVec3D*)&m[4] );
  zMat3DCol( zFrame3DAtt(f), 2, (zVec3D*)&m[8] );
  zVec3DCopy( zFrame3DPos(f), (zVec3D*)&m[12] );
  m[3]=m[7]=m[11]=0.0; m[15]=1.0;
  glMultMatrixd( m );
}

void rkglPoint(zVec3D *p)
{
  glBegin( GL_POINTS );
    rkglVertex( p );
  glEnd();
}

void rkglEdge(zEdge3D *e)
{
  glBegin( GL_LINES );
    rkglVertex( zEdge3DVert(e,0) );
    rkglVertex( zEdge3DVert(e,1) );
  glEnd();
}

void rkglTri(zTri3D *t)
{
  zVec3D invnorm;

  glBegin( GL_TRIANGLES );
    rkglNormal( zTri3DNorm(t) );
    rkglVertex( zTri3DVert(t,0) );
    rkglVertex( zTri3DVert(t,1) );
    rkglVertex( zTri3DVert(t,2) );
    zVec3DRev( zTri3DNorm(t), &invnorm );
    rkglNormal( &invnorm );
    rkglVertex( zTri3DVert(t,0) );
    rkglVertex( zTri3DVert(t,2) );
    rkglVertex( zTri3DVert(t,1) );
  glEnd();
}

void rkglPolygon(zVec3D v[], int n, ...)
{
  zVec3D v0, v1, v2, norm;
  va_list args;
  register int i;

  if( n < 3 ){
    ZRUNERROR( "cannot create polygon from less than three vertices" );
    return;
  }
  /* normal vector */
  va_start( args, n );
    zVec3DCopy( &v[(int)va_arg( args, int )], &v0 );
    zVec3DCopy( &v[(int)va_arg( args, int )], &v1 );
    zVec3DCopy( &v[(int)va_arg( args, int )], &v2 );
  va_end( args );
  zVec3DSubDRC( &v1, &v0 );
  zVec3DSubDRC( &v2, &v0 );
  zVec3DOuterProd( &v1, &v2, &norm );
  zVec3DNormalizeDRC( &norm );
  glBegin( GL_POLYGON );
    rkglNormal( &norm );
    /* vertices */
    va_start( args, n );
    for( i=0; i<n; i++ )
      rkglVertex( &v[(int)va_arg( args, int )] );
    va_end( args );
  glEnd();
}

void rkglPH(zPH3D *ph, rkglDisplayType disptype)
{
  register int i;

  if( disptype == RKGL_WIREFRAME )
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

  for( i=0; i<zPH3DFaceNum(ph); i++ )
    rkglTri( zPH3DFace(ph,i) );
}

void rkglShape(zShape3D *s, zOpticalInfo *oi_alt, rkglDisplayType disptype)
{
  if( oi_alt ){
    rkglMaterial( oi_alt );
  } else
  if( zShape3DOptic(s) )
    rkglMaterial( zShape3DOptic(s) );

  if( disptype == RKGL_BB )
    rkglBox( zShape3DBB(s) );
  else switch( zShape3DType(s) ){
  case ZSHAPE_PH:       return rkglPH( zShape3DPH(s), disptype );
  case ZSHAPE_BOX:      return rkglBox( zShape3DBox(s) );
  case ZSHAPE_SPHERE:   return rkglSphere( zShape3DSphere(s) );
  case ZSHAPE_ELLIPS:   return rkglEllips( zShape3DEllips(s) );
  case ZSHAPE_CYLINDER: return rkglCyl( zShape3DCyl(s) );
  case ZSHAPE_ELLIPTICCYLINDER: return rkglECyl( zShape3DECyl(s) );
  case ZSHAPE_CONE:     return rkglCone( zShape3DCone(s) );
  default: ;
  }
}

int rkglShapeEntry(zShape3D *s, zOpticalInfo *oi_alt, rkglDisplayType disptype)
{
  int result;

  if( s == NULL ) return -1;
  result = rkglBeginList();
  rkglShape( s, oi_alt, disptype );
  glEndList();
  return result;
}

void rkglMShape(zMShape3D *s, rkglDisplayType disptype)
{
  register int i;

  for( i=0; i<zMShape3DShapeNum(s); i++ )
    rkglShape( zMShape3DShape(s,i), NULL, disptype );
}

int rkglMShapeEntry(zMShape3D *s, rkglDisplayType disptype)
{
  int result;

  result = rkglBeginList();
  rkglMShape( s, disptype );
  glEndList();
  return result;
}

void rkglBox(zBox3D *box)
{
  zVec3D vert[8];
  register int i;

  for( i=0; i<8; i++ )
    zBox3DVert( box, i, &vert[i] ); /* vertices */
  /* faces */
  glShadeModel( GL_FLAT );
  rkglPolygon( vert, 4, 0, 1, 2, 3 );
  rkglPolygon( vert, 4, 7, 6, 5, 4 );
  rkglPolygon( vert, 4, 0, 3, 7, 4 );
  rkglPolygon( vert, 4, 1, 5, 6, 2 );
  rkglPolygon( vert, 4, 0, 4, 5, 1 );
  rkglPolygon( vert, 4, 2, 6, 7, 3 );
}

void rkglSphere(zSphere3D *sphere)
{
  register int i, j, i1, j1, n2=zSphere3DDiv(sphere)*2;
  zVec3D vert[zSphere3DDiv(sphere)+1][n2+1], v;

  for( i=0; i<=zSphere3DDiv(sphere); i++ )
    for( j=0; j<=n2; j++ )
      zVec3DCreatePolar( &vert[i][j], zSphere3DRadius(sphere),
        zPI*i/zSphere3DDiv(sphere), zPI*j/zSphere3DDiv(sphere) );
  glShadeModel( GL_SMOOTH );
  for( i=1; i<=zSphere3DDiv(sphere); i++ )
    for( j=1; j<=n2; j++ ){
      i1 = i - 1; j1 = j - 1;
      glBegin( GL_QUADS );
        zVec3DAdd( &vert[i][j],   zSphere3DCenter(sphere), &v );
        rkglNormal( &vert[i][j] );   rkglVertex( &v );
        zVec3DAdd( &vert[i1][j],  zSphere3DCenter(sphere), &v );
        rkglNormal( &vert[i1][j] );  rkglVertex( &v );
        zVec3DAdd( &vert[i1][j1], zSphere3DCenter(sphere), &v );
        rkglNormal( &vert[i1][j1] ); rkglVertex( &v );
        zVec3DAdd( &vert[i][j1],  zSphere3DCenter(sphere), &v );
        rkglNormal( &vert[i][j1] );  rkglVertex( &v );
      glEnd();
    }
}

static zVec3D *_rkglEllipsNormal(zEllips3D *ellips, zVec3D *v, zVec3D *n);
zVec3D *_rkglEllipsNormal(zEllips3D *ellips, zVec3D *v, zVec3D *n)
{
  zVec3D tmp;

  zVec3DCreate( &tmp,
    v->e[zX]/zEllips3DRadiusX(ellips),
    v->e[zY]/zEllips3DRadiusY(ellips),
    v->e[zZ]/zEllips3DRadiusZ(ellips) );
  zMulMatVec3D( zFrame3DAtt(&ellips->f), &tmp, n );
  zVec3DNormalizeDRC( n );
  return n;
}

void rkglEllips(zEllips3D *ellips)
{
  register int i, j, i1, j1, n2=zEllips3DDiv(ellips)*2;
  zVec3D vert[zEllips3DDiv(ellips)+1][n2+1];
  zVec3D norm[zEllips3DDiv(ellips)+1][n2+1];
  zVec3D tmp;

  for( i=0; i<=zEllips3DDiv(ellips); i++ )
    for( j=0; j<=n2; j++ ){
      zVec3DCreatePolar( &tmp, 1.0,
        zPI*i/zEllips3DDiv(ellips), zPI*j/zEllips3DDiv(ellips) );
      _rkglEllipsNormal( ellips, &tmp, &norm[i][j] );
      tmp.e[zX] *= zEllips3DRadiusX(ellips);
      tmp.e[zY] *= zEllips3DRadiusY(ellips);
      tmp.e[zZ] *= zEllips3DRadiusZ(ellips);
      zXfer3D( &ellips->f, &tmp, &vert[i][j] );
    }
  glShadeModel( GL_SMOOTH );
  for( i=1; i<=zEllips3DDiv(ellips); i++ )
    for( j=1; j<=n2; j++ ){
      i1 = i - 1; j1 = j - 1;
      glBegin( GL_QUADS );
        rkglNormal( &norm[i ][j ] ); rkglVertex( &vert[i ][j ] );
        rkglNormal( &norm[i1][j ] ); rkglVertex( &vert[i1][j ] );
        rkglNormal( &norm[i1][j1] ); rkglVertex( &vert[i1][j1] );
        rkglNormal( &norm[i ][j1] ); rkglVertex( &vert[i ][j1] );
      glEnd();
    }
}

static zVec3D *_rkglRadial(zVec3D *d, zVec3D *s);
zVec3D *_rkglRadial(zVec3D *d, zVec3D *s)
{
  if( !zIsTiny( d->e[zX] ) && !zIsTiny( d->e[zY] ) )
    zVec3DCreate( s, d->e[zY],-d->e[zX], 0 );
  else
    zVec3DCreate( s, d->e[zY]-d->e[zZ], d->e[zZ]-d->e[zX], d->e[zX]-d->e[zY] );
  zVec3DNormalizeDRC( s );
  return s;
}

void rkglCyl(zCyl3D *cyl)
{
  zVec3D norm[zCyl3DDiv(cyl)+1], vert[2][zCyl3DDiv(cyl)+1], d, s, aa;
  double l;
  register int i;

  zCyl3DAxis( cyl, &d );
  if( zIsTiny( ( l = zVec3DNorm( &d ) ) ) ) return;
  zVec3DDivDRC( &d, l );
  /* one radial vector */
  _rkglRadial( &d, &s );
  zVec3DMulDRC( &s, zCyl3DRadius(cyl) );
  /* creation of vertices */
  for( i=0; i<=zCyl3DDiv(cyl); i++ ){
    zVec3DMul( &d, -2*zPI*i/zCyl3DDiv(cyl), &aa );
    zVec3DRot( &s, &aa, &norm[i] );
    /* vertices on the top rim */
    zVec3DAdd( zCyl3DCenter(cyl,0), &norm[i], &vert[0][i] );
    /* vertices on the bottom rim */
    zVec3DAdd( zCyl3DCenter(cyl,1), &norm[i], &vert[1][i] );
  }
  /* top faces */
  glShadeModel( GL_FLAT );
  rkglNormal( &d );
  glBegin( GL_TRIANGLE_FAN );
  for( i=zCyl3DDiv(cyl)-1; i>=0; i-- )
    rkglVertex( &vert[1][i] );
  glEnd();
  /* bottom faces */
  zVec3DRevDRC( &d );
  rkglNormal( &d );
  glBegin( GL_TRIANGLE_FAN );
  for( i=0; i<zCyl3DDiv(cyl); i++ )
    rkglVertex( &vert[0][i] );
  glEnd();
  /* side faces */
  glShadeModel( GL_SMOOTH );
  glBegin( GL_QUAD_STRIP );
  for( i=0; i<=zCyl3DDiv(cyl); i++ ){
    rkglNormal( &norm[i] );
    rkglVertex( &vert[0][i] );
    rkglVertex( &vert[1][i] );
  }
  glEnd();
}

void rkglECyl(zECyl3D *ecyl)
{
  zVec3D norm[zECyl3DDiv(ecyl)+1], vert[2][zECyl3DDiv(ecyl)+1], d;
  double l, s, c;
  register int i;

  zECyl3DAxis( ecyl, &d );
  if( ( l = zVec3DNormalizeDRC( &d ) ) < 0 ) return;
  /* creation of vertices */
  for( i=0; i<=zECyl3DDiv(ecyl); i++ ){
    zSinCos( -zPIx2*i/zECyl3DDiv(ecyl), &s, &c );
    zVec3DMul( zECyl3DRadVec(ecyl,0), zECyl3DRadius(ecyl,0)*c, &norm[i] );
    zVec3DCatDRC( &norm[i], zECyl3DRadius(ecyl,1)*s, zECyl3DRadVec(ecyl,1) );
    /* vertices on the top rim */
    zVec3DAdd( zECyl3DCenter(ecyl,0), &norm[i], &vert[0][i] );
    /* vertices on the bottom rim */
    zVec3DAdd( zECyl3DCenter(ecyl,1), &norm[i], &vert[1][i] );
  }
  /* top faces */
  glShadeModel( GL_FLAT );
  rkglNormal( &d );
  glBegin( GL_TRIANGLE_FAN );
  for( i=zECyl3DDiv(ecyl)-1; i>=0; i-- )
    rkglVertex( &vert[1][i] );
  glEnd();
  /* bottom faces */
  zVec3DRevDRC( &d );
  rkglNormal( &d );
  glBegin( GL_TRIANGLE_FAN );
  for( i=0; i<zECyl3DDiv(ecyl); i++ )
    rkglVertex( &vert[0][i] );
  glEnd();
  /* side faces */
  glShadeModel( GL_SMOOTH );
  glBegin( GL_QUAD_STRIP );
  for( i=0; i<=zECyl3DDiv(ecyl); i++ ){
    rkglNormal( &norm[i] );
    rkglVertex( &vert[0][i] );
    rkglVertex( &vert[1][i] );
  }
  glEnd();
}

void rkglCone(zCone3D *cone)
{
  zVec3D norm[zCone3DDiv(cone)+1], vert[zCone3DDiv(cone)+1], d, s, tmp, v;
  double l;
  register int i;

  zCone3DAxis( cone, &d );
  if( zIsTiny( ( l = zVec3DNorm( &d ) ) ) ) return;
  zVec3DDivDRC( &d, l );
  /* one radial vector */
  _rkglRadial( &d, &s );
  zVec3DMulDRC( &s, zCone3DRadius(cone) );
  /* creation of vertices */
  for( i=0; i<=zCone3DDiv(cone); i++ ){
    zVec3DMul( &d, -2*zPI*i/zCone3DDiv(cone), &tmp );
    zVec3DRot( &s, &tmp, &v );
    /* vertices on the rim */
    zVec3DAdd( zCone3DCenter(cone), &v, &vert[i] );
    /* normal vector at each vertex */
    zVec3DSub( &vert[i], zCone3DVert(cone), &tmp );
    zVec3DNormalizeDRC( &tmp );
    zVec3DMulDRC( &tmp, zVec3DInnerProd( &v, &tmp ) );
    zVec3DSub( &v, &tmp, &norm[i] );
  }
  /* bottom faces */
  glShadeModel( GL_FLAT );
  glBegin( GL_TRIANGLE_FAN );
  rkglNormal( zVec3DRevDRC( &d ) );
  for( i=0; i<zCone3DDiv(cone); i++ )
    rkglVertex( &vert[i] );
  glEnd();
  /* side faces */
  glShadeModel( GL_SMOOTH );
  glBegin( GL_QUAD_STRIP );
  for( i=zCone3DDiv(cone); i>=0; i-- ){
    rkglNormal( &norm[i] );
    rkglVertex( zCone3DVert(cone) );
    rkglVertex( &vert[i] );
  }
  glEnd();
}

void rkglTorus(zVec3D *c, zVec3D *n, double r1, double r2, int div1, int div2, GLfloat color[])
{
  register int i, j;
  zVec3D d, aa1, aa2, s, sr, dr, tmp, t;
  zVec3D norm[div1+1][div2+1], vert[div1+1][div2+1];
  double rm, r;
  zOpticalInfo oi;

  if( zVec3DIsTiny( n ) || zIsTiny( r2-r1 ) ) return;
  if( r1 > r2 ) zSwap( double, r1, r2 ); /* always r2 >= r1 */
  rm = 0.5 * ( r1 + r2 );
  r  = 0.5 * ( r2 - r1 );
  zVec3DNormalize( n, &d );
  _rkglRadial( &d, &s );

  for( i=0; i<=div1; i++ ){
    zVec3DMul( &d, 2*zPI*i/div1, &aa1 );
    zVec3DRot( &s, &aa1, &tmp );
    zVec3DMul( &tmp, rm, &sr );
    zVec3DOuterProd( &tmp, &d, &t );
    zVec3DNormalizeDRC( &t ); /* no need in ideal case */
    for( j=0; j<=div2; j++ ){
      zVec3DMul( &t, 2*zPI*j/div2, &aa2 );
      zVec3DRot( &tmp, &aa2, &norm[i][j] );
      zVec3DMul( &norm[i][j], r,  &dr );
      zVec3DAdd( &dr, &sr, &vert[i][j] );
      zVec3DAddDRC( &vert[i][j], c );
    }
  }
  zOpticalInfoCreateSimple( &oi, color[0], color[1], color[2], NULL );
  rkglMaterial( &oi );
  glShadeModel( GL_SMOOTH );
  for( i=0; i<div1; i++ ){
    glBegin( GL_QUAD_STRIP );
    for( j=0; j<=div2; j++ ){
      rkglNormal( &norm[i][j] );   rkglVertex( &vert[i][j] );
      rkglNormal( &norm[i+1][j] ); rkglVertex( &vert[i+1][j] );
    }
    glEnd();
  }
}

int rkglPointCloud(zVec3DList *pc, zVec3D *center, short size)
{
  zVec3DListCell *c;
  zHSV hsv;
  zRGB rgb;
  double d, dmax = 0;
  int entry;

  hsv.sat = hsv.val = 1.0;
  zListForEach( pc, c )
    if( ( d = zVec3DDist( c->data, center ) ) > dmax ) dmax = d;
  if( zIsTiny( dmax ) ) dmax = 1.0; /* dummy */

  entry = rkglBeginList();
  glDisable( GL_LIGHTING );
  glPointSize( size );
  glBegin( GL_POINTS );
  zListForEach( pc, c ){
    hsv.hue = 360 * zMin( zVec3DDist( c->data, center ) / dmax, 1.0 ) + 180;
    zHSV2RGB( &hsv, &rgb );
    rkglRGB( &rgb );
    rkglVertex( c->data );
  }
  glEnd();
  glEnable( GL_LIGHTING );
  glEndList();
  return entry;
}

void rkglArrow(zVec3D *bot, zVec3D *vec, double mag, GLfloat color[])
{
  zCyl3D cyl;
  zCone3D cone;
  zVec3D tip, neck;
  zOpticalInfo oi;
#define RKGL_ARROW_BOTTOM_RAD 0.05
#define RKGL_ARROW_NECK_RAD   0.1
#define RKGL_ARROW_TIP_LEN    0.2

  zOpticalInfoCreateSimple( &oi, color[0], color[1], color[2], NULL );
  rkglMaterial( &oi );
  zVec3DAdd( bot, vec, &tip );
  if( !zVec3DNormalize( vec, &neck ) ) return;
  zVec3DMulDRC( &neck,-RKGL_ARROW_TIP_LEN * mag );
  zVec3DAddDRC( &neck, &tip );
  zCyl3DCreate( &cyl, bot, &neck, RKGL_ARROW_BOTTOM_RAD * mag, 0 );
  zCone3DCreate( &cone, &neck, &tip, RKGL_ARROW_NECK_RAD * mag, 0 );

  rkglCyl( &cyl );
  rkglCone( &cone );
}

int rkglAxis(zDir axis, double d, double w, GLfloat color[])
{
  int entry;
  zEdge3D edge;
  zVec3D e1, e2;

  entry = rkglBeginList();

  glLineWidth( w );
  zVec3DClear( &e1 );
  zVec3DClear( &e2 );
  zEdge3DCreate( &edge, &e1, &e2 );
  zEdge3DVert(&edge,0)->e[axis] = d;
  zEdge3DVert(&edge,1)->e[axis] =-d;
  zEdge3DCalcVec( &edge );

  glColor3fv( color );
  rkglEdge( &edge );

  glEndList();
  return entry;
}

int rkglGauge(zDir axis1, double d1, zDir axis2, double d2, double w, double step, GLfloat color[])
{
  int entry;
  zEdge3D edge;
  zVec3D e1, e2;
  double d;

  zVec3DClear( &e1 );
  zVec3DClear( &e2 );
  zEdge3DCreate( &edge, &e1, &e2 );

  entry = rkglBeginList();

  glLineWidth( w );
  glColor3fv( color );
  zEdge3DVert(&edge,0)->e[axis2] = d2;
  zEdge3DVert(&edge,1)->e[axis2] =-d2;
  for( d=-d1; d<=d1; d+=step ){
    zEdge3DVert(&edge,0)->e[axis1] = d;
    zEdge3DVert(&edge,1)->e[axis1] = d;
    zEdge3DCalcVec( &edge );
    rkglEdge( &edge );
  }
  zEdge3DVert(&edge,0)->e[axis1] = d1;
  zEdge3DVert(&edge,1)->e[axis1] =-d1;
  for( d=-d2; d<=d2; d+=step ){
    zEdge3DVert(&edge,0)->e[axis2] = d;
    zEdge3DVert(&edge,1)->e[axis2] = d;
    zEdge3DCalcVec( &edge );
    rkglEdge( &edge );
  }

  glEndList();
  return entry;
}
