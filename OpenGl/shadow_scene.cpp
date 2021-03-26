#include <GL/glew.h>
#include <GL/freeglut.h>
#include "GLutils.h"
#include "MeshBuffers.h"
#include "MeshTexture.h"
#include "obj_buffers.h"
#include "ObjHalfEdgeMesh.h"
#include "sphere_mesh.h"

std::vector<unsigned int> getTrianglesAdjacency(const ObjHalfEdgeMesh& mesh){
	ObjHalfEdgeMesh m = mesh;
	m.triangulate_faces();
	std::vector<unsigned int> R;
	for(FaceIndex f = 0; f < m.n_faces(); f++){
		std::vector<HEdgeIndex> hedges = m.getHEdges(f);
		unsigned int v[3] = {
			(unsigned int)m.origin(hedges[0]),
			(unsigned int)m.origin(hedges[1]),
			(unsigned int)m.origin(hedges[2])
		};

		HEdgeIndex n[3] = {
			m.prev(m.oposite(hedges[0])),
			m.prev(m.oposite(hedges[1])),
			m.prev(m.oposite(hedges[2]))
		};

		unsigned int u[3] = {
			n[0] != NOHEDGE? m.origin(n[0]): v[2],
			n[1] != NOHEDGE? m.origin(n[1]): v[0],
			n[2] != NOHEDGE? m.origin(n[2]): v[1]
		};

		R.insert(R.end(), {v[0], u[0], v[1], u[1], v[2], u[2]});
	}
	return R;
}

struct ShadowVolume{
	VAO vao;
	GLBuffer vbo;
	GLBuffer ebo;
	int n_verts;

	void init(){
		vao = VAO{true};
		vbo = GLBuffer{GL_ARRAY_BUFFER};
		ebo = GLBuffer{GL_ELEMENT_ARRAY_BUFFER};
	}

	void draw(){
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES_ADJACENCY, n_verts, GL_UNSIGNED_INT, 0);
	}

	void update(const ObjHalfEdgeMesh& mesh){
		glBindVertexArray(vao);

		vbo.data(mesh.getPositions(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		auto E = getTrianglesAdjacency(mesh);
		ebo.data(E, GL_DYNAMIC_DRAW);
		n_verts = E.size();
	}
};

struct Material{
	vec4 specular;
	float shininess;
};

struct Mesh{
	ObjHalfEdgeMesh mesh;
	MeshTexture texture;
	mat4 Model;
	Material mat;

	VAO vao;
	GLBuffer vbo;
	int n_verts;

	ShadowVolume shadow_volume;

	void init(std::string obj, std::string tex, mat4 Model, Material mat){
		mesh = ObjMesh{obj.c_str()};
		texture = tex.c_str();

		vao = VAO{true};
		vbo = GLBuffer{GL_ARRAY_BUFFER};
		update_buffers();

		shadow_volume.init();
		shadow_volume.update(mesh);

		this->Model = Model;
		this->mat = mat;
	}

	void update_buffers(){
		glBindVertexArray(vao);

		auto T = mesh.getTriangles();
		vbo.data(T, GL_DYNAMIC_DRAW);
		n_verts = 3*T.size();

		size_t stride = sizeof(ObjMesh::Vertex);
		size_t offset_position = offsetof(ObjMesh::Vertex, position);
		size_t offset_texCoords = offsetof(ObjMesh::Vertex, texCoords);
		size_t offset_normal = offsetof(ObjMesh::Vertex, normal);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset_position);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset_texCoords);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset_normal);
	}

	void draw(){
		Uniform{"mat_specular"}  = mat.specular;
		Uniform{"mat_shininess"} = mat.shininess;
		Uniform{"Model"} = Model;
		texture.bind();
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, n_verts);
	}

	void draw_shadow_volume(){
		Uniform{"Model"} = Model;
		shadow_volume.draw();
	}

};

Mesh meshes[8];
MeshBuffers light_sphere;

ShaderProgram shaderProgram;
ShaderProgram shadowVolumeShader;
ShaderProgram lightShaderProgram;

bool wireframe = false;

float s = 1.0;
mat4 BaseView = lookAt({0, 10, 20}, {0, 7, 0}, {0, 1, 0});
float vangle = 0;

vec4 light_position = {0, 15, 15, 1};

void init_shader(){
	shaderProgram = ShaderProgram{
		Shader{"PhongShaderTex.vert", GL_VERTEX_SHADER},
		Shader{"PhongShaderTex.frag", GL_FRAGMENT_SHADER}
	};
	shadowVolumeShader = ShaderProgram{
		Shader{"shadow_volume.vert", GL_VERTEX_SHADER},
		Shader{"shadow_volume.geom", GL_GEOMETRY_SHADER},
		Shader{"shadow_volume.frag", GL_FRAGMENT_SHADER}
	};
	lightShaderProgram = ShaderProgram{
		Shader{"SimpleShader.vert", GL_VERTEX_SHADER},
		Shader{"SimpleShader.frag", GL_FRAGMENT_SHADER},
	};
}

