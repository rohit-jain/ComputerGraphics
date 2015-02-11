#include "canvas.h"
#include "core/geometry.h"

canvashdl::canvashdl(int w, int h)
{
    width = w;
    height = h;
    last_reshape_time = -1.0;
    reshape_width = w;
    reshape_height = h;
    initialized = false;
    
    color_buffer = new unsigned char[width*height*3];
    // TODO Assignment 2: Initialize the depth buffer
    
    screen_texture = 0;
    screen_geometry = 0;
    screen_shader = 0;
    
    active_matrix = modelview_matrix;
    
    for (int i = 0; i < 3; i++)
        matrices[i] = identity<float, 4, 4>();
    
    polygon_mode = line;
    culling = backface;
}

canvashdl::~canvashdl()
{
    if (color_buffer != NULL)
    {
        delete [] color_buffer;
        color_buffer = NULL;
    }
    
    // TODO Assignment 2: Clean up the depth buffer
}

void canvashdl::clear_color_buffer()
{
    memset(color_buffer, 0, width*height*3*sizeof(unsigned char));
}

void canvashdl::clear_depth_buffer()
{
    // TODO Assignment 2: Clear the depth buffer
}

void canvashdl::resize(int w, int h)
{
    // TODO Assignment 2: resize the depth buffer
    
    last_reshape_time = -1.0;
    
    if (color_buffer != NULL)
    {
        delete [] color_buffer;
        color_buffer = NULL;
    }
    
    width = w;
    height = h;
    
    color_buffer = new unsigned char[w*h*3];
    
    glActiveTexture(GL_TEXTURE0);
    check_error(__FILE__, __LINE__);
    glBindTexture(GL_TEXTURE_2D, screen_texture);
    check_error(__FILE__, __LINE__);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, color_buffer);
    check_error(__FILE__, __LINE__);
}

/* set_matrix
 *
 * Change which matrix is active.
 */
void canvashdl::set_matrix(matrix_id matid)
{
    // TODO Assignment 1: Change which matrix is active.
    active_matrix = matid;
}

/* load_identity
 *
 * Set the active matrix to the identity matrix.
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/glLoadIdentity.xml
 */
void canvashdl::load_identity()
{
    // TODO Assignment 1: Set the active matrix to the identity matrix.
    matrices[active_matrix] = identity<float, 4, 4>();
}

/* rotate
 *
 * Multiply the active matrix by a rotation matrix.
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/glRotate.xml
 */
void canvashdl::rotate(float angle, vec3f axis)
{
    // TODO Assignment 1: Multiply the active matrix by a rotation matrix.
    mat4f rotation_matrix = identity<float, 4, 4>();
    float sin_val = sin(degtorad(angle));
    float cos_val = cos(degtorad(angle));
    axis = norm(axis);
    float a = axis[0];
    float b = axis[1];
    float c = axis[2];
    
    rotation_matrix[0][0] = a*a*(1-cos_val)+cos_val;
    rotation_matrix[0][1] = a*b*(1-cos_val)-c*sin_val;
    rotation_matrix[0][2] = a*c*(1-cos_val)+b*sin_val;
    rotation_matrix[1][0] = a*b*(1-cos_val)+c*sin_val;
    rotation_matrix[1][1] = b*b*(1-cos_val)+cos_val;
    rotation_matrix[1][2] = c*b*(1-cos_val)-a*sin_val;
    rotation_matrix[2][0] = a*c*(1-cos_val)-b*sin_val;
    rotation_matrix[2][1] = c*b*(1-cos_val)+a*sin_val;
    rotation_matrix[2][2] = c*c*(1-cos_val)+cos_val;
    
    matrices[active_matrix] = rotation_matrix*matrices[active_matrix];
    
    
    /***************************
     *Another way...
     ***************************/
    //(1) Rotate space about the x axis so that the rotation axis lies in the xz plane.
    
    //(2) Rotate space about the y axis so that the rotation axis lies along the z axis.
    
    //(3) Perform the desired rotation by θ about the z axis.
    
    //(4) Apply the inverse of step (3).
    
    //(5) Apply the inverse of step (2).
    
    //(6) Apply the inverse of step (1).
}

/* translate
 *
 * Multiply the active matrix by a translation matrix.
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/glTranslate.xml
 */
