/** Perspective view of a color cube using LookAt() and Perspective()

** Colors are assigned to each vertex and then the rasterizer interpolates
those colors across the triangles.
**************************************************************/
#include "Angel-yjc.h"
#include <fstream>
#include <string>
#include <sstream>


typedef Angel::vec3  color3;
typedef Angel::vec3  point3;
typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

/*	Create checkerboard texture	*/

#define ImageWidth  32
#define ImageHeight 32
GLubyte Image[ImageHeight][ImageWidth][4];

#define	stripeImageWidth 32
GLubyte stripeImage[4 * stripeImageWidth];
GLuint tex[2];

mat3 normal_matrix;

vec3 rv = point3(-8, 0, -3), tv = point3(3 / sqrt(73), 0, -8 / sqrt(73));
GLfloat a = -4, b = 1, c = 4;

vec4 light_position_eyeFrame;
vec4 spot_point_eyeFrame;

// Declaring the accumulating rotation matrix and it's supporting rotation matrices for updating it
mat4 M = mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(0, 0, 0, 1));
mat4 M1 = mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(0, 0, 0, 1));
mat4 M2 = mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(0, 0, 0, 1));
mat4 M3 = mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(0, 0, 0, 1));
mat4 N = mat4(vec4(12, 14, 0, 0), vec4(0, 0, 0, 0), vec4(0, 3, 12, 0), vec4(0, -1, 0, 12));
int t, m;

int discardcount = 0;
float realet = 0, tstart = 0, tsub = 0, et = 0;
GLint l = 0; // l 1 indicates lighting is enabled else no
GLint shading = 1, light_source = 0; // light_source 1 indicates positional light source else directional/distant
// shading 0 indicates smooth shading else flat
GLint shadow = 1;  // 1 indicates shadow
GLint source = 2; //S 0 indicates point source 1 spot light  2 directional light
GLint fog = 0; // 0 is no fog, 1 is linear, 2 is exponential, 3 is exponential square


GLuint Angel::InitShader(const char* vShaderFile, const char* fShaderFile);
void loadFile(std::string name, point3* sPoints, point3* sColor, point3* shColor);
void makeNormal(void);

GLuint program;       /* shader program object id */
GLuint fire_program;
GLuint vPosition;
GLuint vColor;
GLuint vNormal;
GLuint vTexCoord;
GLuint vVelocity;

GLuint sphere_buffer;   /* vertex buffer object id for sphere */
GLuint floor_buffer;  /* vertex buffer object id for floor */
GLuint xline_buffer;   /* vertex buffer object id for x axis */
GLuint yline_buffer;   /* vertex buffer object id for y axis */
GLuint zline_buffer; /* vertex buffer object id for z axis */
GLuint shadow_buffer;
GLuint flat_sphere_buffer;
GLuint smooth_sphere_buffer;
GLuint shading_floor_buffer;
GLuint fire_buffer;


// Projection transformation parameters
GLfloat  fovy = 45.0;  // Field-of-view in Y direction angle (in degrees)
GLfloat  aspect;       // Viewport aspect ratio
GLfloat  zNear = 0.2, zFar = 10.0;

GLfloat angle = 0.0; // rotation angle in degrees
vec4 init_eye(7.0, 7.0, -10.0, 1.0); // initial viewer position
vec4 eye = init_eye;               // current viewer position

int animationFlag = 0; // 1: animation; 0: non-animation. Toggled by key 'a' or 'A'
int start = 0;

int cubeFlag = 0;   // 1: solid cube; 0: wireframe cube. Toggled by key 'c' or 'C'
int floorFlag = 1; // 1: solid floor; 0: wireframe floor. Toggled by key 'f' or 'F'
int xlineFlag = 1;
int ylineFlag = 1;
int zlineFlag = 1;

int ftexflag = 0;  //0 no tex on floor, 1 tex on floor
int stexflag = 0;  //0 No, 1 Contour Lines, 2 Checkerboard
int blend = 0;     //0 no blending, 1 blend shadow
int drawsphereflag = 0;
int drawfloorflag = 0;
int drawshadowflag = 0;
int lattice_flag = 0, lattice_flagtype = 0;
int fire_flag = 0;
int leftclickflag = 0;
int space = 0; // frame for textures
int align = 0; // alignment vertical or slanted

int sphere_NumVertices;
#if 0
point3 cube_points[cube_NumVertices]; // positions for all vertices
color3 cube_colors[cube_NumVertices]; // colors for all vertices
#endif
#if 1
point3 sphere_points[3100];
color3 sphere_colors[3100];
color3 shadow_colors[3100];
vec3 flat_sphere_normals[3100];
vec3 smooth_sphere_normals[3100];
#endif



std::string fname;  //for the filename

const int floor_NumVertices = 6; //(1 face)*(2 triangles/face)*(3 vertices/triangle)
point3 floor_points[floor_NumVertices]; // positions for all vertices
color3 floor_colors[floor_NumVertices]; // colors for all vertices
vec3 floor_normals[floor_NumVertices];
vec2 floor_texcoord[floor_NumVertices];

const int fire_NumVertices = 300;
point3 fire_points[fire_NumVertices];
color3 fire_colors[fire_NumVertices];
vec3 fire_velocity[fire_NumVertices];



const int xline_NumVertices = 6; //(1 face)*(2 triangles/face)*(3 vertices/triangle)
point3 xline_points[xline_NumVertices]; // positions for all vertices
color3 xline_colors[xline_NumVertices]; // colors for all vertices

const int yline_NumVertices = 6; //(1 face)*(2 triangles/face)*(3 vertices/triangle)
point3 yline_points[yline_NumVertices]; // positions for all vertices
color3 yline_colors[yline_NumVertices]; // colors for all vertices

const int zline_NumVertices = 6; //(1 face)*(2 triangles/face)*(3 vertices/triangle)
point3 zline_points[zline_NumVertices]; // positions for all vertices
color3 zline_colors[zline_NumVertices]; // colors for all vertices

/*---- shader lighting parameters -----*/

color4 fog_color(0.7, 0.7, 0.7, 0.5);

color4 global_ambient(1.0, 1.0, 1.0, 1.0);

point4 pos_light_position(-14.0, 12.0, -3.0, 1.0);
point4 spot_point(-6.0, 0.0, -4.5, 1.0);


color4 pos_light_ambient(0.0, 0.0, 0.0, 1.0);
color4 pos_light_diffuse(1.0, 1.0, 1.0, 1.0);
color4 pos_light_specular(1.0, 1.0, 1.0, 1.0);

