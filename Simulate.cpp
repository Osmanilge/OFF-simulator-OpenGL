// Sphere approximation by recursive subdivision of a tetrahedron

#include "Angel.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;


#define N  4  // number of subdivisions
#define M 16*64*3  // number of resulting points
const char* path = "/Models";
GLsizei w=512, h=512;


//point4 data[M];
color4 quad_color[800000];
//exxxxxxxxxxx
void fillData(char *name);
float min(float x, float y);
float max(float x, float y);
int numV=0;
int numF=0;
point4 data[800000];
int faces[800000][3];
float minn[3];
float maxn[3];
float scale=1.0;
char** argvx;
int argcx;
int off=1;
//exxxxxxxxxxxxxxxx

// four equally spaced points on the unit circle


point4 v[4]= {vec4(0.0, 0.0, 1.0, 1.0), vec4(0.0, 0.942809, -0.333333, 1.0),
                vec4(-0.816497, -0.471405, -0.333333, 1.0),
               vec4(0.816497, -0.471405, -0.333333, 1.0)};

float fixx=0.0,fixy=0.0,fixz=0.0,oldfixx=0.0,oldfixy=0.0,oldfixz=0.0;
float viewtheta=0.0;
float viewphi=0.0;
float theta = 0.0;
float phi = 0.0;
float radius = 20.0;
static char type='f';
GLuint program;

GLuint buffers[1];
GLuint loc, loc2 ;
GLint matrix_loc, projection_loc;

static int k =0;

point4 at = vec4(0.0, 0.0, 0.0, 1.0);
//point4 eye = vec4(0.0, 0.0, 2.0, 1.0);
vec4 up = vec4(0.0, 1.0, 0.0, 0.0);

GLfloat left= -16.0, right=16.0, top=16.0, bottom= -16.0, zNear= 0.01, zFar=100.0;

mat4 model_view;
mat4 projection;


/* OpenGL initialization */

void init()
{
	
	fillData(*(argvx+off));
    // Create a vertex array object
	//for(int i=0;i<numV;i++)printf("%f %f %f\n",(Scale(scale,scale,scale)*data[i]).x,(Scale(scale,scale,scale)*data[i]).y,(Scale(scale,scale,scale)*data[i]).z);
	printf("%f %f %f\n",minn[0],minn[1],minn[2]);
	printf("%f %f %f\n",maxn[0],maxn[1],maxn[2]);
	
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object

/* set up vertex buffer object */

   glGenBuffers(1, buffers);
   glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
   //glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
   glBufferData(GL_ARRAY_BUFFER, sizeof(data) + sizeof(quad_color), NULL, GL_STATIC_DRAW);

    // Load shaders and use the resulting shader program

    program = InitShader("vshader51.glsl", "fshader51.glsl");
    glUseProgram(program);

   loc = glGetAttribLocation(program, "vPosition");
   
   glEnableVertexAttribArray(loc);
   glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, 0);
   //newwwww
   loc2 = glGetAttribLocation(program, "vColor");
   glEnableVertexAttribArray(loc2);
   glVertexAttribPointer(loc2, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(data)));

   matrix_loc = glGetUniformLocation(program, "model_view");
   projection_loc = glGetUniformLocation(program, "projection");
   //newwwww	
   glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(data), data );
   glBufferSubData( GL_ARRAY_BUFFER, sizeof(data), sizeof(quad_color), quad_color );

   glClearColor(1.0, 1.0, 1.0, 1.0); /* white background */
}

