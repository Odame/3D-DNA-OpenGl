#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>

const float HELIX_RADIUS = 7.0f;
const float HELIX_HEIGHT = 100.0f;
const float HELIX_SPACING = 5.0f;
static float PITCH = 2.0f;
const float MOLECULE_SPACING = 0.25f;
const float MOLECULE_RADIUS = 0.4f;
const int LINKER_SPACING = 2;
static int BASE_PAIR_SEQUENCE[22];
const float ROTATION_SPEED = 3.0f;

enum COLOR: int {
    YELLOW = 0,
    GREEN,
    BLUE,
    RED,
    PURPLE,
    WHITE
};


const float CIRCLE = 3.14159/180;
static float d=0, z=1.0, x_rotation_angle=0.0, y_rotation_angle=0.0;
static boolean auto_rotate = true;
static int win;
static int menid;
static int submenid;
static int primitive = 0;
static int width = 800;
static int height = 700;

static char ref1[]="REFERENCES:";
static char ref2[]="http://www.livescience.com/37247-dna.html";
static char ref3[]="https://www.cpe.ku.ac.th/~jeab/course/CG/2552-2/3DNA/";
static char* refText[]={ref1, ref2, ref3};

static char text1[30]="DNA STRUCTURE";
static char text2[80]="The human DNA has a double helix structure which is stabilized by hydrogen";
static char text3[80]="bonds between the bases attached to the two strands. The four bases found in";
static char text4[70]="DNA are adenine(A), cytosine(C), guanine(G) and thymine (T)";
static char text5[70]="These four bases are attached to the sugar/phosphate to form ";
static char text6[70]="the complete nucleotide strand.";
static char* infoText[]={
    text1, text2, text3, text4, text5, text6
};

static char menu1[30]="CONTROL PANEL";
static char menu2[30]= "+/- : Zoom in/out";
static char menu3[30]= "4/6 : Y-axis rotation";
static char menu4[30]= "8/2 : X-axis rotation";
static char menu5[30]= "S/s : Stretch/Compress helix";
static char menu6[30]= "c : Restore Default Value";
static char* menuText[] = {
    menu1, menu2, menu3, menu4, menu5, menu6
};

void createMenu(void);
void menu(int value);
void disp(void);

bool _highShininess = false; //Whether the shininess parameter is high
bool _lowSpecularity = false; //Whether the specularity parameter is high
bool _emission = true; //Whether the emission parameter is turned on

void createMenu(void){
  // Create a submenu, this has to be done first.
  submenid = glutCreateMenu(menu);
  // Add sub menu entry
  glutAddMenuEntry("Normal", 2);
  glutAddMenuEntry("DNA Structure", 3);
  // Create the menu, this menu becomes the current menu
  menid = glutCreateMenu(menu);
  // Create an entry
  glutAddMenuEntry("Clear", 1);
  glutAddSubMenu("Draw", submenid);
  // Create an entry
  glutAddMenuEntry("Quit", 5);
  // Let the menu respond on the right mouse button
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void menu(int value){
  if(value == 0){
    glutDestroyWindow(win);
    exit(0);
  }else{
    primitive=value;
  }
  // you would want to redraw now
  glutPostRedisplay();
}

void handleKeypress(unsigned char key, int x, int y) {
    printf("%d\n", key);
	switch (key) {
		case 27: //Escape key
			exit(0);
			break;
        case '+':  // Zoom
             z = (z + 0.1) ;
             glutPostRedisplay();
             break;
        case '-':
             z = (z - 0.1) ;
             glutPostRedisplay();
             break;
        case '6':  // y axis rotate (Auto)
              y_rotation_angle += ROTATION_SPEED + 5;
              auto_rotate = false;
              glutPostRedisplay();
              break;
         case '4':
              //r = (r - 2) ;
              y_rotation_angle -= ROTATION_SPEED + 5;
              auto_rotate = false;
              glutPostRedisplay();
              break;
         case '2':  // x axis rotate
              x_rotation_angle += ROTATION_SPEED + 5;
              glutPostRedisplay();
              break;
         case '8':
              x_rotation_angle -= ROTATION_SPEED + 5;
              glutPostRedisplay();
              break;
         case '5': //auto mode.
              d=0, z=1.0, _emission = true, _lowSpecularity = false, _highShininess = false ;
              auto_rotate=true;
              glutPostRedisplay();
              break;
         case 'c':
         case 'C':
              d=0, z=1.0, _emission = true, _lowSpecularity = false, _highShininess = false ;
              auto_rotate=true;
              x_rotation_angle=0;
              glutPostRedisplay();
              break;
         case 's':
              PITCH -= 0.2;
              if(PITCH<=0){
                PITCH = 0.2;
              }
              glutPostRedisplay();
              break;
         case 'S':
              PITCH += 0.2;
              glutPostRedisplay();
              break;
	}
}

void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_COLOR_MATERIAL); //Disable color materials, so that glMaterial calls work
}