color4 light_ambient(0.0, 0.0, 0.0, 1.0);
color4 light_diffuse(0.8, 0.8, 0.8, 1.0);
color4 light_specular(0.2, 0.2, 0.2, 1.0);

color4 sphere_ambient( 0.2, 0.2, 0.2, 1.0);
color4 sphere_diffuse( 1.0, 0.84, 0.0, 1.0);
color4 sphere_specular( 1.0, 0.84, 0.0, 1.0);
float sphere_shininess = 125.0;

color4 floor_ambient( 0.2, 0.2, 0.2, 1.0);
color4 floor_diffuse( 0.0, 1.0, 0.0, 1.0);
color4 floor_specular( 0.0, 0.0, 0.0, 1.0);
float floor_shininess = 125.0;

float cutoff = 20.0 * (M_PI / 180.0);
float alpha = 15;

vec4 distant_light_position(-0.1, 0.0, 1.0, 0.0);  // in case of directional light

color4 global_ambient_sproduct = global_ambient * sphere_ambient;
color4 global_ambient_fproduct = global_ambient * floor_ambient;

color4 sphere_ambient_product = light_ambient * sphere_ambient;
color4 sphere_diffuse_product = light_diffuse * sphere_diffuse;
color4 sphere_specular_product = light_specular * sphere_specular;

color4 floor_ambient_product = light_ambient * floor_ambient;
color4 floor_diffuse_product = light_diffuse * floor_diffuse;
color4 floor_specular_product = light_specular * floor_specular;

color4 pos_sphere_ambient_product = pos_light_ambient * sphere_ambient;
color4 pos_sphere_diffuse_product = pos_light_diffuse * sphere_diffuse;
color4 pos_sphere_specular_product = pos_light_specular * sphere_specular;

color4 pos_floor_ambient_product = pos_light_ambient * floor_ambient;
color4 pos_floor_diffuse_product = pos_light_diffuse * floor_diffuse;
color4 pos_floor_specular_product = pos_light_specular * floor_specular;

float const_att = 2.0;
float linear_att = 0.01;
float quad_att = 0.001;

void SetUp_Lighting_Uniform_Vars(mat4 mv, int m, int s);






										//-------------------------------
										// generate 2 triangles: 6 vertices and 6 colors
void floor()
{	
	

	floor_points[0] = point3(-5, 0, 8);  floor_colors[0] = color3(0, 1, 0);
	floor_points[1] = point3(5, 0, 8); floor_colors[1] = color3(0, 1, 0);
	floor_points[2] = point3(5, 0, -4);  floor_colors[2] = color3(0, 1, 0);

	floor_points[3] = point3(-5, 0, 8); floor_colors[3] = color3(0, 1, 0);
	floor_points[4] = point3(5, 0, -4); floor_colors[4] = color3(0, 1, 0);
	floor_points[5] = point3(-5, 0, -4); floor_colors[5] = color3(0, 1, 0);

	vec3 a = floor_points[1] - floor_points[0];
	vec3 b = floor_points[5] - floor_points[0];
	vec3 normal = normalize(cross(a, b));

	floor_normals[0] = normal;
	floor_normals[1] = normal;
	floor_normals[2] = normal;
	floor_normals[3] = normal;
	floor_normals[4] = normal;
	floor_normals[5] = normal;

	floor_texcoord[0] = vec2(0.0, 0.0);
	floor_texcoord[1] = vec2(6.0, 0.0);
	floor_texcoord[2] = vec2(6.0, 5.0);

	floor_texcoord[3] = vec2(0.0, 0.0);
	floor_texcoord[4] = vec2(6.0, 5.0);
	floor_texcoord[5] = vec2(0.0, 5.0);
}

void xline()
{
	xline_points[0] = point3(10, 0, 0.05); xline_colors[0] = color3(1, 0, 0);
	xline_points[1] = point3(-10, 0, 0.05); xline_colors[1] = color3(1, 0, 0);
	xline_points[2] = point3(-10, 0, -0.05); xline_colors[2] = color3(1, 0, 0);

	xline_points[3] = point3(-10, 0, -0.05); xline_colors[3] = color3(1, 0, 0);
	xline_points[4] = point3(10, 0, -0.05); xline_colors[4] = color3(1, 0, 0);
	xline_points[5] = point3(10, 0, 0.05); xline_colors[5] = color3(1, 0, 0);
}

void yline()
{
	yline_points[0] = point3(0.05, 10, 0);   yline_colors[0] = color3(1, 0, 1);
	yline_points[1] = point3(0.05, -10, 0);  yline_colors[1] = color3(1, 0, 1);
	yline_points[2] = point3(-0.05, -10, 0); yline_colors[2] = color3(1, 0, 1);

	yline_points[3] = point3(-0.05, -10, 0); yline_colors[3] = color3(1, 0, 1);
	yline_points[4] = point3(-0.05, 10, 0);  yline_colors[4] = color3(1, 0, 1);
	yline_points[5] = point3(0.05, 10, 0);   yline_colors[5] = color3(1, 0, 1);
}

void zline()
{
	zline_points[0] = point3(0.05, 0, 10);   zline_colors[0] = color3(0, 0, 1);
	zline_points[1] = point3(0.05, 0, -10);  zline_colors[1] = color3(0, 0, 1);
	zline_points[2] = point3(-0.05, 0, -10); zline_colors[2] = color3(0, 0, 1);

	zline_points[3] = point3(-0.05, 0, -10); zline_colors[3] = color3(0, 0, 1);
	zline_points[4] = point3(-0.05, 0, 10);  zline_colors[4] = color3(0, 0, 1);
	zline_points[5] = point3(0.05, 0, 10);   zline_colors[5] = color3(0, 0, 1);
}

void makeNormal()
{	
	int i = 0;
	for (i = 0; i < t * 3; i++)
	{
		vec3 u = sphere_points[i + 1] - sphere_points[i];
		vec3 v = sphere_points[i + 2] - sphere_points[i];
		vec3 p = normalize(sphere_points[i]);
		vec3 q = normalize(sphere_points[i + 1]);
		vec3 r = normalize(sphere_points[i + 2]);

		vec3 normal = normalize(cross(u, v));
		
		flat_sphere_normals[i] = normal;
		smooth_sphere_normals[i] = p;
		
		flat_sphere_normals[i + 1] = normal;
		smooth_sphere_normals[i + 1] = q;
		
		flat_sphere_normals[i + 2] = normal;
		smooth_sphere_normals[i + 2] = r;

		i = i + 2;
	}
}

