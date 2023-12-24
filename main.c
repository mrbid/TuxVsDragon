/*
    James William Fletcher  ( notabug.org/Vandarin )
        December 2023       ( github.com/mrbid     )

    You say it best when you say nothing at all.
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef WEB
    #include <emscripten.h>
    #include <emscripten/html5.h>
    #define GL_GLEXT_PROTOTYPES
    #define EGL_EGLEXT_PROTOTYPES
#endif

#define uint GLuint
#define sint GLint

#include "inc/gl.h"
#define GLFW_INCLUDE_NONE
#include "inc/glfw3.h"
#define fTime() (float)glfwGetTime()

#define MAX_MODELS 45 // hard limit, be aware and increase if needed
#include "inc/esAux5.h"

#include "inc/res.h"
#include "assets/arena.h"           //0
#include "assets/a1.h"              //1
#include "assets/a2.h"              //2
#include "assets/a3.h"              //3
#include "assets/a4.h"              //4
#include "assets/a5.h"              //5
#include "assets/a6.h"              //6
#include "assets/a7.h"              //7
#include "assets/a8.h"              //8
#include "assets/a9.h"              //9
#include "assets/a10.h"             //10
#include "assets/a11.h"             //11
#include "assets/a12.h"             //12
#include "assets/a13.h"             //13
#include "assets/a14.h"             //14
#include "assets/a15.h"             //15
#include "assets/a16.h"             //16
#include "assets/a17.h"             //17
#include "assets/a18.h"             //18
#include "assets/a19.h"             //19
#include "assets/a20.h"             //20
#include "assets/a21.h"             //21
#include "assets/a22.h"             //22
#include "assets/a23.h"             //23
#include "assets/a24.h"             //24
#include "assets/tux_idle.h"        //25
#include "assets/tux_throw.h"       //26
#include "assets/ball.h"            //27
#include "assets/b1.h"              //28
#include "assets/b2.h"              //29
#include "assets/b3.h"              //30
#include "assets/b4.h"              //31
#include "assets/b5.h"              //32
#include "assets/b6.h"              //33
#include "assets/b7.h"              //34
#include "assets/b8.h"              //35
#include "assets/b9.h"              //36
#include "assets/b10.h"             //37
#include "assets/b11.h"             //38
#include "assets/b12.h"             //39
#include "assets/b13.h"             //40
#include "assets/b14.h"             //41
#include "assets/arrow.h"           //42
#include "assets/dragon_hit.h"      //43
#include "assets/dragon_dead.h"     //44

//*************************************
// globals
//*************************************
const char appTitle[]="TuxVsDragon";
GLFWwindow* window;
uint winw=1024, winh=768;
float t=0.f, dt=0.f, lt=0.f, fc=0.f, lfct=0.f, aspect;
double mx,my,lx,ly,ww,wh;

// render state
mat projection, view, model, modelview;

// game vars
#define FAR_DISTANCE 333.f
uint keystate[4] = {0};

// player vars
#define MOVE_SPEED 8.f
#define TURN_SPEED 3.f
float px, py, pdx, pdy, pr;
float fire = 0.f;
uint hamo = 0;
uint moving = 0;
uint cvt = 0;

// dragon
float adx, ady, ar;
uint dhp;
float dhit = 0.f;

// ball
uint bspawn = 0;
float bx, by, bdx, bdy;

// arrows
float cx[3];
float cy[3];
float cdx[3];
float cdy[3];

//*************************************
// utility functions
//*************************************
void timestamp(char* ts)
{
    const time_t tt = time(0);
    strftime(ts, 16, "%H:%M:%S", localtime(&tt));
}
void updateModelView()
{
    mMul(&modelview, &model, &view);
    glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (float*)&modelview.m[0][0]);
}
void updateTitle()
{
    if(dhp == 0)
    {
        glfwSetWindowTitle(window, "🌟 YOU KILLED THE DRAGON 🌟");
    }
    else
    {
        char tmp[256];
        if(hamo == 0){sprintf(tmp, "🐉 %u", dhp);}
        else{sprintf(tmp, "🐉 %u ❄️", dhp);}
        glfwSetWindowTitle(window, tmp);
    }
}

//*************************************
// update & render
//*************************************
void main_loop()
{
//*************************************
// core logic
//*************************************
    fc++;
    glfwPollEvents();
    t = fTime();
    dt = t-lt;
    lt = t;

//*************************************
// game logic
//*************************************

    // collision bounds
    if(px > 82.f){px -= px-82.f;}
    else if(px < -82.f){px += -82.f-px;}
    if(py > 82.f){py -= py-82.f;}
    else if(py < -82.f){py += -82.f-py;}
    const float psqd = px*px + py*py;
    if(psqd < 400.f)
    {
        const float d = 20.f-sqrtf(psqd);
        const float len = 1.f/sqrtf(px*px + py*py);
        const float dtpx = px*len;
        const float dtpy = py*len;
        px += dtpx*d;
        py += dtpy*d;
    }

    // forward & backward
    if(keystate[2] == 1)
    {
        px -= pdx * (MOVE_SPEED*dt);
        py -= pdy * (MOVE_SPEED*dt);
        if(moving == 0){moving = 1;}

        // turn left & right
        if(keystate[0] == 1)
        {
            pr += TURN_SPEED*dt;
            const float cos1 = cosf(pr);
            const float sin1 = sinf(pr);
            pdx = cos1 - sin1;
            pdy = sin1 + cos1;
        }
        else if(keystate[1] == 1)
        {
            pr -= TURN_SPEED*dt;
            const float cos1 = cosf(pr);
            const float sin1 = sinf(pr);
            pdx = cos1 - sin1;
            pdy = sin1 + cos1;
        }
    }
    else if(keystate[3] == 1)
    {
        px += pdx * (MOVE_SPEED*dt);
        py += pdy * (MOVE_SPEED*dt);
        if(moving == 0){moving = 2;}

        // turn left & right
        if(keystate[0] == 1)
        {
            pr -= TURN_SPEED*dt;
            const float cos1 = cosf(pr);
            const float sin1 = sinf(pr);
            pdx = cos1 - sin1;
            pdy = sin1 + cos1;
        }
        else if(keystate[1] == 1)
        {
            pr += TURN_SPEED*dt;
            const float cos1 = cosf(pr);
            const float sin1 = sinf(pr);
            pdx = cos1 - sin1;
            pdy = sin1 + cos1;
        }
    }
    else if(moving == 1){moving = 0;}
    else {if(moving != 0){moving = 0;}}
    // else
    // {
    //     // turn left & right
    //     if(keystate[0] == 1)
    //     {
    //         pr += TURN_SPEED*dt;
    //         pdx = cosf(pr) - sinf(pr);
    //         pdy = sinf(pr) + cosf(pr);
    //     }
    //     else if(keystate[1] == 1)
    //     {
    //         pr -= TURN_SPEED*dt;
    //         pdx = cosf(pr) - sinf(pr);
    //         pdy = sinf(pr) + cosf(pr);
    //     }
    // }

    // camera
    mIdent(&view);
    if(cvt == 1)
    {
        mSetPos(&view, (vec){0.f, -5.f, -180.f});
        mRotY(&view, 0.8f);
    }
    else
    {
        // mIdent(&view);
        // mSetPos(&view, (vec){-px, -py, -100.f});
        // float rot = 0.8f;
        // if(py < -42.f){rot = 0.8f-(0.02f*(-py-42.f));}
        // mRotY(&view, rot);
        // printf("%f %f\n", px, py);

        // mIdent(&view);
        // mSetPos(&view, (vec){-px, -py, -100.f});
        // float rot = 0.8f;
        // if(py < -42.f){rot = 0.4f+(0.4f-(0.01f*(-py-42.f)));}
        // mRotY(&view, rot);
        // printf("%f %f\n", px, py);

        mIdent(&view);
        mSetPos(&view, (vec){-px, -py, -100.f});
        float rot = 0.8f;
        if(py < -42.f){rot = 0.453f+(0.347-(0.013f*(-py-42.f)));}
        mRotY(&view, rot);
    }

    // ball spawner
    if(bspawn == 0)
    {
        static float lt = 0.f;
        if(t > lt)
        {
            bspawn = esRand(1, 4);
            lt = t+esRandFloat(12.f, 24.f);
        }
    }

    // debug
    //printf("%f %f %f\n", px, py, pr);

//*************************************
// render
//*************************************

    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render scene
    glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (float*)&view.m[0][0]);
    glDisable(GL_CULL_FACE);
    esBindRender(0);
    glEnable(GL_CULL_FACE);

    // render snowball pickup
    if(bspawn == 1)
    {
        if(hamo == 0)
        {
            const float xm = (px - -34.f);
            const float ym = (py - 34.f);
            const float sqd = xm*xm + ym*ym;
            if(sqd < 36.f)
            {
                hamo = 1;
                bspawn = 0;
                updateTitle();
            }
        }
        mIdent(&model);
        mSetPos(&model, (vec){-34.f, 34.f, 2.2f});
        updateModelView();
        esBindRender(27);
    }
    else if(bspawn == 2)
    {
        if(hamo == 0)
        {
            const float xm = (px - 34.f);
            const float ym = (py - 34.f);
            const float sqd = xm*xm + ym*ym;
            if(sqd < 36.f)
            {
                hamo = 1;
                bspawn = 0;
                updateTitle();
            }
        }
        mIdent(&model);
        mSetPos(&model, (vec){34.f, 34.f, 2.2f});
        updateModelView();
        esBindRender(27);
    }
    else if(bspawn == 3)
    {
        if(hamo == 0)
        {
            const float xm = (px - 34.f);
            const float ym = (py - -34.f);
            const float sqd = xm*xm + ym*ym;
            if(sqd < 36.f)
            {
                hamo = 1;
                bspawn = 0;
                updateTitle();
            }
        }
        mIdent(&model);
        mSetPos(&model, (vec){34.f, -34.f, 2.2f});
        updateModelView();
        esBindRender(27);
    }
    else if(bspawn == 4)
    {
        if(hamo == 0)
        {
            const float xm = (px - -34.f);
            const float ym = (py - -34.f);
            const float sqd = xm*xm + ym*ym;
            if(sqd < 36.f)
            {
                hamo = 1;
                bspawn = 0;
                updateTitle();
            }
        }
        mIdent(&model);
        mSetPos(&model, (vec){-34.f, -34.f, 2.2f});
        updateModelView();
        esBindRender(27);
    }

    // render snowball
    if(bdx != 0 || bdy != 0)
    {
        mIdent(&model);
        bx += bdx * 25.f * dt;
        by += bdy * 25.f * dt;

        if(bx > 82.f){bdx = 0.f, bdy = 0.f;}
        else if(bx < -82.f){bdx = 0.f, bdy = 0.f;}
        if(by > 82.f){bdx = 0.f, bdy = 0.f;}
        else if(by < -82.f){bdx = 0.f, bdy = 0.f;}

        const float sqd = bx*bx + by*by;
        if(sqd < 400.f)
        {
            bdx = 0.f;
            bdy = 0.f;
            if(dhp > 0)
            {
                dhp--;
                dhit = t+1.3f;
                updateTitle();
            }
        }

        mSetPos(&model, (vec){bx, by, 8.f});
        updateModelView();
        esBindRender(27);
    }

    // render tux
    if(t < fire)
    {
        mIdent(&model);
        mSetPos(&model, (vec){px, py, 1.2f});
        mRotZ(&model, -pr+0.785398f);
        updateModelView();
        esBindRender(26);
    }
    else
    {
        static int ri = 1;
        if(moving == 1)
        {
            static float lt = 0.f;
            if(t > lt)
            {
                ri++;
                if(ri > 24){ri = 1;}
                lt = t + 0.05f;
            }

            mIdent(&model);
            mSetPos(&model, (vec){px, py, 1.2f});
            mRotZ(&model, -pr+0.785398f);
            updateModelView();
            esBindRender(ri);
        }
        else if(moving == 2)
        {
            static float lt = 0.f;
            if(t > lt)
            {
                ri--;
                if(ri < 1){ri = 24;}
                lt = t + 0.05f;
            }

            mIdent(&model);
            mSetPos(&model, (vec){px, py, 1.2f});
            mRotZ(&model, -pr+0.785398f);
            updateModelView();
            esBindRender(ri);
        }
        else
        {
            mIdent(&model);
            mSetPos(&model, (vec){px, py, 1.2f});
            mRotZ(&model, -pr+0.785398f);
            updateModelView();
            esBindRender(25);
        }
    }

    // render dragon
    if(dhp == 0)
    {
        mIdent(&model);
        mSetPos(&model, (vec){0.f, 0.f, 11.f});
        mScale1(&model, 20.f);
        mRotZ(&model, -ar-2.356194f);
        updateModelView();
        esBindRender(44);
    }
    else if(t < dhit)
    {
        mIdent(&model);
        mSetPos(&model, (vec){0.f, 0.f, 11.f});
        mScale1(&model, 20.f);
        mRotZ(&model, -ar-2.356194f);
        updateModelView();
        esBindRender(43);
    }
    else
    {
        static uint ri = 28;
        static uint flip = 0;
        static float lt = 0.f;
        static float wait = 0.f;
        if(t > wait && t > lt)
        {
            if(flip == 0)
            {
                ri++;
                if(ri > 41)
                {
                    ri = 41;
                    flip = 1;
                    float art = ar+d2PI;
                    float cos1 = cosf(art);
                    float sin1 = sinf(art);
                    cdx[0] = cos1 - sin1;
                    cdy[0] = sin1 + cos1;
                    cx[0] = cdx[0]*15.f;
                    cy[0] = cdy[0]*15.f;
                    cx[1] = cdx[0]*15.f;
                    cy[1] = cdy[0]*15.f;
                    cx[2] = cdx[0]*15.f;
                    cy[2] = cdy[0]*15.f;
                    art = ar+d2PI+0.23;
                    cos1 = cosf(art);
                    sin1 = sinf(art);
                    cdx[1] = cos1 - sin1;
                    cdy[1] = sin1 + cos1;
                    art = ar+d2PI-0.23;
                    cos1 = cosf(art);
                    sin1 = sinf(art);
                    cdx[2] = cos1 - sin1;
                    cdy[2] = sin1 + cos1;
                }
            }
            else
            {
                ri--;
                if(ri < 28){ri=28; flip=0; wait=t+esRandFloat(0.75f, 6.f);}
            }
            lt = t + 0.05f;
        }
        mIdent(&model);
        mSetPos(&model, (vec){0.f, 0.f, 11.f});
        mScale1(&model, 20.f);
        const float len = 1.f/sqrtf(px*px + py*py);
        const float dtpx = px*len;
        const float dtpy = py*len;
        const float dot = (dtpx * adx) + (dtpy * ady);
        const float ts = esRandFloat(0.1f, fabsf(dot));
        if(dot < 0.f){ar += ts * dt;}
        else         {ar -= ts * dt;}
        const float cos1 = cosf(ar);
        const float sin1 = sinf(ar);
        adx = cos1 - sin1;
        ady = sin1 + cos1;
        mRotZ(&model, -ar-2.356194f);
        updateModelView();
        esBindRender(ri);
    }

    // render fire bolts
    for(uint i = 0; i < 3; i++)
    {
        if(cdx[i] != 0.f || cdy[i] != 0.f)
        {
            mIdent(&model);
            cx[i] += cdx[i] * 25.f * dt;
            cy[i] += cdy[i] * 25.f * dt;

            const float xm = (px - cx[i]);
            const float ym = (py - cy[i]);
            const float d = sqrtf(xm*xm + ym*ym);
            if(d < 3.f)
            {
                px = -38.159271f;
                py = 34.487778f;
                pr = -4.996970f;
                float cos1 = cosf(pr);
                float sin1 = sinf(pr);
                pdx = cos1 - sin1;
                pdy = sin1 + cos1;
                dhp = 24;
                updateTitle();
            }

            if(cx[i] > 82.f){cdx[i] = 0.f, cdy[i] = 0.f;}
            else if(cx[i] < -82.f){cdx[i] = 0.f, cdy[i] = 0.f;}
            if(cy[i] > 82.f){cdx[i] = 0.f, cdy[i] = 0.f;}
            else if(cy[i] < -82.f){cdx[i] = 0.f, cdy[i] = 0.f;}
            
            float high = 12.f;
            if(d < 12.f)
            {
                if(d < 8.f)
                    high = 8.f;
                else
                    high = d;
            }
            mSetPos(&model, (vec){cx[i], cy[i], high});
            mSetDir(&model, (vec){cdx[i], cdy[i], 0.f});
            mRotX(&model, t*4.f);
            updateModelView();
            esBindRender(42);
        }
    }

    //

    // display render
    glfwSwapBuffers(window);
}

//*************************************
// input
//*************************************
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // control
    if(action == GLFW_PRESS)
    {
        if(     key == GLFW_KEY_A || key == GLFW_KEY_LEFT)  { keystate[0] = 1; }
        else if(key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) { keystate[1] = 1; }
        else if(key == GLFW_KEY_W || key == GLFW_KEY_UP)    { keystate[2] = 1; }
        else if(key == GLFW_KEY_S || key == GLFW_KEY_DOWN)  { keystate[3] = 1; }
        else if(key == GLFW_KEY_SPACE && moving == 0 && fire < t) // throw snowball
        {
            if(hamo == 1)
            {
                bx = px + (pdx*-2.6f);
                by = py + (pdy*-2.6f);
                const float ra = pr-d2PI;
                float cos1 = cosf(ra);
                float sin1 = sinf(ra);
                bx += (cos1 - sin1)*2.f;
                by += (sin1 + cos1)*2.f;
                bdx = -pdx;
                bdy = -pdy;
                hamo = 0;
                updateTitle();
            }
            fire = t + 0.1f;
        }
        else if(key == GLFW_KEY_F) // show average fps
        {
            if(t-lfct > 2.0)
            {
                char strts[16];
                timestamp(&strts[0]);
                printf("[%s] FPS: %g\n", strts, fc/(t-lfct));
                lfct = t;
                fc = 0;
            }
        }
        else if(key == GLFW_KEY_C) // change view
        {
            cvt = 1 - cvt;
        }
    }
    else if(action == GLFW_RELEASE)
    {
        if(     key == GLFW_KEY_A || key == GLFW_KEY_LEFT)  { keystate[0] = 0; }
        else if(key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) { keystate[1] = 0; }
        else if(key == GLFW_KEY_W || key == GLFW_KEY_UP)    { keystate[2] = 0; }
        else if(key == GLFW_KEY_S || key == GLFW_KEY_DOWN)  { keystate[3] = 0; }
    }
}
void window_size_callback(GLFWwindow* window, int width, int height)
{
    winw = width, winh = height;
    glViewport(0, 0, winw, winh);
    aspect = (float)winw / (float)winh;
    ww = winw, wh = winh;
    mIdent(&projection);
    mPerspective(&projection, 60.0f, aspect, 0.1f, FAR_DISTANCE);
    glUniformMatrix4fv(projection_id, 1, GL_FALSE, (float*)&projection.m[0][0]);
}
#ifdef WEB
EM_BOOL emscripten_resize_event(int eventType, const EmscriptenUiEvent *uiEvent, void *userData)
{
    winw = uiEvent->documentBodyClientWidth;
    winh = uiEvent->documentBodyClientHeight;
    window_size_callback(window, winw, winh);
    emscripten_set_canvas_element_size("canvas", winw, winh);
    return EM_FALSE;
}
#endif

//*************************************
// process entry point
//*************************************
int main(int argc, char** argv)
{
    // allow custom msaa level
    int msaa = 16;
    if(argc >= 2){msaa = atoi(argv[1]);}

    // help
    printf("----\n");
    printf("James William Fletcher (github.com/mrbid) (notabug.org/Vandarin)\n");
    printf("%s - You say it best when you say nothing at all.\n", appTitle);
    printf("----\n");
    printf("You have to kill the dragon. It takes 24 snowball hits to kill the dragon. If Tux gets hit just once you have to start all over again! Snowballs will randomly spawn in one of the four grassy patches, Tux can only carry one at a time and cant throw a snowball unless he is standing still. Good luck!\n");
    printf("----\n");
#ifndef WEB
    printf("One command line argument, msaa 0-16.\n");
    printf("e.g; ./tuxvsdragon 16\n");
    printf("----\n");
#endif
    printf("W,A,S,D / Arrow Keys = Move\n");
    printf("Space Bar = Throw Snowball\n");
    printf("C = Change camera view\n");
    printf("----\n");
    printf("All assets apart from Tux generated using LUMA GENIE. (https://lumalabs.ai/genie)\n");
    printf("----\n");
    printf("Tux made by Andy Cuccaro\n");
    printf("https://andycuccaro.gumroad.com/\n");
    printf("----\n");
    printf("Dragon animated by Emilio O. (ELO2024)\n");
    printf("https://www.youtube.com/@emilio2024x\n");
    printf("----\n");
    printf("Font is Plastic Love by Azkarizki\n");
    printf("https://www.fontspace.com/plastic-love-font-f49676\n");
    printf("----\n");
    printf("Icon by Forest Walter\n");
    printf("https://www.forrestwalter.com/\n");
    printf("----\n");
    printf("🎄 Merry Christmas 2023! and a happy new year 2024! 🎅\n");
    printf("----\n");
    printf("%s\n", glfwGetVersionString());
    printf("----\n");

    // init glfw
    if(!glfwInit()){printf("glfwInit() failed.\n"); exit(EXIT_FAILURE);}
#ifdef WEB
    double width, height;
    emscripten_get_element_css_size("body", &width, &height);
    winw = (uint)width, winh = (uint)height;
#endif
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SAMPLES, msaa);
    window = glfwCreateWindow(winw, winh, appTitle, NULL, NULL);
    if(!window)
    {
        printf("glfwCreateWindow() failed.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    const GLFWvidmode* desktop = glfwGetVideoMode(glfwGetPrimaryMonitor());
#ifndef WEB
    glfwSetWindowPos(window, (desktop->width/2)-(winw/2), (desktop->height/2)-(winh/2)); // center window on desktop
#endif
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1); // 0 for immediate updates, 1 for updates synchronized with the vertical retrace, -1 for adaptive vsync

    // set icon
    glfwSetWindowIcon(window, 1, &(GLFWimage){16, 16, (unsigned char*)icon_image});

//*************************************
// bind vertex and index buffers
//*************************************
    register_arena();
    register_a1();
    register_a2();
    register_a3();
    register_a4();
    register_a5();
    register_a6();
    register_a7();
    register_a8();
    register_a9();
    register_a10();
    register_a11();
    register_a12();
    register_a13();
    register_a14();
    register_a15();
    register_a16();
    register_a17();
    register_a18();
    register_a19();
    register_a20();
    register_a21();
    register_a22();
    register_a23();
    register_a24();
    register_tux_idle();
    register_tux_throw();
    register_ball();
    register_b1();
    register_b2();
    register_b3();
    register_b4();
    register_b5();
    register_b6();
    register_b7();
    register_b8();
    register_b9();
    register_b10();
    register_b11();
    register_b12();
    register_b13();
    register_b14();
    register_arrow();
    register_dragon_hit();
    register_dragon_dead();

//*************************************
// compile & link shader programs
//*************************************
    makeLambert3();

//*************************************
// configure render options
//*************************************
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.22f, 0.22f, 0.22f, 0.f);

    shadeLambert3(&position_id, &projection_id, &modelview_id, &lightpos_id, &normal_id, &color_id, &opacity_id);
    glUniformMatrix4fv(projection_id, 1, GL_FALSE, (float*)&projection.m[0][0]);
    window_size_callback(window, winw, winh);

//*************************************
// execute update / render loop
//*************************************

    // init
    srand(time(0));
    t = fTime();
    lfct = t;
    px = -38.159271f;
    py = 34.487778f;
    pr = -4.996970f;
    float cos1 = cosf(pr);
    float sin1 = sinf(pr);
    pdx = cos1 - sin1;
    pdy = sin1 + cos1;
    ar = 0.f;
    cos1 = cosf(ar);
    sin1 = sinf(ar);
    adx = cos1 - sin1;
    ady = sin1 + cos1;
    dhp = 24;

    // loop
#ifdef WEB
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, EM_FALSE, emscripten_resize_event);
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    while(!glfwWindowShouldClose(window)){main_loop();}
#endif

    // done
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
    return 0;
}