void display()
{	
	
     vec4 t;
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  /*clear the window */
     //point4 eye(radius*cos(viewtheta)*cos(viewphi), radius*cos(viewtheta)*sin(viewphi), radius*sin(viewtheta), 1.0);
     
     point4 eye(0.0, 0.0, 1.0, 1.0);
     model_view =  LookAt(eye, at, up);
     model_view = (Translate( vec4(0, 0, -radius, 0.0) ) * 
     			RotateX( viewphi ) *
			 	RotateY( viewtheta )*
     			Translate( vec4(-fixx+oldfixx, -fixy+oldfixy, -fixz+oldfixz, 0.0))*
     			//RotateXY(phi,theta)*
     			
                		RotateX( phi ) *
			 	RotateY( theta)*
			 	Scale(scale,scale,scale)*
			 	Translate( vec4(-oldfixx, -oldfixy, -oldfixz, 0.0))
			 	
			 	
			   );
     //projection = Ortho(left, right, bottom, top, zNear, zFar);
     printf("%f",zNear);
     projection = Perspective( 80, 1.0, zNear, zFar);
     glUniformMatrix4fv(matrix_loc, 1, GL_TRUE, model_view);
     glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);
     switch(type){
     case 'f':
     	for(int i = 0; i<numF; i+=1) {
     		
       	glDrawElements(GL_TRIANGLES, 3,GL_UNSIGNED_INT,faces[i]); 
       	
       	}
        break;
     case 'e':
     	for(int i = 0; i<numF; i+=1) {
       	//glDrawArrays(GL_LINE_LOOP, i, 3);
       	
       	glDrawElements(GL_LINE_LOOP, 3,GL_UNSIGNED_INT,faces[i]);  }
        break;
     case 'v':
     	for(int i = 0; i<numV; i+=3) 
       	glDrawArrays(GL_POINTS, i, 3); 
        break;
       
     }
     glutSwapBuffers();
}



void mykey(unsigned char key, int mousex, int mousey)
{
   float dr = 2.0;
   if(key=='q'||key=='Q') exit(0);
   if(key == 'x') fixx-=1.0;
   if(key == 'X') fixx+=1.0;
   if(key == 'y') fixy-=1.0;
   if(key == 'Y') fixy+=1.0;
   if(key == 'z') fixz-=1.0;
   if(key == 'Z') fixz+=1.0;
   if(key == 's') scale*=0.5;
   if(key == 'S') scale*=2.0;
   if(key == 'r') radius*=9.0/10;
   if(key == 'R') radius*=10.0/9;
   
   
   if(key == 'o') viewtheta += dr;
   if(key == 'O') viewtheta -= dr;
   if(key == 'u') viewphi += dr;
   if(key == 'U') viewphi -= dr;
   if(key == 'e') type='e';
   if(key == 'v') type='v';
   if(key == 'f') type='f';
   if(key == 'n'){
   	if(++off%argcx==0)++off;
   	fillData(*(argvx+ off % argcx));
   	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(data), data );
   	glBufferSubData( GL_ARRAY_BUFFER, sizeof(data), sizeof(quad_color), quad_color );
   }
   if(key == 'p'){
   	if((off=argcx-1+off)%argcx==0)off=argcx-1;
   	fillData(*(argvx+ off % argcx));
   	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(data), data );
   	glBufferSubData( GL_ARRAY_BUFFER, sizeof(data), sizeof(quad_color), quad_color );
   }
   
   glutPostRedisplay();
}
	
void specialKeyboard( int key, int x, int y ) 
{
    switch (key)
    {
    case GLUT_KEY_RIGHT:
        theta += 2.0;        
        if ( theta > 360.0 ) 
            theta -= 360.0;
        break;
    case GLUT_KEY_LEFT:
        theta -= 2.0;
        if ( theta < 0.0 ) 
            theta += 360.0;
        break;
        case GLUT_KEY_DOWN:
        phi += 2.0;        
        if ( phi > 360.0 ) 
            phi -= 360.0;
        break;
    case GLUT_KEY_UP:
        phi -= 2.0;
        if ( phi < 0.0 ) 
            phi += 360.0;
        break;
    }
    glutPostRedisplay();
}

void reshape(GLsizei ww, GLsizei hh)
{
   glViewport(0,0, ww, hh);
   w = ww;
   h = hh;
}