void handleResize(int w, int h) {
	glViewport(-225, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (float)w / (float)h, 1.0, 200.0);
}

void initLight(float radius) {
   	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -20.0);
	GLfloat ambientLight[] = {0.2, 0.2, 0.2, 1.0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	GLfloat lightColor[] = {0.6, 0.6, 0.6, 1.0};
	GLfloat lightPos[] = {1.5 * radius, 3 * radius, 1.5 * radius, 1.0};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);   	//Diffuse (non-shiny) light component
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);   	//Specular (shiny) light component
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
}

void draw_circle(float radius, float r, float g, float b) {
	//Determine the specularity, emissivity, and shininess parameters, based on
	//variables that can be toggled with keystrokes
	float specularity;
	if (_lowSpecularity) {
		specularity = 0.3;
	}
	else {
		specularity = 1.5;
	}
	float emissivity;
	if (_emission) {
		emissivity = 0.03;
	}
	else {
		emissivity = 0;
	}
	float shininess;
	if (_highShininess) {
		shininess = 25;
	}
	else {
		shininess = 12;
	}
	GLfloat materialColor[] = {r, g, b, 1.0};	  //The color of the sphere
	GLfloat materialSpecular[] = {specularity, specularity, specularity, 1.0};	  //The specular (shiny) component of the material
	GLfloat materialEmission[] = {emissivity, emissivity, emissivity, 1.0};	   //The color emitted by the material
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, materialColor);
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialfv(GL_FRONT, GL_EMISSION, materialEmission);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess); //The shininess parameter
    glutSolidSphere(radius, 20, 20);
}

void draw_circle_object(float radius, float x, float y, float z, COLOR color) {
     glTranslatef(x,y,z);
     if(color==YELLOW)
        draw_circle(radius, 1.0, 1.0, 0.0);
     else if(color==RED)
        draw_circle(radius, 1.0, 0.0, 0.0);
	 else if(color==GREEN)
        draw_circle(radius, 0.0, 1.0, 0.0);
	 else if(color==PURPLE)
        draw_circle(radius, 0.6, 0.4, 0.7);
     else if(color==BLUE)
        draw_circle(radius, 0.0, 0.0, 1.0);
	 else if(color==WHITE)
        draw_circle(radius, 255.0, 255.0, 255.0);
     glTranslatef(-x,-y,-z);
     glClearColor(0.0f, 0.0f, 0.0f, 0.5);
}

