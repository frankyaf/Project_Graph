/** 
 * Carga y dibujo de un modelo OBJ.
 * El modelo OBJ debe contener al menos coordenadas de vértices y normales.
 */
#include <GL/gl.h>  //OpenGL
#include <iostream> //Manejo de flujos estándar.
#include <fstream>  //Manejo de archivos.
#include <sstream>  //Para partición de cadenas.
#include <vector>   //Vectores de C++.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"  //Cargador de imágenes. YA NO SE DEBE VOLVER A INCLUIR EN NINGÚN OTRO ARCHIVO.

//Representación de un vector o punto en 3 dimensiones.
struct Vector3 {
    GLfloat x;
    GLfloat y;
    GLfloat z;
};

//Definición de la clase para cargar y dibujar modelos OBJ.
class ModeloOBJ {
public:
//CONSTRUCTORES. 
    //Recibe el nombre del archivo OBJ.
    ModeloOBJ(std::string nombre_obj) {
        tiene_tex = false;
        cargarOBJ(nombre_obj);
    }

    //Recibe el nombre del archivo OBJ y el nombre de la textura
    ModeloOBJ(std::string nombre_obj, std::string nombre_tex) {
        tiene_tex = true;
        cargarOBJ(nombre_obj);
        config_tex(nombre_tex);
    }

//DESTRUCTOR
    ~ModeloOBJ() {
        stbi_image_free(tex);
    }

//MÉTODOS PÚBLICOS
    //Método para dibujar el modelo.
    void dibujar() {
        if (tiene_tex) dibujar_con_tex();
        else dibujar_sin_tex();
    }

private:
//ATRIBUTOS
    std::vector<Vector3> verts; //Vector con todos los vértices del modelo.
    std::vector<Vector3> norms; //Vector con todas las normales del modelo.
    std::vector<Vector3> uvs;   //Vector con todas las coordenadas de textura del modelo.
    std::vector<int> ivs;       //Vector con todos los índices de los vértices de las caras.
    std::vector<int> ins;       //Vector con todos los índices de las normales de las caras.
    std::vector<int> its;       //Vector con todos los índices de las UVs de las caras.
    bool tiene_tex;             //Indicador de si el modelo tiene textura o no.
    GLuint id_tex;              //ID de la textura
    unsigned char *tex = NULL;              //Imagen de la textura.
    int ancho_tex, alto_tex, num_comps_tex; //Propiedades de la textura.
    GLenum modelo_color = GL_RGB;           //Modelo de color de la textura.
    
//MÉTODOS PRIVADOS
    //Método para leer y cargar a la clase el modelo OBJ.
    void cargarOBJ(std::string nombre_archivo) {
        std::ifstream archivo;          //Archivo OBJ.
        std::string linea;              //Linea leída del archivo.
        std::stringstream cadena_aux;   //Cadena auxiliar para particionar la original por tokens.
        std::string subcadena;          //Cadena obtenida de particionar cadena_aux;
        GLfloat vx, vy, vz;             //Vértice leído en una iteración.
        GLfloat nx, ny, nz;             //Normal leída en una iteración.
        GLfloat u, v;                   //Coords. UV leídas en una iteración.
        int iv, it, in;                 //Índice de la coordenada de vértice, textura y normal.

        //Apertura y validación de apertura del archivo.
        archivo.open(nombre_archivo, std::ios::in); 
        if (!archivo.is_open()) {
            std::cout << "No se pudo abrir el archivo '" << nombre_archivo << "'." << std::endl;
            std::cout << "Probablemente el nombre, ruta y/o extension es incorrecto." << std::endl;
            exit(-1);
        }
        
        while (std::getline(archivo, linea)) {	//Mientras haya líneas por leer, se toma una.
            //Si se está especificando un vértice.
            if (linea[0] == 'v') {
                //Si la línea es una normal, se obtiene su valor y se guarda en el vector de normales.
                if (linea[1] == 'n') {
                    if (std::sscanf(linea.c_str(), "vn %f %f %f", &nx, &ny, &nz) == 3) {
                        norms.push_back({nx, ny, nz});
                    }
                    else {
                        std::cout << "Hay un error en el archivo y no se va a dibujar.\n";
                        return;
                    }
                }
                //Si la línea es una coordenada de textura, se obtiene su valor y se guarda en el vector de normales.
                else if (linea[1] == 't') {
                    if (std::sscanf(linea.c_str(), "vt %f %f", &u, &v) == 2) {
                        uvs.push_back({u, v, 0});
                    }
                    else {
                        std::cout << "Hay un error en las UVs. No se usaran texturas.\n";
                        tiene_tex = false;
                        return;
                    }
                }
                //Si la línea es un vértice, se obtiene su valor y se guarda en el vector de normales.
                else if (linea[1] == ' ') {
                    if (std::sscanf(linea.c_str(), "v %f %f %f", &vx, &vy, &vz) == 3) {
                        verts.push_back({vx, vy, vz});
                    }
                    else {
                        std::cout << "Hay un error en el archivo y no se va a dibujar.\n";
                        return;
                    }
                }
            }
            //Si la línea es una cara, se guardan los índices.
            else if(linea[0] == 'f') {
                cadena_aux = std::stringstream(linea);
                getline(cadena_aux, subcadena, ' ');            //Se ignora la primer subcadena, que es la letra F.
                while(getline(cadena_aux, subcadena, ' ')) {    //Se tokeniza la cadena por espacios, para obtener los índices.
                    if (std::sscanf(subcadena.c_str(), "%i/%i/%i", &iv, &it, &in) == 3) {   //Se leen los tres índices, si los hay.
                        ivs.push_back(iv-1);
                        ins.push_back(in-1);
                        its.push_back(it-1);
                    }
                    else if (std::sscanf(subcadena.c_str(), "%i//%i", &iv, &in) == 2) {      //Se leen sólo índices de vértices y normales.
                        ivs.push_back(iv-1);
                        ins.push_back(in-1);
                    }
                    else {
                        std::cout << "Hay un error en el archivo y no se va a dibujar. Probablemente no tenga normales.\n";
                        return;
                    }
                }
            }
            ivs.push_back(-1);
            ins.push_back(-1);
            its.push_back(-1);
        }
        archivo.close();
    }