int main(int argc,char **argv)
{	
	
	argcx=argc;
	argvx=argv;
    glutInit(&argc, argv);

#ifdef __APPLE__
    glutInitDisplayMode( GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
#else
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
#endif

    glutInitWindowSize(512, 512);

//    glutInitContextVersion( 3, 2 );
//	glutInitContextProfile( GLUT_CORE_PROFILE );
    glutCreateWindow("Simulation");
	glewExperimental = GL_TRUE;
	glewInit();
    init();
    
	glutSpecialFunc(specialKeyboard);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(mykey);


    glEnable(GL_DEPTH_TEST);
    glutMainLoop();
    return 0;
}



//newwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww


int readInt(FILE *file);
float readFloat(FILE *file);

void fillData(char *name){
	char c;
	float tempData[3];
	int tempFaces[3];
	//FILE *fopen(char *dosya-adı, char *mod);
	FILE *file =fopen(name, "r");
	//free(data);
	//free(faces);
	while(fgetc(file)!='\n');
	
	//c=fgetc(file);
	
	numV=readInt(file);
	
	int n[3];
	numF=readInt(file);
	//data=(vec4*)malloc(16*numV);
	
	//faces=(int*)malloc(sizeof(n)*numF);
	
	
	while(fgetc(file)!='\n');
	for(int i=0;i<numV;i++){
		
		tempData[0]=readFloat(file);tempData[1]=readFloat(file);tempData[2]=readFloat(file);
		if(i==0){minn[0]=tempData[0];maxn[0]=tempData[0];minn[1]=tempData[1];maxn[1]=tempData[1];minn[2]=tempData[2];maxn[2]=tempData[2];}
		else{minn[0]=min(minn[0],tempData[0]);minn[1]=min(minn[1],tempData[1]);minn[2]=min(minn[2],tempData[2]);
		     maxn[0]=max(maxn[0],tempData[0]);maxn[1]=max(maxn[1],tempData[1]);maxn[2]=max(maxn[2],tempData[2]);}
		data[i]=point4(tempData[0],tempData[1],tempData[2],1.0);
		quad_color[i]=vec4((rand()%100)/100.0,(rand()%100)/100.0,(rand()%100)/100.0,1.0);
		
		}
	
	for(int i=0;i<numF;i++){
		if(readInt(file)!=3)printf("Error face reading");
		tempFaces[0]=readInt(file);tempFaces[1]=readInt(file);tempFaces[2]=readInt(file);
		faces[i][0]=tempFaces[0];
		faces[i][1]=tempFaces[1];
		faces[i][2]=tempFaces[2];
		}
		
	while((c=fgetc(file))!=EOF)printf("%c",c);
	float x=max(maxn[0]-minn[0],maxn[1]-minn[1]);
	x=max(x,maxn[2]-minn[2]);
	
	scale=20.0/(x*10.0/9.0);	
	fixx=(minn[0]+maxn[0])/2.0;
	fixy=(minn[1]+maxn[1])/2.0;
	fixz=(minn[2]+maxn[2])/2.0;
	oldfixx=fixx;
	oldfixy=fixy;
	oldfixz=fixz;
	theta=0;
	phi=0;
	viewtheta=0;
	viewphi=0;
	
	

}

float readFloat(FILE *file){
	static int k=0;
	char c;
	float is=1.0;
	float n=0.0;
	float result;
	if((c=fgetc(file))=='-')is=-1.0;
	else n+=c-48.0;
	
	float np=0.0;
	while((c=fgetc(file))!='.'&& c!=' ' &&c!='\n'&& c!=EOF)n=c-48.0+n*10.0;
	int i=0;
	if(c=='.')
		for(i=0;(c=fgetc(file))!=' ' && c!='\n'&& c!='e'&& c!=EOF;i++)np=c-48.0+np*10.0;
	
	for(;i>0;i--)np/=10.0;
	
	result=(n+np);
	
	int ex=0;
	if(c=='e')ex=readInt(file);
		
	if(ex<0)for(int i=0;i<-ex;i++)result/=10.0;
	else    for(int i=0;i<ex;i++)result*=10.0;
	
	/*if(k++%3==0)printf("\n");
	printf("%f ",is*(n+np));*/
	return is*result;
}


int readInt(FILE *file){
	static int k=-2;
	char c;
	int n=0;
	int is=1;
	if((c=fgetc(file))=='-')is=-1;
	else n+=c-48;
	
	while((c=fgetc(file))!=' '&& c!='\n'&& c!=EOF)n=c-48+n*10;
	/*if(k++%4==0)printf("\n");
	printf("%d ",n);*/
	return n*is;
	
}


float min(float x, float y){
	return (x>y) ? y : x;
}
float max(float x, float y){
	return (x<y) ? y : x;
}



