# Animating-Pixels-using-Shader-based-OpenGL
Rolling a sphere with controls


Start by opening a new project in VC++
You need to have the GLUT and GLEW packages added to run the program

Add all the source files, header files and the inuput data files to the project

For OpenGL projects that use GLEW, it is is necessary to declare the dependencies for the GLEW library
  Click on Project --> [Project Name] Properties
  Below Configuration Properties, select Linker --> Input
  In the right window, click additional dependencies adn type in glew32.lib
  (Also type in glut32.lib, opengl32.lib, glu32.lib in case they cannot be automatically found during linking)
  
Run the program
