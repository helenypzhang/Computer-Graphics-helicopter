
/*

    Specification : display a hierarchical object helicopter

*/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "gsrc.h"

// int    option;     // specify whether the animation is uniform, accelerating or decelerating
// double t_prev;     // previous time elapsed
// double exponent = 0.1;   // control the rate of change of acceleration/decleration
float theta = 0.0f;
float phi = 0.0f;
// float plane_rotate = 0.0;

float zoom = 60.0f;
float camX = 0;
float camY = 0;
float camZ = 0;
float upvX = 0;
float upvY = 0;
float upvZ = 0;

float WIN_HEIGHT;
float WIN_WIDTH;

float ratio;

float const PI = 3.14159265f;
float other_co = -180.0f;
static float propeller_rotate = 0, plane_rotate = 0;
float light_co = -180;

float pathX = (float)100 * (sin(other_co * PI / 180));
float pathZ = (float)100 * ((cos(other_co * PI / 180) * sin(other_co * PI / 180)));

float heli_rotatey = PI;
float speed = 0.1f;
float propeller_rotate_speed = 0.9f;
bool is_moving = false;
bool is_original = true;

float light_px;
float light_pz;
bool is_sunlight = true;
bool is_circlelight = true;


GLUquadricObj* partobj1, * partobj2, * partobj3; //quadric objects to store properties of the quadric mesh

//test
GLfloat ground_length = 300.0f;   // the size of the ground

GLfloat ambient[4] = {0.0, 0.0, 0.0, 0.0};
GLfloat diffuse[4] = {1.0, 0.9, 0.7, 0.0};
GLfloat light[4] = {0.0, 1.0, 0.0, 0.0};
//GLfloat fog = 0.0f;

GLfloat light_position_vector[4] = {0, -3, 6.5, 1.0};
GLfloat light_direction_vector[4] = {0, 0, 1, 0.0};

//test
GLuint ground_texture;
//GLfloat shadowcolour[] = { 0.0,0.0,0.0,0.0 };
//GLfloat fogColor[] = { 1.0,1.0,1.0,1.0 };

//////////////////////////////////////////////////
void set_view(int width, int height) {

    // prevent window size of 0
    if (height == 0)
        height = 1;

    WIN_WIDTH = width;
    WIN_HEIGHT = height;

    ratio =  width * 1.0 / height;


}

//////////////////////////////////////////////set the texture
// LOAD TEXTURE STEPS:
//open the file
//load important data from the file(height, width, bpp, etc.)
//skip to the data(always at the end of the header info)
//copy the data to memory
//switch redand blue values
//use data as a texture

//////////////////////////////////////////////
// function of load the texture (convert bmp to texture)
GLuint load_texture(char* file_Name)   // load the texture
{
    errno_t err;

    unsigned int width, height;
    unsigned int imageSize;   // = width*height*3
    unsigned char* data;     // Actual RGB data

    // Open the file
    FILE* file;
    err = fopen_s(&file, file_Name, "rb");
    if (err == 0)
    {
    }
    else
    {

        return false;
    }

    fseek(file, 0x0012, SEEK_SET);
    fread(&width, 4, 1, file);
    fread(&height, 4, 1, file);


    fseek(file, 54, SEEK_SET);
    {
        GLint line_bytes = width * 3;
        while (line_bytes % 4 != 0)
            ++line_bytes;
        imageSize = line_bytes * height;
    }

    data = (GLubyte*)malloc(imageSize);    // Create a buffer

    if (data == 0)
    {
        fclose(file);
        return 0;
    }


    if (fread(data, imageSize, 1, file) <= 0)
    {
        free(data);

        fclose(file);
        return 0;
    }

    fclose(file);


    GLuint textureID;

    glGenTextures(1, &textureID);  //generate one texture with name "textureID"
    glBindTexture(GL_TEXTURE_2D, textureID); //to reuse the texture, copy the texture to "textureID" 

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  //if the texture is too small, then repeat 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // if the texture is to be magnified, linear interpolate 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // if the texture is to be reduced, linear interpolate
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // set texture environment parameters
    // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);

    free(data);
    return textureID;
}

