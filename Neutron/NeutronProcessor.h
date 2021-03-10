#pragma once
#ifndef NEUTRON_PROCESSOR
#define NEUTRON_PROCESSOR
#include <map>
#include <vector>
#include <glad/glad.h>
#define GLM_FORCE_SWIZZLE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <string>
#include <filesystem>
namespace neutron {
	// A set of commands that can be used to process variables and use complex initiators
	namespace processor {
		struct ShaderProgramSource {
			std::string VertexSource;
			std::string FragmentSource;
			void clear() {
				VertexSource = "";
				FragmentSource = "";
			}
		};

		extern std::map<const std::string, unsigned int> preLoad;

		extern ShaderProgramSource ParseShader(const std::string& filepath);

		extern unsigned int CompileShader(unsigned int type, const std::string& source);

		extern unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

		extern unsigned int LoadShader(const std::string& filepath, bool cache = true);

		extern void uniM4(unsigned int shader, const GLchar* address, glm::mat4 matrix);

		extern void uniF4(unsigned int shader, const GLchar* address, glm::vec4 vec);

		extern void uniF3(unsigned int shader, const GLchar* address, glm::vec3 vec);

		extern void uniF2(unsigned int shader, const GLchar* address, glm::vec2 vec);

		extern void uniF1(unsigned int shader, const GLchar* address, float a);

		extern void uniF4(unsigned int shader, const GLchar* address, float a, float b, float c, float d);

		extern void uniF3(unsigned int shader, const GLchar* address, float a, float b, float c);

		extern void uniF2(unsigned int shader, const GLchar* address, float a, float b);

		extern void uniI4(unsigned int shader, const GLchar* address, int a, int b, int c, int d);

		extern void uniI3(unsigned int shader, const GLchar* address, int a, int b, int c);

		extern void uniI2(unsigned int shader, const GLchar* address, int a, int b);

		extern void uniI1(unsigned int shader, const GLchar* address, int a);

		extern void uniB(unsigned int shader, const GLchar* address, bool a);

		//Your shader ID, the name of the sampler in your shader, (i.e. "diffuseTexture"), the ID of your texture, the number you want to give to your texture, and a target to go allong with it (i.e. "0" and "GL_TEXTURE0")
		extern void uniSampler(unsigned int shader, const char* name, unsigned int texture, int shaderNumber);
		// Returns a translation matrix
		extern glm::mat4 translation(glm::vec3 pos);
		// Returns a rotation matrix off of a given matrix
		extern glm::mat4 rotation(glm::mat4 mat, glm::vec3 rot);
		// Returns a rotation matrix off of [1]
		extern glm::mat4 rotation(glm::vec3 rot);
		// Returns a scalar matri
		extern glm::mat4 scaled(glm::vec3 scaling);
		// Returns a positioned, rotated, and possibly scaled matrix off of given matrix, or [1]
		extern glm::mat4 form(glm::vec3 pos, glm::vec3 scale = glm::vec3(1), glm::vec3 rot = glm::vec3(0), glm::mat4 mat = glm::mat4(1));

		extern glm::mat4 rect(float x, float y, float w, float h, glm::vec3 rot = glm::vec3(0), glm::mat4 mat = glm::mat4(1));

		// values
		extern unsigned int colstoimg, dVAO, /*Distance VAO*/ dVBO, /*Distance VBO*/ distanceMap /*A bilinear texture of a dot in the center*/;

