#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "cargadorOBJ.cpp"

//Variables para interacción con el usuario.
GLfloat tx = 0;
GLfloat ty = 0;
GLfloat rx = 0;
GLfloat ry = 0;
GLfloat es = 1;
GLboolean proy_orto = GL_FALSE;
GLboolean somb_plano = GL_FALSE;

ModeloOBJ cubo("./modelos/cubo_tierra.obj","./texturas/hierba.jpg");
ModeloOBJ cabeza("./modelos/cabeza_v.obj","./texturas/vaca2.jpg");
ModeloOBJ cuerpo("./modelos/cuerpo_v.obj","./texturas/vaca2.jpg");

void dibujar_escena() {
    glPushMatrix();
        glTranslatef(0,2,0);
        cabeza.dibujar();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-1.5,1.5,0);
        cuerpo.dibujar();
    glPopMatrix();


    glPushMatrix();
        for(float i=-5;i<=5;i++){
            for(float j=-5; j<=5;j++){
                glPushMatrix();
                glTranslatef(i,0,j);
                glRotatef(90, 0, 0, 1);
                cubo.dibujar();
                glPopMatrix();
            }
        }
    glPopMatrix();
    
}

void dibujar() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glPushMatrix();
        glTranslatef(tx, ty, 0);
        glRotatef(rx, 1, 0, 0);
        glRotatef(ry, 0, 1, 0);
        glScalef(es, es, es);
        dibujar_escena();
    glPopMatrix();

    glutSwapBuffers();
}

void camara (void) {            
    float ancho = GLUT_WINDOW_WIDTH;
    float alto = GLUT_WINDOW_HEIGHT;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (proy_orto) {
        glOrtho(-5, 5, -5, 5, -5, 5);    
    }
    else {
        gluPerspective(45, ancho/alto, 0.1, 1000);
        gluLookAt(0, 0, 15, 0, 0, 0, 0, 1, 0);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void teclado (unsigned char key, int x, int y) {
        switch (key) {
        //PROYECCIÓN
        case 'P': case 'p':
            proy_orto = !proy_orto;
            camara();
            break;
        //SOMBREADO
        case ' ':
            somb_plano = !somb_plano;
            somb_plano ? glShadeModel(GL_FLAT) : glShadeModel(GL_SMOOTH);
            break;
        //TRASLACIÓN
        case 'W': case 'w': ty += 0.1; break;
        case 'S': case 's': ty -= 0.1; break;
        case 'A': case 'a': tx -= 0.1; break;
        case 'D': case 'd': tx += 0.1; break;
        //ROTACIÓN
        case 'I': case 'i': rx += 5; break;
        case 'K': case 'k': rx -= 5; break;
        case 'J': case 'j': ry += 5; break;
        case 'L': case 'l': ry -= 5; break;
        //ESCALA
        case '+': es += 0.1; break;
        case '-': es -= 0.1; break;
        case 27:
            exit(0);
            break;
    }
    glutPostRedisplay();
}

void redimensionar (int ancho, int alto) {
    if (alto == 0) alto = 1;
    int lado = ancho < alto ? ancho : alto;
    glViewport((ancho - lado) / 2, (alto - lado) / 2, lado, lado);
    camara();
}

void config_GLUT(void) {
    glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Textura");
    glutDisplayFunc(dibujar);
    glutKeyboardFunc(teclado);
    glutReshapeFunc(redimensionar);
}

void config_OGL(void) {
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_TEXTURE_2D);
    camara();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    config_GLUT();
    config_OGL();
    glutMainLoop();
    return 0;
}