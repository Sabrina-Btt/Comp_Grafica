#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>
#include <algorithm>
#include "GLutils.h"
#include "ObjMesh.h"

/***************************************************************************************/
mat4 inverse(mat4 M){
	float x = M[0][3];
	float y = M[1][3];
	float z = M[2][3];

	return translate(-x, -y, -z);
}

/***************************************************************************************/
class Pose{
	std::map<std::string, mat4> M;

	public:
	mat4 operator[](std::string key) const{
		auto it = M.find(key);
		if(it == M.cend()){
			std::cerr << "key not found: " << key << '\n';
			return loadIdentity();
		}
		return it->second;
	}

	mat4& operator[](std::string key){
		return M[key];
	}
};

/***************************************************************************************/
class Chronometer{
	using clock = std::chrono::high_resolution_clock;
	using time_point = std::chrono::time_point<clock>;

	float time = 0;
	time_point clock_time = clock::now();
	bool playing = true;

	public:
	void play_pause(){
		playing = !playing;
		clock_time = clock::now();
	}

	void stop(){
		playing = false;
		set_time(0);
	}

	float get_time(){
		if(playing){
			time_point now = clock::now();
			std::chrono::duration<float> dif = now - clock_time;

			clock_time = now;

			time += dif.count();
		}

		return time;
	}

	void set_time(float t){
		time = t;
		clock_time = clock::now();
	}

};

/***************************************************************************************/
struct Joint{
	std::string id;
	vec3 offset;
	std::vector<std::string> channels;
	std::vector<Joint> children;

	mat4 local_transform() const{
		return translate(offset.x, offset.y, offset.z);
	}
};

void print(const Joint& j){
	std::cout << j.id << ':' << j.offset.x << ' ' << j.offset.y << ' ' << j.offset.z << "{\n";
	for(const Joint& c: j.children)
		print(c);
	std::cout << "}\n";
}

/***************************************************************************************/
class BVHParser{
	std::ifstream in;
	Joint& root;
	float& frame_time;
	std::vector<Pose>& pose;

	public:
	BVHParser(std::string filename, Joint& root, float& frame_time, std::vector<Pose>& pose) :
		in{filename}, root{root}, frame_time{frame_time}, pose{pose}
	{
		bool res = read_hierarchy() && read_motion();

		if(!res)
			std::cerr << "error parsing file\n";
	}

	bool read_hierarchy(){
		return read("HIERARCHY") && read("ROOT") && read(root);
	}

	bool read_motion(){
		int n = 0;
		return read("MOTION") &&
		       read("Frames:") && read(n) &&
		       read("Frame") && read("Time:") && read(frame_time) &&
		       read_transforms(n);
	}

	bool read(const std::string exp){
		auto pos = in.tellg();
		std::string name;
		if(in >> name && name == exp)
			return true;

		in.seekg(pos);
		return false;
	}

	bool read_str(std::string& out){
		if(in && in >> out){
			for(char& c: out)
				if(c == '.' || c == ':')
					c = '_';
			return true;
		}

		return false;
	}

	bool read(int& n){
		return in && in >> n;
	}

	bool read(float& n){
		return in && in >> n;
	}

	bool read(vec3& v){
		return in && in >> v.x >> v.y >> v.z;
	}

	bool read(Joint& joint){
		return read_str(joint.id) && read("{") && read_joint_data(joint) && read("}");
	}

	bool read_joint_data(Joint& joint){
		while(
			( read("OFFSET")   && read(joint.offset)         ) ||
			( read("CHANNELS") && read_channels(joint)       ) ||
			( read("JOINT")    && read(add_child(joint))     ) ||
			( read("End")      && end_site(add_child(joint)) )
		){}

		return true;
	}

	bool read_channels(Joint& joint){
		int n = 0;
		bool res = read(n);
		joint.channels.resize(n);
		for(std::string& c: joint.channels)
			res = res && read_str(c);
		return res;
	}

	Joint& add_child(Joint& joint){
		joint.children.push_back(Joint{});
		return joint.children.back();
	}

