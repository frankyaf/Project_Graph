#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "cargadorOBJ.cpp"
#include "camara.cpp"
#define NUM_SUBDIVS 50
#define GRADO_CURVA 3


#define PI 3.14159265358979323846

//Macros que definen la velocidad de traslación y rotación de la cámara.
#define VELOCIDAD_MOV 1
#define VELOCIDAD_ROT 0.1

//Creación del objeto de cámara.
Camara cam (0, 6, 15,    //Cámara posicionada en el centro.
            0, 0, 1,    //Cámara viendo hacia el fondo de la pantalla (+Z)
            0, 1, 0);   //Cámara sin rotación (UP = +Y)
//Rotación de la cámara sobre su eje X local (voltear arriba y abajo.)
GLfloat rotX = 0.0f;
GLfloat rot_1= 0.0f;
GLfloat rot_2= 45.0f;

//Variables para interacción con el usuario.
GLfloat tx = 0;
GLfloat tz = 0;
GLfloat ry = 0;
GLfloat es = 1;
GLboolean proy_orto = GL_FALSE;
GLboolean somb_plano = GL_FALSE;

struct t_punto {
    float x;
    float y;
    float z;
};

typedef struct t_punto Punto_2;

Punto_2 P_2[GRADO_CURVA+1] = {
    { 0, -0.014, 0},
    { -1.46, 0.2, 0},
    { -0.96, -0.56, 0},
    { -0.006,1.424, 0}
};

typedef struct t_punto Punto;

Punto P[GRADO_CURVA+1] = {
    { 0, -0.014, 0},
    { -0.95, 0.16, 0},
    { -0.8, -0.71, 0},
    { -0.8,  2.5, 0}
};

Punto *puntos_curva = NULL;
Punto *normales_curva = NULL;

Punto_2 *puntos_curva2 = NULL;
Punto_2 *normales_cruva2 = NULL;

GLuint n1_tex,n3_tex;

unsigned char *torre_tex = NULL;
unsigned char *tex_d =NULL;

int ancho_tex_torre, alto_tex_torre, num_comps_tex_torre;
int ancho_tex_d, alto_tex_d, num_comps_tex_d;

ModeloOBJ cubo("./modelos/cubo_tierra.obj","./texturas/hierba.jpg");
ModeloOBJ cabeza("./modelos/cabeza_v.obj","./texturas/vaca2.jpg");
ModeloOBJ cuerpo("./modelos/cuerpo_v.obj","./texturas/vaca2.jpg");
ModeloOBJ pata("./modelos/pata_v_1.obj","./texturas/vaca2.jpg");

