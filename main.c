
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include <GL/gl.h>
#include <GL/glut.h>

#define VIEWHEIGHT 500
#define VIEWWIDTH 500
#define OFFHEIGHT 100
#define OFFWIDTH 100

#define MINX -30.0f
#define MAXX 30.0f
#define MINY -500.0f
#define MAXY 500.0f

struct pt {
	GLint x;
	GLint y;
} linestart, lineend, wnddim, circle[800];

GLfloat viewport[VIEWHEIGHT][VIEWWIDTH][3];

bool ptCompare(struct pt lhs, struct pt rhs)
{
	if(lhs.x != rhs.x || lhs.y != rhs.y)
		return false;
	return true;
}

struct pt dispToCoord(struct pt pos);
void drawView(void);
void drawFunc(void);
void display(void);
void resize(GLsizei width, GLsizei height);
void mpress(int btn, int state, int x, int y);

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	drawView();
	/* drawFunc(); */
	glFlush();
 	glutSwapBuffers();
}

struct pt dispToCoord(struct pt pos)
{
	return (struct pt){
		(pos.x + OFFWIDTH) * wnddim.x / (VIEWWIDTH + 2 * OFFWIDTH),
			(pos.y + OFFHEIGHT) * wnddim.y / (VIEWHEIGHT + 2 * OFFHEIGHT)
			};
}

float dispToFuncX(int x)
{
	return x * (MAXX - MINX) /
		(VIEWWIDTH - 1) + MINX;
}

float dispToFuncY(int y)
{
	return (y - OFFHEIGHT) * (MAXY - MINY) /
		(VIEWHEIGHT - 1) + MINY;
}

void mpress(int btn, int state, int x, int y)
{
	y = wnddim.y - y;
	if(btn == GLUT_LEFT_BUTTON) {
		if(x < OFFWIDTH || x > OFFWIDTH + VIEWWIDTH ||
			 y < OFFHEIGHT || y > OFFHEIGHT + VIEWHEIGHT) {
			/* Outside the viewport */
			printf("x: %d\ny: %d\n", x, y);
		}
		else {
			printf("x: %f\ny: %f\n", dispToFuncX(x),
						 dispToFuncY(y));
		}
	}
}

void drawView(void)
{
	/* Draws the data stored in the viewport array */
	glBegin(GL_POINTS);
	struct pt bound_bl = dispToCoord((struct pt){0, 0}),
		bound_tr = dispToCoord((struct pt){VIEWWIDTH, VIEWHEIGHT}),
		pos = {0, 0}, offset;
	for(pos.x = bound_bl.x, offset.x = 0; pos.x < bound_tr.x; pos.x++, offset.x++)
		for(pos.y = bound_bl.y, offset.y = 0; pos.y < bound_tr.y; pos.y++, offset.y++) {
			glColor3f(viewport[offset.y][offset.x][0],
								viewport[offset.y][offset.x][1],
								viewport[offset.y][offset.x][2]);
			glVertex2i(pos.x, pos.y);
		}
	glEnd();
}

float f(float x)
{
	return 500 * exp(-x * x / 150) * sin(2 * x);
}

float g(float x)
{
	return 300 * exp(-x * x / 300);
}

