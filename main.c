
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include <GL/gl.h>
#include <GL/glut.h>

#define VIEWHEIGHT 500.0
#define VIEWWIDTH 500.0
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
	drawFunc();
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
	return (x - OFFWIDTH) * (MAXX - MINX) /
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
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POINTS);
	struct pt bound_bl = dispToCoord((struct pt){0, 0}),
		bound_tr = dispToCoord((struct pt){VIEWWIDTH, VIEWHEIGHT}),
		pos = {0, 0};
	for(pos.x = bound_bl.x; pos.x < bound_tr.x; pos.x++)
		for(pos.y = bound_bl.y; pos.y < bound_tr.y; pos.y++) {
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
		float funcX = dispToFuncX(i);
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

int main(int argc, char **argv)
{
	memset(&linestart, 0, sizeof(linestart));
	memset(&lineend, 0, sizeof(lineend));
	memset(&wnddim, 0, sizeof(wnddim));
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