//////////////////////////////////////////
// functions of setting the lights
// set sunlight using light0
void set_sunlight(){

    glEnable(GL_LIGHT0);    
    glShadeModel(GL_SMOOTH); //select smooth shading for the light
    glEnable(GL_NORMALIZE);  // normal vectors are normalized to unit length after transformation and before lighting. 
    glEnable(GL_COLOR_MATERIAL);  // keeps some of orginal material color

    GLfloat specular[] = { 0.9,0.9,0.9,1 };           // white light
    GLfloat ambient[] = { 0.3,0.3,0.3,1 }; //grey ambient color
    GLfloat diffuse[] = { 0.3,0.3,0.3,1 };           // grey color of the diffuse 
    GLfloat shininess = 7.0;                                // control the shininess of the object
    GLfloat position[] = { 0, 20, 0, 0 };           // make the sunlight point dowards, along -y axis
    float specular_reflection[] = { 0.3f, 0.3f, 0.3f, 1.0f };    // bright value of the sunlight

    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);   
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular); 

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);     
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);

    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);    
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);

    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    glLightfv(GL_LIGHT0, GL_POSITION, position);  //set the position of the sunlight

    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_reflection);    // reflect sunlight
}

void set_circlelight(){

    // Turn light 1 on
    glEnable(GL_LIGHT1);

    glEnable(GL_NORMALIZE);  // normal vectors are normalized to unit length after transformation and before lighting. 
    glEnable(GL_COLOR_MATERIAL);  // keeps some of orginal material colors

    GLfloat diffuse[] = {0, 0, 1, 0.0};
    GLfloat specular1[] = { 0, 0, 1, 0.0 };
    GLfloat ambient[] = { 0, 0, 0.5, 0.0 };
    GLfloat light_position_vector[] = { light_px, 15, light_pz, 1.0 };
    GLfloat light_direction_vector[] = { 0, -1, 0, 0.0 };
    float ca = 45.0f;
    float specular_reflection[] = { 0.3f, 0.3f, 0.3f, 1.0f };    // brightness of the circle light

    glLightfv(GL_LIGHT1,GL_DIFFUSE, diffuse); // colors of the circle light
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);

    glLightfv(GL_LIGHT1,GL_SPECULAR, specular1); // color of the specular light
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular1);

    glLightfv(GL_LIGHT1,GL_AMBIENT, ambient); // color of the environment reflected light
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);

    glLightfv(GL_LIGHT1,GL_POSITION, light_position_vector);

    glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION, light_direction_vector); // circle light pointing direction

    glLightf(GL_LIGHT1,GL_SPOT_CUTOFF, ca); // width of the circle light beam

    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT,15);

    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_reflection);    // specular reflection of the circle light

}

void move_light(void) {

    light_co += speed;

    if (light_co >= 180) {
        light_co = -180;
    }

    float Ld = (float)18*( (1 + sin(light_co*PI/180)*sin(light_co*PI/180)))  ;
    float Lc = (float)18*((cos(light_co*PI/180) * (sin(light_co*PI/180))) ) ;

    light_px=Ld - 30;
    light_pz=Lc;

}

////////////////////////////////////////////////////////
// functions of building parts of the helicopter