void fireworks()
{
	for (int i = 0; i < 300; i++)
	{
		fire_points[i] = point3(0.0, 1.0, 0.0);
		fire_colors[i] = color3( (rand() % 256) / 256.0, (rand() % 256) / 256.0, (rand() % 256) / 256.0);
		fire_velocity[i] = vec3(2.0 * ((rand() % 256) / 256.0 - 0.5), 1.2 * 2.0 * ((rand() % 256) / 256.0),
			2.0 * ((rand() % 256) / 256.0 - 0.5));
	}
}

void image_set_up(void)
{
	int i, j, c;

	/* --- Generate checkerboard image to the image array ---*/
	for (i = 0; i < ImageHeight; i++)
		for (j = 0; j < ImageWidth; j++)
		{
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0));

			if (c == 1) /* white */
			{
				c = 255;
				Image[i][j][0] = (GLubyte)c;
				Image[i][j][1] = (GLubyte)c;
				Image[i][j][2] = (GLubyte)c;
			}
			else  /* green */
			{
				Image[i][j][0] = (GLubyte)0;
				Image[i][j][1] = (GLubyte)150;
				Image[i][j][2] = (GLubyte)0;
			}

			Image[i][j][3] = (GLubyte)255;
		}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/*--- Generate 1D stripe image to array stripeImage[] ---*/
	for (j = 0; j < stripeImageWidth; j++) {
		/* When j <= 4, the color is (255, 0, 0),   i.e., red stripe/line.
		When j > 4,  the color is (255, 255, 0), i.e., yellow remaining texture
		*/
		stripeImage[4 * j] = (GLubyte)255;
		stripeImage[4 * j + 1] = (GLubyte)((j>4) ? 255 : 0);
		stripeImage[4 * j + 2] = (GLubyte)0;
		stripeImage[4 * j + 3] = (GLubyte)255;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	/*----------- End 1D stripe image ----------------*/

	/*--- texture mapping set-up is to be done in
	init() (set up texture objects),
	display() (activate the texture object to be used, etc.)
	and in shaders.
	---*/

} /* end function */
//----------------------------------------------------------------------------
// OpenGL initialization
void init()
{
	// colorcube();
	makeNormal();
	image_set_up();
	fireworks();

#if 0 //YJC: The following is not needed
	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
#endif
	/*--- Create and Initialize a texture object ---*/
	glGenTextures(2, tex);      // Generate texture obj name(s)

	glActiveTexture(GL_TEXTURE0);  // Set the active texture unit to be 0 
	glBindTexture(GL_TEXTURE_2D, tex[0]); // Bind the texture to this texture unit

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageWidth, ImageHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, Image);


	glActiveTexture(GL_TEXTURE1);  // Set the active texture unit to be 1
	glBindTexture(GL_TEXTURE_1D, tex[1]);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, stripeImageWidth,
		0, GL_RGBA, GL_UNSIGNED_BYTE, stripeImage);




	/** Note: If using multiple textures, repeat the above process starting from
	glActiveTexture(), but each time use a *different texture unit*,
	so that each texture is bound to a *different texture unit*.    **/
	// Create and initialize a vertex buffer object for cube, to be used in display()
	glGenBuffers(1, &sphere_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, sphere_buffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(point3) * sphere_NumVertices + sizeof(color3) * sphere_NumVertices,
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point3) * sphere_NumVertices, sphere_points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(point3) * sphere_NumVertices, sizeof(color3) * sphere_NumVertices,
		sphere_colors);

#if 0
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_points) + sizeof(cube_colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cube_points), cube_points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cube_points), sizeof(cube_colors),
		cube_colors);
#endif
#if 1
	
	glGenBuffers(1, &flat_sphere_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, flat_sphere_buffer);

		glBufferData(GL_ARRAY_BUFFER, sizeof(point3) * sphere_NumVertices + sizeof(color3) * sphere_NumVertices,
			NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point3) * sphere_NumVertices, sphere_points);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(point3) * sphere_NumVertices, sizeof(color3) * sphere_NumVertices,
			flat_sphere_normals);


	
		glGenBuffers(1, &smooth_sphere_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, smooth_sphere_buffer);

		glBufferData(GL_ARRAY_BUFFER, sizeof(point3) * sphere_NumVertices + sizeof(color3) * sphere_NumVertices,
			NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point3) * sphere_NumVertices, sphere_points);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(point3) * sphere_NumVertices, sizeof(color3) * sphere_NumVertices,
			smooth_sphere_normals);
	
#endif

	glGenBuffers(1, &shadow_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, shadow_buffer);

#if 0
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_points) + sizeof(cube_colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cube_points), cube_points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cube_points), sizeof(cube_colors),
		cube_colors);
#endif
#if 1
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(point3) * sphere_NumVertices + sizeof(color3) * sphere_NumVertices,
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point3) * sphere_NumVertices, sphere_points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(point3) * sphere_NumVertices, sizeof(color3) * sphere_NumVertices,
		shadow_colors);
#endif
	floor();

		// Create and initialize a vertex buffer object for floor, to be used in display()
		glGenBuffers(1, &shading_floor_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, shading_floor_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_normals) + sizeof(floor_texcoord),
			NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(floor_points), floor_points);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points), sizeof(floor_normals),
			floor_normals);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_normals), sizeof(floor_texcoord),
			floor_texcoord);
	
		// Create and initialize a vertex buffer object for floor, to be used in display()
		glGenBuffers(1, &floor_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, floor_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_colors) + sizeof(floor_texcoord),
			NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(floor_points), floor_points);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points), sizeof(floor_colors),
			floor_colors);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_colors), sizeof(floor_texcoord),
			floor_texcoord);

		
	xline();
	glGenBuffers(1, &xline_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, xline_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(xline_points) + sizeof(xline_colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(xline_points), xline_points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(xline_points), sizeof(xline_colors),
		xline_colors);


	yline();
	glGenBuffers(1, &yline_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, yline_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(yline_points) + sizeof(yline_colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(yline_points), yline_points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(yline_points), sizeof(yline_colors),
		yline_colors);


	zline();
	glGenBuffers(1, &zline_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, zline_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(zline_points) + sizeof(zline_colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(zline_points), zline_points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(zline_points), sizeof(zline_colors),
		zline_colors);

	glGenBuffers(1, &fire_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, fire_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fire_points) + sizeof(fire_colors) + sizeof(fire_velocity),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(fire_points), fire_points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(fire_points), sizeof(fire_colors),
		fire_colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(fire_points) + sizeof(fire_colors), sizeof(fire_velocity),
		fire_velocity);


	//Load shaders and create a shader program (to be used in display())
	//program = InitShader("vshader42.glsl", "fshader42.glsl");

	program = InitShader("vshader53.glsl", "fshader53.glsl");
	fire_program = InitShader("vFireworks.glsl", "fFireworks.glsl");

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.529, 0.807, 0.92, 0.0);
	glLineWidth(2.0);
	glPointSize(3.0);
}


