#include <GL/glut.h>
#include <stdio.h>
#include <cmath>

GLfloat tam=0,angX=0,angY=0,posX=0;
GLfloat ancho = 1.0;
GLint obj=1;

typedef struct{
	GLubyte *dibujo;
	GLuint bpp;
	GLuint largo;
	GLuint ancho;
	GLuint ID;
}textura;

textura tCerebro;

int cargaTGA( char *nombre, textura *imagen){
	GLubyte plantillaTGA[12]={0,0,2,0,0,0,0,0,0,0,0,0};
	GLubyte cabeceraTGA[12];
	GLubyte detallesTGA[6];
	GLuint bytesporpunto;
	GLuint tamanoTGA;
	GLuint temp,i;
	GLuint tipo=GL_RGBA;
	
	FILE *archivo = fopen(nombre,"rb");
	if(archivo == NULL ||
	fread(cabeceraTGA,1,sizeof(cabeceraTGA),archivo)!=sizeof(cabeceraTGA) ||
	memcmp(plantillaTGA,cabeceraTGA,sizeof(cabeceraTGA))!=0 ||
	fread(detallesTGA,1,sizeof(detallesTGA),archivo)!=sizeof(detallesTGA))
	{ 
		if(archivo==NULL){
			printf("No se encontro el archivo %s\n",nombre);
			return 0;
		}else{
			fclose(archivo);
			return 0;
		}
	}
	
	imagen->largo=256*detallesTGA[1]+detallesTGA[0];
	imagen->ancho=256*detallesTGA[3]+detallesTGA[2];
	
	if(imagen->largo <= 0 ||
	imagen->ancho <=0 ||
	(detallesTGA[4]!=24 && detallesTGA[4]!=32)){
		printf("Datos invalidos\n");
		fclose(archivo);
		return 0;
	}
	
	imagen->bpp=detallesTGA[4];
	bytesporpunto=detallesTGA[4]/8;
	tamanoTGA=imagen->largo*imagen->ancho*bytesporpunto;
	
	imagen->dibujo = (GLubyte *)malloc(tamanoTGA);
	if(imagen->dibujo == NULL || 
	fread(imagen->dibujo,1,tamanoTGA,archivo) != tamanoTGA){
		if(imagen->dibujo != NULL){ 
			printf("Error leyendo imagen\n");
			free(imagen->dibujo);
		}else{
			printf("Error asignando memoria\n");
		}
		fclose(archivo);
		return 0;		
	}
	
	for (i=0; i<(int)tamanoTGA;i+=bytesporpunto){
		temp=imagen->dibujo[i];
		imagen->dibujo[i]=imagen->dibujo[i+2];
		imagen->dibujo[i+2]=temp;
	}
	fclose(archivo);
	
	glGenTextures(1,&imagen[0].ID);
	glBindTexture(GL_TEXTURE_2D,imagen[0].ID);
	if(imagen->bpp==24) tipo=GL_RGB;
	glTexImage2D(GL_TEXTURE_2D,0,tipo,imagen[0].ancho,imagen[0].largo,0,tipo,GL_UNSIGNED_BYTE,imagen[0].dibujo);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	return 1;
}

void init(){
	GLfloat light_position[]={1,1,1,0};
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0,GL_POSITION,light_position);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0,0.0,0.0,0.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glEnable(GL_TEXTURE_2D);
	if(!cargaTGA("cerebro.tga",&tCerebro)){
		printf("Error cargando textura\n");
		exit(0);
	}					
}

void cerebro(){
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,tCerebro.ID);
		glColor4f(1,1,1,1);
		glBegin(GL_QUADS);
			glTexCoord2f(0,0);glVertex3d(-300,-300,-300);
			glTexCoord2f(1,0);glVertex3d(300,-300,-300);
			glTexCoord2f(1,1);glVertex3d(300,300,-300);
			glTexCoord2f(0,1);glVertex3d(-300,300,-300);
		glEnd();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