void cube()
{
    glBegin(GL_QUAD_STRIP);//
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(0.0f, 0.0f, -1.0f);
    glVertex3f(0.0f, 1.0f, -1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();
    glBegin(GL_QUAD_STRIP);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, -1.0f);
    glVertex3f(1.0f, 0.0f, -1.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glEnd();

}

void cylinder(void)
// draw body1 : cylinder of radius 1 height 1  x-y as base
{
    glPushMatrix();

    //glColor3f(0.0, 0.0, 1.0);

    gluCylinder(partobj1, 1, 1, 1, 10, 10);
    // base radius 1  
    // top  radius 1
    // height 1
    // 10 x 10 controls the sampling


    // draw a solid disk to cover the base
    gluDisk(partobj2, 0, 1, 10, 10);

    // draw a solid disk to cover the top
    glPushMatrix();
    glTranslatef(0, 0, 1);

    gluDisk(partobj3, 0, 1, 10, 10);
    glPopMatrix();


    glPopMatrix();
}

void cone(void)
// draw head : cone of radius 1 height 1  x-y as base
{
    glPushMatrix();

    //glColor3f(0.0, 0.0, 1.0);
    //glRotatef(-90.0, 1.0, 0.0, 0.0);   // rotate about x axis by -90 deg.

    // draws a hollow cylinder along the z axis with base on x-y axis, origin at (0, 0, 0) 
    //gluCylinder(partobj1, 30, 30, 40, 10, 10);
    glutSolidCone(1, 1, 10, 10);
    // base radius 1 
    // height 1
    // 10 x 10 controls the sampling


    // draw a solid disk to cover the base
    gluDisk(partobj2, 0, 1, 10, 10);


    glPopMatrix();
}

void sphere()
// draw a standard 2 x 2 x 2 cube whose center is at (0, 1, 0)
{

    /* The use of glPushMatrix and glPopMatrix here protects the glRotatef from
       exercising its influence on the relative transformation beyond this function */
    glPushMatrix();

    glTranslatef(0, 1, 0);
    glutSolidSphere(1.0, 50, 50);
    //glutSolidCube(2);

    glPopMatrix();

}

////////////////////////////////////////////////////////////
// functions of building the scene (helicopter, shadow, ground, path,)
void my_helicopter(void) {

    glRotatef(180, 0.0f, 1.0f, 0.0f);

    glClearColor(0.49f, 0.75f, 0.93f, 1.0f);

    ////////////////////////////////////////////////////////////////////////////
    //static float propeller_rotate = 0, plane_rotate = 0;
    //propeller_rotate += 0.9;  //rotate of the wings
    //if (propeller_rotate > 360)
    //    propeller_rotate = 0;

    //
    //glMultMatrixf(gsrc_getmo());  // get the rotation matrix from the rotation user-interface
/*  Enable Z buffer method for visibility determination. */
    glClear(GL_DEPTH_BUFFER_BIT); //  Z buffer code starts
    glEnable(GL_DEPTH_TEST); // Z buffer code ends
    //

    glPushMatrix(); //structure of the whole plane

    // glRotatef(phi_acc, 0.0, 0.0, 1.0);      // rotate the whole plane by phi degrees
    glRotatef(phi, 0.0, 0.0, 1.0);      // rotate the whole plane by phi degrees
    if (is_original == true ) // adjust the origianl direction of the plane
    {
        glRotatef(-45, 0.0, 1.0, 0.0);
    }
    glScalef(11, 11, 11); //10 times of the original size

    glPushMatrix();
    glColor3f(0, 0.3, 1);
    glRotatef(propeller_rotate, 0, 1, 0);
    glTranslatef(0, 0, 0.7);
    glScalef(0.1, 0.02, 1.4);
    cube(); //the first propeller
    glPopMatrix();

    //the whole hierarchical structure as follows: propellers->base->body->head->tail planes
    glPushMatrix();
    glColor3f(0, 0.3, 1);
    glRotatef(propeller_rotate + 90.0, 0, 1, 0);
    glTranslatef(0, 0, 0.7);
    glScalef(0.1, 0.02, 1.4);
    cube(); //the second propeller
    glPopMatrix();

    //hierarchical structure as follows: base->body->head->tail
    glColor3f(0, 0.3, 1);
    glTranslatef(0, -0.1, 0); //
    glScalef(0.1, 0.1, 0.1); //smaller by 0.1
    cube(); //base of the propeller

    glTranslatef(0, -2, -2); //adjust the distance between the propeller+base and the other parts of the plane
    glScalef(10, 10, 10);  //become to the original size 0.1*10=1
    glColor3f(0, 0.3, 1);
    glTranslatef(0.04, -0.05, -0.9);
    glScalef(0.1, 0.1, 1.5);
    cylinder(); //body1 of the plane

    glPushMatrix();
    glColor3f(1, 1, 0);
    glTranslatef(0.0, -3, 0.7); //move the body2 of the plane into the right position
    glScalef(3, 3, 0.35);
    sphere(); //body2 of the plane
    glPopMatrix();

    glColor3f(0, 0.3, 1);
    glTranslatef(0, 0.0, 1); //move the head of the plane into the right position
    glScalef(1.5, 1.5, 0.2);
    cone(); //head of the plane

    glColor3f(1, 1, 0);
    glTranslatef(0, 0.7, -4.5);
    glScalef(0.18, 1.8, 0.6);
    cube(); //vertical plate

    glTranslatef(-13, 0.3, 0);
    glScalef(27, 0.1, 1);
    cube(); //horizontal plate

    glPopMatrix();
    ////////////////////////////////////////////////////////////////////////////

}

void my_helicopter_shadow(void) {

    glRotatef(180, 0.0f, 1.0f, 0.0f);

    glClearColor(0.0f, 0.0f, 0.f, 1.0f);

    ////////////////////////////////////////////////////////////////////////////
    //static float propeller_rotate = 0, plane_rotate = 0;
    //propeller_rotate += 0.9;  //rotate of the wings
    //if (propeller_rotate > 360)
    //    propeller_rotate = 0;

    //
    //glMultMatrixf(gsrc_getmo());  // get the rotation matrix from the rotation user-interface
/*  Enable Z buffer method for visibility determination. */
    glClear(GL_DEPTH_BUFFER_BIT); //  Z buffer code starts
    glEnable(GL_DEPTH_TEST); // Z buffer code ends
    //

    glPushMatrix(); //structure of the whole plane


    glRotatef(phi, 0.0, 0.0, 1.0); // rotate the whole plane by phi degrees
    if (is_original == true) // adjust the origianl direction of the plane
    {
        glRotatef(-45, 0.0, 1.0, 0.0);
    } 
    glScalef(11, 11, 11); //10 times of the original size

    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);
    glRotatef(propeller_rotate, 0, 1, 0);
    glTranslatef(0, 0, 0.7);
    glScalef(0.1, 0.02, 1.4);
    cube(); //the first propeller
    glPopMatrix();

    //the whole hierarchical structure as follows: propellers->base->body->head->tail planes
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);
    glRotatef(propeller_rotate + 90.0, 0, 1, 0);
    glTranslatef(0, 0, 0.7);
    glScalef(0.1, 0.02, 1.4);
    cube(); //the second propeller
    glPopMatrix();

    //hierarchical structure as follows: base->body->head->tail
    glColor3f(0.0, 0.0, 0.0);
    glTranslatef(0, -0.1, 0); //
    glScalef(0.1, 0.1, 0.1); //smaller by 0.1
    cube(); //base of the propeller

    glTranslatef(0, -2, -2); //adjust the distance between the propeller+base and the other parts of the plane
    glScalef(10, 10, 10);  //become to the original size 0.1*10=1
    glColor3f(0.0, 0.0, 0.0);
    glTranslatef(0.04, -0.05, -0.9);
    glScalef(0.1, 0.1, 1.5);
    cylinder(); //body1 of the plane

    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);
    glTranslatef(0.0, -3, 0.7); //move the body2 of the plane into the right position
    glScalef(3, 3, 0.35);
    sphere(); //body2 of the plane
    glPopMatrix();

    glColor3f(0.0, 0.0, 0.0);
    glTranslatef(0, 0.0, 1); //move the head of the plane into the right position
    glScalef(1.5, 1.5, 0.2);
    cone(); //head of the plane

    glColor3f(0.0, 0.0, 0.0);
    glTranslatef(0, 0.7, -4.5);
    glScalef(0.18, 1.8, 0.6);
    cube(); //vertical plate

    glTranslatef(-13, 0.3, 0);
    glScalef(27, 0.1, 1);
    cube(); //horizontal plate

    glPopMatrix();
    ////////////////////////////////////////////////////////////////////////////

}