void init_scene(){

	meshes[0].init("house/Cabin.obj", "house/Cabin Texture.png",
	              translate(-3, 3, 6)*rotate_y(1)*scale(.0075, .0075, .0075),
	              Material{{1.0, 1.0, 1.0, 1.0}, 40.0} );

	meshes[2].init("modelos/wall.obj", "grama.jpg",
                  scale(20, 20, 20),
                  Material{{1.0, 1.0, 1.0, 1.0}, 40.0});

	meshes[3].init("animals/Horse.obj", "animals/Horse.png",
	              translate(7,0,5)*rotate_y(-1)*scale(0.15, 0.15, 0.15),
	              Material{{1.0, 1.0, 1.0, 1.0}, 40.0} );

	meshes[4].init("modelos/Trees/forest.obj", "modelos/Trees/forest.png",
	              translate(5, 2, -5)*scale(8, 8, 8),
	              Material{{1.0, 1.0, 1.0, 1.0}, 40.0} );

	meshes[5].init("modelos/Pond/pond.obj", "modelos/Pond/pond.png",
	              translate(6, 0.5, 10)*scale(2.5, 2.5, 2.5),
	              Material{{1.0, 1.0, 1.0, 1.0}, 40.0} );

    meshes[6].init("animals/Horse.obj", "animals/Horse.png",
	              translate(3,0,15)*rotate_y(1)*scale(0.15, 0.15, 0.15),
	              Material{{1.0, 1.0, 1.0, 1.0}, 40.0} );

	light_sphere = sphere_mesh(20, 20);
}

void init(){
	glewInit();
	glEnable(GL_DEPTH_TEST);

	init_shader();
	init_scene();
}

void draw_scene(){
	for(Mesh& m: meshes)
		m.draw();
}

void draw_shadow_volumes(mat4 Projection, mat4 View){
	glDepthMask(GL_FALSE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glStencilMask(0xFF);
	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_CLAMP);

	glStencilFunc(GL_ALWAYS, 0, 0xFF);

	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);

	glUseProgram(shadowVolumeShader);
	Uniform{"Projection"} = Projection;
	Uniform{"View"} = View;
	Uniform{"C"} = vec3{0.4, 0, 0};
	Uniform{"light_position"} = light_position;
	Uniform{"robust"} = 1;
	Uniform{"zpass"} = 0;
	for(Mesh& m: meshes)
		m.draw_shadow_volume();

	glDepthMask(GL_TRUE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilMask(0x00);
	glStencilFunc(GL_EQUAL, 0, 0xFF);
	glDisable(GL_DEPTH_CLAMP);
}

void draw_light_sphere(mat4 Projection, mat4 View){
	glUseProgram(lightShaderProgram);

	mat4 Model = translate(light_position.x, light_position.y, light_position.z)*scale(0.5, 0.5, 0.5);

	Uniform{"M"} = Projection*View*Model;
	Uniform{"C"} = vec3{1, 1, 1};

	light_sphere.draw();
}

void draw_color_scene(mat4 Projection, mat4 View, bool ambient_only){
	glUseProgram(shaderProgram);

	Uniform{"Projection"} = Projection;
	Uniform{"View"} = View;

	glActiveTexture(GL_TEXTURE0);
	Uniform{"mat_texture"} = 0;

	Uniform{"light_position"} = light_position;
	Uniform{"light_ambient"}  = vec4{ 0.2, 0.2, 0.2, 1.0 };
	if(ambient_only){
		Uniform{"light_diffuse"}  = vec4{ 0.0, 0.0, 0.0, 0.0 };
		Uniform{"light_specular"} = vec4{ 0.0, 0.0, 0.0, 0.0 };
	}else{
		Uniform{"light_diffuse"}  = vec4{ 1.0, 1.0, 1.0, 1.0 };
		Uniform{"light_specular"} = vec4{ 1.0, 1.0, 1.0, 1.0 };
	}

	draw_scene();
}

void draw(){
	glClearColor(0.44, 0.76, 0.96, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	float a = w/(float)h;
	mat4 Projection = scale(1,1,-1)*perspective(45, a, 0.1, 150);
	mat4 View = rotate_x(vangle)*BaseView;

	glDisable(GL_STENCIL_TEST);
	draw_color_scene(Projection, View, true);

	glEnable(GL_STENCIL_TEST);
	draw_shadow_volumes(Projection, View);

	glClear(GL_DEPTH_BUFFER_BIT);
	draw_color_scene(Projection, View, false);

	draw_light_sphere(Projection, View);

	glutSwapBuffers();
}

int last_x;
int last_y;

void mouse(int button, int state, int x, int y){
	last_x = x;
	last_y = y;
}

void mouseMotion(int x, int y){
	int dx = x - last_x;
	int dy = y - last_y;

	vangle += 0.01*dy;
	vangle = clamp(vangle, -1.5, 1.5);
	BaseView = rotate_y(dx*0.01)*BaseView;

	last_x = x;
	last_y = y;

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y){
	if(key == 'x'){
		wireframe = !wireframe;
		glutPostRedisplay();
		return;
	}

	vec3 dir = {0, 0, 0};
	float zmove = 0;
	float xmove = 0;

	if(key == 'w')
		zmove += 0.2;

	if(key == 's')
		zmove -= 0.2;

	if(key == 'a')
		xmove += 0.2;

	if(key == 'd')
		xmove -= 0.2;

	BaseView = translate(xmove, 0, zmove)*BaseView;
	glutPostRedisplay();
}

void special(int c, int x, int y){
	float dif = 0.1;
	if(c == GLUT_KEY_RIGHT)
		light_position.x += dif;
	if(c == GLUT_KEY_LEFT)
		light_position.x -= dif;
	if(c == GLUT_KEY_PAGE_UP)
		light_position.y += dif;
	if(c == GLUT_KEY_PAGE_DOWN)
		light_position.y -= dif;
	if(c == GLUT_KEY_DOWN)
		light_position.z += dif;
	if(c == GLUT_KEY_UP)
		light_position.z -= dif;
	glutPostRedisplay();
}

int main(int argc, char* argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(800, 600);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("janela");
	glutDisplayFunc(draw);

	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

	printf("GL Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	init();
	glutMainLoop();
}
