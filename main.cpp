#include "stdafx.h"

#define SELECT_UNO 111
#define SELECT_DOS 222
#define SELECT_TRES 333
#define SELECT_CUATRO 444
#define SELECT_CINCO 555

#define BUFSIZE 512
GLuint selectBuf[BUFSIZE];


#define checkImageWidth 128
#define checkImageHeight 128
#define subImageWidth 64
#define subImageHeight 64


static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

GLfloat diffuseMaterial[4] = { 0.5, 0.5, 0.5, 1.0 };

GLuint tex;

static GLuint texName[2];


static int cuerpo = 0, cabeza = 0;

static int hombro = 0, codo = 0, munieca = 0, dedo1 = 0, dedo2 = 0;

GLfloat colors[][3] = { { 0.99,0.99,0.99 },{ 1.0,0.0,0.0 },
{ 1.0,1.0,0.0 },{ 0.0,1.0,0.0 },{ 0.0,0.0,1.0 },
{ 1.0,0.0,1.0 },{ 1.0,1.0,1.0 },{ 0.0,1.0,1.0 } },


select1[3],
select2[3],
select3[3],
select4[3],
select5[3],


shadowMat[16],
vLuz[4] = { 2., 3.5, 0, 1. },
//vLuz[4] = { 1., 1., 1., 0. },
vPizo[4] = { 0.0f, 1.0f, 0.0f, 3.5 },

ctrlpoints[4][4][3],

texpts[2][2][2] = { {{0.0, 0.0}, {0.0, 2.0}},
{{3.0, 0.0}, {3.0, 2.0}} };


#define GL_PI 3.1415f


GLfloat xRot = 0.45f;
GLfloat yRot = 0.35f;
GLboolean bCull = glIsEnabled(GL_CULL_FACE);
GLboolean bDepth = !glIsEnabled(GL_DEPTH_TEST);
GLboolean bOutline = (GLboolean)true;


GLenum shademode = GL_FLAT;


GLfloat ex = 0.0f;
GLfloat ey = 50.0f;
GLfloat ez = -300.0f;
GLfloat delta = 0.01f;
GLfloat deltaR = 0.01f;
GLfloat ALFA = 0.25;