//test
void draw_ground(void)   //draw the ground with texture
{
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ground_texture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(ground_length / 2, 0.0, ground_length / 2);
    glTexCoord2f(0.0, 1.0); glVertex3f(ground_length / 2, 0.0, -ground_length / 2);
    glTexCoord2f(1.0, 1.0); glVertex3f(-ground_length / 2, 0.0, -ground_length / 2);
    glTexCoord2f(1.0, 0.0); glVertex3f(-ground_length / 2, 0.0, ground_length / 2);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}


// equations of the "8" path.
float path_eight(float a, float t) {
    return a * sin(t);
}


void my_path(float a, int numofSegs) {
    const float width = 10.;
    float t = 0.0;
    float delta_t = 2.0 * PI / numofSegs; //change value

    float prevX = path_eight(a, t);
    float prevZ = prevX * cos(t);
    
    float pathX, pathZ;

    glBegin(GL_TRIANGLE_STRIP);
    glNormal3f(0, 1, 0);
    glColor3f(0.2, 0.2, 0.2);
    // draw the path in loop
    for (int i = 0; i <= numofSegs; ++i)
    {

        // increase the path 
        t += delta_t;
        pathX = path_eight(a, t);
        pathZ = pathX * cos(t);
        float distanceX = pathX - prevX;
        float distanceZ = pathZ - prevZ;
        prevX = pathX;
        prevZ = pathZ;

        // normalize distanceX, distanceZ
        float normFactor = 1. / sqrt(distanceX*distanceX + distanceZ*distanceZ);
        distanceX *= normFactor;
        distanceZ *= normFactor;

        // rotate distanceX, distanceZ by 90 deg, add the wide of the path
        float tempX = distanceX;
        distanceX = -distanceZ * width;
        distanceZ = tempX * width;

        // draw vertices of the path
        glVertex3f(pathX-distanceX, 0.0, pathZ-distanceZ);
        glVertex3f(pathX+distanceX, 0.0, pathZ+distanceZ);
    }
    glEnd();
}