void SetUp_Lighting_Uniform_Vars(mat4 mvm, int m, int s)
{	
	if (m == 0)     // 0 is sphere 1 is floor
	{
		if (s == 0)     
		{
			glUniform4fv(glGetUniformLocation(program, "AmbientProduct"),
				1, sphere_ambient_product);
			glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"),
				1, sphere_diffuse_product);
			glUniform4fv(glGetUniformLocation(program, "SpecularProduct"),
				1, sphere_specular_product);
			glUniform4fv(glGetUniformLocation(program, "GlobalAmbientProduct"),
				1, global_ambient_sproduct);


			glUniform1f(glGetUniformLocation(program, "Shininess"),
				sphere_shininess);
			light_position_eyeFrame = distant_light_position;

			
		}
		else if(s == 1)
		{
			glUniform4fv(glGetUniformLocation(program, "AmbientProduct"),
				1, pos_sphere_ambient_product);
			glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"),
				1, pos_sphere_diffuse_product);
			glUniform4fv(glGetUniformLocation(program, "SpecularProduct"),
				1, pos_sphere_specular_product);
			glUniform4fv(glGetUniformLocation(program, "GlobalAmbientProduct"),
				1, global_ambient_sproduct);

			glUniform4fv(glGetUniformLocation(program, "DirAmbientProduct"),
				1, sphere_ambient_product);
			glUniform4fv(glGetUniformLocation(program, "DirDiffuseProduct"),
				1, sphere_diffuse_product);
			glUniform4fv(glGetUniformLocation(program, "DirSpecularProduct"),
				1, sphere_specular_product);


			glUniform1f(glGetUniformLocation(program, "Shininess"),
				sphere_shininess);

			light_position_eyeFrame = mvm * pos_light_position;

			glUniform1f(glGetUniformLocation(program, "ConstAtt"),
				const_att);
			glUniform1f(glGetUniformLocation(program, "LinearAtt"),
				linear_att);
			glUniform1f(glGetUniformLocation(program, "QuadAtt"),
				quad_att);

			spot_point_eyeFrame = mvm * spot_point;

			glUniform4fv(glGetUniformLocation(program, "SpotPoint"),
				1, spot_point_eyeFrame);
			glUniform1f(glGetUniformLocation(program, "CutOff"),
				cutoff);
			glUniform1f(glGetUniformLocation(program, "E"),
				alpha);



		}
		
		glUniform4fv(glGetUniformLocation(program, "LightPosition"),
			1, light_position_eyeFrame);

		glUniform4fv(glGetUniformLocation(program, "DirLightPosition"),
			1, distant_light_position);

	}
	else if (m == 1)
	{
		if (s == 0)
		{
			glUniform4fv(glGetUniformLocation(program, "AmbientProduct"),
				1, floor_ambient_product);
			glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"),
				1, floor_diffuse_product);
			glUniform4fv(glGetUniformLocation(program, "SpecularProduct"),
				1, floor_specular_product);
			glUniform4fv(glGetUniformLocation(program, "GlobalAmbientProduct"),
				1, global_ambient_fproduct);

			glUniform1f(glGetUniformLocation(program, "Shininess"),
				floor_shininess);
			light_position_eyeFrame = distant_light_position;
		}
		else
		{
			glUniform4fv(glGetUniformLocation(program, "AmbientProduct"),
				1, pos_floor_ambient_product);
			glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"),
				1, pos_floor_diffuse_product);
			glUniform4fv(glGetUniformLocation(program, "SpecularProduct"),
				1, pos_floor_specular_product);
			glUniform4fv(glGetUniformLocation(program, "GlobalAmbientProduct"),
				1, global_ambient_fproduct);

			glUniform4fv(glGetUniformLocation(program, "DirAmbientProduct"),
				1, floor_ambient_product);
			glUniform4fv(glGetUniformLocation(program, "DirDiffuseProduct"),
				1, floor_diffuse_product);
			glUniform4fv(glGetUniformLocation(program, "DirSpecularProduct"),
				1, floor_specular_product);

			glUniform1f(glGetUniformLocation(program, "Shininess"),
				floor_shininess);
			light_position_eyeFrame = mvm * pos_light_position;



			glUniform1f(glGetUniformLocation(program, "ConstAtt"),
				const_att);
			glUniform1f(glGetUniformLocation(program, "LinearAtt"),
				linear_att);
			glUniform1f(glGetUniformLocation(program, "QuadAtt"),
				quad_att);

			spot_point_eyeFrame = mvm * spot_point;

			glUniform4fv(glGetUniformLocation(program, "SpotDirection"),
				1, spot_point_eyeFrame);
			glUniform1f(glGetUniformLocation(program, "CutOff"),
				cutoff);
			glUniform1f(glGetUniformLocation(program, "E"),
				alpha);
			
		}
		glUniform4fv(glGetUniformLocation(program, "LightPosition"),
			1, light_position_eyeFrame);

		glUniform4fv(glGetUniformLocation(program, "DirLightPosition"),
			1, distant_light_position);
		

	}
	
}
//----------------------------------------------------------------------------
// drawObj(buffer, num_vertices):
//   draw the object that is associated with the vertex buffer object "buffer"
//   and has "num_vertices" vertices.
//
void drawObj(GLuint buffer, int num_vertices, GLint Light)
{
	//--- Activate the vertex buffer object to be drawn ---//
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	
	if (Light == 0)
	{

		//----- Set up vertex attribute arrays for each vertex attribute -----
		GLuint vPosition = glGetAttribLocation(program, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(0));


		GLuint vColor = glGetAttribLocation(program, "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(sizeof(point3) * num_vertices));     

		GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
		glEnableVertexAttribArray(vTexCoord);
		glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(sizeof(point3) * num_vertices + sizeof(color3) * num_vertices));
		// the offset is the (total) size of the previous vertex attribute array(s)
		glDrawArrays(GL_TRIANGLES, 0, num_vertices);
		
	}// the offset is the (total) size of the previous vertex attribute array(s)
	else if (Light == 1)
	{

		GLuint vPosition = glGetAttribLocation(program, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(0));

		GLuint vNormal = glGetAttribLocation(program, "vNormal");
		glEnableVertexAttribArray(vNormal);
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(sizeof(point3) * num_vertices));
		// the offset is the (total) size of the previous vertex attribute array(s)

		GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
		glEnableVertexAttribArray(vTexCoord);
		glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(sizeof(point3) * num_vertices + sizeof(color3) * num_vertices));
		// the offset is the (total) size of the previous vertex attribute array(s)

		glDrawArrays(GL_TRIANGLES, 0, num_vertices);

	}

	else if (Light == 2)
	{
		//----- Set up vertex attribute arrays for each vertex attribute -----
		GLuint vPosition = glGetAttribLocation(fire_program, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(0));


		GLuint vColor = glGetAttribLocation(fire_program, "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(sizeof(point3) * num_vertices));

		GLuint vTexCoord = glGetAttribLocation(fire_program, "vVelocity");
		glEnableVertexAttribArray(vVelocity);
		glVertexAttribPointer(vVelocity, 3, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(sizeof(point3) * num_vertices + sizeof(color3) * num_vertices));
		// the offset is the (total) size of the previous vertex attribute array(s)

		glDrawArrays(GL_POINTS, 0, num_vertices);
	}
	

	/* Draw a sequence of geometric objs (triangles) from the vertex buffer
	(using the attributes specified in each enabled vertex attribute array) */
	//glDrawArrays(GL_TRIANGLES, 0, num_vertices);

	/*--- Disable each vertex attribute array being enabled ---*/
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vColor);
	glDisableVertexAttribArray(vNormal);
	glDisableVertexAttribArray(vTexCoord);
	glDisableVertexAttribArray(vVelocity);

}
//----------------------------------------------------------------------------
void display(void)
{
	GLuint  model_view;  // model-view matrix uniform shader variable location
	GLuint  projection;  // projection matrix uniform shader variable location
	GLuint ModelView;
	GLuint Projection;

	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program); // Use the shader program

	

	model_view = glGetUniformLocation(program, "model_view");
	projection = glGetUniformLocation(program, "projection");
	GLint I = glGetUniformLocation(program, "Ill");
	GLint S = glGetUniformLocation(program, "Sou");
	glUniform1i(glGetUniformLocation(program, "fog_mode"),
		fog);
	glUniform4fv(glGetUniformLocation(program, "fogColor"),
		1, fog_color);

	

	/*---  Set up and pass on Projection matrix to the shader ---*/
	mat4  p = Perspective(fovy, aspect, zNear, zFar);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major
	//glUniformMatrix4fv(Projection, 1, GL_TRUE, p);
	

												   /*---  Set up and pass on Model-View matrix to the shader ---*/
												   // eye is a global variable of vec4 set to init_eye and updated by keyboard()
	vec4    at(-7.0, -3.0, 10.0, 0.0);
	vec4    up(0.0, 1.0, 0.0, 0.0);

	mat4 mv = LookAt(eye, at, up);
	mat4 mv1 = mv;

