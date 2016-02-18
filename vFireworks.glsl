/*--------------
Vertex Shader: filename "vTexture.glsl"
---------------*/
// #version 150    // YJC: Comment/un-comment this line to resolve compilation errors
                   //      due to different settings of the default GLSL version

in  vec3 vPosition;
in  vec3 vVelocity;
in  vec3 vColor;

uniform mat4 ModelView;
uniform mat4 Projection;
uniform float T;


out vec4 color;
out float ycoord;

void main() 
{     
 
 vec4 vPosition4 = vec4(0.0 + 0.001 * vVelocity.x * T, 1.0 + 0.001 * vVelocity.y * T + 0.5 * -0.00000049 * T * T,
						 0.0 + 0.001 * vVelocity.z * T, 1.0);
 gl_Position = Projection * ModelView * vPosition4;

 vec4 vColor4 = vec4(vColor.r, vColor.g, vColor.b, 1.0);
		color = vColor4;
	ycoord = vPosition4.y;

} 