GLfloat vdata[13][3] = {
			{0.0f, 0.0f, 25.0f},
			{5.0f, 0.0f, 0.0f}, {0.0f, 3.0f, 0.0f}, {-5.0f, 0.0f, 0.0f},
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void normalize(GLfloat v[3]) {
	GLfloat d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	if (d == 0.0) { exit(100); return; } // wrong input exit
	v[0] /= d; v[1] /= d; v[2] /= d;
}

void normcrossprod(GLfloat v1[3], GLfloat v2[3], GLfloat out[3])
{
	out[0] = v1[1] * v2[2] - v1[2] * v2[1]; out[1] = v1[2] * v2[0] - v1[0] * v2[2];
	out[2] = v1[0] * v2[1] - v1[1] * v2[0];	normalize(out);
}

void gltGetNormalVector(GLfloat v0[3], GLfloat //build normal 'norm'
	v1[3], GLfloat v2[3], GLfloat norm[3])	// by the triangle vrtices
{
	GLfloat d1[3], d2[3];
	for (int j = 0; j < 3; j++) {
		d1[j] = v0[j] - v1[j];
		d2[j] = v1[j] - v2[j];
	}
	normcrossprod(d1, d2, norm);
}

void arbol()
{
	glPushMatrix();
	glTranslatef(8.0, -3.5, 2.0);
	glScalef(0.1, 0.2, 0.1);
	glRotatef(270.0, 1.0, 0.0, 0.0);
	GLfloat norm[3];
	glBegin(GL_TRIANGLE_FAN);
	glVertex3fv(&vdata[0][0]);

	glVertex3fv(&vdata[1][0]);

	gltGetNormalVector(&vdata[0][0], &vdata[1][0], &vdata[2][0], norm);/* color information here */
	glNormal3fv(norm);/* color information here */
	glColor3f(0.0, 1.0, 0.0);
	glVertex3fv(&vdata[2][0]);

	gltGetNormalVector(&vdata[0][0], &vdata[2][0], &vdata[3][0], norm);
	glNormal3fv(norm);

	glVertex3fv(&vdata[3][0]);

	gltGetNormalVector(&vdata[0][0], &vdata[3][0], &vdata[1][0], norm);
	glNormal3fv(norm);

	glVertex3fv(&vdata[1][0]);
	glEnd();
	glPopMatrix();
}

void uncolored_arbol()
{
	glPushMatrix();
	glTranslatef(8.0, -3.5, 2.0);
	glScalef(0.1, 0.2, 0.1);
	glRotatef(270.0, 1.0, 0.0, 0.0);
	GLfloat norm[3];
	glBegin(GL_TRIANGLE_FAN);
	glVertex3fv(&vdata[0][0]);

	glVertex3fv(&vdata[1][0]);

	gltGetNormalVector(&vdata[0][0], &vdata[1][0], &vdata[2][0], norm);
	glNormal3fv(norm);
	glVertex3fv(&vdata[2][0]);

	gltGetNormalVector(&vdata[0][0], &vdata[2][0], &vdata[3][0], norm);
	glNormal3fv(norm);

	glVertex3fv(&vdata[3][0]);

	gltGetNormalVector(&vdata[0][0], &vdata[3][0], &vdata[1][0], norm);
	glNormal3fv(norm);

	glVertex3fv(&vdata[1][0]);
	glEnd();
	glPopMatrix();
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void makeCheckImages(void)
{
	int i, j, c;
	for (i = 0; i < checkImageHeight; i++) {
		for (j = 0; j < checkImageWidth; j++) {
			c = ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 128;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)128;
		}
	}

}

void init_surface(void)
{
	int u, v;
	for (u = 0; u < 4; u++) {
		for (v = 0; v < 4; v++) {
			ctrlpoints[u][v][2] = 2.0 * ((GLfloat)u - 1.5);
			ctrlpoints[u][v][1] = 2.0 * ((GLfloat)v - 1.5);

			if ((u == 1 || u == 2) && (v == 1 || v == 2))
				ctrlpoints[u][v][0] = 3.0;
			else
				ctrlpoints[u][v][0] = -3.0;
		}
	}
}


void gltMakeShadowMatrix(GLfloat vPlaneEquation[], GLfloat vLightPos[], GLfloat destMat[])
{
	GLfloat dot;
	// Dot product of plane and light position
	dot = vPlaneEquation[0] * vLightPos[0] + vPlaneEquation[1] * vLightPos[1] +
		vPlaneEquation[2] * vLightPos[2] + vPlaneEquation[3] * vLightPos[3];
	// Now do the projection
	// First column
	destMat[0] = dot - vLightPos[0] * vPlaneEquation[0];
	destMat[4] = 0.0f - vLightPos[0] * vPlaneEquation[1];
	destMat[8] = 0.0f - vLightPos[0] * vPlaneEquation[2];
	destMat[12] = 0.0f - vLightPos[0] * vPlaneEquation[3];

	// Second column
	destMat[1] = 0.0f - vLightPos[1] * vPlaneEquation[0];
	destMat[5] = dot - vLightPos[1] * vPlaneEquation[1];
	destMat[9] = 0.0f - vLightPos[1] * vPlaneEquation[2];
	destMat[13] = 0.0f - vLightPos[1] * vPlaneEquation[3];

	// Third Column
	destMat[2] = 0.0f - vLightPos[2] * vPlaneEquation[0];
	destMat[6] = 0.0f - vLightPos[2] * vPlaneEquation[1];
	destMat[10] = dot - vLightPos[2] * vPlaneEquation[2];
	destMat[14] = 0.0f - vLightPos[2] * vPlaneEquation[3];
	// Fourth Column
	destMat[3] = 0.0f - vLightPos[3] * vPlaneEquation[0];
	destMat[7] = 0.0f - vLightPos[3] * vPlaneEquation[1];
	destMat[11] = 0.0f - vLightPos[3] * vPlaneEquation[2];
	destMat[15] = dot - vLightPos[3] * vPlaneEquation[3];
}

void objetodeforme()
{
	glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2,
		0, 1, 4, 2, &texpts[0][0][0]);
	glEnable(GL_MAP2_TEXTURE_COORD_2);
	init_surface();

	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4,
		0, 1, 12, 4, &ctrlpoints[0][0][0]);

	glEnable(GL_MAP2_VERTEX_3);
	glEnable(GL_AUTO_NORMAL);
	glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
}


