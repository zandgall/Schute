#pragma once
#ifndef NEUTRON_PASSES
#define NEUTRON_PASSES
#include <map>
#include <glm/glm.hpp>
#include <string>
namespace neutron {
	// The stuff you pass to the draw function to draw exactly what you're looking for
	namespace passes {
		// A preset VAO with a shape as described in the name
		extern unsigned int VAO_FLAT, VAO_CUBE, VAO_SPHERE;
		// The VAO draw() should use to render : def is the default on neutron::reset()
		extern unsigned int vao, vao_def;

		// A preset shader that uses every parameter as expected
		extern unsigned int SHADER_FLAT, SHADER_INSTANCE, SHADER_BUTTON, SHADER_POST, SHADER_COLOR_CORRECTION, SHADER_TEXT, SHADER_SKYBOX;
		// What shader Neutron should use : def is the default on neutron::reset();
		extern unsigned int shader, shader_def;

		/// COLOR PROCESSING
		// What mask texture to use, or use none with 0 : def is the default on neutron::reset();
		extern unsigned int mask_texture, mask_texture_def;
		// What multiply texture to use, or use none with 0 : def is the default on neutron::reset();
		extern unsigned int mult_texture, mult_texture_def;
		// What additive texture to use, or use none with 0 : def is the default on neutron::reset();
		extern unsigned int add_texture, add_texture_def;

		// A color to multiply to the result of draw()
		extern glm::vec4 mult_color, mult_color_def;
		// A color to add to the result of draw()
		extern glm::vec4 add_color, add_color_def;

		/// COORDINATES
		// A transformation matrix used to offset, rotate, or scale UV coordinates
		extern glm::mat4 uvTransform, uvTransform_def;

		/// CUSTOM
		// Custom unsigned ints
		extern std::map<std::string, unsigned int> cust_texures, def_cust_textures;
		// Custom floats
		extern std::map<std::string, float> cust_floats, def_cust_floats;
		// Custom float4s
		extern std::map<std::string, glm::vec4> cust_float4s, def_cust_float4s;
		// Custom float3s
		extern std::map<std::string, glm::vec3> cust_float3s, def_cust_float3s;
		// Custom mat4s
		extern std::map<std::string, glm::mat4> cust_mat4s, def_cust_mat4s;

		/// GEOMETRY PROCESSING
		// Where the object is, and how it's scaled and rotated : def is the default on neutron::reset();
		extern glm::mat4 transform, transform_def;
		// To apply automatically to everything when drawing : def is the default on neutron::reset();
		extern glm::mat4 globalTransform, globalTransform_def;
		// Used to correct any drawing methods that require scalars from the transform matrix, e.i. text correction
		extern glm::vec3 rotation;
		// Used for 2d elements in 3d space that always face the camera
		extern bool face_camera;

		/// GLYPH PROCESSING
		// What texture to use as a distance texture, or 0 for no texture at all. used in glyph rendering : def is the default on neutron::reset();
		extern unsigned int distance_texture, distance_texture_def;
		// Glyph nearest mask texture to create hard edges : def is the default on neutron::reset();
		extern unsigned int glyph_nearest_mask, glyph_nearest_mask_def;
		// What value the glyph distancefield clips at : def is the default on neutron::reset();
		extern float glyph_clip, glyph_clip_def;
		// How soft the edges will seem when drawing a glyph : def is the default on neutron::reset();
		extern float glyph_soft_edge, glyph_soft_edge_def;
		// The spread/blurriness of a glyph when anti-aliased
		extern float glyph_anti_alias_spread, glyph_anti_alias_spread_def;

		// Sets the default value of every passed variable : like a hard reset
		extern void set();

		// Sets every pass variable to the set default value : like a soft reset
		extern void reset();
	};

};
#endif