#if 0 // The following is to verify the correctness of the function NormalMatrix():
	// Commenting out Rotate() and un-commenting mat4WithUpperLeftMat3() 
	// gives the same result.
	mv = Translate(-1.0, -0.5, 0.0) * mv * Scale(1.4, 1.4, 1.4)
		* Rotate(angle, 0.0, 0.0, 2.0);
	// * mat4WithUpperLeftMat3(NormalMatrix(Rotate(angle, 0.0, 0.0, 2.0), 1));
#endif
#if 1 // The following is to verify that Rotate() about (0,2,0) is RotateY():
	// Commenting out Rotate() and un-commenting RotateY()
	// gives the same result.
	/*mv = Translate(-1.0, -0.5, 0.0) * mv * Scale (1.4, 1.4, 1.4)
	* Rotate(angle, 0.0, 2.0, 0.0);*/
	// * RotateY(angle);
#endif
#if 0  // The following is to verify that Rotate() about (3,0,0) is RotateX():
	// Commenting out Rotate() and un-commenting RotateX()
	// gives the same result.
	mv = Translate(-1.0, -0.5, 0.0) * mv * Scale(1.4, 1.4, 1.4)
		* Rotate(angle, 3.0, 0.0, 0.0);
	// * RotateX(angle);
#endif
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv1); // GL_TRUE: matrix is row-major
	glUniform1i(I, 0);

	if (ylineFlag == 1) // Filled y axis
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else              // Wireframe y axis
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	drawObj(yline_buffer, yline_NumVertices, 0);  // draw the y axis
	
	
	 // draw the sphere starts
	drawsphereflag = 1;
	glUniform1i(glGetUniformLocation(program, "Sphere"),
		drawsphereflag);
	glUniform1i(glGetUniformLocation(program, "stexmode"),
		stexflag);
	glUniform1i(glGetUniformLocation(program, "texture_1D"), 1);
	glUniform1i(glGetUniformLocation(program, "Lattice"), lattice_flag);

	glUniform1i(glGetUniformLocation(program, "LatticeType"), lattice_flagtype);
	glUniform1i(glGetUniformLocation(program, "Space"), space);
	glUniform1i(glGetUniformLocation(program, "Align"), align);


	if (rv.x == 9)
	{
		mv = mv * Translate(a, b, c) * Translate((angle *(M_PI / 360)) * tv.x, tv.y, (angle *(M_PI / 360)) * tv.z) * Rotate(angle, rv.x, rv.y, rv.z) * M1;
	}
	if (rv.x == -1)
	{
		mv = mv * Translate(a, b, c) * Translate((angle *(M_PI / 360)) * tv.x, tv.y, (angle *(M_PI / 360)) * tv.z) * Rotate(angle, rv.x, rv.y, rv.z) * M2;
	}
	if (rv.x == -8)
	{
		mv = mv * Translate(a, b, c) * Translate((angle *(M_PI / 360)) * tv.x, tv.y, (angle *(M_PI / 360)) * tv.z) * Rotate(angle, rv.x, rv.y, rv.z) * M3;
	}


	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major

		if (l == 1)					// 1 indicates lighting is enabled
		{
			if (cubeFlag == 1)
			{
				if (light_source == 1)   // 1 is positional else directional
				{
					SetUp_Lighting_Uniform_Vars(mv1, 0, 1);
					if (source == 0)			// 0 is point source 1 is spot light 2 is directional
						glUniform1i(S, source);
					else
						glUniform1i(S, source);
				}
				else
				{
					SetUp_Lighting_Uniform_Vars(mv1, 0, 0);
					glUniform1i(S, 2);
				}



				mat3 normal_matrix = NormalMatrix(mv, 0);
				glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"),
					1, GL_TRUE, normal_matrix);
				glUniform1i(I, l);

				


				
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				

				if (shading == 1)
					drawObj(flat_sphere_buffer, sphere_NumVertices, 1);  // draw the sphere ends
				else
					drawObj(smooth_sphere_buffer, sphere_NumVertices, 1);  // draw the sphere ends
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				drawObj(sphere_buffer, sphere_NumVertices, 0);
			}
			

			
		}
		else
		{
			if (cubeFlag == 1)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				drawObj(sphere_buffer, sphere_NumVertices, 0);
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				drawObj(sphere_buffer, sphere_NumVertices, 0);
			}
		}

		drawsphereflag = 0;
		glUniform1i(glGetUniformLocation(program, "Sphere"),
			drawsphereflag);
	
	
	

 // draw the floor starts

		drawfloorflag = 1;
		glUniform1i(glGetUniformLocation(program, "Floor"),
			drawfloorflag);
		glUniform1i(glGetUniformLocation(program, "ftexmode"),
			ftexflag);
		glUniform1i(glGetUniformLocation(program, "texture_2D"), 0);

	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv1); // GL_TRUE: matrix is row-major

	if (l == 1)
	{
		
		
		mat3 normal_matrix = NormalMatrix(mv1, 0);
		glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"),
			1, GL_TRUE, normal_matrix);
		glUniform1i(I, l);
		if (light_source == 1)
		{
			SetUp_Lighting_Uniform_Vars(mv1, 1, 1);
			if(source == 0)
				glUniform1i(S, source);
			if(source == 1)
				glUniform1i(S, source);
		}
		else
		{
			SetUp_Lighting_Uniform_Vars(mv1, 1, 0);
			glUniform1i(S, 2);
		}
			


		
		
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		glDepthMask(GL_FALSE);
		drawObj(shading_floor_buffer, floor_NumVertices, 1);  // draw the floor ends
	}
	else
	{
		
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		glDepthMask(GL_FALSE);
		drawObj(floor_buffer, floor_NumVertices, 0);  // draw the floor ends
	}
	
	drawfloorflag = 0;
	glUniform1i(glGetUniformLocation(program, "Floor"),
		drawfloorflag);
	//draw the axes
	
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv1); // GL_TRUE: matrix is row-major
	glUniform1i(I, 0);
	if (xlineFlag == 1) // Filled x axis
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else              // Wireframe x axis
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	drawObj(xline_buffer, xline_NumVertices, 0);  // draw the x axis

	if (zlineFlag == 1) // Filled z axis
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else              // Wireframe z axis
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	drawObj(zline_buffer, zline_NumVertices, 0);  // draw the z axis


	// draw shadow starts
	drawshadowflag = 1;
	glUniform1i(glGetUniformLocation(program, "Shadow"), drawshadowflag);

	if(blend == 0)
	{
		glDisable(GL_BLEND);
	}
	else if(blend == 1)
	{
		glEnable(GL_BLEND);
	}
	if (rv.x == 9)             // draw the shadow starts
	{
		mv = mv1 * N * Translate(a, b, c) * Translate((angle *(M_PI / 360)) * tv.x, tv.y, (angle *(M_PI / 360)) * tv.z) * Rotate(angle, rv.x, rv.y, rv.z) * M1;
	}
	if (rv.x == -1)
	{
		mv = mv1 * N * Translate(a, b, c) * Translate((angle *(M_PI / 360)) * tv.x, tv.y, (angle *(M_PI / 360)) * tv.z) * Rotate(angle, rv.x, rv.y, rv.z) * M2;
	}
	if (rv.x == -8)
	{
		mv = mv1 * N * Translate(a, b, c) * Translate((angle *(M_PI / 360)) * tv.x, tv.y, (angle *(M_PI / 360)) * tv.z) * Rotate(angle, rv.x, rv.y, rv.z) * M3;
	}

	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
	glUniform1i(I, 0);
	if (cubeFlag == 1) // Filled sphere
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else              // Wireframe cube
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glDepthMask(GL_TRUE);
	if (shadow == 1)
	{
		if(eye.y >= 1.0)
			drawObj(shadow_buffer, sphere_NumVertices, 0);  // draw the shadow ends
	}
	glDisable(GL_BLEND);

	drawshadowflag = 0;
	glUniform1i(glGetUniformLocation(program, "Shadow"), drawshadowflag);
	
	//draw shadow ends

	glDepthMask(GL_TRUE);

	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv1); // GL_TRUE: matrix is row-major
	if (floorFlag == 1) // Filled floor
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else              // Wireframe floor
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	drawObj(floor_buffer, floor_NumVertices, 1);  // draw the floor to only z buffer


	
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	if (fire_flag == 1)
	{
		glUseProgram(fire_program); // Use the shader program

		ModelView = glGetUniformLocation(fire_program, "ModelView");
		Projection = glGetUniformLocation(fire_program, "Projection");

		glUniformMatrix4fv(ModelView, 1, GL_TRUE, mv1); // GL_TRUE: matrix is row-major
		glUniformMatrix4fv(Projection, 1, GL_TRUE, p);
		glUniform1f(glGetUniformLocation(fire_program, "T"), et);
		drawObj(fire_buffer, fire_NumVertices, 2);
	}
	
	
	

	glutSwapBuffers();
}
//---------------------------------------------------------------------------

