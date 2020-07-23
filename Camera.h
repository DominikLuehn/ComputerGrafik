#pragma once

#include "SDL.h"
#include "glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

enum Direction {
	FORWARD,
	BACK,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = 90.0f;
const float PITCH = 0.0f;
const float SPEED = 0.25f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
	public:

	glm::vec3 Position_;
	glm::vec3 Front_;
	glm::vec3 Up_;
	glm::vec3 Right_;
	glm::vec3 WorldUp_;

	float Yaw_;
	float Pitch_;

	float MovementSpeed_;
	float MouseSensitivity_;
	float Zoom_;

		// constructor with vectors
		Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front_(glm::vec3(0.0f, 0.0f, 1.0f)), MovementSpeed_(SPEED), MouseSensitivity_(SENSITIVITY), Zoom_(ZOOM)
		{
			Position_ = position;
			WorldUp_ = up;
			Yaw_ = yaw;
			Pitch_ = pitch;
			UpdateCameraVectors();
		}

		// constructor with scalar values
		Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front_(glm::vec3(0.0f, 0.0f, 1.0f)), MovementSpeed_(SPEED), MouseSensitivity_(SENSITIVITY), Zoom_(ZOOM)
		{
			Position_ = glm::vec3(posX, posY, posZ);
			WorldUp_ = glm::vec3(upX, upY, upZ);
			Yaw_ = yaw;
			Pitch_ = pitch;
			UpdateCameraVectors();
		}

		glm::mat4 getViewMatrix() {
			return glm::lookAt(Position_, Position_ + Front_, Up_);
		}

		void ProcessKeyBoard(Direction direction) {

			float cameraSpeed = MovementSpeed_;
			switch (direction) {
			case FORWARD:
				Position_ += cameraSpeed * Front_;
				break;
			case BACK:
				Position_ -= cameraSpeed * Front_;
				break;
			case LEFT:
				Position_ -= glm::normalize(glm::cross(Front_, Up_)) * cameraSpeed;
				break;
			case RIGHT:
				Position_ += glm::normalize(glm::cross(Front_, Up_)) * cameraSpeed;
				break;
			}
		}

		void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
			xoffset *= MouseSensitivity_;
			yoffset *= MouseSensitivity_;

			Yaw_ += xoffset;
			Pitch_ += -yoffset;

			// make sure that when pitch is out of bounds, screen doesn't get flipped
			if (constrainPitch)
			{
				if (Pitch_ > 89.0f)
					Pitch_ = 89.0f;
				if (Pitch_ < -89.0f)
					Pitch_ = -89.0f;
			}

			// update Front, Right and Up Vectors using the updated Euler angles
			UpdateCameraVectors();
		}

		void ProcessMouseScroll(float yoffset) {
			Zoom_ -= yoffset;
			if (Zoom_ < 1.0f) {
				Zoom_ = 1.0f;
			} else if (Zoom_ > 45.0f) {
				Zoom_ = 45.0f;
			}
		}

		glm::vec3 getPosition() {
			return Position_;
		}

	private:

		void UpdateCameraVectors() {
			glm::vec3 front;
			front.x = glm::cos(glm::radians(Yaw_)) * glm::cos(glm::radians(Pitch_));
			front.y = glm::sin(glm::radians(Pitch_));
			front.z = glm::sin(glm::radians(Yaw_)) * glm::cos(glm::radians(Pitch_));
			Front_ = glm::normalize(front);
			
			Right_ = glm::normalize(glm::cross(Front_, WorldUp_));
			Up_ = glm::normalize(glm::cross(Right_, Front_));
		}
};