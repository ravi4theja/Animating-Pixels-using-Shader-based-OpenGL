/* 
File Name: "fshader53.glsl":
           Fragment Shader
*/

// #version 150 


in  vec4 color;
in  vec2 texCoord;
out vec4 fColor;
in float fd;
in float spheretexmode, floortexmode, latticemode, shadowmode;

in float sphere1dtexcoord;
in vec2 sphere2dtexcoord, latticetexcoord;

uniform int fog_mode;
uniform int Sphere;
uniform int Floor, Lattice;

uniform vec4 fogColor;

uniform sampler2D texture_2D; /* Note: If using multiple textures,
                                       each texture must be bound to a
                                       *different texture unit*, with the
                                       sampler uniform var set accordingly.
                                 The (fragment) shader can access *all texture units*
                                 simultaneously.
                              */

uniform sampler1D texture_1D;


void main() 
{ 
	float f, cf;
	float start = 0.0, end = 18.0, density = 0.09;
	vec4 precolor;

	if(fog_mode == 0)
	{
		
		if(Sphere == 1)
		{
			
			if(spheretexmode == 0.0)
			{
				fColor =  color;
			}
			if(spheretexmode == 1.0)
			{
				//fColor =  color;
				fColor = color * texture( texture_1D, sphere1dtexcoord);
			}
			if(spheretexmode == 2.0)
			{	
				vec4 tmpcolor = texture( texture_2D, sphere2dtexcoord);
				if(tmpcolor.r == 0)
				{
					fColor = color * vec4(0.9, 0.1, 0.1, 1.0);
				}
				else
					fColor =  color * tmpcolor;
			}
			if(Lattice == 1)
			{
				if(abs(fract(4 * latticetexcoord.x)) < 0.35 && abs(fract(4 * latticetexcoord.y)) < 0.35)
					discard;
			}
			
		}
		else if(Floor == 1)
		{
			if(floortexmode == 0.0)
			{
				fColor =  color;
			}
			if(floortexmode == 1.0)
			{
				fColor =  color * texture( texture_2D, texCoord);
			}
		}
		else if(shadowmode == 1.0)
		{
			if(Lattice == 1)
			{
				if(abs(fract(4 * latticetexcoord.x)) < 0.35 && abs(fract(4 * latticetexcoord.y)) < 0.35)
					discard;
			}
			fColor = color;
		}
		else
			fColor = color;
		
	}
	if(fog_mode == 1)
	{ 
		f = (end - fd)/(end - start);
		cf = clamp(f, 0, 1);

		if(Sphere == 1)
		{
			if(Lattice == 1)
			{
				if(abs(fract(4 * latticetexcoord.x)) < 0.35 && abs(fract(4 * latticetexcoord.y)) < 0.35)
					discard;
			}
			if(spheretexmode == 0.0)
			{
				fColor =  mix(fogColor, color, cf);
			}
			if(spheretexmode == 1.0)
			{
				//fColor =  color;
				precolor = color * texture( texture_1D, sphere1dtexcoord);
				fColor =  mix(fogColor, precolor, cf);
			}
			if(spheretexmode == 2.0)
			{	
				vec4 tmpcolor = texture( texture_2D, sphere2dtexcoord);
				if(tmpcolor.r == 0)
				{
					precolor = color * vec4(0.9, 0.1, 0.1, 1.0);
					fColor =  mix(fogColor, precolor, cf);
				}
				else
				{
					precolor =  color * tmpcolor;
					fColor =  mix(fogColor, precolor, cf);
				}
			}
		}
		else if(Floor == 1)
		{
			if(floortexmode == 0.0)
			{
				fColor =  mix(fogColor, color, cf);
			}
			if(floortexmode == 1.0)
			{
				precolor =  color * texture( texture_2D, texCoord);
				fColor =  mix(fogColor, precolor, cf);
			}
		}
		else if(shadowmode == 1.0)
		{
			if(Lattice == 1)
			{
				if(abs(fract(4 * latticetexcoord.x)) < 0.35 && abs(fract(4 * latticetexcoord.y)) < 0.35)
					discard;
			}
			fColor = color;
		}
		else
			fColor = mix(fogColor, color, cf);
		

	}
	if(fog_mode == 2)
	{
		f = exp(-(density * fd));
		cf = clamp(f, 0, 1);

		if(Sphere == 1)
		{
			if(Lattice == 1)
			{
				if(abs(fract(4 * latticetexcoord.x)) < 0.35 && abs(fract(4 * latticetexcoord.y)) < 0.35)
					discard;
			}
			if(spheretexmode == 0.0)
			{
				fColor =  mix(fogColor, color, cf);
			}
			if(spheretexmode == 1.0)
			{
				//fColor =  color;
				precolor = color * texture( texture_1D, sphere1dtexcoord);
				fColor =  mix(fogColor, precolor, cf);
			}
			if(spheretexmode == 2.0)
			{	
				vec4 tmpcolor = texture( texture_2D, sphere2dtexcoord);
				if(tmpcolor.r == 0)
				{
					precolor = color * vec4(0.9, 0.1, 0.1, 1.0);
					fColor =  mix(fogColor, precolor, cf);
				}
				else
				{
					precolor =  color * tmpcolor;
					fColor =  mix(fogColor, precolor, cf);
				}
			}
		}
		else if(Floor == 1)
		{
			if(floortexmode == 0.0)
			{
				fColor =  mix(fogColor, color, cf);
			}
			if(floortexmode == 1.0)
			{
				precolor =  color * texture( texture_2D, texCoord);
				fColor =  mix(fogColor, precolor, cf);
			}
		}
		else if(shadowmode == 1.0)
		{
			if(Lattice == 1)
			{
				if(abs(fract(4 * latticetexcoord.x)) < 0.35 && abs(fract(4 * latticetexcoord.y)) < 0.35)
					discard;
			}
			fColor = color;
		}
		else
			fColor = mix(fogColor, color, cf);
		
	}
	if(fog_mode == 3)
	{
		f = exp(-(pow((density * fd), 2)));
		cf = clamp(f, 0, 1);

		if(Sphere == 1)
		{
			if(Lattice == 1)
			{
				if(abs(fract(4 * latticetexcoord.x)) < 0.35 && abs(fract(4 * latticetexcoord.y)) < 0.35)
					discard;
			}
			if(spheretexmode == 0.0)
			{
				fColor =  mix(fogColor, color, cf);
			}
			if(spheretexmode == 1.0)
			{
				//fColor =  color;
				precolor = color * texture( texture_1D, sphere1dtexcoord);
				fColor =  mix(fogColor, precolor, cf);
			}
			if(spheretexmode == 2.0)
			{	
				vec4 tmpcolor = texture( texture_2D, sphere2dtexcoord);
				if(tmpcolor.r == 0)
				{
					precolor = color * vec4(0.9, 0.1, 0.1, 1.0);
					fColor =  mix(fogColor, precolor, cf);
				}
				else
				{
					precolor =  color * tmpcolor;
					fColor =  mix(fogColor, precolor, cf);
				}
			}
		}
		else if(Floor == 1)
		{
			if(floortexmode == 0.0)
			{
				fColor =  mix(fogColor, color, cf);
			}
			if(floortexmode == 1.0)
			{
				precolor =  color * texture( texture_2D, texCoord);
				fColor =  mix(fogColor, precolor, cf);
			}
		}
		else if(shadowmode == 1.0)
		{
			if(Lattice == 1)
			{
				if(abs(fract(4 * latticetexcoord.x)) < 0.35 && abs(fract(4 * latticetexcoord.y)) < 0.35)
					discard;
			}
			fColor = color;
		}
		else
			fColor = mix(fogColor, color, cf);
		
	}
}

   