//Voids
void objeto(GLint obj,GLfloat tam){
	
	GLfloat mat_ambient[]={0,0,1,1};
	GLfloat mat_diffuse[]={0,0,1,1};
	GLfloat mat_specular[]={1,1,1,1};
	GLfloat mat_shininess[]={50};
	
	glMaterialfv(GL_FRONT,GL_AMBIENT,mat_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);
	
	GLfloat t=tam;
	switch(obj){
		case 4://Dragon
			t=t/4;
			glTranslated(-t*6,0,0);
			glRotated(90,1,0,0);
			glutSolidCone(t,t*4,4,4);//cabeza
			
			glRotated(180,1,0,0);
			glTranslated(-t/2,t/2,0);
			glutSolidCone(t/2,t*4,4,4);//cuernos
			glTranslated(0,-t,0);
			glutSolidCone(t/2,t*4,4,4);//cuernos
			glTranslated(0,t,0);
			glTranslated(t/2,-t/2,0);
			glRotated(-180,1,0,0);
			
			glRotated(-90,1,0,0);
			glTranslated(t*4,0,0);
			
			//cuerpo
			for(GLint i=0;i<=180;i+=10){
				glTranslated(t*4*cos(i*M_PI/180),t*4*sin(i*M_PI/180),0);
				glutSolidSphere(t*0.8,5,8);
				
				//patas
				if(i==60){
					glRotated(60,1,0,0);
					glutSolidCone(t*0.8,t*4,4,4);
					glRotated(60,1,0,0);
					glutSolidCone(t*0.8,t*4,4,4);
					glRotated(-120,1,0,0);//retornar angulo
				}
				//Retornar posición inicial
				glTranslated(-t*4*cos(i*M_PI/180),-t*4*sin(i*M_PI/180),0);
			}
			glTranslated(t*8,0,0);
			for(GLint i=180;i<=360;i+=10){
				glTranslated(t*4*cos(i*M_PI/180),t*4*sin(i*M_PI/180),0);
				glutSolidSphere(t*0.8,5,8);
				
				if(i==220){
					glRotated(60,1,1,0);
					glutSolidCone(t*0.8,t*4,4,4);
					glRotated(60,1,1,0);
					glutSolidCone(t*0.8,t*4,4,4);
					glRotated(-120,1,1,0);
				}
				
				glTranslated(-t*4*cos(i*M_PI/180),-t*4*sin(i*M_PI/180),0);
			}
			glTranslated(t*4,0,0);
			glRotated(-90,1,0,0);
			glutSolidCone(t,t*4,4,4);
			break;
		case 3://Robot
			glRotated(90,0,1,0);
			glutSolidCube(t);//Centro de rotacion
			glTranslated(0,t/2,0);
			glutSolidCube(t);
			glTranslated(0,0,t/4);
			glutSolidCube(t*0.8);
			glTranslated(0,0,-t/4);
			//Brazo izquierdo
			glRotated(90,0,1,0);
			glTranslated(0,0,t/2);
			t=t/5;
			glTranslated(0,0,t/2);
			glutSolidTorus(t/2,t,4,50);
			glTranslated(0,0,t);
			glutSolidTorus(t/2,t,4,50);
			glTranslated(0,0,t);
			glutSolidTorus(t/2,t,4,50);
			glTranslated(0,0,t);
			glutSolidTorus(t/2,t,4,50);
			//Dedos
			t=t/2;
			glTranslated(t*3/2,t*3/2,t*3/2);
			glutSolidSphere(t,4,4);
			glTranslated(-t*3,0,0);
			glutSolidSphere(t,4,4);
			glTranslated(0,-t*3,0);
			glutSolidSphere(t,4,4);
			glTranslated(t*3,0,0);
			glutSolidSphere(t,4,4);
			//Brazo derecho
			glTranslated(-t*3/2,t*3/2,-t*3/2);
			t=t*2;
			glTranslated(0,0,-t*9);
			glutSolidTorus(t/2,t,4,50);
			glTranslated(0,0,-t);
			glutSolidTorus(t/2,t,4,50);
			glTranslated(0,0,-t);
			glutSolidTorus(t/2,t,4,50);
			glTranslated(0,0,-t);
			glutSolidTorus(t/2,t,4,50);
			//Dedos
			t=t/2;
			glTranslated(t*3/2,t*3/2,-t*3/2);
			glutSolidSphere(t,4,4);
			glTranslated(-t*3,0,0);
			glutSolidSphere(t,4,4);
			glTranslated(0,-t*3,0);
			glutSolidSphere(t,4,4);
			glTranslated(t*3,0,0);
			glutSolidSphere(t,4,4);
			//
			glTranslated(-t*3/2,t*3/2,t*3/2);
			t=t*2;
			glTranslated(0,0,t*6);
			glRotated(-90,0,1,0);
			t=t*5;
			glTranslated(0,-t/2,0);
			glutSolidCube(t);
			//Pierna izquierda
			glTranslated(t/4,-t/2,0);
			t=t*1/3;
			glTranslated(0,-t/2,0);
			glutSolidCube(t);
			glTranslated(0,-t,0);
			glutSolidCube(t);
			//Pie
			glTranslated(0,-t,0);
			t=t*4/3;
			glutSolidCube(t);
			glTranslated(0,0,t/2);
			glutSolidCube(t);
			glTranslated(0,0,-t);
			glutSolidCube(t);
			//Pierna derecha
			glTranslated(0,0,t/2);
			t=t*3/4;
			glTranslated(-t*6/4,t*2,0);
			glutSolidCube(t);
			glTranslated(0,-t,0);
			glutSolidCube(t);
			//Pie
			glTranslated(0,-t,0);
			t=t*4/3;
			glutSolidCube(t);
			glTranslated(0,0,t/2);
			glutSolidCube(t);
			glTranslated(0,0,-t);
			glutSolidCube(t);
			//Cuello
			glTranslated(0,0,t/2);
			t=t*3/4;
			glTranslated(t*3/4,t*5/2,0);
			t=t*3;
			glTranslated(0,t/2+t*3/2-t/2,0);
			t=t/5;
			glRotated(-90,1,0,0);
			glTranslated(0,0,t*1/4);
			glutSolidTorus(t/2,t,4,50);//cuello
			//Cabeza
			glTranslated(0,0,-t*1/4);
			glRotated(90,1,0,0);
			t=t*4;
			glTranslated(0,t/2+t/10,0);
			glutSolidCube(t);
			//Cara
			glTranslated(t/4,t/4,t/2);
			t=t/8;
			glutSolidTorus(t/2,t,4,50);//ojo
			glTranslated(-t*4,0,0);
			glutSolidTorus(t/2,t,4,50);//ojo
			t=t*2;
			glTranslated(t/2,-t*2,0);
			glutSolidCube(t);//boca
			glTranslated(t,0,0);
			glutSolidCube(t);//boca
			break;
		case 2://Torre
			glutSolidCube(tam*1.25);//bloque medio
			glTranslated(0,tam,0);
			glutSolidCube(tam);//bloque superior
			glTranslated(tam/3,tam*2/3,tam/3);
			glutSolidCube(tam/3);//torrecillas
			glTranslated(0,0,-tam*2/3);
			glutSolidCube(tam/3);//torrecillas
			glTranslated(-tam*2/3,0,0);
			glutSolidCube(tam/3);//torrecillas
			glTranslated(0,0,tam*2/3);
			glutSolidCube(tam/3);//torrecillas
			glTranslated(tam*1/3,-tam*2/3,-tam*1/3);
			glTranslated(0,-2*tam,0);
			tam=tam*1.5;
			glutSolidCube(tam);//bloque inferior
			glTranslated(tam*1/2,-tam*1/3,0);
			glutSolidCube(tam*1/3);//porton
			glTranslated(0,tam*1/3,0);
			glutSolidCube(tam*1/3);//porton
			break;
		case 1://Tetera
			glutSolidTeapot(tam*1.2);
			break;
	}
	
	glLoadIdentity();
}