void cuadrado() {
	glBegin(GL_POLYGON);
	glVertex2f(1., 1.);
	glVertex2f(1., -1.);
	glVertex2f(-1., -1.);
	glVertex2f(-1., 1.);
	glEnd();
}


void cuadrado_text() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName[0]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glColor4f(0.0, 1.0, 0., 0.50);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0);  glVertex2f(1., 1.);
	glTexCoord2f(0.0, 1.0);  glVertex2f(1., -1.);
	glTexCoord2f(1.0, 1.0);  glVertex2f(-1., -1.);
	glTexCoord2f(1.0, 0.0);   glVertex2f(-1., 1.);

	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void mesh_text() {
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName[0]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glColor3f(0.0, 1.0, 0.0);
	glTranslatef(0.0, 0.5, 6.5);
	glTranslatef(0, -1, 0);
	glRotatef(90.0, 0.0, 0.0, 1.0);
	glEvalMesh2(GL_FILL, 0, 20, 0, 20);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void mago(GLfloat color[3]) {

	glPushMatrix();
	glTranslatef(0.0, -6.5, 0.0);
	glColor3f(0.5, 0.2, 0.0);
	glRotatef(yRot, 0.0, 1.0, 0.0);
	glScalef(2.0, 2.0, 2.0);

	glPushMatrix();
	glTranslatef(0, 2, 0);
	glTranslatef(0.0f, -0.5f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glutSolidCone(0.15, 0.15, 10, 10);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

	glTranslatef(0.0f, 0.3f, 0.0f);
	glScalef(2.0f, 2.5f, 2.0f);
	glColor3fv(color);
	glRotatef((GLfloat)cuerpo, 1.0, 0.0, 0.0);
	glutSolidSphere(0.1, 10, 10);

	glScalef(0.5f, 0.3f, 0.5f);
	glTranslatef(0.0f, 0.4f, 0.0f);
	glColor3f(1.0, 1.0, 0.0);
	glRotatef((GLfloat)cabeza, 0.0, 1.0, 0.0);
	glutSolidTeapot(0.15);

	glTranslatef(0.0f, 0.05f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(1.0f, 1.0f, 2.5f);
	//glColor3f(0.3, 0.3, 0.7);
	glColor3fv(color);
	glutSolidCone(0.15, 0.20, 10, 10);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glPopMatrix();
	glPopMatrix();
}

void uncolored_mago() {
	glPushMatrix();
	glTranslatef(0.0, -6.5, 0.0);
	glRotatef(yRot, 0.0, 1.0, 0.0);
	glScalef(2.0, 2.0, 2.0);

	glPushMatrix();
	glTranslatef(0, 2, 0);
	glTranslatef(0.0f, -0.5f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glutSolidCone(0.15, 0.15, 10, 10);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

	glTranslatef(0.0f, 0.3f, 0.0f);
	glScalef(2.0f, 2.5f, 2.0f);
	glRotatef((GLfloat)cuerpo, 1.0, 0.0, 0.0);
	glutSolidSphere(0.1, 10, 10);

	glScalef(0.5f, 0.3f, 0.5f);
	glTranslatef(0.0f, 0.4f, 0.0f);
	glRotatef((GLfloat)cabeza, 0.0, 1.0, 0.0);
	glutSolidTeapot(0.15);

	glTranslatef(0.0f, 0.05f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(1.0f, 1.0f, 2.5f);
	glutSolidCone(0.15, 0.20, 10, 10);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glPopMatrix();
	glPopMatrix();
}

void uncolored_cube(void)
{
	/*cara 1*/
	glPushMatrix();
	glTranslatef(0, 0, 1.);
	cuadrado();
	glPopMatrix();

	/*cara 2*/
	glPushMatrix();
	glRotatef(90., 0., 1., 0);
	glTranslatef(0, 0, 1.);
	cuadrado();
	glPopMatrix();

	/*cara 3*/
	glPushMatrix();
	glRotatef(180., 0., 1., 0);
	glTranslatef(0, 0, 1.);
	cuadrado();
	glPopMatrix();

	/*cara 4*/
	glPushMatrix();
	glRotatef(270., 0., 1., 0);
	glTranslatef(0, 0, 1.);
	cuadrado();
	glPopMatrix();

	/*cara 5*/
	glPushMatrix();
	glRotatef(90., 1., 0., 0);
	glTranslatef(0, 0, 1.);
	cuadrado();
	glPopMatrix();

	/*cara 6*/
	glPushMatrix();
	glRotatef(-90., 1., 0., 0);
	glTranslatef(0, 0, 1.);
	cuadrado();
	glPopMatrix();
}


void colorcube(void)
{
	glPushMatrix();
	/*cara 1*/
	glColor3fv(colors[0]);
	glPushMatrix();
	glTranslatef(0, 0, 1.);
	cuadrado();
	glPopMatrix();
	/*cara 2*/
	glColor3fv(colors[0]);
	glPushMatrix();
	glRotatef(90., 0., 1., 0);
	glTranslatef(0, 0, 1.);
	cuadrado();
	glPopMatrix();
	/*cara 3 (la que puede ser seleccionada)*/
	glColor3fv(/*colors[2]*/colors[0]);
	//glPushName(CARA_TRES);
	glPushMatrix();
	glRotatef(180., 0., 1., 0);
	glTranslatef(0, 0, 1.);
	cuadrado();
	glPopMatrix();
	//glPopName();
	/*cara 4*/
	glColor3fv(colors[0]);
	glPushMatrix();
	glRotatef(270., 0., 1., 0);
	glTranslatef(0, 0, 1.);
	cuadrado();
	glPopMatrix();
	/*cara 5*/
	glColor3fv(colors[0]);
	glPushMatrix();
	glRotatef(90., 1., 0., 0);
	glTranslatef(0, 0, 1.);
	cuadrado();
	glPopMatrix();
	/*cara 6*/
	glColor3fv(colors[0]);
	glPushMatrix();
	glRotatef(-90., 1., 0., 0);
	glTranslatef(0, 0, 1.);
	cuadrado();
	glPopMatrix();
	glPopMatrix();
}

void cubos()
{
	glPushMatrix();
	glTranslatef(9.5, -1.5, 2.5);
	glScalef(0.5, 2.0, 0.5);
	colorcube();
	glTranslatef(0.0, 0.0, -11.0);
	colorcube();
	glTranslatef(0.0, 1.25, 5.5);
	glScalef(1.0, 0.25, 6.5);
	colorcube();
	glPopMatrix();
}

void uncolored_cubos()
{
	glPushMatrix();
	glTranslatef(9.5, -1.5, 2.5);
	glScalef(0.5, 2.0, 0.5);
	uncolored_cube();
	glTranslatef(0.0, 0.0, -11.0);
	uncolored_cube();
	glTranslatef(0.0, 1.25, 5.5);
	glScalef(1.0, 0.25, 6.5);
	uncolored_cube();
	glPopMatrix();
}

void objetosDeformes()
{
	glPushMatrix();
	glTranslatef(-3.5, 0.0, 0.0);
	objetodeforme();
	mesh_text();
	glTranslatef(-3.5, 0.0, 0.0);
	objetodeforme();
	mesh_text();
	glTranslatef(10.5, 0.0, 0.0);
	objetodeforme();
	mesh_text();
	glTranslatef(3.5, 0.0, 0.0);
	objetodeforme();
	mesh_text();
	glPopMatrix();
}

void magos(GLenum mode)
{
	glPushMatrix();

	glTranslatef(0.0, 0.0, -2.0);
	if (mode == GL_SELECT) glPushName(SELECT_UNO);
	mago(select1);
	if (mode == GL_SELECT) glPopName();

	glTranslatef(5.0, 0.0, 2.0);
	glPushName(SELECT_DOS);
	mago(select2);
	glPopName();

	glTranslatef(-1.5, 0.0, 0.0);
	glPushName(SELECT_TRES);
	mago(select3);
	glPopName();

	glTranslatef(-6.0, 0.0, 2.0);
	glPushName(SELECT_CUATRO);
	mago(select4);
	glPopName();

	glTranslatef(-3.0, 0.0, -5.0);
	glPushName(SELECT_CINCO);
	mago(select5);
	glPopName();

	glPopMatrix();

}

void uncolored_magos()
{
	glPushMatrix();
	glTranslatef(0.0, 0.0, -2.0);
	uncolored_mago();
	glTranslatef(5.0, 0.0, 2.0);
	uncolored_mago();
	glTranslatef(-1.5, 0.0, 0.0);
	uncolored_mago();
	glTranslatef(-6.0, 0.0, 2.0);
	uncolored_mago();
	glTranslatef(-3.0, 0.0, -5.0);
	uncolored_mago();
	glPopMatrix();

}


void arboles()
{
	glPushMatrix();
	arbol();
	glTranslatef(-4.0, 0.0, -5.0);
	arbol();
	glTranslatef(-9.0, 0.0, 5.0);
	arbol();
	glTranslatef(2.0, 0.0, -5.0);
	arbol();
	glTranslatef(-4.0, 0.0, 1.0);
	arbol();
	glPopMatrix();
}

void uncolored_arboles()
{
	glPushMatrix();
	uncolored_arbol();
	glTranslatef(-4.0, 0.0, -5.0);
	uncolored_arbol();
	glTranslatef(-9.0, 0.0, 5.0);
	uncolored_arbol();
	glTranslatef(2.0, 0.0, -5.0);
	uncolored_arbol();
	glTranslatef(-4.0, 0.0, 1.0);
	uncolored_arbol();
	glPopMatrix();
}


void SombradeMiMundo(GLfloat datosdeLuz[], GLfloat datosdePlano[]) {
	glColor4f(0.33, 0.33, 0.33, ALFA);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPushMatrix();
	gltMakeShadowMatrix(datosdePlano, datosdeLuz, shadowMat);
	glMultMatrixf(shadowMat);
	uncolored_cubos(); // Objetos_de_MiMundo();
	uncolored_magos();
	uncolored_arboles();
	glPopMatrix();
}
void init();

void SetupRC()
{ // Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Set drawing color to green
	//glColor3f(0.0f, 1.0f, 0.0f);

	// Set color shading model to flat
	glShadeModel(shademode);
	// Clockwise-wound polygons are front facing; this is reversed
	// because we are using triangle fans
	glFrontFace(GL_CW); //CW= clockwise; CCW= counter clockwise

	makeCheckImages();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(2, texName);
	glBindTexture(GL_TEXTURE_2D, texName[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, checkImageHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);

	///// ///////////////////////////////////////////////////////

	//glGenTextures(2, texName2);
	//glBindTexture(GL_TEXTURE_2D, texName2[0]);


	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, checkImageHeight,
	//	0, GL_RGBA, GL_UNSIGNED_BYTE, fly);

	//////////////////////////////////////////////////////////////////

	glEnable(GL_TEXTURE_2D);

	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat mat_brillo[] = { 50.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_brillo);
	glLightfv(GL_LIGHT0, GL_POSITION, vLuz);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	init();
}
// Called to draw scene
void piso() {
	glColor4f(0.66, 0.66, 0.66, ALFA);
	glPushMatrix();
	glTranslatef(0, -vPizo[3], 0);
	glRotatef(-90., 1., 0, 0);
	glScalef(10., 5, 10.);
	cuadrado_text();
	glDisable(GL_DEPTH_TEST);
	if (bDepth)  glEnable(GL_DEPTH_TEST);
	glPopMatrix();
}


void luz() {
	glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslatef(vLuz[0], vLuz[1], vLuz[2]);
	glutSolidSphere(.1, 20., 20.);
	glPopMatrix();
}


void Stencil_config()
{
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);

	// enable stencil buffer
	glEnable(GL_STENCIL_TEST);

	// write a one to the stencil buffer everywhere we are about to draw
	glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFF);
	// this is to always pass a one to the stencil buffer where we draw
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

	// render the plane which the shadow will be on
	// color and depth buffer are disabled, only the stencil buffer
	// will be modified
	piso(); //DrawFloor(0,0,0);

	// turn the color and depth buffers back on
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);

	// until stencil test is diabled, only write to areas where the
	// stencil buffer has a one. This is to draw the shadow only on
	// the floor.
	glStencilFunc(GL_EQUAL, 1, 0xFFFFFFFF);

	// don't modify the contents of the stencil buffer
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}


void display(GLenum mode)
{
	glMatrixMode(GL_PROJECTION);
	if (mode == GL_RENDER)
		glLoadIdentity();


	gluPerspective(60.0, 1.0, 1.5, 1500.0);
	glMatrixMode(GL_MODELVIEW);


	// Reset coordinate system
	glLoadIdentity();
	gluLookAt(ex, ey, ez, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


	GLfloat x, y, angle;         // Storage for coordinates and angles
	int iPivot = 1;            // Used to flag alternating colors
	   // Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


	// Turn culling on if flag is set
	if (bCull)   glEnable(GL_CULL_FACE);
	else        glDisable(GL_CULL_FACE);


	// Enable depth testing if flag is set
	if (bDepth)  glEnable(GL_DEPTH_TEST);
	else        glDisable(GL_DEPTH_TEST);




	// Draw the back side as a wireframe only, if flag is set
	if (bOutline)glPolygonMode(GL_BACK, GL_FILL);
	else        glPolygonMode(GL_BACK, GL_LINE);


	// Save matrix state and do the rotation
	glPushMatrix();


	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	glScalef(25., 25, 25.);
	/*
	dibujar mundo reflejado
	1. invertir orientación: glFrontFace(GL_CCW);
	2. aplicar la transformacion reflejo
	dlTranslatef(0, -vPizo[3] (AlturqaPiso), 0);
	glScalef(1, -1, 1);
	dlTranslatef(0, vPizo[3] (AlturqaPiso), 0);
	3. dibujar mundo (colorcube)
	4. regresar a la orientacion original: glFrontFace(GL_CW);
	*/
	/*reflejo*/
	glEnable(GL_COLOR_MATERIAL);
	Stencil_config();
	glFrontFace(GL_CCW);
	glPushMatrix();
	glTranslatef(0, -vPizo[3], 0);
	glScalef(1, -1, 1);
	glTranslatef(0, vPizo[3], 0);
	luz();
	cubos();
	magos(mode);
	arboles();
	glPopMatrix();
	glFrontFace(GL_CW);
	glDisable(GL_STENCIL_TEST);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);


	piso();


	glDisable(GL_BLEND);
	magos(mode);

	objetosDeformes();
	mesh_text();
	arboles();
	cubos();
	glDisable(GL_LIGHT0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	Stencil_config();
	SombradeMiMundo(vLuz, vPizo);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHT0);
	if (bDepth)  glEnable(GL_DEPTH_TEST);
	luz();
	glDisable(GL_COLOR_MATERIAL);

	// Restore transformations
	glPopMatrix();
	// Flush drawing commands
	glFlush();// glutSwapBuffers();
}


void RenderScene(void)
{
	//glClear(GL_COLOR_BUFFER_BIT);
	display(GL_RENDER);
	glFlush();
}


// Called by GLUT library when the window has changed size
void ChangeSize(GLsizei w, GLsizei h)
{    // Set Viewport to window dimensions
	//glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	////glOrtho (-100.0, 100.0, -100, 100, -270.0, 270.0);
	////glFrustum (-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
	//gluPerspective(60.0, 1.0, 1.5, 1500.0);
	//glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(65.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -5.0);
}


void SpecialKeys(int key, int x, int y)
{
	GLfloat dx, dz;
	if (key == GLUT_KEY_UP)
	{//increase distance from camera to origin
		ex *= (1.0f + deltaR);  ey *= (1.0f + deltaR); ez *= (1.0f + deltaR);
	}
	if (key == GLUT_KEY_DOWN)
	{//reduce distance from camera to origin (close up)
		ex *= (1.0f - deltaR); ey *= (1.0f - deltaR); ez *= (1.0f - deltaR);
	}
	if (key == GLUT_KEY_LEFT)
		//Rotate camera around origin in Oxz plane
	{
		dx = -ez;     dz = ex;
		GLfloat s = sqrtf(ex * ex + ey * ey + ez * ez);
		ex += delta * dx;       ez += delta * dz;
		GLfloat s1 = sqrtf(ex * ex + ey * ey + ez * ez) / s;
		ex /= s1; ey /= s1; ey /= s1;
	}


	if (key == GLUT_KEY_RIGHT)
		//Rotate camera around origin in Oxz plane
	{
		dx = -ez;     dz = ex;
		GLfloat s = sqrtf(ex * ex + ey * ey + ez * ez);
		ex -= delta * dx;      ez -= delta * dz;
		GLfloat s1 = sqrtf(ex * ex + ey * ey + ez * ez) / s;
		ex /= s1; ey /= s1; ey /= s1;
	}


	if (key == GLUT_KEY_F1) bCull = !bCull;
	if (key == GLUT_KEY_F2)bDepth = !bDepth;
	if (key == GLUT_KEY_F3)bOutline = !bOutline;
	if (key == GLUT_KEY_F4)
	{
		if (shademode == GL_FLAT) { shademode = GL_SMOOTH; }
		else { if (shademode == GL_SMOOTH) { shademode = GL_FLAT; } };
		glShadeModel(shademode);
	}


	if (key == GLUT_KEY_F11) {
		vLuz[1] += 0.25;
	}
	if (key == GLUT_KEY_F12) {
		vLuz[1] -= 0.25;
	}
	// Refresh the Window
	glutPostRedisplay();
}


void init_color_cara3() {


	select1[0] = colors[7][0];
	select1[1] = colors[7][1];
	select1[2] = colors[7][2];

	select2[0] = colors[1][0];
	select2[1] = colors[1][1];
	select2[2] = colors[1][2];

	select3[0] = colors[3][0];
	select3[1] = colors[3][1];
	select3[2] = colors[3][2];

	select4[0] = colors[4][0];
	select4[1] = colors[4][1];
	select4[2] = colors[4][2];

	select5[0] = colors[5][0];
	select5[1] = colors[5][1];
	select5[2] = colors[5][2];
}


void init(void)
{
	init_color_cara3();
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
}


void anallizename(int name)
{
	int i;

	if (name == SELECT_UNO) {
		for (i = 0; i < 3; i++)
			select1[i] *= 0.9;
	}
	else if (name == SELECT_DOS) {
		for (i = 0; i < 3; i++)
			select2[i] *= 0.9;
	}
	else if (name == SELECT_TRES) {
		for (i = 0; i < 3; i++)
			select3[i] *= 0.9;
	}
	else if (name == SELECT_CUATRO) {
		for (i = 0; i < 3; i++)
			select4[i] *= 0.9;
	}
	else if (name == SELECT_CINCO) {
		for (i = 0; i < 3; i++)
			select5[i] *= 0.9;
	}
	else {
		return;
	}

	glutPostRedisplay();
}




void processHits(GLint hits, GLuint buffer[])
{
	if (hits == 0) {/*restore all colors*/
		init();
		return;
	}


	unsigned int i, j;
	GLuint names, * ptr;


	ptr = (GLuint*)buffer;
	for (i = 0; i < hits; i++) {    /*  for each hit  */
		names = *ptr;
		ptr += 3;
		for (j = 0; j < names; j++)  /*  for each name */
			anallizename(*ptr++);
	}
}

void pick_Mago(int button, int state, int x, int y)
{
	//GLuint selectBuf[BUFSIZE];
	GLint hits;
	GLint viewport[4];


	if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
		return;


	glGetIntegerv(GL_VIEWPORT, viewport);
	glSelectBuffer(BUFSIZE, selectBuf);


	(void)glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);


	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	/*  create 5x5 pixel picking region near cursor location    */
	gluPickMatrix((GLdouble)x, (GLdouble)(viewport[3] - y),
		5.0, 5.0, viewport);
	//glOrtho(-5.0, 5.0, -5.0, 5.0, -5.0, 5.0);
	display(GL_SELECT);


	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW); /*respeta la logica
								de maniopulacion de glMatrixMode
								tomada en beziercurv()*/
	hits = glRenderMode(GL_RENDER);
	processHits(hits, selectBuf);
	glutPostRedisplay();
}


void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'Q':
		if (cuerpo != 25) {
			cuerpo = (cuerpo + 1) % 360;
			glutPostRedisplay();
		}
		else {
			glutPostRedisplay();
		}
		break;
	case 'q':
		if (cuerpo != -25) {
			cuerpo = (cuerpo - 1) % 360;
			glutPostRedisplay();
		}
		else {
			glutPostRedisplay();
		}
		break;
	case 'W':
		if (cabeza != 75) {
			cabeza = (cabeza + 1) % 360;
			glutPostRedisplay();
		}
		else {
			glutPostRedisplay();
		}
		break;
	case 'w':
		if (cabeza != -75) {
			cabeza = (cabeza - 1) % 360;
			glutPostRedisplay();
		}
		else {
			glutPostRedisplay();
		}
		break;
	}
}


///////////////////////////////////////////////////////////


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(/*GLUT_DOUBLE*/GLUT_SINGLE | GLUT_RGBA | GLUT_STENCIL);
	glutInitWindowSize(200, 200);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutMouseFunc(pick_Mago);
	glutKeyboardFunc(keyboard);
	SetupRC();
	glutMainLoop();
	return 0;
}