void idle(void)
{
	M = Rotate(angle, rv.x, rv.y, rv.z);
	angle += 0.3; //change this value to adjust the cube rotation speed.


	vec4 o = Translate(a, b, c) * Translate((angle * (M_PI / 360)) * tv.x, tv.y, (angle *(M_PI / 360)) * tv.z) *  vec4(0, 0, 0, 1);


	if (rv.x == -8 && rv.y == 0 && rv.z == -3 && o.x > -1)
	{
		M1 = M * M3;
		angle = 0;
		a = -1, b = 1, c = -4;
		rv = point3(9, 0, -4);
		tv = point3(4 / sqrt(97), 0, 9 / sqrt(97));
	}
	if (rv.x == 9 && rv.y == 0 && rv.z == -4 && o.x > 3)
	{
		M2 = M * M1;

		angle = 0;
		a = 3, b = 1, c = 5;
		rv = point3(-1, 0, 7);
		tv = point3(-7 / sqrt(50), 0, -1 / sqrt(50));
	}
	if (rv.x == -1 && rv.y == 0 && rv.z == 7 && o.x < -4)
	{
		M3 = M * M2;
		angle = 0;
		a = -4, b = 1, c = 4;
		rv = point3(-8, 0, -3);
		tv = point3(3 / sqrt(73), 0, -8 / sqrt(73));
	}

	if (fire_flag == 1)
	{
		for (int i = 0; i < 300; i++)
		{
			if ((1.0 + 0.001 * fire_velocity[i].y * et + 0.5 * -0.00000049 * et * et) > 0.1)
			{
				discardcount = 0;
				break;
			}
			else
			{
				discardcount += 1;
				if (discardcount == 300)
				{
					tsub = (float)glutGet(GLUT_ELAPSED_TIME);
					discardcount = 0;
				}
			}
		}
		realet = (float)glutGet(GLUT_ELAPSED_TIME);
		et = realet - tsub;
	}
	

	glutPostRedisplay();
}
//----------------------------------------------------------------------------

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;

	case 'X': eye[0] += 1.0; break;
	case 'x': eye[0] -= 1.0; break;
	case 'Y': eye[1] += 1.0; break;
	case 'y': eye[1] -= 1.0; break;
	case 'Z': eye[2] += 1.0; break;
	case 'z': eye[2] -= 1.0; break;

	case 'b': case 'B': // Toggle between animation and non-animation
	//	animationFlag = 1 - animationFlag;
		start = 1;
		if (animationFlag == 0)
		{
			//tsub = (float)glutGet(GLUT_ELAPSED_TIME) - tsub;
			tsub = tsub + (float)glutGet(GLUT_ELAPSED_TIME) - tstart;
			animationFlag = 1;
			glutIdleFunc(idle);
		}
		else
		{
			tstart = (float)glutGet(GLUT_ELAPSED_TIME);
			animationFlag = 0;
			glutIdleFunc(NULL);
		}
		
		break;
	case 'l': case 'L': // Toggle lattice
		if (cubeFlag == 1) {
			if (lattice_flag == 0)
			{
				lattice_flag = 1;
				lattice_flagtype = 0;
			}
			else
			{
				lattice_flag = 0;
			}
		}
		
		break;

	case 'u' : case 'U': // toggle upright lattice
		lattice_flagtype = 0;
		break;

	case 't' : case 'T': // toggle tilted lattice
		lattice_flagtype = 1;
		break;

	case 'o' : case 'O': // toggle object space
		space = 0;
		break;

	case 'e' : case 'E': // toggle eye space
		space = 1;
		break;

	case 'v' : case 'V': // vertical alignment
		align = 0;
		break;

	case 's' : case 'S': //  slanted alignment
		align = 1;
		break;
	}
	glutPostRedisplay();
}

