#pragma once
#ifndef NEUTRON_WORLD
#define NEUTRON_WORLD
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <map>
#include <ft2build.h>
#include <freetype/freetype.h>
#include<iostream>
namespace neutron {
	namespace world {

		extern float clip_near, clip_far;

		// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
		enum Camera_Movement {
			FORWARD,
			BACKWARD,
			LEFT,
			RIGHT,
			UP,
			DOWN
		};

		// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
		class Camera {
		public:
			static const unsigned int MODE_ORBIT = 0, MODE_FREE = 1;
			unsigned int moveMode = MODE_FREE;
			bool LOCK_UP_VALUES = true;
			// Camera Attributes
			glm::vec3 position;
			glm::vec3 Front;
			glm::vec3 Up;
			glm::vec3 Right;
			glm::vec3 Left;
			glm::vec3 Down;
			glm::vec3 Back;
			glm::vec3 WorldUp;
			// Euler Angles
			float Yaw;
			float Pitch;
			// Camera options
			float MovementSpeed;
			float MouseSensitivity;
			float Zoom;
			float OrbitDistance = 1, OrbitDistanceMin = 0.1, OrbitDistanceMax = INFINITY;

			// Constructor with vectors
			Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.f, float pitch = 0.f) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(2.5f), MouseSensitivity(0.1f), Zoom(116.0f) {
				this->position = position;
				this->WorldUp = up;
				this->Yaw = yaw;
				this->Pitch = pitch;
				updateCameraVectors();
			}
			// Constructor with scalar values
			Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(2.5f), MouseSensitivity(0.1f), Zoom(116.0f) {
				position = glm::vec3(posX, posY, posZ);
				WorldUp = glm::vec3(upX, upY, upZ);
				Yaw = yaw;
				Pitch = pitch;
				updateCameraVectors();
			}

			// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
			glm::mat4 GetViewMatrix();

			// Returns a new view matrix with a position, front, and up value
			glm::mat4 genViewMatrix(glm::vec3 Position, glm::vec3 Front, glm::vec3 Up) {
				return glm::lookAt(Position, Position + Front, Up);
			}

			// Smoothly track to a position
			void centerOn(glm::vec3 pos, float smoothness = 0.75);

			// Constrain to area
			void constrain(float left, float up, float right, float down) {
				position.x = (std::max)((std::min)(right, position.x), left);
				position.y = (std::max)((std::min)(down, position.y), up);
			}

			// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
			void ProcessKeyboard(Camera_Movement direction);

			// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
			void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

			// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
			void ProcessMouseScroll(float yoffset);

		private:
			// Calculates the front vector from the Camera's (updated) Euler Angles
			void updateCameraVectors() {
				// Calculate the new Front vector
				glm::vec3 front;
				front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
				front.y = sin(glm::radians(Pitch));
				front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
				Front = glm::normalize(front);
				// Also re-calculate the Right and Up vector
				Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
				Up = glm::normalize(glm::cross(Right, Front));
				Left = glm::vec3(-Right.x, -Right.y, -Right.z);
				Down = glm::vec3(-Up.x, -Up.y, -Up.z);
				Back = glm::vec3(-Front.x, -Front.y, -Front.z);
			}
		};

		struct Character {
			unsigned int texture;
			glm::ivec2 size;
			glm::ivec2 bearing;
			glm::ivec2 advance;
		};
		extern std::map<std::string, std::map<char32_t, Character>> characters;
		extern std::map<std::string, int> sizeMap;

		extern FT_Library ft;

		extern void loadFont(const char* filepath, std::string id = "arial", int size = 48, short glyphsNum = 255);

		extern glm::vec2 measureString(const std::u32string &text, const char* font, float xspacing = 0, float yspacing = 0);
		extern glm::vec2 measureString(const std::string &text, const char* font, float xspacing = 0, float yspacing = 0);

		extern world::Camera camera;

		extern void set();

		extern void reset();
	};

};
#endif