void var_objeto(GLfloat angX, GLfloat angY, GLfloat posX){
	posX-=245;
	glTranslated(posX,-130,0);
	glRotated(angX,1,0,0);
	glRotated(angY,0,1,0);
}

void var_imagen_ojo(GLfloat angX, GLfloat angY, GLfloat posX){
	
	angX = 180 - angX;
	angY = -angY;
	
	glTranslated(-40,-130,0);
	glRotated(angX,1,0,0);
	glRotated(angY,0,1,0);
}

void var_imagen_cerebro(GLfloat angX, GLfloat angY, GLfloat posX){
	
	glTranslated(130,90,0);
	glRotated(angX,1,0,0);
	glRotated(angY,0,1,0);
}

void venas(){
	glDisable(GL_LIGHTING);//deshabilitar iluminación

	glColor4f(1.0,0.0,0.0,0.4);	//dar color (rojo)
	glBegin(GL_LINE_STRIP);
		glVertex2i(-90,-95);				
		glVertex2i(-85,-90);				
		glVertex2i(-70,-80);			
		glVertex2i(-65,-80);			
		glVertex2i(-55,-83);			
		glVertex2i(-40,-88);	
		glVertex2i(-33,-93);			
		glVertex2i(-30,-100);			
		glVertex2i(-20,-110);		
		glVertex2i(-10,-130);		
		glVertex2i(5,-135);//punto de encuentro
		glVertex2i(20,-133);
		glVertex2i(35,-134);
		glVertex2i(49,-136);
		glVertex2i(60,-132);
		glVertex2i(75,-128);
		glVertex2i(89,-130);
		glVertex2i(106,-130);
		glVertex2i(120,-125);
		glVertex2i(130,-115);
		glVertex2i(145,-105);
		glVertex2i(152,-90);
		glVertex2i(154,-75);
		glVertex2i(159,-60);
		glVertex2i(159,-30);
		glVertex2i(162,-10);
		glVertex2i(165,5);
		glVertex2i(175,20);
		glVertex2i(185,22);
		glVertex2i(195,22);
	glEnd();

	glEnable(GL_LIGHTING);//habilitar iluminación
}