void demo_menu(int id)
{
	switch (id)
	{
	case 1:
		if (animationFlag == 0)
		{
			eye = init_eye;
			animationFlag = 1;
			glutIdleFunc(idle);
		}
		if (animationFlag == 1)
		{
			eye = init_eye;
		}
		break;
	case 2:
		exit(0);
		break;
	case 9:
		cubeFlag = 0;
		stexflag = 0;
		lattice_flag = 0;
		break;




	}
}

void shadow_menu(int id)
{
	switch (id)
	{
	case 3:
			shadow = 1;
		if (animationFlag == 0)
		{
			//tsub = tsub + (float)glutGet(GLUT_ELAPSED_TIME) - tstart;
			animationFlag = 1;
			glutIdleFunc(idle);
		}


		break;

	case 4:
			shadow = 0;
		if (animationFlag == 0)
		{
			//tsub = tsub + (float)glutGet(GLUT_ELAPSED_TIME) - tstart;
			animationFlag = 1;
			glutIdleFunc(idle);
		}


		break;
	}
}

void shading_menu(int id)
{
	switch (id)
	{
	case 5:
			shading = 1;
			cubeFlag = 1;
		if (animationFlag == 0)
		{
			animationFlag = 1;
			glutIdleFunc(idle);
		}


		break;
	case 6:
			shading = 0;
			cubeFlag = 1;
		if (animationFlag == 0)
		{
			animationFlag = 1;
			glutIdleFunc(idle);
		}
		break;
	}
		

}

void lighting_menu(int id)
{
	switch (id)
	{
	case 7:
			l = 1;
		if (animationFlag == 0)
		{
			animationFlag = 1;
			glutIdleFunc(idle);
		}
			

		break;
	case 8:
			l = 0;
			
		if (animationFlag == 0)
		{
			animationFlag = 1;
			glutIdleFunc(idle);
		}
		break;
	}
}

void light_menu(int id)
{
	switch (id)
	{
	case 10:
		if (l == 1)
		{
			light_source = 1;
			source = 0;
		}
		if (animationFlag == 0)
		{
			animationFlag = 1;
			glutIdleFunc(idle);
		}


		break;
	case 11:
		if (l == 1)
		{
			light_source = 1;
			source = 1;
		}
		if (animationFlag == 0)
		{
			animationFlag = 1;
			glutIdleFunc(idle);
		}
		break;
	}
}


void fog_menu(int id)
{
	switch (id)
	{
	case 12:
		fog = 0;
		if (animationFlag == 0)
		{
			animationFlag = 1;
			glutIdleFunc(idle);
		}
		break;
	case 13:
		fog = 1;
		if (animationFlag == 0)
		{
			animationFlag = 1;
			glutIdleFunc(idle);
		}
		break;
	case 14:
		fog = 2;
		if (animationFlag == 0)
		{
			animationFlag = 1;
			glutIdleFunc(idle);
		}
		break;
	case 15:
		fog = 3;
		if (animationFlag == 0)
		{
			animationFlag = 1;
			glutIdleFunc(idle);
		}
		break;
	}
}

void blend_menu(int id)
{
	switch (id)
	{
	case 16:
		blend = 0;
		if (animationFlag == 0)
		{
			animationFlag = 1;
			glutIdleFunc(idle);
		}
		break;
	case 17:
		blend = 1;
		if (animationFlag == 0)
		{
			animationFlag = 1;
			glutIdleFunc(idle);
		}
		break;
	}
}

