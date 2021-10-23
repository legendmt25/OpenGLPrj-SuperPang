# OpenGLPrj-SuperPang

Project for Basics of Computer Graphics course at [FCSE](https://finki.ukim.mk/). It is a striped down version of [Polythonic/Glitter](https://github.com/Polytonic/Glitter) project that includes:
  - [GLAD](https://github.com/Dav1dde/glad) as OpenGL Function Loader
  - [GLFW](https://github.com/glfw/glfw) for Windowing and Input
  - [GLM](https://github.com/g-truc/glm) for OpenGL Mathematics
  - [stb](https://github.com/nothings/stb) for images and texture loading
  - [freetype](https://github.com/freetype/freetype) for text rendering
  - [irrKlang](https://www.ambiera.com/irrklang) for audio decoding

This is a small student project in with OpenGL 3.3

## Setup
  Recursively clone the repository:

    git clone --recursive https://github.com/legendmt25/OpenGLPrj.git
    
  If you are using an IDE that supports CMake builds (QtCreator, Jetbrains CLion, Visual Studio), open the cloned directory as project (the directory `OpenGLPrj` with the `CMakeLists.txt` file).
  
  Use CMake to build the project and you must build the project in build directory directly in OpenGLPrj **"OpenGLPrj/build"**
  
  Project files can be generated using the following steps:
  
  ### Using cmake:
  
  --Open cmd in the project folder
  mkdir build
  cd build
  cmake ..
  
  
  ###  Using cmake-gui:
     
  Open the `cmake-gui` app. For the source folder select the `OpenGLPrj` directory. For build directory choose an empty directory named `build` at the same level as  `OpenGLPrj`. With both folders choosen, click **Configure** and if successfull procede to **Generate** the build files.