/////////////////////////////////////////////////////////////
// functions of moving helicopter
void move_helicopter(void) {
    if (is_moving) {
        //propeller_rotate += 0.9;  //rotate of the wings
        propeller_rotate += propeller_rotate_speed;  //rotate of the wings
        if (propeller_rotate > 360)
            propeller_rotate = 0;
        other_co += speed;
    }
    if (other_co >= 180) {
        other_co = -180;
    }

    float X_prev = pathX;
    float Z_prev = pathZ;

    pathX = (float)100 * (sin(other_co * PI / 180));
    pathZ = (float)100 * ((cos(other_co * PI / 180) * sin(other_co * PI / 180)));
    float dirX = pathX - X_prev;
    float dirZ = pathZ - Z_prev;

    float magnitude = sqrt(pathX*pathX + pathZ*pathZ);

    dirX = dirX / magnitude;
    dirZ = dirZ / magnitude;

    float dot = dirZ*-1.0f;
    float det = dirX*-1.0f;

    if (is_moving) {
        heli_rotatey = atan2f(det, dot);
    }

    glTranslatef(pathX,0.0f,pathZ);
}

/////////////////////////////////////////////////////////////
// functions of moving camera
void move_camera(void) {

    // camera angle range from zero to 360
    if (theta > 360)theta = fmod((double)theta, 360.0);
    if (phi > 360)phi = fmod((double)phi, 360.0);

    // convert degrees to radians using conversion factor = 0.0174532
    camX = zoom * sin(theta * 0.0174532) * sin(phi * 0.0174532);
    camY = zoom * cos(theta * 0.0174532);
    camZ = zoom * sin(theta * 0.0174532) * cos(phi * 0.0174532);

    // get another vertex on the sme sphere's longitude by reducing t a little bit
    GLfloat delta_t = 1.0;
    GLfloat camXtemp = zoom * sin(theta * 0.0174532 - delta_t) * sin(phi * 0.0174532);
    GLfloat camYtemp = zoom * cos(theta * 0.0174532 - delta_t);
    GLfloat camZtemp = zoom * sin(theta * 0.0174532 - delta_t) * cos(phi * 0.0174532);

    // get upright vector of the camera.
    upvX = camXtemp - camX;
    upvY = camYtemp - camY;
    upvZ = camZtemp - camZ;


}