void canvashdl::translate(vec3f direction)
{
    // TODO Assignment 1: Multiply the active matrix by a translation matrix.
    
    mat4f translation_matrix = identity<float, 4, 4>();
    vec4f direction2 = homogenize(direction);
    translation_matrix.set_col(3, direction2);
    matrices[active_matrix] = translation_matrix*matrices[active_matrix];
}

/* scale
 *
 * Multiply the active matrix by a scaling matrix.
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/glScale.xml
 */
void canvashdl::scale(vec3f size)
{
    // TODO Assignment 1: Multiply the active matrix by a scaling matrix.
    mat4f scaling_matrix = identity<float, 4, 4>();
    for (int i = 0; i < 3; ++i)
    {
        scaling_matrix[i][i] = size[i];
    }
    matrices[active_matrix] = scaling_matrix*matrices[active_matrix];

}

/* perspective
 *
 * Multiply the active matrix by a perspective projection matrix.
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/gluPerspective.xml
 */
void canvashdl::perspective(float fovy, float aspect, float nearVal, float farVal)
{
    // TODO Assignment 1: Multiply the active matrix by a perspective projection matrix.
    
    mat4f perspective_matrix = identity<float, 4, 4>();
    float f = 1/tan(degtorad(fovy/2));
    
    perspective_matrix[0][0] = f/aspect;
    perspective_matrix[1][1] = f;
    perspective_matrix[2][2] = (nearVal+farVal)/(nearVal-farVal);
    perspective_matrix[2][3] = (2*nearVal*farVal)/(nearVal-farVal);
    perspective_matrix[3][2] = -1;
    perspective_matrix[3][3] = 0;
    matrices[active_matrix] = perspective_matrix*matrices[active_matrix];
    
}

/* frustum
 *
 * Multiply the active matrix by a frustum projection matrix.
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/glFrustum.xml
 */
void canvashdl::frustum(float left, float right, float bottom, float top, float nearVal, float farVal)
{
    // TODO Assignment 1: Multiply the active matrix by a frustum projection matrix.
    mat4f frustum_matrix = identity<float, 4, 4>();
    
    frustum_matrix[0][0] = (2*nearVal)/(right-left);
    frustum_matrix[0][2] = (right+left)/(right-left);
    frustum_matrix[1][1] = (2*nearVal)/(top-bottom);
    frustum_matrix[1][2] = (top+bottom)/(top-bottom);
    frustum_matrix[2][2] = -(farVal+nearVal)/(farVal-nearVal);
    frustum_matrix[2][3] = -(2*farVal*nearVal)/(farVal-nearVal);
    frustum_matrix[3][2] = -1;
    frustum_matrix[3][3] = 0;
    matrices[active_matrix] = frustum_matrix*matrices[active_matrix];
}

/* ortho
 *
 * Multiply the active matrix by an orthographic projection matrix.
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/glOrtho.xml
 */
void canvashdl::ortho(float left, float right, float bottom, float top, float nearVal, float farVal)
{
    // TODO Assignment 1: Multiply the active matrix by an orthographic projection matrix.
    
    //The strategy to transform a glOrtho()-defined viewing box into the canonical box is straightforward:
    //Translate the viewing box so that its center coincides with that of the canonical one, then scale
    //its sides so that the match those of the canonical box.
    
    //The center of the viewing box defined by a call to glOrtho() is:
    //[(r+l)/2, (t+b)/2, -(f+n)/2]. Therefore:
    vec3f displacement_vector(-(right+left)/2, -(top+bottom)/2, (farVal+nearVal)/2);
    translate(displacement_vector);
    
    //Since the viewing box is of size (r-l) x (t-b) x (f-n), while the canonical box is of size 2 x 2 x 2
    //the scaling transformation matching the size of the former with those of the latter is:
    vec3f scaling_vector(2/(right-left), 2/(top-bottom), 2/(farVal-nearVal));
    scale(scaling_vector);
    
    //Finally, to account for the reversal in direction of the lines of sight, the needed transformation
    //is (x,y,z) -> (x,y,-z):
    vec3f scaling_vector2(1,1,-1);
    scale(scaling_vector2);
    
}

/* look_at
 *
 * Move and orient the modelview so the camera is at the 'at' position focused on the 'eye'
 * position and rotated so the 'up' vector is up
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/gluLookAt.xml
 */
void canvashdl::look_at(vec3f eye, vec3f at, vec3f up)
{
    // TODO Assignment 1: Emulate the functionality of gluLookAt
}

/* to_window
 *
 * Pixel coordinates to window coordinates.
 */
