#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>      // Must have for Windows platform builds
#include <gl\gl.h>         // Microsoft OpenGL headers (version 1.1 by themselves)
#include <gl\glu.h>         // OpenGL Utilities
#include <gl\glut.h>
#include <math.h>
#include <crtdbg.h>

#define GAME_SPEED 300

struct Thing
{
    GLbyte wait;
    GLbyte x;
    GLbyte y;
    struct Thing* next;
};

bool Map[21][21];
struct Thing Food;
struct Thing* Worm = NULL;
bool pause;

GLbyte xMove = 0;
GLbyte yMove = 0;
GLshort wSize = 1;

void CreateFood()
{
    srand ( rand() );
    short x, y;
    do
    {
        x = ( (rand() % 20) - 10 )*5;
        y = ( (rand() % 20) - 10 )*5;
    } while( Map[(x/5)+10][(y/5)+10] == true );

    Food.x = x;
    Food.y = y;
}

void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Food
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(Food.x, Food.y);
        glVertex2f(Food.x, Food.y+5);
        glVertex2f(Food.x+5, Food.y+5);
        glVertex2f(Food.x+5, Food.y);
    glEnd();
    
    glColor3f(1.0f, 1.0f, 1.0f);
    struct Thing* next = Worm;
    // Draw the point
    glBegin(GL_QUADS);
        while ( next != NULL )
        {
              glVertex2f(next->x, next->y);
              glVertex2f(next->x, next->y+5);
              glVertex2f(next->x+5, next->y+5);
              glVertex2f(next->x+5, next->y);
              
              next = next->next;
        }
    glEnd();
    glutSwapBuffers();
}

void SetupRC()
{
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
   glPointSize(20);

   for ( int x = 0; x < 21; x++ )
   {
       for ( int y = 0; y < 21; y++ )
       {
            Map[x][y] = false;
       }
   }

   pause = false;
}

void SpecialKeys(int key, int x, int y)
{
    if(key == GLUT_KEY_UP && yMove != -5)
    {
        xMove = 0;
        yMove = 5;
    }

    if(key == GLUT_KEY_DOWN && yMove != 5)
    {
        xMove = 0;
        yMove = -5;
    }

    if(key == GLUT_KEY_LEFT && xMove != 5)
    {
        xMove = -5;
        yMove = 0;
    }

    if(key == GLUT_KEY_RIGHT && xMove != -5)
    {
        xMove = 5;
        yMove = 0;
    }

    if ( key == GLUT_KEY_END )
    {
        pause = !pause;
    }
}

void ChangeSize(int w, int h)
{
    GLfloat nRange = 50.0f;

    if(h == 0)
    {
        h = 1;
    }

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (w <= h)
    {
        glOrtho (-nRange, nRange, -nRange*h/w, nRange*h/w, -nRange, nRange);
    }
    else
    {
        glOrtho (-nRange*w/h, nRange*w/h, -nRange, nRange, -nRange, nRange);
    }

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void Start()
{
    xMove = 0;
    yMove = 0;
    wSize = 1;

    struct Thing* next = Worm->next;
    while ( next != NULL )
    {
        struct Thing* temp = next->next;
        Map[(next->x/5)+10][(next->y/5)+10] = false;
        free(next);
        next = temp;
    }

    Worm->next = NULL;
    Worm->wait = 0;
    Worm->x = 0;
    Worm->y = 0;
    Map[10][10] = true;

    CreateFood();
}

void MoveWorm ( int value )
{
    if ( pause )
    {
        glutTimerFunc(GAME_SPEED,MoveWorm, 1);
        return;
    }

    struct Thing* temp = Worm;
    // Reset the last point...
    if ( temp->wait != 0 )
    {    
        while ( temp->next != NULL )
        {
            struct Thing* next = temp->next;
            if ( next->wait == 0 )
            {
                Map[(next->x/5)+10][(next->y/5)+10] = false;
                break;
            }
            else
            {
                temp = next;
            }
        }
    }
    else
    {
        Map[(temp->x/5)+10][(temp->y/5)+10] = false;
    }

    temp = Worm;
    // Move the worm!
    while ( temp->next != NULL )
    {
        struct Thing* next = temp->next;
        if ( next->wait == 0 )
        {
            temp->x = next->x;
            temp->y = next->y;
        }
        else if ( next->wait > 0 )
        {
            next->wait--;
        }
        temp = next;
    }

    temp->x += xMove;
    temp->y += yMove;
    if ( Map[(temp->x/5)+10][(temp->y/5)+10] == true || temp->x >= 50 || temp->x <= -55 || temp->y >= 50 || temp->y <= -55 )
    {
        Start();
        glutTimerFunc(GAME_SPEED,MoveWorm, 1);
        return;
    }
    Map[(temp->x/5)+10][(temp->y/5)+10] = true;
    
    if ( temp->x == Food.x && temp->y == Food.y )
    {
        struct Thing* node = (struct Thing*) malloc(sizeof(struct Thing));
        node->next = Worm;
        node->x = Food.x;
        node->y = Food.y;
        node->wait = wSize;
        wSize++;

        Worm = node;

        CreateFood();
    }

    glutTimerFunc(GAME_SPEED,MoveWorm, 1);

    RenderScene();
}

void CleanUp()
{
    struct Thing* next = Worm;
    while ( next != NULL )
    {
        struct Thing* temp = next->next;
        free(next);
        next = temp;
    }
}

int main(int argc, char* argv[])
{
    #ifndef _DEBUG
        FreeConsole();
    #endif
    Worm = (struct Thing*) malloc(sizeof(struct Thing));
    Worm->next = NULL;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Snake v0.1");
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);
    glutDisplayFunc(RenderScene);
    SetupRC();
    Start();
    glutTimerFunc(500,MoveWorm, 1);
    atexit(CleanUp);
    glutMainLoop();

    return 0;
}