////////////////////////////////////////////////////////////
// function of rotating 
//animation
//void rotating(void)
//// this animation function will swing theta from 0 deg. to 360 deg. in 1 seconds, then stop at 360 deg.
//{
//    double	t;
//    double swing_angle = 360.0;                  // 360 degrees
//    double swing_time = 1000.0;				 // 1000 ms
//
//    t = glutGet(GLUT_ELAPSED_TIME) - t_prev;            // return elapsed time in ms since the last call  
//    //phi = phi - phi_prev;
//
//    if (t < swing_time)
//    {
//        if (option == 0) //key==1
//            phi_acc = swing_angle * t / swing_time;
//        else if (option == 1) //key==2
//            phi_acc = swing_angle * (1 - pow(cos(PI * t / (2 * swing_time)), exponent));
//        else  //option== 2  key==3
//            phi_acc = swing_angle * pow(sin(PI * t / (2 * swing_time)), exponent);
//    }
//    else
//        phi_acc = swing_angle;                     // stop at swing_angle
//
//    glutPostRedisplay();
//}
//


////////////////////////////////////////////////////////////
// function of render the scene
void render_scene(void) {

    glMatrixMode(GL_PROJECTION); // projection matrix

    glLoadIdentity(); //reset

    glScissor(0, 0, (GLsizei) WIN_WIDTH, (GLsizei) WIN_HEIGHT); //using glScissor to only render the cutting window
    glEnable(GL_SCISSOR_TEST);
    glClearDepth(1.0);

    glClearColor(0.40f, 0.99f, 0.70f, 1.0f); // specify clear values for the color buffers

    glViewport (0, 0, (GLsizei) WIN_WIDTH, (GLsizei) WIN_HEIGHT); // using viewport as the whole window

    gluPerspective(45.0f, ratio, 1.0f, 1000.0f); // set perspective values

    glMatrixMode(GL_MODELVIEW); // transfer to the model view

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //shaded the polygon mash/plane

    glLoadIdentity();

    glMultMatrixf(gsrc_getmo());  // get the rotation matrix from the rotation user-interface //mouse move


    move_camera(); //move the camera

    gluLookAt(camX + pathX, camY, camZ + pathZ, pathX, 0.0f, pathZ, upvX, upvY, upvZ); // set the camera value

    glEnable(GL_DEPTH_TEST); //enable the deepth test

    glShadeModel(GL_SMOOTH); // select smooth shading for the light
    glEnable(GL_NORMALIZE);  // normal vectors are normalized to unit length after transformation and before lighting. 

    GLfloat material_specular[] = {0.2, 0.2, 0.2, 1.0};
    GLfloat material_shininess[] = { 10.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_LIGHTING);

    if (is_sunlight) { set_sunlight(); }

    else { glDisable(GL_LIGHT0); }

    //test
    char filename[] = "grass.bmp"; //using small size bmp to prevent overloading
    char* p = filename;
    ground_texture = load_texture(p);

    glPushMatrix();
    glTranslatef(0.0, -10.0, 0.0); 
    draw_ground();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, -9.8, 0.0f);
    my_path(100.0, 100.0);
    glPopMatrix();

    /////////////////////////////////////////////////////
    GLfloat light[3] = { 0.0, 80.0, 0.0 };

    GLfloat m[16];			//projection matrix
    int i, j;
    //set the projection matrix
    for (i = 0; i < 16; i++)
        m[i] = 0.0;
    m[0] = m[5] = m[10] = 1.0;
    m[7] = -1.0 / light[1];
    m[13] = -9.8;


    //draw the helicopter's shadow 
    glPushMatrix(); // save state
    //
    glTranslatef(light[0], light[1], light[2]);
    glMultMatrixf(m);		//multiplied by projection matrix //column-major
    glTranslatef(-light[0], -light[1], -light[2]);
    //
    move_helicopter();
    glRotatef(heli_rotatey / PI * 180, 0.0f, 1.0f, 0.0f);
    //glColor3fv(shadowcolour);
    glColor3f(0.0, 0.0, 0.0);
    my_helicopter_shadow();				//draw the object again
    glColor3f(0.0, 0.0, 0.0);
    glPopMatrix();

    //finished

    //draw the helicopter

    glPushMatrix();
    move_helicopter();
    glRotatef(heli_rotatey / PI * 180, 0.0f, 1.0f, 0.0f);
    my_helicopter();				//draw normal object
    if (is_circlelight) {
        move_light();
        set_circlelight();
    }
    else {
        glDisable(GL_LIGHT1);
    }

    glPopMatrix();

    // finished 

    //////////////////////////////////////////////////////////

    glColor3f(0.0f,0.0f,0.0f);

    glPushMatrix();
    glLoadIdentity();

    // use Orthographic Projection 
    glMatrixMode(GL_PROJECTION);// switch to projection mode
    // save previous matrix which contains the settings for the perspective projection
    glPushMatrix();
    glLoadIdentity(); //reset
    gluOrtho2D(0, WIN_WIDTH, WIN_HEIGHT, 0); // 2 dimension orthographic projection
    glMatrixMode(GL_MODELVIEW); //transfer to the modelview
    //
    glTranslatef(100.0f, 150, 0.0f);
    glPopMatrix();

    // set projection and restore previous projection matrix 
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW); // get back to modelview mode

    //
    glutSwapBuffers();

}