void tex_ground_menu(int id)
{
	switch (id)
	{
	case 18:
		ftexflag = 0;
		if (animationFlag == 0)
		{
			animationFlag = 1;
			glutIdleFunc(idle);
		}
		break;
	case 19:
		ftexflag = 1;
		if (animationFlag == 0)
		{
			animationFlag = 1;
			glutIdleFunc(idle);
		}
		break;
	}
}

void tex_sphere_menu(int id)
{
	switch (id)
	{
	case 20:
		stexflag = 0;
		if (animationFlag == 0)
		{
			animationFlag = 1;
			glutIdleFunc(idle);
		}
		break;
	case 21:
		if (cubeFlag == 1)
				stexflag = 1;
		
		if (animationFlag == 0)
		{
			animationFlag = 1;
			glutIdleFunc(idle);
		}
		break;
	case 22:
		if (cubeFlag == 1)
			stexflag = 2;
		if (animationFlag == 0)
		{
			animationFlag = 1;
			glutIdleFunc(idle);
		}
		break;
	}
}

void fire_menu(int id)
{
	switch (id)
	{
	case 23:
		fire_flag = 0;
		if (animationFlag == 0)
		{
			animationFlag = 1;
			glutIdleFunc(idle);
		}
		break;
	case 24:
		if (fire_flag == 0)
		{
			fire_flag = 1;
			tsub = (float)glutGet(GLUT_ELAPSED_TIME);
		}
		if (animationFlag == 0)
		{
			animationFlag = 1;
			glutIdleFunc(idle);
		}
		break;
	}
}

void myMouse(int button, int state, int x, int y)
{
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP && start == 1)
	{
		if(animationFlag == 0)
		{
			tsub = tsub + (float)glutGet(GLUT_ELAPSED_TIME) - tstart;
			animationFlag = 1;
			glutIdleFunc(idle);
		}
		else
		{
			tstart = (float)glutGet(GLUT_ELAPSED_TIME);
			animationFlag = 0;
			glutIdleFunc(NULL);
		}

		/*animationFlag = 1 - animationFlag;
		if(animationFlag == 0) glutIdleFunc(NULL);
		else glutIdleFunc(idle);*/
	}
	/*if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		if (leftclickflag == 0)
		{
			tstart = (float)glutGet(GLUT_ELAPSED_TIME);
			leftclickflag = 1;
		}
		else
		{
			tsub = tsub + (float)glutGet(GLUT_ELAPSED_TIME) - tstart;
			leftclickflag = 0;
		}
	}*/

}

//----------------------------------------------------------------------------
void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	aspect = (GLfloat)width / (GLfloat)height;
	glutPostRedisplay();
}
//----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	int err, shadow_submenu, shading_submenu, lighting_submenu, light_submenu, fog_submenu, blend_submenu, 
		tex_ground_submenu, tex_sphere_submenu, fire_submenu;

	std::cout << "Type the input filename which contains your desired sphere \n";
	std::cin >> fname;
	loadFile(fname, sphere_points, sphere_colors, shadow_colors);
	





	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	// glutInitContextVersion(3, 2);
	// glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Rotating Sphere");

	/* Call glewInit() and error checking */
	err = glewInit();
	if (GLEW_OK != err)
	{
		printf("Error: glewInit failed: %s\n", (char*)glewGetErrorString(err));
		exit(1);
	}
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glutKeyboardFunc(keyboard);
	glutMouseFunc(myMouse);

	shadow_submenu = glutCreateMenu(shadow_menu);
	glutAddMenuEntry("Yes", 3);
	glutAddMenuEntry("No", 4);

	shading_submenu = glutCreateMenu(shading_menu);
	glutAddMenuEntry("flat shading", 5);
	glutAddMenuEntry("smooth shading", 6);

	lighting_submenu = glutCreateMenu(lighting_menu);
	glutAddMenuEntry("Yes", 7);
	glutAddMenuEntry("No", 8);

	light_submenu = glutCreateMenu(light_menu);
	glutAddMenuEntry("Point Source", 10);
	glutAddMenuEntry("Spot Light", 11);

	fog_submenu = glutCreateMenu(fog_menu);
	glutAddMenuEntry("no fog", 12);
	glutAddMenuEntry("linear", 13);
	glutAddMenuEntry("exponential", 14);
	glutAddMenuEntry("exponential square", 15);

	blend_submenu = glutCreateMenu(blend_menu);
	glutAddMenuEntry("No", 16);
	glutAddMenuEntry("Yes", 17);

	tex_ground_submenu = glutCreateMenu(tex_ground_menu);
	glutAddMenuEntry("No", 18);
	glutAddMenuEntry("Yes", 19);

	tex_sphere_submenu = glutCreateMenu(tex_sphere_menu);
	glutAddMenuEntry("No", 20);
	glutAddMenuEntry("Yes - Contour Lines", 21);
	glutAddMenuEntry("Yes - CheckerBoard", 22);

	fire_submenu = glutCreateMenu(fire_menu);
	glutAddMenuEntry("No", 23);
	glutAddMenuEntry("Yes", 24);
	

	glutCreateMenu(demo_menu);
	glutAddMenuEntry("Quit", 2);
	glutAddMenuEntry("Default View Point", 1);
	glutAddMenuEntry("Wire Frame Sphere", 9);

	glutAddSubMenu("Shadow", shadow_submenu);
	glutAddSubMenu("Shading", shading_submenu);
	glutAddSubMenu("Lighting", lighting_submenu);
	glutAddSubMenu("Light Source", light_submenu);
	glutAddSubMenu("Fog", fog_submenu);
	glutAddSubMenu("Blending Shadow", blend_submenu);
	glutAddSubMenu("Texture Mapped Ground", tex_ground_submenu);
	glutAddSubMenu("Texture Mapped Sphere", tex_sphere_submenu);
	glutAddSubMenu("Firework", fire_submenu);


	glutAttachMenu(GLUT_LEFT_BUTTON);

	init();
	glutMainLoop();
	return 0;


}

void loadFile(std::string name, point3* sPoints, point3* sColor, point3* shColor)
{
	std::ifstream infile(name);
	infile >> t;
	sphere_NumVertices = t * 3;
	GLfloat a, b, c;
	int i = 0;
	std::string line;
	while (getline(infile, line))
	{
		std::istringstream iss(line);

		if (iss >> a >> b >> c)
		{
			sPoints[i] = point3(a, b, c);
			sColor[i] = point3(1.0, 0.84, 0);
			shColor[i] = point3(0.25, 0.25, 0.25);

			i++;
		}
	}
}