void config_torre_tex(){
    torre_tex = stbi_load("./texturas/torre.jpg", &ancho_tex_torre, &alto_tex_torre, &num_comps_tex_torre, 0);
    glGenTextures(1, &n1_tex);
    glBindTexture(GL_TEXTURE_2D, n1_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void config_tex_punta() {
    tex_d = stbi_load("./texturas/tejado.jpg", &ancho_tex_d, &alto_tex_d, &num_comps_tex_d, 0);
    glGenTextures(1, &n3_tex);
    glBindTexture(GL_TEXTURE_2D, n3_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

float fac(float n) { return (n==1.0 || n==0.0) ? 1: n * fac(n - 1); }

float coef_bin(float n, float i) { return (fac(n) / (fac(i)*fac(n-i))); };

Punto *calcular_curva_bezier (Punto *P, int n, int subdivs) {
    //Inicialización de variables auxiliares.
    float dt = 1.0 / (float) subdivs;
    float x = 0, y = 0, z = 0, t;
    int i, subdiv = 0;
    //Se crea el arreglo que contendrá los puntos calculados.
    Punto *puntos = (Punto *)malloc((NUM_SUBDIVS+1)*sizeof(Punto));

    //Se calculan los puntos que caen sobre la curva y se llena el arreglo con ellos.
    for (t = 0; t < 1.000001; t += dt) {
        x = 0, y = 0, z = 0;
        for (i = 0; i <= n; i++) {    
            puntos[subdiv].x += coef_bin(n, i) * pow((1-t), (n-i)) * pow(t, i) * P[i].x;
            puntos[subdiv].y += coef_bin(n, i) * pow((1-t), (n-i)) * pow(t, i) * P[i].y;
            puntos[subdiv].z += coef_bin(n, i) * pow((1-t), (n-i)) * pow(t, i) * P[i].z;
        }
        subdiv ++;
    }
    //Se regresa el arreglo con los puntos calculados.
    return puntos;
}

void revolucion_Y(Punto *puntos, int n, int subdivs) {
    float Dt = 2 * PI / subdivs;
    float x, y, z;
    float t, r;
    int i;

    for (i = 0; i < n-1; i++) {
        glBegin(GL_QUAD_STRIP);
        for (t = 0; t <= 2*PI+0.0001; t += Dt) {
            r = puntos[i].x;
            x = r * cos(t);
            y = puntos[i].y;
            z = r * sin(t);
            glNormal3f(x, y, z);
            glTexCoord2f(t/(2*PI), y);
            glVertex3f(x, y, z);
            r = puntos[i+1].x;
            x = r * cos(t);
            y = puntos[i+1].y;
            z = r * sin(t);
            glNormal3f(x, y, z);
            glTexCoord2f(t/(2*PI), y);
            glVertex3f(x, y, z);
        }
        glEnd();
    }

}   

void dibujar_sup_revolucion(void) {
    glEnable(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ancho_tex_torre, alto_tex_torre,
        0, GL_RGB, GL_UNSIGNED_BYTE, torre_tex);
    revolucion_Y(puntos_curva, NUM_SUBDIVS+1, 50);
    glDisable(GL_TEXTURE_2D);
}

void dibujar_sup_revolucion_2(void){
    glEnable(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ancho_tex_d, alto_tex_d,
        0, GL_RGB, GL_UNSIGNED_BYTE, tex_d);
    revolucion_Y(puntos_curva2, NUM_SUBDIVS+1, 50);
    glDisable(GL_TEXTURE_2D);
}

void pata_animada_1(){
    if(rot_1 < 45){
        glRotatef(rot_1,0,0,1);
    }else{
        glRotatef(rot_2,0,0,1);
    }
}

void pata_animada_2(){
    if(rot_1 < 45){
        glRotatef(-rot_1,0,0,1);
    }else{
        glRotatef(-rot_2,0,0,1);
    }
}

void dibujar_patas(){
    glPushMatrix();
        
        //Pata delantera y trasera 1
        glPushMatrix();
            
            glPushMatrix();
                glTranslatef(-0.8,0.5,0.3);
                glRotatef(45,0,0,1);
                glScalef(0.5,0.5,0.5);
                glPushMatrix();
                    pata_animada_2();    
                    pata.dibujar();
                glPopMatrix();
            glPopMatrix();

            glPushMatrix();
                glTranslatef(-2.2,0.5,0.3);
                glScalef(0.5,0.5,0.5);
                glPushMatrix();
                    pata_animada_1();    
                    pata.dibujar();
                glPopMatrix();
            glPopMatrix();
            
        glPopMatrix();

        //Pata delantera y trasera 2
        glPushMatrix();

            glPushMatrix();
                glTranslatef(-0.8,0.5,-0.3);
                glScalef(0.5,0.5,0.5);
                glPushMatrix();
                    pata_animada_1();    
                    pata.dibujar();
                glPopMatrix();
            glPopMatrix();

            glPushMatrix();
                glTranslatef(-2.2,0.5,-0.3);
                glRotatef(45,0,0,1);
                glScalef(0.5,0.5,0.5);
                glPushMatrix();
                    pata_animada_2();    
                    pata.dibujar();
                glPopMatrix();
            glPopMatrix();
            
        glPopMatrix();

    glPopMatrix();
}

void dibujar_vaca() {

    glPushMatrix();
        glTranslatef(tx, -0.5, tz);
        glRotatef(ry, 0, 1, 0);
        glPushMatrix();
            glTranslatef(0,2,0);
            cabeza.dibujar();
        glPopMatrix();

        glPushMatrix();
            glTranslatef(-1.5,1.5,0);
            glRotatef(180,0,1,0);
            cuerpo.dibujar();
        glPopMatrix();
        
        glPushMatrix();
            dibujar_patas();
        glPopMatrix();

    glPopMatrix();
}

void dibujar_piso(){
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
        glTranslatef(0,-1,0);
        //Se coloca/actualiza la cámara.
        glRotatef(rotX, 1.0f, 0.0f, 0.0f);
        gluLookAt(cam.mPos.x,  cam.mPos.y,  cam.mPos.z,
                cam.mView.x, cam.mView.y, cam.mView.z,
                cam.mUp.x,   cam.mUp.y,   cam.mUp.z);
        glPushMatrix();
            glPushMatrix();
                glTranslatef(0,-1,0);
                dibujar_piso();
            glPopMatrix();

            glPushMatrix();
                dibujar_vaca();
            glPopMatrix();

            glPushMatrix();
                glPushMatrix();
                    glTranslatef(4,0,0);
                    glScalef(1,3,1);
                    dibujar_sup_revolucion();
                glPopMatrix();

                glPushMatrix();
                    glTranslatef(4,7.5,0);
                    glScalef(1.5,1.0,1.5);
                    dibujar_sup_revolucion_2();
                glPopMatrix();
            glPopMatrix();
        glPopMatrix();
    glPopMatrix();

    glutSwapBuffers();
}

void camara (void) {            
    float ancho = GLUT_WINDOW_WIDTH;
    float alto = GLUT_WINDOW_HEIGHT;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, ancho/alto, 0.1, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void movimiento(){
    if(rot_1<45){
            rot_1= rot_1 +5;
        }else if(rot_1 == 45&&rot_2!=0){
            rot_2= rot_2 -5;
        }else if(rot_2 == 0){
            rot_1 = 0;
            rot_2 = 45;
    }
}

//Función para activar sólo una luz.
void activar_luz(int num_luz) {
    //Primero se desactivan todas las luces.
    glDisable(GL_LIGHT0); 
    glDisable(GL_LIGHT1); 
    glDisable(GL_LIGHT2); 
    //Luego se activa sólo la luz especificada.
    switch(num_luz) {
        case 0: glEnable(GL_LIGHT0); break;
        case 1: glEnable(GL_LIGHT1); break;
        case 2: glEnable(GL_LIGHT2); break;
    }
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
        //ACTIVACIÓN DE LUCES
        case '1': activar_luz(0); break;
        case '2': activar_luz(1); break;
        case '3': activar_luz(2); break;
        //TRASLACIÓN
        case 'W': case 'w': movimiento(); tz += 0.1; break;
        case 'S': case 's': movimiento(); tz -= 0.1; break;
        case 'A': case 'a': movimiento(); tx -= 0.1; break;    
        case 'D': case 'd': movimiento(); tx += 0.1; break;
        //ROTACIÓN
        case 'Q': case 'q': movimiento(); ry += 5;  break;
        case 'E': case 'e': movimiento(); ry -= 5; break;

        //TRASLACIÓN de la camara
        case 'I': case 'i': rotX -= VELOCIDAD_MOV; break;
        case 'K': case 'k': rotX += VELOCIDAD_MOV; break;
        case 'J': case 'j': cam.rotarY(-VELOCIDAD_ROT); break;
        case 'L': case 'l': cam.rotarY(VELOCIDAD_ROT); break;
        case '+': cam.desplazarY(VELOCIDAD_MOV); break;
        case '-': cam.desplazarY(-VELOCIDAD_MOV); break;
        //ROTACIÓN de la camara 
        case '8': rotX -= VELOCIDAD_MOV; break;
        case '5': rotX += VELOCIDAD_MOV; break;
        case '4': cam.rotarY(-VELOCIDAD_ROT); break;
        case '6': cam.rotarY(VELOCIDAD_ROT); break;
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
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Textura");
    glutDisplayFunc(dibujar);
    glutKeyboardFunc(teclado);
    glutReshapeFunc(redimensionar);
}

//CONFIGURACIÓN DE LA LUZ DIRECCIONAL.
void luz_direccional(void) {
    //Valores de las componentes ambiental, difusa, especular y la posición.
    GLfloat amb[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat dif[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat esp[] = {1.0, 1.0, 1.0, 1.0};    
    GLfloat pos[] = {0.0, 1.0, 4.0, 0.0};    
    //Asignación de valores de material y posición a la luz.
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, esp);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
}

//CONFIGURACIÓN DE LA LUZ PUNTUAL.
void luz_puntual(void) {
    //Valores de las componentes ambiental, difusa, especular y la posición.
    GLfloat amb[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat dif[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat esp[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat pos[] = {0.0, 0.0, 0.0, 1.0};    
    //Asignación de valores de material y posición a la luz
    glLightfv(GL_LIGHT1, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, dif);
    glLightfv(GL_LIGHT1, GL_SPECULAR, esp);
    glLightfv(GL_LIGHT1, GL_POSITION, pos);
    //Modificación de las constantes de atenuación.
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.1);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.1);
}

//CONFIGURACIÓN DE LA SPOTLIGHT.
void luz_spot(void) {
    //Valores de las componentes ambiental, difusa, especular y la posición.
    GLfloat amb[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat dif[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat esp[] = {1.0, 1.0, 1.0, 1.0};    
    GLfloat pos[] = {0.0, 6.0, 5.0, 1.0};
    GLfloat dir[] = {0.0, 0.0, -4.0};    
    //Asignación de valores de material y posición a la luz.
    glLightfv(GL_LIGHT2, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, dif);
    glLightfv(GL_LIGHT2, GL_SPECULAR, esp);
    glLightfv(GL_LIGHT2, GL_POSITION, pos);
    //Se modifican los parámetros adicionales para el spotlight.
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, dir);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 40.0);
    glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 3);
}

//INICIALIZACIÓN DE TODAS LAS LUCES.
void luces() {
    //Se activa la iluminación.
    glEnable(GL_LIGHTING);
    //Se inicializan todas las luces.
    luz_puntual();
    luz_direccional();
    luz_spot();
    //Se activa la luz por defecto.
    activar_luz(0);
}

void config_OGL(void) {
    glClearColor(0.2, 0.2, 0.2, 1.0);
    luces();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_TEXTURE_2D);
    puntos_curva = calcular_curva_bezier(P, GRADO_CURVA, NUM_SUBDIVS);
    puntos_curva2 = calcular_curva_bezier(P_2,GRADO_CURVA,NUM_SUBDIVS);
    config_torre_tex();
    config_tex_punta();
    camara();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    config_GLUT();
    config_OGL();
    glutMainLoop();
    return 0;
}