/////////////////////////////////////////////////////////////
//function of resetting default values
void reset_defaults(void) {

    theta = 0.0f;
    phi = 0.0f;
    // phi_acc = 0.0f;
    zoom = 60.0f;
    camX = 0;
    camY = 0;
    camZ = 0;
    upvX = 0;
    upvY = 0;
    upvZ = 0;

}

//////////////////////////////////////////////////////////////
// functions of keyboard and specialkeyboard
// scan_keyboard method
void scan_keyboard(GLubyte key, GLint xMouse, GLint yMouse) //mouse move
//void scan_keyboard(unsigned char key, int x, int y)
{
    switch(key) {

        case 'Z':
            if (zoom > 1.0f )
                zoom -= 4.0f;
            break;
        case 'z':
            zoom += 4.0f;
            break;
        case 'r':
            reset_defaults();
            break;
        case 'q':
            exit(0);
            break;
        case 's':
            is_moving = true;
            is_original = false;
            //printf("the helicopter starts moving. ");
            break;
        case 'e':
            is_moving = false;
            is_original = false;
            break;
        case 'a':
            if (is_moving && speed < 1.0f) {
                speed += 0.1f;
                propeller_rotate_speed += 0.3;  //acc rotate of the wings
            }
            break;
        case 'd':
            if (is_moving && speed > 0.1f) {
                speed -= 0.1f;
                if (speed < 0.1f) {
                    speed = 0.1f;
                }
                propeller_rotate_speed -= 0.3f;
                if (propeller_rotate_speed < 0.3f) {
                    propeller_rotate_speed = 0.3f;
                }
            }
            break;
        /*case '1':		
            option = 0; t_prev = glutGet(GLUT_ELAPSED_TIME);
            break;
        case '2':		
            option = 1; t_prev = glutGet(GLUT_ELAPSED_TIME);
            break;
        case '3':		
            option = 2; t_prev = glutGet(GLUT_ELAPSED_TIME);
            break;*/

        default:
            break;
    }

    move_camera();

    glutPostRedisplay(); /* this redraws the scene without
                          waiting for the display callback so that any changes appear
                          instantly */
}