vec3f canvashdl::to_window(vec2i pixel)
{
    /* TODO Assignment 1: Given a pixel coordinate (x from 0 to width and y from 0 to height),
     * convert it into window coordinates (x from -1 to 1 and y from -1 to 1).
     */
    return vec3f();
}

/* unproject
 *
 * Unproject a window coordinate into world coordinates.
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/gluUnProject.xml
 */
vec3f canvashdl::unproject(vec3f window)
{
    // TODO Assignment 1: Unproject a window coordinate into world coordinates.
    return vec3f();
}

/* shade_vertex
 *
 * This is the vertex shader.
 */
vec8f canvashdl::shade_vertex(vec8f v)
{
    // TODO Assignment 1: Do all of the necessary transformations (normal, projection, modelview, etc)
    
    // TODO Assignment 2: Implement Flat and Gouraud shading.
    return vec8f();
}

/* shade_fragment
 *
 * This is the fragment shader. The pixel color is determined here.
 */
vec3f canvashdl::shade_fragment(vec8f v)
{
    // TODO Assignment 1: Pick a color, any color (as long as it is distinguishable from the background color).
    
    /* TODO Assignment 2: Figure out the pixel color due to lighting and materials
     * and implement phong shading.
     */
    return vec3f();
}

/* plot
 *
 * Plot a pixel.
 */
void canvashdl::plot(vec2i xy, vec8f v)
{
    // TODO Assignment 1: Plot a pixel, calling the fragment shader.
    
    // TODO Assignment 2: Check the pixel depth against the depth buffer.
}

/* plot_point
 *
 * Plot a point given in window coordinates.
 */
void canvashdl::plot_point(vec8f v)
{
    // TODO Assignment 1: Plot a point given in window coordinates.
}

/* plot_line
 *
 * Plot a line defined by two points in window coordinates.
 */
void canvashdl::plot_line(vec8f v1, vec8f v2)
{
    // TODO Assignment 1: Implement Bresenham's Algorithm.
    
    // TODO Assignment 2: Add interpolation for the normals and texture coordinates as well.
}

/* plot_half_triangle
 *
 * Plot half of a triangle defined by three points in window coordinates (v1, v2, v3).
 * The remaining inputs are as follows (s1, s2, s3) are the pixel coordinates of (v1, v2, v3),
 * and (ave) is the average value of the normal and texture coordinates for flat shading.
 */
void canvashdl::plot_half_triangle(vec2i s1, vec2i s2, vec2i s3, vec8f v1, vec8f v2, vec8f v3, vec5f ave)
{
    /* TODO Assignment 2: Implement Bresenham's algorithm. You may plot the horizontal
     * half or the vertical half. Add interpolation for the normals and texture coordinates as well.
     */
}

/* plot_triangle
 *
 * Plot a triangle. (v1, v2, v3) are given in window coordinates.
 */
void canvashdl::plot_triangle(vec8f v1, vec8f v2, vec8f v3)
{
    /* TODO Assignment 1: Use the above functions to plot a whole triangle. Don't forget to
     * take into account the polygon mode. You should be able to render the
     * triangle as 3 points or 3 lines.
     */
    
    // TODO Assignment 2: Add in the fill polygon mode.
}

/* draw_points
 *
 * Draw a set of 3D points on the canvas. Each point in geometry is
 * formatted (vx, vy, vz, nx, ny, nz, s, t).
 */
void canvashdl::draw_points(const vector<vec8f> &geometry)
{
    // TODO Assignment 1: Clip the points against the frustum, call the vertex shader, and then draw them.
}

/* Draw a set of 3D lines on the canvas. Each point in geometry
 * is formatted (vx, vy, vz, nx, ny, nz, s, t).
 */
void canvashdl::draw_lines(const vector<vec8f> &geometry, const vector<int> &indices)
{
    // TODO Assignment 1: Clip the lines against the frustum, call the vertex shader, and then draw them.
}

/* Draw a set of 3D triangles on the canvas. Each point in geometry is
 * formatted (vx, vy, vz, nx, ny, nz, s, t). Don't forget to clip the
 * triangles against the clipping planes of the projection. You can't
 * just not render them because you'll get some weird popping at the
 * edge of the view. Also, this is where font/back face culling is implemented.
 */
