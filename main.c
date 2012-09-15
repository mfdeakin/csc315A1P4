
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <GL/glut.h>

#define VIEWHEIGHT 500.0
#define VIEWWIDTH 500.0
#define OFFHEIGHT 100
#define OFFWIDTH 100
#define CENTERX 350
#define CENTERY 350

struct pt {
	GLint x;
	GLint y;
	bool valid;
} linestart, lineend, wnddim, circle[800];

bool ptCompare(struct pt lhs, struct pt rhs)
{
	if(lhs.x != rhs.x || lhs.y != rhs.y)
		return false;
	return true;
}

struct pt dispToCoord(struct pt pos);
void drawView(void);
void drawCircle(float radius);
void drawCircle2(float radius);
void drawLine(void);
void display(void);
void resize(GLsizei width, GLsizei height);
void mpress(int btn, int state, int x, int y);

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	drawView();
	drawLine();
	/* drawCircle(200); */
	glFlush();
 	glutSwapBuffers();
}

void drawLine(void)
{
	struct pt bound_bl = dispToCoord((struct pt){0, 0, false}),
		bound_tr = dispToCoord((struct pt){VIEWWIDTH, VIEWHEIGHT, false});
	if(!linestart.valid || !lineend.valid ||
		 ptCompare(linestart, lineend) ||
		 linestart.x < bound_bl.x ||
		 linestart.x > bound_tr.x ||
		 lineend.x < bound_bl.x ||
		 lineend.x > bound_tr.x ||
		 linestart.y < bound_bl.y ||
		 linestart.y > bound_tr.y ||
		 lineend.y < bound_bl.y ||
		 lineend.y > bound_tr.y)
		return;
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	GLint deltax = lineend.x - linestart.x,
		deltay = lineend.y - linestart.y,
		x = 0,
		y = 0;
	
	GLfloat m = (float)deltay / deltax;

	if(m == INFINITY) {
		while(y < deltay) {
			glVertex2i(linestart.x, linestart.y + y);
			y++;
		}
	}
	else if(m == -INFINITY) {
		while(y > deltay) {
			glVertex2i(linestart.x, linestart.y + y);
			y--;
		}
	}
	else {
		float residual = 0;
		while(x > deltax) {
			x--;
			int cnt = 0;
			do {
				glVertex2i(linestart.x + x, linestart.y + y);
				cnt++;
				y--;
			} while(cnt < m + residual);
			do {
				glVertex2i(linestart.x + x, linestart.y + y);
				cnt--;
				y++;
			} while(cnt > m + residual);
			residual = m + residual - cnt;
		}
		while(x < deltax) {
			x++;
			int cnt = 0;
			do {
				glVertex2i(linestart.x + x, linestart.y + y);
				cnt++;
				y++;
			} while(cnt < m + residual);
			do {
				glVertex2i(linestart.x + x, linestart.y + y);
				cnt--;
				y--;
			} while(cnt > m + residual);
			residual = m + residual - cnt;
		}
	}
	glEnd();
}

float inCircle(float x, float y, float rad)
{
	return x * x + y * y - rad * rad;
}

void drawCircle(float radius)
{
	float curx = 0, cury = radius;
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POINTS);
	while(cury >= curx) {
		glVertex2i(CENTERX + curx, CENTERY + cury);
		glVertex2i(CENTERX - curx, CENTERY + cury);
		glVertex2i(CENTERX + curx, CENTERY - cury);
		glVertex2i(CENTERX - curx, CENTERY - cury);

		glVertex2i(CENTERX + cury, CENTERY + curx);
		glVertex2i(CENTERX - cury, CENTERY + curx);
		glVertex2i(CENTERX + cury, CENTERY - curx);
		glVertex2i(CENTERX - cury, CENTERY - curx);
		float old = inCircle(curx++, cury - 0.5, radius);
		if(old >= 1) {
			cury--;
		}
		else if(old >= 0) {
			cury -= 0.5;
		}
	}
	glEnd();
}

struct pt dispToCoord(struct pt pos)
{
	return (struct pt){
		(pos.x + OFFWIDTH) * wnddim.x / (VIEWWIDTH + 2 * OFFWIDTH),
			(pos.y + OFFHEIGHT) * wnddim.y / (VIEWHEIGHT + 2 * OFFHEIGHT),
			true
			};
}

void mpress(int btn, int state, int x, int y)
{
	y = wnddim.y - y;
	struct pt check;
	check.x = 0;
	check.y = 0;
	check.valid = true;
	struct pt checkbl = dispToCoord(check);
	check.x = VIEWWIDTH;
	check.y = VIEWHEIGHT;
	check.valid = true;
	struct pt checktr = dispToCoord(check);
	if(x < checkbl.x || y < checkbl.y ||
		 x > checktr.x || y > checktr.y)
		return;
	if(btn == GLUT_LEFT_BUTTON) {
		linestart.x = x;
		linestart.y = y;
		linestart.valid = true;
		glutPostRedisplay();
	}
	else if(btn == GLUT_RIGHT_BUTTON) {
		lineend.x = x;
		lineend.y = y;
		lineend.valid = true;
		glutPostRedisplay();
	}
}

void drawView(void)
{
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POINTS);
	struct pt bound_bl = dispToCoord((struct pt){0, 0, false}),
		bound_tr = dispToCoord((struct pt){VIEWWIDTH, VIEWHEIGHT, false}),
		pos = {0, 0, true};
	for(pos.x = bound_bl.x; pos.x < bound_tr.x; pos.x++)
		for(pos.y = bound_bl.y; pos.y < bound_tr.y; pos.y++) {
			glVertex2i(pos.x, pos.y);
		}
	glEnd();
}

void resize(GLsizei width, GLsizei height)
{
	wnddim.x = width;
	wnddim.y = height;
	wnddim.valid = true;
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

int gcf(int a, int b)
{
	if(a % b)
		return gcf(b, a % b);
	return b;
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