	bool end_site(Joint& joint){
		joint.id = "end site";
		return read("Site") && read("{") && read("OFFSET") && read(joint.offset) && read("}");
	}

	bool read_transforms(int n){
		pose.resize(n);
		for(int i = 0; i < n; i++)
			add_joint_transform(loadIdentity(), root, pose[i]);
		return true;
	}

	void add_joint_transform(mat4 parent_transform, const Joint& node, Pose& T){
		if(node.id == "end site")
			return;

		vec3 t = node.offset;
		mat4 R = loadIdentity();

		for(std::string c: node.channels){
			float v;
			in >> v;
			if(c == "Xposition")
				t.x = v;
			if(c == "Yposition")
				t.y = v;
			if(c == "Zposition")
				t.z = v;
			if(c == "Xrotation")
				R = R*rotate_x(v*M_PI/180);
			if(c == "Yrotation")
				R = R*rotate_y(v*M_PI/180);
			if(c == "Zrotation")
				R = R*rotate_z(v*M_PI/180);
		}

		mat4 transform = parent_transform*translate(t.x, t.y, t.z)*R;

		T[node.id] = transform;

		for(const Joint& child: node.children)
			add_joint_transform(transform, child, T);
	}
};

/***************************************************************************************/
class BVH{
	Joint root;
	float frame_time = 1;
	std::vector<Pose> pose;
	Pose bind_pose;
	Pose inv_bind_pose;

	public:
	BVH() = default;

	BVH(std::string filename){
		BVHParser parser{filename, root, frame_time, pose};
		get_bind_pose(loadIdentity(), root);
	}

	float        get_frame_time()    const{ return frame_time;       }
	int          n_pose()            const{ return pose.size();      }
	const Joint& get_root()          const{ return root;             }
	const Pose&  get_pose(int f)     const{ return pose[f%n_pose()]; }
	const Pose&  get_bind_pose()     const{ return bind_pose;        }
	const Pose&  get_inv_bind_pose() const{ return inv_bind_pose;    }

	private:

	void get_bind_pose(mat4 parent_transform, const Joint& node){
		if(node.id == "end site")
			return;

		mat4 transform = parent_transform*node.local_transform();

		bind_pose[node.id] = transform;
		inv_bind_pose[node.id] = inverse(transform);

		for(const Joint& child: node.children)
			get_bind_pose(transform, child);
	}
};

/***************************************************************************************/
class BonesMesh{
	struct Vertex{
		vec3 position;
		int bone;
		float weight;
	};

	std::vector<std::string> bone_ids;

	VAO vao;
	GLBuffer vbo;
	int n_verts;

	public:
	BonesMesh() = default;

	BonesMesh(const BVH& bvh){
		std::vector<Vertex> V = get_bones(bvh);
		vao = VAO{true};
		glBindVertexArray(vao);
		vbo = GLBuffer{GL_ARRAY_BUFFER};

		vbo.data(V, GL_STATIC_DRAW);

		size_t stride = sizeof(Vertex);

		size_t offset_position = offsetof(Vertex, position);
		size_t offset_bone     = offsetof(Vertex, bone);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)offset_position);

		glEnableVertexAttribArray(1);
		glVertexAttribIPointer(1, 1, GL_INT, stride, (void *)offset_bone);

		n_verts = V.size();
	}

	void draw(const Pose& pose) const{
		int i = 0;
		for(std::string id: bone_ids){
			Uniform{"M[" + std::to_string(i) + "]"} = pose[id];
			i++;
		}

		glBindVertexArray(vao);
		glDrawArrays(GL_LINES, 0, n_verts);
	}

	private:
	std::vector<Vertex> get_bones(const BVH &bvh){
		std::vector<Vertex> bones;
		get_bones(bvh.get_root(), bones);
		return bones;
	}

	void get_bones(const Joint& node, std::vector<Vertex>& bones){
		if(node.id == "end site")
			return;

		if(!node.channels.empty()){
			int k = bone_ids.size();

			bones.push_back({vec3{0, 0, 0}, k});
			bones.push_back({node.children[0].offset, k});

			bone_ids.push_back(node.id);
		}

		for (const Joint &child : node.children)
			if (child.id != "end site")
				get_bones(child, bones);
	}
};

