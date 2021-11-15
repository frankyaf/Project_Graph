/*  Clase que representa una cámara sencilla.
    Calcula los valores que hay que pasarle a la función gluLookAt.
*/

#include <math.h>

// Estructura que representa un vector. 
// Incluye sobrecarga de operadores para facilitar operaciones.
typedef struct tVector3 {			
    // Constructor vacío.
    tVector3() {}	
    // Constructor con parámetros.
    tVector3 (float new_x, float new_y, float new_z)
    {x = new_x; y = new_y; z = new_z;}
    tVector3 operator+(tVector3 vVector) {return tVector3(vVector.x+x, vVector.y+y, vVector.z+z);}
    tVector3 operator-(tVector3 vVector) {return tVector3(x-vVector.x, y-vVector.y, z-vVector.z);}
    tVector3 operator*(float number)	 {return tVector3(x*number, y*number, z*number);}
    tVector3 operator/(float number)	 {return tVector3(x/number, y/number, z/number);}
    
    float x, y, z;  // Coordenadas 3D.
} tVector3;


class Camara {
public:
    //Vectores de posición para pasarle a la cámara de GLU.
    tVector3 mPos;
    tVector3 mView;
    tVector3 mUp;

    //Constructor de la cámara. Inicializa las variable a pasarle a GLU.
    Camara (float pos_x,  float pos_y,  float pos_z,
            float view_x, float view_y, float view_z,
            float up_x,   float up_y,   float up_z) {
                mPos  = tVector3(pos_x,  pos_y,  pos_z ); 
                mView = tVector3(view_x, view_y, view_z); 
                mUp   = tVector3(up_x,   up_y,   up_z  );
    }

    //Método para mover la cámara sobre su eje Z local (adelante y atrás).
    void desplazarZ(float speed) {
        tVector3 vVector = mView - mPos;	
        mPos.x  = mPos.x  + vVector.x * speed;
        mPos.z  = mPos.z  + vVector.z * speed;
        mView.x = mView.x + vVector.x * speed;
        mView.z = mView.z + vVector.z * speed;
    }

    //Método para mover la cámara sobre su eje X local (izquierda y derecha).
    void desplazarX(float speed) {
        tVector3 vVector = mView - mPos;
        tVector3 vOrthoVector;
        vOrthoVector.x = -vVector.z;
        vOrthoVector.z =  vVector.x;
        mPos.x  = mPos.x  + vOrthoVector.x * speed;
        mPos.z  = mPos.z  + vOrthoVector.z * speed;
        mView.x = mView.x + vOrthoVector.x * speed;
        mView.z = mView.z + vOrthoVector.z * speed;
    }

    //Método para mover la cámara sobre su eje Y local (arriba y abajo).
    void desplazarY(float speed) {
        tVector3 vVector = mView - mPos;
        tVector3 vOrthoVector;
        vOrthoVector.y = 1;
        mPos.y  = mPos.y  + vOrthoVector.y * speed;
        mView.y = mView.y + vOrthoVector.y * speed;
    }

    //Método para rotar la cámara sobre su eje Y local (voltear a los lados).
    void rotarY(float speed) {
        tVector3 vVector = mView - mPos;
        mView.z = (float)(mPos.z + sin(speed)*vVector.x + cos(speed)*vVector.z);
        mView.x = (float)(mPos.x + cos(speed)*vVector.x - sin(speed)*vVector.z);
    }
};