void arterias(){
	glDisable(GL_LIGHTING);
	
	glColor4f(0.0,0.0,1.0,0.4);	
	glBegin(GL_LINE_STRIP);
		glVertex2i(-85,-170);
		glVertex2i(-75,-172);
		glVertex2i(-68,-175);
		glVertex2i(-55,-175);	
		glVertex2i(-45,-170);		
		glVertex2i(-35,-160);	
		glVertex2i(-20,-140);		
		glVertex2i(5,-135);//punto de encuentro
		glVertex2i(20,-136);
		glVertex2i(35,-133);
		glVertex2i(49,-136);
		glVertex2i(60,-138);
		glVertex2i(75,-140);
		glVertex2i(89,-134);
		glVertex2i(106,-128);
		glVertex2i(120,-120);
		glVertex2i(133,-117);
		glVertex2i(142,-108);
		glVertex2i(155,-93);
		glVertex2i(151,-73);
		glVertex2i(155,-62);
		glVertex2i(153,-30);
		glVertex2i(158,-10);
		glVertex2i(160,5);
		glVertex2i(170,13);
		glVertex2i(185,18);
		glVertex2i(195,25);
	glEnd();

	glEnable(GL_LIGHTING);
}

void rangovision(){
	glDisable(GL_LIGHTING);//deshabilitar luz
	glColor4f(1,0.93,0.28,0.7);//amarillento
	
	//rango grande
	glBegin(GL_TRIANGLES);
			glVertex3i(-300,-25,-299);
			glVertex3i(-105,-125,-299);
			glVertex3i(-300,-225,-299);
	glEnd();
	//rango pequeño
	glBegin(GL_TRIANGLES);
			glVertex3i(-15,-90,-299);
			glVertex3i(-98,-125,-299);
			glVertex3i(-15,-160,-299);
	glEnd();
	
	glEnable(GL_LIGHTING);
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Variables
	tam=25;//Tamaño
	
	if(obj>4){obj=1;}
	else if(obj<1){obj=4;}
	
	//Elementos
	cerebro();
	rangovision();
	venas();
	arterias();
	
	glLoadIdentity();
	
	//Invocar objetos
	
	//OBJETO REAL
	var_objeto(angX,angY,posX);
	objeto(obj,tam);
	
	//Tamaño en relacion a posX
	tam = tam + tam*posX/100;
	
	//IMAGEN OJO
	var_imagen_ojo(angX,angY,posX);
	tam = tam*0.5;
	objeto(obj,tam);
	
	//IMAGEN CEREBRO
	var_imagen_cerebro(angX,angY,posX);
	tam = tam*4;
	objeto(obj,tam);
	
	glutSwapBuffers();//renderizado de imágenes 3D
}

void reshape(int ancho, int altura){
	glViewport(0,0,(GLsizei)ancho,(GLsizei)altura);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	int h=301;
	glOrtho(-h,h,-h,h,-h,h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y){
	switch(key){
		case 27:
			exit(0);break;
		case ' ':
			obj+=1;break;
		case 45:
			if(posX>0) {posX-=5;}break;
		case 43:
			if(posX<50) {posX+=5;}break;
	}
}

void keyboard_s(int key,int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:
			angX+=5;break;
		case GLUT_KEY_UP:
			angX-=5;break;
		case GLUT_KEY_RIGHT:
			angY+=5;break;
		case GLUT_KEY_LEFT:
			angY-=5;break;
	}
}

void idle(){//mantener animación continua
	display();
}

void mouse(int button,int state,int x,int y){
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state==GLUT_DOWN) obj+=1;break;
		case GLUT_RIGHT_BUTTON:
			if(state==GLUT_DOWN) obj-=1;break;
		case 3: if(posX<50) {posX+=5;}break;	
		case 4:	if(posX>0) {posX-=5;}break;				
	}
}

int main(int argc, char** argv) {
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(700,700);
	glutInitWindowPosition(50,50);
	glutCreateWindow("Proyección ocular");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboard_s);
	glutMouseFunc(mouse);
	
	glutIdleFunc(idle);			
	glutMainLoop();	
	
	return 0;
}



