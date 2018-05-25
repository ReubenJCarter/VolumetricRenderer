#pragma once


#include "../Common.hpp"


class Object3D
{
	protected:
		bool visible;
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
		Object3D* parent; 
		std::vector<Object3D*> children; 
		
	public:
		Object3D();
		~Object3D();
		virtual void Init(){}
		virtual void Render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix){}
		virtual void Destroy(){}
		glm::mat4 GetModelMatrix(); 
		glm::vec3 GetPosition();
		void SetPosition(glm::vec3 pos);
		glm::vec3 GetRotation();
		void SetRotation(glm::vec3 rot);
		glm::vec3 GetScale();
		void SetScale(glm::vec3 sca);
		bool GetVisible();
		void SetVisible(bool v);
		void SetParent(Object3D* par);
};