/***************************************************************************************/
#define NWEIGHTS 8
struct WeightVertex{
	int bone[NWEIGHTS];
	float weight[NWEIGHTS];
	int nweights = 0;
};

struct WeightInfo{
	std::vector<std::string> joints;
	std::vector<WeightVertex> vertices;

	WeightInfo(std::string filename){
		std::ifstream in{filename};
		int n = 0;
		in >> n;
		joints.resize(n);
		for(std::string& name: joints)
			in >> name;

		in >> n;
		std::vector<float> weights(n);
		for(float& w: weights)
			in >> w;

		in >> n;
		vertices.resize(n);
		for(WeightVertex& v: vertices)
			in >> v.nweights;

		struct BoneWeight{
			int bone;
			float weight;
		};

		for(WeightVertex& v: vertices){
			std::vector<BoneWeight> bws(v.nweights);
			for(BoneWeight& bw: bws){
				int b, w;
				in >> b >> w;
				bw = {b, weights[w]};
			}

			int n = std::min(v.nweights, NWEIGHTS);
			for(int i = 0; i < n; i++){
				auto it = std::max_element(bws.begin(), bws.end(), [](BoneWeight a, BoneWeight b){ return a.weight < b.weight; });
				v.bone[i] = it->bone;
				v.weight[i] = it->weight;
				bws.erase(it);
			}
			if(v.nweights >= NWEIGHTS)
				v.nweights = NWEIGHTS;

			float s = 0;
			for(int i = 0; i < v.nweights; i++)
				s += v.weight[i];

			for(int i = 0; i < v.nweights; i++)
				v.weight[i] /= s;
		}

	}

	std::vector<mat4> getTransforms(const Pose& M) const{
		std::vector<mat4> T;

		for(const std::string& name: joints)
			T.push_back(M[name]);

		return T;
	}

};

/***************************************************************************************/
class SkinnedMesh{
	struct Vertex{
		ObjMesh::Vertex obj_data;
		WeightVertex weight_data;
	};

	WeightInfo weight_info;

	VAO vao;
	GLBuffer vbo;
	int n_verts;

