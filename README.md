# hair

this code currently renders a cyan triangle. it sets up two shader programs that you can use:
- m_basicProgram: just has a vertex and frag shader
- m_fullProgram: has vertex, tessellation, geometry, and frag shaders

it's currently running with the basic program. if you want to use the full one, switch the bound program in paintGL and change the drawing mode from GL_TRIANGLES to GL_PATCHES

## classes
- GLWidget: main OpenGL class containing initializeGL and paintGL
- OpenGLShape: wrapper for a vertex buffer and a VAO. See m_triangle in GLWidget for an example of how to use it.
- ErrorChecker: has static function printGLErrors that prints all OpenGL errors that have accumulated since the last call to printGLErrors
- ResourceLoader: has methods for initializing GLEW and setting up shader programs