void drawFunc(void)
{
	int i;
	glBegin(GL_POINTS);
	float fY, gY;
	for(i = OFFWIDTH; i < OFFWIDTH + VIEWWIDTH; i++) {
		/* Calculate the functions X coordinate */
		float funcX = dispToFuncX(i - OFFWIDTH);
		int j;
		/* Cheating.
		 * I considered using my line drawing algorithm,
		 * and calculating the points of intersection of
		 * the lines, which is probably the "good" way of
		 * doing this, but didn't like that it would miss
		 * points in functions. 
		 * Now I still miss points, but considerably less,
		 * at the expense of more computation. It also can
		 * overwrite points which marked intersections,
		 * which is also bad.
		 * Obviously the best solution depends on the goal
		 * of the program, but I don't think this is ever it,
		 * especially since I don't really take the function
		 * into account on how many points I calculate */
		int vfY = INT_MAX,
			vgY = INT_MAX,
			vfYprev, vgYprev;
		/* Number of points to draw per x coordinate */
		const int xpCount = 64;
		for(j = 0; j < 64; j++) {
			fY = f(funcX);
			gY = g(funcX);
			funcX += 1 / (float)xpCount * (MAXX - MINX) / (VIEWWIDTH - 1);
			/* So we take the functions into account at least
			 * a little, don't draw points already drawn by
			 * the function */
			vfYprev = vfY;
			vgYprev = vgY;
			/* Convert the Y coordinate to the viewports coordinates */
			vfY = (fY - MINY) * VIEWHEIGHT / (MAXY - MINY) + OFFWIDTH;
			vgY = (gY - MINY) * VIEWHEIGHT / (MAXY - MINY) + OFFWIDTH;
			if(vfY == vfYprev && vgY == vgYprev)
				continue;
			if(vfY == vgY) {
				glColor3f(1.0f, 0.0f, 0.0f);
				glVertex2i(i, vfY);
			}
			else {
				glColor3f(0.0f, 0.0f, 1.0f);
				glVertex2i(i, vfY);
				glVertex2i(i, vgY);
			}
		}
	}
	puts("");
	glEnd();
}

void resize(GLsizei width, GLsizei height)
{
	wnddim.x = width;
	wnddim.y = height;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, width, 0.0,
					height, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
}

void keypress(unsigned char key, int x, int y)
{
	switch(key) {
	case 'q':
	case 'Q':
		exit(0);
	}
}

void initViewport()
{
	struct pt idx;
	for(idx.y = 0; idx.y < VIEWHEIGHT; idx.y++)
		for(idx.x = 0; idx.x < VIEWWIDTH; idx.x++) {
			float *coord = viewport[idx.y][idx.x];
			if(coord[0] == 0.0f && coord[2] == 0.0f) {
				coord[0] = 1.0f;
				coord[1] = 1.0f;
				coord[2] = 1.0f;
			}
		}
}

void calcFuncs()
{
	/* Precompute the functions and how they look on the viewport *
	 * Use one color for each function, *
	 * and then finish with a final pass */
	int i;
	for(i = 0; i < VIEWWIDTH; i++) {
		float funcX = dispToFuncX(i);
		int vfY, vgY;
		/* Number of points to draw per x coordinate */
		const int xpCount = 64;
		int j;
		for(j = 0; j < 64; j++) {
			float fY = f(funcX);
			float gY = g(funcX);
			funcX += 1 / (float)xpCount * (MAXX - MINX) / (VIEWWIDTH - 1);
			/* Convert the Y coordinate to the viewports coordinates */
			vfY = (fY - MINY) * VIEWHEIGHT / (MAXY - MINY);
			vgY = (gY - MINY) * VIEWHEIGHT / (MAXY - MINY);
			if(vfY < VIEWHEIGHT && vfY > 0)
				viewport[vfY][i][0] = 1.0f;
			if(vgY < VIEWHEIGHT && vgY > 0)
				viewport[vgY][i][2] = 1.0f;
		}
	}
	for(i = 0; i < VIEWHEIGHT; i++) {
		int j;
		for(j = 0; j < VIEWWIDTH; j++) {
			if(viewport[i][j][0] && viewport[i][j][2])
				viewport[i][j][2] = 0.0f;
			else if(viewport[i][j][0]) {
				viewport[i][j][0] = 0.0f;
				viewport[i][j][2] = 1.0f;
			}
		}
	}
}

int main(int argc, char **argv)
{
	memset(&linestart, 0, sizeof(linestart));
	memset(&lineend, 0, sizeof(lineend));
	memset(&wnddim, 0, sizeof(wnddim));
	memset(viewport, 0.0f, sizeof(viewport));
	calcFuncs();
	initViewport();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 10);
	glutInitWindowSize(700, 700);
	glutCreateWindow("Program 1");
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutMouseFunc(mpress);
	glutKeyboardFunc(keypress);
	glutMainLoop();
  return 0;
}