void canvashdl::draw_triangles(const vector<vec8f> &geometry, const vector<int> &indices)
{
    /* TODO Assignment 1: Clip the triangles against the frustum, call the vertex shader,
     * break the resulting polygons back into triangles, implement front and back face
     * culling, and then draw the remaining triangles.
     */
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Do not edit anything below here, that code just sets up OpenGL to render a single
 * quad that covers the whole screen, applies the color_buffer as a texture to it, and
 * keeps the buffer size and texture up to date.
 */
void canvashdl::load_texture()
{
    glGenTextures(1, &screen_texture);
    check_error(__FILE__, __LINE__);
    glActiveTexture(GL_TEXTURE0);
    check_error(__FILE__, __LINE__);
    glBindTexture(GL_TEXTURE_2D, screen_texture);
    check_error(__FILE__, __LINE__);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    check_error(__FILE__, __LINE__);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    check_error(__FILE__, __LINE__);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    check_error(__FILE__, __LINE__);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    check_error(__FILE__, __LINE__);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    check_error(__FILE__, __LINE__);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, color_buffer);
    check_error(__FILE__, __LINE__);
}

void canvashdl::load_geometry()
{
    // x, y, s, t
    const GLfloat geometry[] = {
        -1.0, -1.0, 0.0, 0.0,
        1.0, -1.0, 1.0, 0.0,
        -1.0,  1.0, 0.0, 1.0,
        -1.0,  1.0, 0.0, 1.0,
        1.0, -1.0, 1.0, 0.0,
        1.0,  1.0, 1.0, 1.0
    };
    
    glGenBuffers(1, &screen_geometry);
    glBindBuffer(GL_ARRAY_BUFFER, screen_geometry);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*4*6, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*4*6, geometry);
}

void canvashdl::load_shader()
{
    GLuint vertex = load_shader_file(working_directory + "res/canvas.vx", GL_VERTEX_SHADER);
    GLuint fragment = load_shader_file(working_directory + "res/canvas.ft", GL_FRAGMENT_SHADER);
    
    screen_shader = glCreateProgram();
    glAttachShader(screen_shader, vertex);
    glAttachShader(screen_shader, fragment);
    glLinkProgram(screen_shader);
}

void canvashdl::init_opengl()
{
    glEnable(GL_TEXTURE_2D);
    glViewport(0, 0, width, height);
    
    load_texture();
    load_geometry();
    load_shader();
    initialized = true;
}

void canvashdl::check_error(const char *file, int line)
{
    GLenum error_code;
    error_code = glGetError();
    if (error_code != GL_NO_ERROR)
        cerr << "error: " << file << ":" << line << ": " << gluErrorString(error_code) << endl;
}

double canvashdl::get_time()
{
    timeval gtime;
    gettimeofday(&gtime, NULL);
    return gtime.tv_sec + gtime.tv_usec*1.0E-6;
}

void canvashdl::viewport(int w, int h)
{
    glViewport(0, 0, w, h);
    last_reshape_time = get_time();
    reshape_width = w;
    reshape_height = h;
}

void canvashdl::swap_buffers()
{
    if (!initialized)
        init_opengl();
    
    if (last_reshape_time > 0.0 && get_time() - last_reshape_time > 0.125)
        resize(reshape_width, reshape_height);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(screen_shader);
    check_error(__FILE__, __LINE__);
    
    glActiveTexture(GL_TEXTURE0);
    check_error(__FILE__, __LINE__);
    glBindTexture(GL_TEXTURE_2D, screen_texture);
    check_error(__FILE__, __LINE__);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, color_buffer);
    check_error(__FILE__, __LINE__);
    glUniform1i(glGetUniformLocation(screen_shader, "tex"), 0);
    check_error(__FILE__, __LINE__);
    
    glBindBuffer(GL_ARRAY_BUFFER, screen_geometry);
    check_error(__FILE__, __LINE__);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    check_error(__FILE__, __LINE__);
    glEnableClientState(GL_VERTEX_ARRAY);
    check_error(__FILE__, __LINE__);
    
    glTexCoordPointer(2, GL_FLOAT, sizeof(GLfloat)*4, (float*)(sizeof(GLfloat)*2));
    check_error(__FILE__, __LINE__);
    glVertexPointer(2, GL_FLOAT, sizeof(GLfloat)*4, NULL);
    check_error(__FILE__, __LINE__);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    check_error(__FILE__, __LINE__);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    check_error(__FILE__, __LINE__);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    check_error(__FILE__, __LINE__);
    
    glutSwapBuffers();
    check_error(__FILE__, __LINE__);
}

int canvashdl::get_width()
{
    return width;
}

int canvashdl::get_height()
{
    return height;
}
