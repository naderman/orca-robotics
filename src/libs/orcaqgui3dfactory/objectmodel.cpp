#include "objectmodel.h"
#include <iostream>
#include <sstream>
#include <GL/glut.h>
#include <cmath>

using namespace std;

namespace orcaqgui3d {

#if !HAVE_LIB3DS
ObjectModel::ObjectModel( const char *filename )
{
    throw std::string("ERROR: Can't instantiate ObjectModel without lib3ds.");
}    
ObjectModel::~ObjectModel() {}
void ObjectModel::render() {}
#else

ObjectModel::ObjectModel( const char *filename )
{
    cout<<"TRACE(objectmodel.cpp): Loading file: " << filename << endl;

    file_=lib3ds_file_load(filename);
    if (!file_) 
    {
        puts("***ERROR*** Loading 3DS file failed.");
        stringstream ss;
        ss << "ERROR: ObjectModel: Failed to load .3ds file '"<<filename<<"'";
        throw ss.str();
    }
    
    lib3ds_file_eval(file_,0);
}

ObjectModel::~ObjectModel()
{
    lib3ds_file_free(file_);
}

void
ObjectModel::render()
{
    if (!file_) {
        return;
    }

    {
        Lib3dsNode *p;
        for (p=file_->nodes; p!=0; p=p->next) {
            renderNode(p);
        }
    }
}

void
ObjectModel::renderNode(Lib3dsNode *node)
{
    ASSERT(file_);

    {
        Lib3dsNode *p;
        for (p=node->childs; p!=0; p=p->next) {
            renderNode(p);
        }
    }
    if (node->type==LIB3DS_OBJECT_NODE) {
        if (strcmp(node->name,"$$$DUMMY")==0) {
            return;
        }

        if (!node->user.d) {
            Lib3dsMesh *mesh=lib3ds_file_mesh_by_name(file_, node->name);
            ASSERT(mesh);
            if (!mesh) {
                return;
            }

            node->user.d=glGenLists(1);
            glNewList(node->user.d, GL_COMPILE);

            {
                unsigned p;
                Lib3dsVector *normalL = (Lib3dsVector*) malloc(3 * sizeof(Lib3dsVector) * mesh->faces);

                {
                    Lib3dsMatrix M;
                    lib3ds_matrix_copy(M, mesh->matrix);
                    lib3ds_matrix_inv(M);
                    glMultMatrixf(&M[0][0]);
                }
                lib3ds_mesh_calculate_normals(mesh, normalL);

                for (p=0; p<mesh->faces; ++p) {
                    Lib3dsFace *f=&mesh->faceL[p];
                    Lib3dsMaterial *mat=0;
                    if (f->material[0]) {
                        mat=lib3ds_file_material_by_name(file_, f->material);
                    }

                    if (mat) {
                        static GLfloat a[4]={0,0,0,1};
                        float s;
                        glMaterialfv(GL_FRONT, GL_AMBIENT, a);
                        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat->diffuse);
                        glMaterialfv(GL_FRONT, GL_SPECULAR, mat->specular);
                        s = pow(2, 10.0*mat->shininess);
                        if (s>128.0) {
                            s=128.0;
                        }
                        glMaterialf(GL_FRONT, GL_SHININESS, s);
                    }
                    else {
                        Lib3dsRgba a={0.2, 0.2, 0.2, 1.0};
                        Lib3dsRgba d={0.8, 0.8, 0.8, 1.0};
                        Lib3dsRgba s={0.0, 0.0, 0.0, 1.0};
                        glMaterialfv(GL_FRONT, GL_AMBIENT, a);
                        glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
                        glMaterialfv(GL_FRONT, GL_SPECULAR, s);
                    }
                    {
                        int i;
                        glBegin(GL_TRIANGLES);
                        glNormal3fv(f->normal);
                        for (i=0; i<3; ++i) {
                            glNormal3fv(normalL[3*p+i]);
                            glVertex3fv(mesh->pointL[f->points[i]].pos);
                        }
                        glEnd();
                    }
                }

                free(normalL);
            }

            glEndList();
        }

        if (node->user.d) {
            Lib3dsObjectData *d;

            glPushMatrix();
            d=&node->data.object;
            glMultMatrixf(&node->matrix[0][0]);
            glTranslatef(-d->pivot[0], -d->pivot[1], -d->pivot[2]);
            glCallList(node->user.d);
            /*glutSolidSphere(50.0, 20,20);*/
            glPopMatrix();
        }
    }
}


#endif // HAVE_LIB3DS
}
