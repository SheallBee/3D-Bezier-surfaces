#include "Scene.h"

#include "imgui/imgui.h"
#include "Renderer.h"
#include <iostream>
#include <cstdlib>
#include "imgui/imgui.h"

namespace Scenes
{
	//Generate control points
	void Scene::GenControlPoints()
	{
		for (int i = 0; i < YSections; i++)
		{
			for (int j = 0; j < XSections; j++)
			{
				ControlPoints.push_back({ (2.0 * ((float)j / (XSections - 1))) - 1, -0.5,  (2.0 * ((float)i / (YSections - 1))) - 1 });
			}
		}
	}

	//Constructor does normal constructor stuff
	Scene::Scene() : Mesh(4096), Radius(4.0), Precision(10)
	{
		glEnable(GL_DEPTH_TEST);

		GenControlPoints();

		m_Proj = glm::perspective(95.0f, (float)HEADER::WINDOW_WIDTH / HEADER::WINDOW_HEIGHT, 0.1f, 100.0f);
		CamPos = glm::vec3(0.0, 0.0, 1.0) * Radius;
		m_View = glm::lookAt(CamPos, glm::vec3(0.0, 0.0, -4.0), glm::vec3(0.0, 1.0, 0.0));
		m_Model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));
		m_MVP = m_Proj * m_View * m_Model;

		m_Shader = std::make_unique<Shader>("res/shaders/3DShader.shader"); //3D shader initialised here

		m_Shader->Bind();
		m_Shader->SetUniformMat4f("u_MVP", m_MVP);

	}

	//Interpolate between the points on one axis, don't worry about it, it's kinda complicated
	glm::vec3 Scene::Interpolate(glm::vec3 *arr, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, float a, int i, int sect, int off)
	{
		glm::vec3 f = (a * (v2 - v1)) + v1;
		glm::vec3 g = (a * (v3 - v2)) + v2;
		glm::vec3 P1 = (a * (g - f)) + f;
		if (i - off + 3 < sect)
		{
			glm::vec3 P2 = Interpolate(arr, arr[i + 1], arr[i + 2], arr[i + 3], a, i + 1, sect, off);
			return (a * (P2 - P1)) + P1;
		}
		return P1;
	}

	//Generates actual points
	void Scene::GenPoints()
	{
		int timesx = 0, timesy = 0;

		for (float i = 0.0f; timesx <= Precision; i += Period)
		{
			glm::vec3 *Pxs = (glm::vec3 *)malloc(YSections * sizeof(glm::vec3));
			for (int k = 0; k < YSections; k++)
			{
				int ind = k * XSections;
				Pxs[k] = Interpolate(&ControlPoints[0], ControlPoints[ind], ControlPoints[ind + 1], ControlPoints[ind + 2], i, ind, XSections, ind);
			}

			for (float j = 0.0f; timesy <= Precision; j += Period)
			{
				glm::vec3 P = Interpolate(Pxs, Pxs[0], Pxs[1], Pxs[2], j, 0, YSections, 0);

				Mesh.NewVertex({ {P.x, P.y, P.z}, {0.9, 0.0, 0.3, 1.0} });

				timesy++;
			}
			free(Pxs);
			timesy = 0;
			timesx++;

		}
	}

	//Generates quads, or fills up the index buffer
	void Scene::GenQuads()
	{
		for (int i = 0, l = Mesh.Points.size() - Precision - 1; i < l; i++)
		{
			Mesh.NewQuad(i, i + 1, i + Precision + 1, i + Precision);
		}
	}

	//We do not need it currently, but it is here just in case we might need it
	Scene::~Scene()
	{
		
	}

	void Scene::OnUpdate(float deltaTime)
	{
		//Checks whether the object has to be recalculated
		if (ReMesh)
		{
			ControlPoints.clear();
			GenControlPoints();
			HasChanged = true;
		}
		if (HasChanged) 
		{
			Mesh.ResetPointData();
			Mesh.ResetIndexdata();
			GenPoints();
			GenQuads();
		}

		Mesh.FillBuffers();

		glm::vec3 axis_y(0, 1, 0);
		//if (HEADER::IS_MOUSE_DOWN) <====== This is an expirimental feature, if your click detection doesn't work correctly, uncomment this
		{
			AngleX = (AngleX + (HEADER::MOUSE_X - HEADER::MOUSE_PREV_X) / HEADER::WINDOW_WIDTH * 360.0f);
			AngleY = fmax(fmin(AngleY + (HEADER::MOUSE_Y - HEADER::MOUSE_PREV_Y) / HEADER::WINDOW_HEIGHT * 270.0, 89.99), -89.99);

			glm::mat4 matx = glm::rotate(glm::mat4(1.0), glm::radians(-AngleX), axis_y);
			glm::vec4 vecy = glm::normalize(glm::vec4(0, sin(glm::radians(AngleY)), cos(glm::radians(AngleY)), 0));

			glm::vec4 vec = glm::normalize(glm::vec4(matx * vecy));

			CamPos = vec * (4.0f + Radius);
			m_View = glm::lookAt(CamPos, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

			m_MVP = m_Proj * m_View * m_Model;
			m_Shader->SetUniformMat4f("u_MVP", m_MVP);
		}

		//Your function runs HERE
		SelectedPoint = &ControlPoints[0]; //after you are done writing the function just tell me to remove this line or remove it yourself
		UpdateSelectedPoint();

	}
	void Scene::OnRender()
	{
		GLCall(glClearColor(0.99, 0.99, 1.0, 1.0)); //background color, this might change, idk maybe we will make light and dark themes
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		Renderer renderer;

		renderer.Draw(*Mesh.VertexArr, *Mesh.IndexBuff, *m_Shader);
	}
	void Scene::OnImGuiRender()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New"))
				{
					ResetSceneObject();
				}
				if (ImGui::MenuItem("Save as"))
				{
					SaveFile();
				}
				if (ImGui::MenuItem("Open"))
				{
					OpenFile();
				}
				if (ImGui::MenuItem("Export as whatever")) //change this to obj or fbx, whichever yiu will implement
				{
					ExportFile();
				}
				if (ImGui::MenuItem("Import a whatever"))
				{
					ImportFile();
				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		if (ImGui::Button("Reset"))
		{
			glm::vec3 axis_y(0, 1, 0);
			AngleX = 0.0f;
			AngleY = 0.0f;
			Radius = 4.0f;
			glm::mat4 matx = glm::rotate(glm::mat4(1.0), glm::radians(-AngleX), axis_y);
			glm::vec4 vecy = glm::normalize(glm::vec4(0, sin(glm::radians(AngleY)), cos(glm::radians(AngleY)), 0));

			glm::vec4 vec = glm::normalize(glm::vec4(matx * vecy));                        

			CamPos = vec * (4.0f + Radius);
			m_View = glm::lookAt(CamPos, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

			m_MVP = m_Proj * m_View * m_Model;
			m_Shader->SetUniformMat4f("u_MVP", m_MVP);
		}
		ImGui::SliderFloat("Distance", &Radius, 1.0, 10.0);
		int prex = XSections;
		ImGui::SliderInt("X sections", &XSections, 3, 6);
		int prey = YSections;
		ImGui::SliderInt("Y sections", &YSections, 3, 6);
		int pres = Precision;
		ImGui::SliderInt("Quality", &Precision, 1, 30);
		glm::vec3 prevVec = *SelectedPoint;
		ImGui::SliderFloat3("Coords", &(SelectedPoint->x), -6.0, 6.0);
		//Check whether the parameters have changed
		if (prex != XSections || prey != YSections)
			ReMesh = true;
		else
			ReMesh = false;
		if (pres != Precision || prevVec != *SelectedPoint)
			HasChanged = true;
		else
			HasChanged = false;
		Period = 1.0 / (double)Precision;
	}
}