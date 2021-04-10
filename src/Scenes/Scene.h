#pragma once

#include <memory>
#include <array>

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Renderer.h"

#include "Header.h"

#include "Polygons.h"

namespace Scenes
{
	class Scene
	{
	public:
		Scene();
		~Scene();

		void OnUpdate(float deltaTime);
		void OnRender();
		void OnImGuiRender();

	private:
		Polygons::DynaMesh Mesh; //Dynamic object

		std::vector<glm::vec3> ControlPoints; //Controlling points, THESE SHOULD RENDER ON THE SCREEN, use 2DShader.shader
		glm::vec3* SelectedPoint; //Currently selected point, SHOULD BE HIGHLIGHTED

		double Period = 0.1; //distance between points, (1 / Precision)
		int Precision = 2; //Precision
		int XSections = 4; //How many control points on X and Y axis
		int YSections = 4;
		bool ReMesh = false; //Has the number of points changed
		bool HasChanged = true; //Have the coordinates of control points changed or Precision changed

		std::unique_ptr<Shader> m_Shader; //Current used shader, if you want to, create a seperate one for 2D

		glm::mat4 m_Proj, m_View, m_Model, m_MVP; //MVP matrix
		glm::vec3 CamPos; //position of the camera

		float AngleX = 0.0f; //Angle of rotation of camera on x and y axis
		float AngleY = 0.0f;
		float Radius; //Distance of camera from the object

		//YOUR PART in MenuBar.cpp
		void OpenFile(); //change the input parameters in needed
		void SaveFile();
		void ResetSceneObject(); //this is basically "New" button, it should reset the object and camera position
		void ExportFile();
		void ImportFile();

		//YOUR PART, in a file Selection.cpp
		void UpdateSelectedPoint();

		//Functions to modify or update the mesh, or it's control points. new ones may be added
		void GenControlPoints();
		glm::vec3 Interpolate(glm::vec3* arr, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, float a, int i, int sect, int off);
		void GenPoints();
		void GenQuads();
	};
}