	public:
	SkinnedMesh(const ObjMesh& obj_mesh, const WeightInfo& _weight_info) : weight_info{_weight_info}{
		std::vector<Vertex> vertices = getTriangles(obj_mesh, weight_info.vertices);
		if(vertices.empty()){
			return;
		}

		vao = VAO{true};
		glBindVertexArray(vao);
		vbo = GLBuffer{GL_ARRAY_BUFFER};
		vbo.data(vertices, GL_STATIC_DRAW);

		size_t stride = sizeof(Vertex);

		size_t offset_obj_data = offsetof(Vertex, obj_data);
		size_t offset_weights_data = offsetof(Vertex, weight_data);

		size_t offset_position  = offset_obj_data + offsetof(ObjMesh::Vertex, position);
		size_t offset_texCoords = offset_obj_data + offsetof(ObjMesh::Vertex, texCoords);
		size_t offset_normal    = offset_obj_data + offsetof(ObjMesh::Vertex, normal);

		size_t offset_nweights  = offset_weights_data + offsetof(WeightVertex, nweights);
		size_t offset_bone1     = offset_weights_data + offsetof(WeightVertex, bone);
		size_t offset_weight1   = offset_weights_data + offsetof(WeightVertex, weight);
		size_t offset_bone2     = offset_bone1 + 4*sizeof(int);
		size_t offset_weight2   = offset_weight1 + 4*sizeof(float);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset_position);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset_texCoords);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset_normal);

		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(3, 1, GL_INT, stride, (void*)offset_nweights);

		glEnableVertexAttribArray(4);
		glVertexAttribIPointer(4, 4, GL_INT, stride, (void*)offset_bone1);

		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, stride, (void*)offset_weight1);

		glEnableVertexAttribArray(6);
		glVertexAttribIPointer(6, 4, GL_INT, stride, (void*)offset_bone2);

		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, stride, (void*)offset_weight2);

		n_verts = vertices.size();
	}

	void draw(const Pose& pose, const Pose& inv_bind_pose) const{
		int i = 0;
		for(std::string name: weight_info.joints){
			Uniform{"M[" + std::to_string(i) + "]"} = pose[name]*inv_bind_pose[name];
			i++;
		}

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, n_verts);
	}

	private:
	std::vector<Vertex> getTriangles(const ObjMesh& mesh, const std::vector<WeightVertex>& vert_weights) const{
		std::vector<Vertex> tris;

		if(vert_weights.empty()){
			return tris;
		}

		for(ObjMesh::Face face: mesh.faces){
			if(face.verts.empty())
				continue;

			ObjMesh::VertIndices i0 = face.verts[0];
			Vertex v0 = {mesh.getVertex(i0), vert_weights[i0.pos-1]};

			for(int i = 1; i+1 < face.verts.size(); i++){
				ObjMesh::VertIndices i1 = face.verts[i];
				ObjMesh::VertIndices i2 = face.verts[i+1];
				Vertex v1 = {mesh.getVertex(i1), vert_weights[i1.pos-1]};
				Vertex v2 = {mesh.getVertex(i2), vert_weights[i2.pos-1]};
				tris.push_back(v0);
				tris.push_back(v1);
				tris.push_back(v2);
			}
		}

		return tris;
	}
};

/***************************************************************************************/
bool show_animation = true;
bool view_skin = true;
bool view_bones = false;

Chronometer chronometer;

ShaderProgram bones_shader;
ShaderProgram skin_shader;

BVH bvh;
BonesMesh bones_mesh;
std::vector<SkinnedMesh> skinned_meshes;

std::vector<std::string> meshes;
std::vector<std::string> movements;

mat4 BaseView = lookAt({0, 10, 40}, {0, 7, 0}, {0, 1, 0});
float vangle = 0;
float s = 0.1;

void read_skins(std::string base){
	std::ifstream in{base + "_meshes.txt"};
	skinned_meshes.clear();

	std::string name;
	while(in >> name){
		std::string mesh = base + "/" + name;
		std::string obj_file =  mesh + ".obj";
		std::string weights_file =  mesh + "_weights.txt";
		skinned_meshes.emplace_back(
			ObjMesh{obj_file.c_str()},
			WeightInfo{weights_file}
		);
	}
}

int selected_mesh = 0;

void init(){
	glewInit();
	glEnable(GL_DEPTH_TEST);

	bones_shader = ShaderProgram{
		Shader{"BoneShader1.vert", GL_VERTEX_SHADER},
		Shader{"SimpleShader.frag", GL_FRAGMENT_SHADER}
	};

	skin_shader = ShaderProgram{
		Shader{"SkinShader.vert", GL_VERTEX_SHADER},
		Shader{"SimpleShader.frag", GL_FRAGMENT_SHADER}
	};

	movements = {"urso/meu_urso.bvh"};
	meshes = {"urso/meu_urso"};

	selected_mesh = 0;
	bvh = BVH{ movements[0] };
	read_skins( meshes[0] );

	bones_mesh = BonesMesh{bvh};
}

void draw_bones(mat4 Projection, mat4 View, mat4 Model, const Pose& pose){
	glUseProgram(bones_shader);
	Uniform{"C"} = vec3{0, 0, 0};
	Uniform{"Projection"} = Projection;
	Uniform{"View"} = View;
	Uniform{"Model"} = Model;

	bones_mesh.draw(pose);
}

