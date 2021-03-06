#include "core/geometry.h"
#include "standard.h"

using namespace core;

#ifndef object_h
#define object_h

struct canvashdl;

/* This represents a rigid body, which
 * is just a group of geometry to be
 * rendered together. Its grouped in
 * this way so that you can apply different
 * materials to different parts of the
 * same model.
 */
struct rigidhdl
{
	rigidhdl();
	~rigidhdl();

	vector<vec8f> geometry;
	vector<int> indices;
	// TODO Assignment 2: Add a material name here

	void draw(canvashdl *canvas);
};

struct objecthdl
{
	objecthdl();
	virtual ~objecthdl();

	vector<rigidhdl> rigid;
	// TODO Assignment 2: Add a list of materials indexed by name

	vec3f position;
	vec3f orientation;
	float scale;

	// The bounding box of this object
	// (left, right, bottom, top, front, back)
	vec6f bound;

	void draw(canvashdl *canvas);
	void draw_bound(canvashdl *canvas);
	void draw_normals(canvashdl *canvas, bool face = false);
};

#endif