		// Gives an assigned VBO
		extern unsigned int VBO();
		// Gives an assigned VBO but with vertices in it
		extern unsigned int VBO(float vertices[]);
		// Gives an assigned EBO
		extern unsigned int EBO();
		// Gives an assigned VAO
		extern unsigned int VAO();
		// Attributes 'in' values for a shader
		extern void attribute(int order_in_shader, int length, int elements_per_vertex, int index_in_array);
		// Attributes but with a direct index
		extern void attributedirect(int order_in_shader, int length, int elements_per_vertex, const void* index_in_array);
		// Loads a texture with a given interpolation, or bilinear default
		extern unsigned int getTextureFrom(const char* filepath, unsigned int interpol = 0x2601, unsigned int wrap = 0x2901, bool cache = true);
		/// UNDOCUMENTED: returns a cubemapid from a resolution
		extern std::vector<unsigned int> createCubemapID_UNDOCUMENTED(int GEN_RESOLUTION = 124);
		/// UNTESTED: Creates an empty cubemap with width and height
		extern unsigned int createEmptyCubemap(int width, int height);
		/// UNTESTED: Binds a texture to the side of a cubemap
		extern unsigned int bindSideOfCubemap(std::vector<unsigned int> textures, unsigned int cubeTex);
		// Loads a cubemap from set of faces as filepaths
		extern unsigned int loadCubemap(std::vector<std::string> faces, bool cache = false);
		// Returns a texture from the given colors, size, and possibly interpolation, or nearest-neighbor default 
		extern unsigned int textureFromColors(std::vector<glm::vec4> colors, int width, int height, unsigned int interpol = 0x2600);
		// Returns a texture from the given colors, size, and possibly interpolation, or nearest-neighbor default 
		extern unsigned int textureFromColors3i(std::vector<glm::ivec3> colors, int width, int height, unsigned int interpol = 0x2601);
		// Returns a texture from the given color
		extern unsigned int textureFromColor(glm::vec4 color, int width = 1, int height = 1);
		// Returns a "distance field" for glyph rendering based on given colors, size, and margin
		extern unsigned int getDistanceField(std::vector<glm::vec4> colors, int width, int height, int marginX = 6, int marginY = 8);
		/// UNFINISHED Returns a set of colors from a filepath
		extern std::vector<glm::vec4> colorsFromTextureUNFINISHED(const char* filepath);
		// Returns a set of colors from a textureID
		extern std::vector<glm::vec4> colorsFromTexture(unsigned int texture);
		// Returns a set of colors from a textureID
		extern std::vector<glm::ivec3> colorsFromTexture3i(unsigned int texture);
		// Gets the size of a texture
		extern glm::ivec2 sizeofTexture(unsigned int texture, unsigned int textureType = 0x0DE1);
		// Gets a distance field from a textureID
		extern unsigned int getDistanceField(unsigned int texture);
		// Returns a distance field for a text specific glyph
		extern unsigned int getTextDistanceField(unsigned int TEXTure);
		// Crops a texture
		extern void cropTexture(unsigned int *texture, int x, int y, int width, int height);
		// Get a cropped version of a texture
		extern unsigned int getCroppedTexture(unsigned int texture, int x, int y, int width, int height);

		// Screenspace coordinate to pixel coord
		extern glm::vec4 toPixelCoord(glm::vec4 screenspaceCoordinate);
		// Pixel coordinate to screenspace coord
		extern glm::vec4 toScreenCoord(glm::vec4 pixelCoordinate);
		// Worldspace transformation to screenspace transformation
		extern glm::mat4 toScreenTransform(glm::mat4 transform);
		/// Get coord again trying lets see if this works haha yeah Change desc when possible
		extern glm::vec4 useMatrices(glm::vec4 position);

		extern std::vector<std::string> split(const std::string& s, char delimiter);

		extern float mapTo(float in, float inMax, float inMin, float outMax, float outMin);

		extern unsigned int margin(unsigned int texture, unsigned int marginX, unsigned int marginY);

		extern void drawToTexture2D(unsigned int &texture, int width, int height, int interpolation = 0x2600, int colors = 0x1908);

		// Create a color correction matrix
		extern glm::mat4x3 getColorCorrection(glm::vec3 selectColor, glm::vec3 mult = glm::vec3(1), glm::vec3 add = glm::vec3(0), glm::vec3 tolerance = glm::vec3(0.1));
		// Modify a color correction matrix's contrast and brightness
		extern void changeColorCorrectionBrightness(glm::mat4x3 correction, float brightness, float contrast = 1.0, bool hsv = true);
		// Set a color correction matrix's contrast and brightness
		extern void setColorCorrectionBrightness(glm::mat4x3 correction, float brightness, float contrast = 1.0, bool hsv = true);

		extern void set();
	};

};
#endif