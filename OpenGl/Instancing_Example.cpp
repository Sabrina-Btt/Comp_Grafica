#include <GL/glew.h>
#include <GL/freeglut.h>
#include "GLutils.h"
#include "MeshTexture.h"
#include "MeshBuffers.h"
#include "obj_buffers.h"


struct Mesh{
	MeshBuffers buffers;
	MeshTexture texture;
	mat4 Model;
	bool offset_test;
	GLBuffer vbo_offset;


	void init(int N){
		std::vector<vec3> offset(N*N);
		for(int i = 0; i < N; i++)
			for(int j = 0; j < N; j++){
                offset[i*N + j] = {
                    rand()%600/200.0f,
                    rand()%600/200.0f,
                };
			}

		vbo_offset = GLBuffer{GL_ARRAY_BUFFER};
		vbo_offset.data(offset, GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribDivisor(2, 1);

	}

};

ShaderProgram shaderProgram;
Mesh meshes[2];

// Instances number
int N = 20;

// Rotation matrix
mat4 R = loadIdentity();
int last_x, last_y;

void init(){
	glewInit();

	glEnable(GL_DEPTH_TEST);

	shaderProgram = ShaderProgram{
		Shader{"instancing.vert", GL_VERTEX_SHADER},
		Shader{"TextureShader.frag", GL_FRAGMENT_SHADER}
	};

	glUseProgram(shaderProgram);

    meshes [0] = {
        obj_buffers("modelos/wall.obj"),
		"grama.jpg", scale(1,1,1)*rotate_x(1.57)*translate(20,-0.9,-20), false

	};

	meshes [1] = {
		obj_buffers("modelos/Trees/pine_tree.obj"),
		"modelos/Trees/pine_tree.png", scale(1,1,1)*rotate_x(1.57), true
	};

	meshes[1].init(N);


}

void desenha(){
	glClearColor(0.44, 0.76, 0.96, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	float aspect = (float)w/(float)h;

	glUseProgram(shaderProgram);
	Uniform{"Projection"} = scale(1,1,-1)*perspective(50, aspect, 1, 50);
	Uniform{"View"} = lookAt({0, 0, 20}, {0, 0, 0}, {0, 1, 0});

	for(Mesh& mesh : meshes){
        Uniform{"S"} = mesh.Model;
        Uniform{"R"} = R;
        Uniform{"N"} = N;
        Uniform{"offset_test"} = mesh.offset_test;
        mesh.texture.bind();
        mesh.buffers.draw();
        glDrawElementsInstanced(GL_TRIANGLES, mesh.buffers.n_verts,GL_UNSIGNED_INT, 0, N*N);

    }
    glutSwapBuffers();

}

void mouse(int button, int state, int x, int y){
	last_x = x;
	last_y = y;
}

void mouseMotion(int x, int y){
	int dx = x - last_x;
	int dy = y - last_y;

	R = rotate_y(dx*0.01)*rotate_x(dy*0.01)*R;

	last_x = x;
	last_y = y;
	glutPostRedisplay();
}

int main(int argc, char* argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("janela");
	glutDisplayFunc(desenha);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);

	printf("GL Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	init();
	glutMainLoop();
}