void drawModel() {
    glColor3f(0.0,0.0,0.0);
    ///draw the two helices
    float x1, y1, z1, x2, y2, z2;
    float t=HELIX_HEIGHT/2 *-1;

    /// draw the helix
    int linker_position = 0;
    while(t < HELIX_HEIGHT/2){
        //the coordinates for the first helix's molecule
        x1 = HELIX_RADIUS * cos(t); /// x(t) = radius * cos(t)
        z1 = HELIX_RADIUS * sin(t); /// z(t) = radius * sin(t)
        y1 = PITCH * t;       /// y(t) = pitch * t

        //the coordinates for the second helix's corresponding molecule
        x2 = x1;
        y2 = y1 + HELIX_SPACING; //translation in the y-direction
        z2 = z1 - HELIX_SPACING; //translation in the z-direction

        t += MOLECULE_SPACING;

        //draw the molecules
        draw_circle_object(MOLECULE_RADIUS * 2, x1, y1, z1, PURPLE); //first helix's molecule
        draw_circle_object(MOLECULE_RADIUS * 2, x2, y2, z2 , PURPLE); //second helix's molecule


        if((linker_position+1)%LINKER_SPACING==0){
            /// draw the linker
            COLOR color1;
            COLOR color2;
            int mode = BASE_PAIR_SEQUENCE[linker_position%22];
            float linker_item_y = y1;
            float linker_item_z = z1;
            float dy = fabs(y1-y2);
            float dz = fabs(z1-z2);
            float gradient = dy/dz;

            /// cytosine C >>> yellow >> 0
            /// guanine G >>> green >> 1
            /// adenine A >>> blue >> 2
            /// thymine T >>> red >> 3

            switch(mode){
                /// A-T
                case 0:
                    color1=BLUE;
                    color2=RED;
                    break;

                /// T-A
                case 1:
                    color1=RED;
                    color2=BLUE;
                    break;

                /// C-G
                case 2:
                    color1=YELLOW;
                    color2=GREEN;
                    break;

                /// G-C
                case 3:
                    color1=GREEN;
                    color2=YELLOW;
                    break;
            }

            int item_counter = 0;
            while((linker_item_y <= y2) || (linker_item_z >= z2)){
                COLOR color;
                if(item_counter <= 5){
                    color = color1; //the lower part of the base-pair
                }
                else{
                    color = color2;  //the upper part of the base-pair
                }
                item_counter++;

                draw_circle_object(MOLECULE_RADIUS , x2, linker_item_y, linker_item_z, color);
                linker_item_y += (dy/gradient *0.1 );
                linker_item_z -= (dz/gradient *0.1 );
            }
        }
        linker_position++;
    }
}

void renderBitmapString(float x, float y, float z, void *font, char *string) {
	char *c;
	glRasterPos3f(x, y,z);
	for (c=string; *c != '\0'; c++) {
		glColor3d(0, 255, 255);
		glutBitmapCharacter(font, *c);
	}
}

void drawText(){
    //show the control panel text
    float print_location_x = -80;
    float print_location_y = -40;
    int print_line_spacing = 5;
    int i=0;
    for(i=0;i<6;i++){
        renderBitmapString(print_location_x, print_location_y-(print_line_spacing*i), 0, GLUT_BITMAP_HELVETICA_18, menuText[i]);
    }

    //show the reference link
    print_location_x = 50;
    print_location_y = -50;
    for(i=0;i<3;i++){
        renderBitmapString(print_location_x, print_location_y-(print_line_spacing*i), 0, GLUT_BITMAP_HELVETICA_18, refText[i]);
    }

    //show the info text
    print_location_x = 50;
    print_location_y = 60;
    print_line_spacing = 5;
    for(i=0;i<6;i++){
        renderBitmapString(print_location_x, print_location_y-(print_line_spacing*i), 0, GLUT_BITMAP_HELVETICA_18, infoText[i]);
    }
}

void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glutFullScreen();
	initLight(20);  	// Setting Light Environment

    ///camera positioning
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    gluLookAt(  0, 0, 130,
                0, 0, 0,
                0, 1, 0
    );
    /// /////////////////////

    glPushMatrix();
        glScalef(z,z,z);
        glRotatef(y_rotation_angle,0.0,1.0,0.0);
        glRotatef(x_rotation_angle,1.0,0.0,0.0);
        drawModel();
    glPopMatrix();

    /// display the text
    glPushMatrix();
        drawText();
    glPopMatrix();

	glutSwapBuffers();
}

//Called every 25 milliseconds
void update(int value) {
	d += 0.1;
    if(auto_rotate){
        y_rotation_angle += ROTATION_SPEED;
    }
	glutPostRedisplay();
	glutTimerFunc(25, update, 0);
}

void initRandomSequence(){
    srand(22);
    int i;
    for(i=0;i<22;i++){
        BASE_PAIR_SEQUENCE[i] = rand()%4;
    }
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutCreateWindow("3D model of the DNA of a HUMAN");
	createMenu();
	initRandomSequence();
	initRendering();
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(handleResize);
	glutTimerFunc(25, update, 0);
	glutMainLoop();
	return 0;
}