void draw_skin(mat4 Projection, mat4 View, mat4 Model, const Pose& pose){
	glUseProgram(skin_shader);
	Uniform{"C"} = vec3{0.7, 0.7, 0.7};
	Uniform{"Projection"} = Projection;
	Uniform{"View"} = View;
	Uniform{"Model"} = Model;

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1,1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for(const SkinnedMesh& skin: skinned_meshes)
		skin.draw(pose, bvh.get_inv_bind_pose());
	glDisable(GL_POLYGON_OFFSET_FILL);

	Uniform{"C"} = vec3{0, 0, 1};

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for(const SkinnedMesh& skin: skinned_meshes)
		skin.draw(pose, bvh.get_inv_bind_pose());
}

const Pose& getPose(){
	if(show_animation && bvh.n_pose() == 0)
		show_animation = false;

	if(show_animation){
		float t = chronometer.get_time();
		int f = floor(t/bvh.get_frame_time());
		return bvh.get_pose(f);
	}

	return bvh.get_bind_pose();
}

void draw(){
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	float a = w/(float)h;
	mat4 Projection = scale(1,1,-1)*perspective(50, a, 0.1, 500);
	mat4 View = rotate_x(vangle)*BaseView;
	mat4 Model = scale(s, s, s);

	const Pose& Pose = getPose();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	if(view_bones)
		draw_bones(Projection, View, Model, Pose);

	if(view_skin)
		draw_skin(Projection, View, Model, Pose);

	glutSwapBuffers();
}

void idle(){
	glutPostRedisplay();
}

void keyboard(unsigned char c, int, int){
	if(c == ' ')
		chronometer.play_pause();

	if(c == 'x' || c == 'a')
		chronometer.stop();

	if(c == 'a')
		show_animation = !show_animation;

	if(c == 'b')
		view_bones = !view_bones;

	if(c == 's')
		view_skin = !view_skin;

	if(c == '1' || c == '2' || c == '3'){
		if(selected_mesh > 2)
			bvh = BVH{ movements[0] };

		selected_mesh = c - '1';
		read_skins(meshes[selected_mesh]);
	}

	if(selected_mesh <= 2){
		if(c == '!')
			bvh = BVH{movements[0]};
		if(c == '@')
			bvh = BVH{movements[1]};
		if(c == '#')
			bvh = BVH{movements[2]};
	}

	/*if(c == '4'){
		selected_mesh = 4;
		bvh = BVH{ "modelos/Old Man Idle/oldmanidle.bvh" };
		read_skins("modelos/Old Man Idle/Old Man Idle" );
	}

	if(c == '5'){
		selected_mesh = 5;
		bvh = BVH{ "modelos/Jumping Down/mouse.bvh" };
		read_skins("modelos/Jumping Down/mouse" );
	}*/
}

int last_x;
int last_y;

void mouse(int button, int, int x, int y){
	last_x = x;
	last_y = y;

	if (button == 3)
		s *= 1.1;
	else if(button == 4)
		s /= 1.1;
}

void mouseMotion(int x, int y){
	int dx = x - last_x;
	int dy = y - last_y;

	vangle += 0.01*dy;
	vangle = clamp(vangle, -1.5, 1.5);
	BaseView = rotate_y(dx*0.01)*BaseView;

	last_x = x;
	last_y = y;
}

void special(int key, int x, int y){
	vec3 dir = {0, 0, 0};
	float zmove = 0;
	float xmove = 0;

	if(key == GLUT_KEY_UP)
		zmove += 0.2;

	if(key == GLUT_KEY_DOWN)
		zmove -= 0.2;

	if(key == GLUT_KEY_LEFT)
		xmove += 0.2;

	if(key == GLUT_KEY_RIGHT)
		xmove -= 0.2;

	BaseView = translate(xmove, 0, zmove)*BaseView;
}

int main(int argc, char* argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE | GLUT_DEPTH);
	glutInitWindowPosition(800, 600);
	glutInitWindowSize(800, 600);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("janela");
	glutDisplayFunc(draw);

	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutIdleFunc(idle);
	glutSpecialFunc(special);
	glutKeyboardFunc(keyboard);

	printf("GL Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	init();
	glutMainLoop();
}