    //Método para leer y configurar la textura.
    void config_tex(std::string nombre_archivo) {
        //Se cargan las imágenes ya volteadas.
        stbi_set_flip_vertically_on_load(1);
        //Se cargan las texturas desde las imágenes.
        tex = stbi_load(nombre_archivo.c_str(), &ancho_tex, &alto_tex, &num_comps_tex, 0);
        if (tex == NULL) {
            std::cout << "Hubo un error abriendo el archivo de textura. No se usaran texturas.";
            tiene_tex = false;
            stbi_image_free(tex);
            return;
        }
        
        //Se obtiene el modelo de color de la textura
        if (num_comps_tex == 3) modelo_color = GL_RGB;
        else if (num_comps_tex == 4) modelo_color = GL_RGBA;
        else { 
            std::cout << "La textura tiene un modelo de color diferente a RGB o RGBA. No se usaran texturas.\n";
            tiene_tex = false;
            stbi_image_free(tex);
            return;
        }
        
        //Se genera el id de la textura.
        glGenTextures(1, &id_tex);
    }

    //Método para dibujar el modelo con su textura.
    void dibujar_con_tex(void) {
        //Se activan las texturas.
        glEnable(GL_TEXTURE_2D);
        //Se habilita, configura y aplica la textura.
        glBindTexture(GL_TEXTURE_2D, id_tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, modelo_color, ancho_tex, alto_tex, 
                    0, modelo_color, GL_UNSIGNED_BYTE, tex);
        //Se dibuja el objeto con normales y UVs.
        for (int i = 0; i < ivs.size(); i++) {
            glBegin(GL_POLYGON);
            if (ivs[i] == -1) {
                glEnd();
                continue;
            }
            else {
                glNormal3f(norms[ins[i]].x, norms[ins[i]].y, norms[ins[i]].z);
                glTexCoord2f(uvs[its[i]].x, uvs[its[i]].y);
                glVertex3f(verts[ivs[i]].x, verts[ivs[i]].y, verts[ivs[i]].z);
            }
        }
        //Se deshabilitan las texturas.
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }

    //Método para dibujar el modelo con su textura.
    void dibujar_sin_tex(void) {
        //Se dibuja el objeto sólo con normales.
        for (int i = 0; i < ivs.size(); i++) {
            glBegin(GL_POLYGON);
            if (ivs[i] == -1) {
                glEnd();
                continue;
            }
            else {
                glNormal3f(norms[ins[i]].x, norms[ins[i]].y, norms[ins[i]].z);
                glVertex3f(verts[ivs[i]].x, verts[ivs[i]].y, verts[ivs[i]].z);
            }
        }
    }
};