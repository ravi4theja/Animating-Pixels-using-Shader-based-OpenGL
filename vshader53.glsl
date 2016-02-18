/* 
File Name: "vshader53.glsl":
Vertex shader:
  - Per vertex shading for a single point light source;
    distance attenuation is Yet To Be Completed.
  - Entire shading computation is done in the Eye Frame.
*/

// #version 150 


in  vec3 vPosition;

in  vec3 vNormal;
in vec3 vColor;
in  vec2 vTexCoord;
out vec4 color;
out float fd;
out vec2 texCoord;
out float spheretexmode, floortexmode, latticemode, shadowmode;
out float sphere1dtexcoord;
out vec2 sphere2dtexcoord, latticetexcoord;



uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct, GlobalAmbientProduct, DirAmbientProduct, DirDiffuseProduct, DirSpecularProduct;
uniform mat4 model_view;
uniform mat4 projection;
uniform mat3 Normal_Matrix;
uniform vec4 LightPosition, DirLightPosition;   // Must be in Eye Frame

uniform vec4 SpotPoint;

uniform float CutOff;
uniform float E;
uniform float Shininess;
uniform int Ill, Sou, stexmode, ftexmode, Lattice, Shadow, LatticeType, Space, Align;


vec3 L;

uniform float ConstAtt;  // Constant Attenuation
uniform float LinearAtt; // Linear Attenuation
uniform float QuadAtt;   // Quadratic Attenuation

void main()
{
  // Transform vertex  position into eye coordinates 
  float attenuation, distance;
  vec3 L, Lf, DirL;
  float cosfi;
  
	vec4 vPosition4 = vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);
	
   if(Ill == 1) 
   {
		vec3 pos = (model_view * vPosition4).xyz;
		float dis = distance(pos, LightPosition.xyz);
		if(Sou == 0)
		{
			L = normalize( LightPosition.xyz - pos ); 
			attenuation = 1/(ConstAtt + (LinearAtt * dis) + (QuadAtt * dis * dis));
		}
		else if(Sou == 1)
		{
			L = normalize( LightPosition.xyz - pos ); 
			Lf = normalize((LightPosition.xyz - SpotPoint.xyz));
			cosfi = dot(L, Lf);
			if(cosfi < cos(CutOff))
				attenuation = 0;
			else
				attenuation = pow(cosfi, E) * (1/(ConstAtt + (LinearAtt * dis) + (QuadAtt * dis * dis)));
		}
		else if(Sou == 2)
		{
			L = normalize(LightPosition.xyz);
			attenuation = 1.0;
		}
			

		DirL = normalize(DirLightPosition.xyz);
		// vec3 L = normalize( LightPosition.xyz - pos ); 
		
		vec3 E = normalize( -pos );
		vec3 H = normalize( L + E );
		vec3 DirH = normalize(DirL + E);

		// Transform vertex normal into eye coordinates
		// vec3 N = normalize( model_view*vec4(vNormal, 0.0) ).xyz;
		vec3 N = normalize(Normal_Matrix * vNormal);
		

		//--- To Do: Compute attenuation ---
		 

		// Compute terms in the illumination equation
		vec4 ambient = AmbientProduct;
		vec4 dirambient = DirAmbientProduct;

		float d = max( dot(L, N), 0.0 );
		float Dird = max( dot(DirL, N), 0.0);
		vec4  diffuse = d * DiffuseProduct;
		vec4 dirdiffuse = Dird * DirDiffuseProduct;

		float s = pow( max(dot(N, H), 0.0), Shininess );
		float Dirs = pow( max(dot(N, DirH), 0.0), Shininess );
		vec4  specular = s * SpecularProduct;
		vec4 dirspecular = Dirs * DirSpecularProduct;
    
		if( dot(L, N) < 0.0 ) {
		specular = vec4(0.0, 0.0, 0.0, 1.0);
		}

		if( dot(DirL, N) < 0.0)
			dirspecular = vec4(0.0, 0.0, 0.0, 1.0);

		if(Sou == 2)
		{
			color = GlobalAmbientProduct + (attenuation * (ambient + diffuse + specular));
		}
		else
			color = GlobalAmbientProduct + (attenuation * (ambient + diffuse + specular)) + dirambient + dirdiffuse + dirspecular;
		
		
		//--- attenuation below must be computed properly ---
   }	
   else  
   { 
		if(Shadow == 1)
		{
			shadowmode = 1.0;
			vec4 vColor4 = vec4(vColor.r, vColor.g, vColor.b, 0.65);
			color = vColor4;
		}
		else
		{
			vec4 vColor4 = vec4(vColor.r, vColor.g, vColor.b, 1.0);
			color = vColor4;
		}
		
   }
   
   

   

    vec4 final_position = model_view * vPosition4;
    gl_Position = projection * model_view * vPosition4;
	fd = abs(final_position.z/final_position.w);
   if(stexmode == 0)
   {
		spheretexmode = 0.0;
   }
   if(stexmode == 1)
   {
		spheretexmode = 1.0;
		if(Space == 0)
		{
			if(Align == 0)
				sphere1dtexcoord = 2.5 *(vPosition.x);
			else
				sphere1dtexcoord = (1.5 * (vPosition.x + vPosition.y + vPosition.z ));
		}
		else
		{
			if(Align == 0)
				sphere1dtexcoord = 2.5 *(final_position.x);
			else
				sphere1dtexcoord = (1.5 * (final_position.x + final_position.y + final_position.z));
		}	
   }
   if(stexmode == 2)
   {
		spheretexmode = 2.0;
		if(Space == 0)
		{
			if(Align == 0)
				sphere2dtexcoord = vec2(0.75 * (vPosition.x + 1), 0.75 * (vPosition.y + 1));
			else
				sphere2dtexcoord = vec2(0.45 * (vPosition.x + vPosition.y + vPosition.z), 0.45 * (vPosition.x - vPosition.y + vPosition.z));
				
		}
		else
		{
			if(Align == 0)
				sphere2dtexcoord = vec2(0.75 * (final_position.x + 1), 0.75 * (final_position.y + 1));
			else
				sphere2dtexcoord = vec2(0.45 * (final_position.x + final_position.y + final_position.z), 0.45 * (final_position.x - final_position.y + final_position.z));
		}
									
   }
   if(ftexmode == 0)
   {
		floortexmode = 0.0;
   }
   if(ftexmode == 1)
   {
		floortexmode = 1.0;
		texCoord = vTexCoord;
   }

	if(Lattice == 1)
	{
		
		latticemode = 1.0;
		if(LatticeType == 0)
			latticetexcoord = vec2(0.5 * (vPosition.x + 1), 0.5 * (vPosition.y + 1));
		else
			latticetexcoord = vec2(0.3 * (vPosition.x + vPosition.y + vPosition.z), 0.3 * (vPosition.x - vPosition.y + vPosition.z));
	}
} 