// special scan_keyboard method
void special_keyboard(int key, int x, int y) {
    switch(key) {
        case GLUT_KEY_F1:
            if (is_sunlight)
                is_sunlight = false;
            else
                is_sunlight = true;
            break;
        case GLUT_KEY_F2:
            if (is_circlelight)
                is_circlelight = false;
            else
                is_circlelight = true;
            break;
        case GLUT_KEY_UP:
            theta -= 5.0f;
            break;
        case GLUT_KEY_DOWN:
            theta += 5.0f;
            break;
        case GLUT_KEY_RIGHT:
            phi += 5.0f;
            break;
        case GLUT_KEY_LEFT:
            phi -= 5.0f;
            break;
        default:
            break;
    }

    move_camera();

    glutPostRedisplay();

}

//////////////////////////////////////////////////////////////
// main function
int main(int argc, char **argv) {

    // init GLUT and create window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(600,160);
    glutInitWindowSize(350,320);
    WIN_WIDTH=350;
    WIN_HEIGHT=320;
    glutCreateWindow("Crazy Helicopter_Mini Project_ZHANG Yupei");

    ////////////////////////
    // t_prev = glutGet(GLUT_ELAPSED_TIME);
    partobj1 = gluNewQuadric();
    partobj2 = gluNewQuadric();
    partobj3 = gluNewQuadric();
    ////////////////////////
    //printf("Step1.Rate of accleration/decleration [range from 0 to 1, the smaller, the faster] = ");
    //scanf_s("%lf", &exponent);

    //printf("\nStep2.This animation will swing psi from 0 to 45 degrees in 1 seconds \n");

    printf("You can click the keyboard to control the speed: \n");
    printf("s : The helicopter will start moving at a Uniform speed        \n");
    printf("a : The helicopter will move at a Higher speed        \n");
    printf("d : The helicopter will move at a Lower speed          \n");
    printf("e : The helicopter will stop                            \n\n");
    //printf("1 : The helicopter will swing at a Uniform speed        \n");
    //printf("2 : The helicopter will swing at a Accelerating speed   \n");
    //printf("3 : The helicopter will swing at a Decelerating	speed   \n");

    printf("You can click the keyboard to rack the focus: \n");
    printf("Z : The camera will zoom in on the plane  \n");
    printf("z : The camera will zoom out  \n\n");

    printf("You can click the keyboard to turn off / turn on the lights: \n");
    printf("F1 : Control the status of the sunlight  \n");
    printf("F2 : Control the status of the blue circle light \n\n");

    printf("You can click the keyboard to control the slope of the plane(recommanded): \n");
    printf("UP   : Uptilt the plane  \n");
    printf("DOWN : Tilt down the plane \n");
    printf("LEFT : Rotate the ground and the helicopter CCW   \n");
    printf("RIGHT: Rotate the ground and the helicopter CW  \n\n");

    printf("You can click the key 'r' to reset the settings: \n\n");
    printf("By the way, you can also use mouse to change the position of the whole scene:\n(can not resetting, not recommanded) \n\n"); //mouse move
    
    printf("You can click the key 'q' to exit: \n");

    glutDisplayFunc(render_scene);
    glutReshapeFunc(set_view);
    glutKeyboardFunc(scan_keyboard);
    glutSpecialFunc(special_keyboard);
    glutIdleFunc(render_scene);
    // glutIdleFunc(rotating); //called the animate function when program is free

    // Register mouse-click and mouse-move glut callback functions //mouse move
        // for the rotation user-interface.
        // 
    glutMouseFunc(gsrc_mousebutton);
    glutMotionFunc(gsrc_mousemove);
    //

    move_camera();
    glutMainLoop();

    return 1;
}
