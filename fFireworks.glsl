/*--------------
Fragment Shader: filename "fTexture.glsl"
---------------*/
// #version 150   // YJC: Comment/un-comment this line to resolve compilation errors
                  //      due to different settings of the default GLSL version

in  vec4 color;
in float ycoord;


out vec4 fColor;

void main() 
{ 
	if(ycoord < 0.1)
	{
		discard;
	}
    fColor = color;
  
} 

