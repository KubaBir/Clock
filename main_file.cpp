#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "myCube.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

float speed_x = 0;
float speed_y = 0;
float speed_arrows = 0;
float aspectRatio = 1;

std::vector<glm::vec4> clockVerts;
std::vector<glm::vec4> clockNorms;
std::vector<glm::vec2> clockTexCoords;
std::vector<unsigned int> clockIndices;

std::vector<glm::vec4> faceVerts;
std::vector<glm::vec4> faceNorms;
std::vector<glm::vec2> faceTexCoords;
std::vector<unsigned int> faceIndices;

std::vector<glm::vec4> arrow1Verts;
std::vector<glm::vec4> arrow1Norms;
std::vector<glm::vec2> arrow1TexCoords;
std::vector<unsigned int> arrow1Indices;

std::vector<glm::vec4> arrow2Verts;
std::vector<glm::vec4> arrow2Norms;
std::vector<glm::vec2> arrow2TexCoords;
std::vector<unsigned int> arrow2Indices;

std::vector<glm::vec4> bingBongVerts;
std::vector<glm::vec4> bingBongNorms;
std::vector<glm::vec2> bingBongTexCoords;
std::vector<unsigned int> bingBongIndices;

std::vector<glm::vec4> gear1Verts;
std::vector<glm::vec4> gear1Norms;
std::vector<glm::vec2> gear1TexCoords;
std::vector<unsigned int> gear1Indices;

std::vector<glm::vec4> gear2Verts;
std::vector<glm::vec4> gear2Norms;
std::vector<glm::vec2> gear2TexCoords;
std::vector<unsigned int> gear2Indices;

std::vector<glm::vec4> gear3Verts;
std::vector<glm::vec4> gear3Norms;
std::vector<glm::vec2> gear3TexCoords;
std::vector<unsigned int> gear3Indices;

std::vector<glm::vec4> gear4Verts;
std::vector<glm::vec4> gear4Norms;
std::vector<glm::vec2> gear4TexCoords;
std::vector<unsigned int> gear4Indices;


ShaderProgram* sp;
GLuint texWood;
GLuint texFace;
GLuint texBingBong;
GLuint texSilver;
GLuint texGold;




GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);
	//Wczytanie do pamięci komputera
	std::vector<unsigned char> image; //Alokuj wektor do wczytania obrazka
	unsigned width, height; //Zmienne do których wczytamy wymiary obrazka
	//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);
	//Import do pamięci karty graficznej
	glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
	//Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return tex;
}

void loadModel(std::string plik) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(plik, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

	if (!scene) {
		std::cout << "Blad wczytywania pliku modelu" << std::endl;
		return;
	}
	else std::cout << "Pobrano model" << std::endl;

	aiMesh* mesh;


	//body
	mesh = scene->mMeshes[1];
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i];
		clockVerts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		aiVector3D normal = mesh->mNormals[i];
		clockNorms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		clockTexCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace& face = mesh->mFaces[i];

		for (int j = 0; j < face.mNumIndices; j++) {
			clockIndices.push_back(face.mIndices[j]);
		}
	}



	//face
	mesh = scene->mMeshes[2];
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i];
		faceVerts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		aiVector3D normal = mesh->mNormals[i];
		faceNorms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		faceTexCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace& face = mesh->mFaces[i];

		for (int j = 0; j < face.mNumIndices; j++) {
			faceIndices.push_back(face.mIndices[j]);
		}
	}

	//arrow1
	mesh = scene->mMeshes[0];
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i];
		arrow1Verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		aiVector3D normal = mesh->mNormals[i];
		arrow1Norms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		arrow1TexCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace& face = mesh->mFaces[i];

		for (int j = 0; j < face.mNumIndices; j++) {
			arrow1Indices.push_back(face.mIndices[j]);
		}
	}

	//arrow2
	mesh = scene->mMeshes[4];
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i];
		arrow2Verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		aiVector3D normal = mesh->mNormals[i];
		arrow2Norms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		arrow2TexCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace& face = mesh->mFaces[i];

		for (int j = 0; j < face.mNumIndices; j++) {
			arrow2Indices.push_back(face.mIndices[j]);
		}
	}


	//BingBong
	mesh = scene->mMeshes[3];
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i];
		bingBongVerts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		aiVector3D normal = mesh->mNormals[i];
		bingBongNorms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		bingBongTexCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace& face = mesh->mFaces[i];

		for (int j = 0; j < face.mNumIndices; j++) {
			bingBongIndices.push_back(face.mIndices[j]);
		}
	}
	//gear
	



}


void loadGears(std::string plik) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(plik, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
	if (!scene) {
		std::cout << "Blad wczytywania pliku modelu" << std::endl;
		return;
	}
	else std::cout << "Pobrano model" << std::endl;

	aiMesh* mesh;


	mesh = scene->mMeshes[0];
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i];
		gear1Verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		aiVector3D normal = mesh->mNormals[i];
		gear1Norms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		gear1TexCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace& face = mesh->mFaces[i];

		for (int j = 0; j < face.mNumIndices; j++) {
			gear1Indices.push_back(face.mIndices[j]);
		}
	}

	mesh = scene->mMeshes[1];
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i];
		gear2Verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		aiVector3D normal = mesh->mNormals[i];
		gear2Norms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		gear2TexCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace& face = mesh->mFaces[i];

		for (int j = 0; j < face.mNumIndices; j++) {
			gear2Indices.push_back(face.mIndices[j]);
		}
	}

	mesh = scene->mMeshes[2];
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i];
		gear3Verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		aiVector3D normal = mesh->mNormals[i];
		gear3Norms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		gear3TexCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace& face = mesh->mFaces[i];

		for (int j = 0; j < face.mNumIndices; j++) {
			gear3Indices.push_back(face.mIndices[j]);
		}
	}

	mesh = scene->mMeshes[3];
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i];
		gear4Verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		aiVector3D normal = mesh->mNormals[i];
		gear4Norms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		gear4TexCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace& face = mesh->mFaces[i];

		for (int j = 0; j < face.mNumIndices; j++) {
			gear4Indices.push_back(face.mIndices[j]);
		}
	}
}

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) speed_x = -PI / 2;
		if (key == GLFW_KEY_RIGHT) speed_x = PI / 2;
		if (key == GLFW_KEY_UP) speed_y = PI / 2;
		if (key == GLFW_KEY_DOWN) speed_y = -PI / 2;
		if (key == GLFW_KEY_D) speed_arrows = PI * 1.2;
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT) speed_x = 0;
		if (key == GLFW_KEY_RIGHT) speed_x = 0;
		if (key == GLFW_KEY_UP) speed_y = 0;
		if (key == GLFW_KEY_DOWN) speed_y = 0;
		if (key == GLFW_KEY_D) speed_arrows = 0;
	}
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	if (height == 0) return;
	aspectRatio = (float)width / (float)height;
	glViewport(0, 0, width, height);
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0.3, 0.3, 0.7, 1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);

	sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
	texFace = readTexture("face.png");
	texWood = readTexture("wood.png");
	texBingBong = readTexture("bingBong.png");
	texSilver = readTexture("silver.png");
	texGold = readTexture("gold.png");



	loadModel("clock.obj");
	loadGears("gears.obj");
}

//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************
	delete sp;
}


void drawClock(GLFWwindow* window, glm::mat4 M, float angle_arrows) {
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

	glm::mat4 M_arrow_hours = M;
	glm::mat4 M_arrow_minutes = M;

	// CLOCK LOADING
	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, clockVerts.data());

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, clockNorms.data());

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 4, GL_FLOAT, false, 0, clockTexCoords.data());

	glUniform1i(sp->u("textureMap0"), 0);
	glUniform1i(sp->u("textureMap1"), 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texWood);

	glDrawElements(GL_TRIANGLES, clockIndices.size(), GL_UNSIGNED_INT, clockIndices.data()); //Narysuj obiekt
	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("normal"));
	glDisableVertexAttribArray(sp->a("texCoord0"));


	// FACE LOADING
	glEnableVertexAttribArray(sp->a("vertex"));  //Włącz przesyłanie danych do atrybutu vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, faceVerts.data()); //Wskaż tablicę z danymi dla atrybutu vertex

	glEnableVertexAttribArray(sp->a("normal"));  //Włącz przesyłanie danych do atrybutu vertex
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, faceNorms.data()); //Wskaż tablicę z danymi dla atrybutu vertex

	glEnableVertexAttribArray(sp->a("texCoord0"));  //Włącz przesyłanie danych do atrybutu vertex
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, faceTexCoords.data()); //Wskaż tablicę z danymi dla atrybutu vertex

	glUniform1i(sp->u("textureMap0"), 0);
	glUniform1i(sp->u("textureMap1"), 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texFace);

	glDrawElements(GL_TRIANGLES, faceIndices.size(), GL_UNSIGNED_INT, faceIndices.data()); //Narysuj obiekt
	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("normal"));
	glDisableVertexAttribArray(sp->a("texCoord0"));


	// BINGBONG LOADING
	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, bingBongVerts.data());

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, bingBongNorms.data());

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 4, GL_FLOAT, false, 0, bingBongTexCoords.data());

	glUniform1i(sp->u("textureMap0"), 0);
	glUniform1i(sp->u("textureMap1"), 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texBingBong);

	glDrawElements(GL_TRIANGLES, bingBongIndices.size(), GL_UNSIGNED_INT, bingBongIndices.data()); //Narysuj obiekt
	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("normal"));
	glDisableVertexAttribArray(sp->a("texCoord0"));

	// ARROW1 LOADING
	M_arrow_hours = glm::translate(M_arrow_hours, glm::vec3(0.3f, 0.0f, 0.0f));
	M_arrow_hours = glm::rotate(M_arrow_hours, angle_arrows, glm::vec3(0.0f, 0.0f, -1.0f)); //Wylicz macierz modelu
	M_arrow_hours = glm::translate(M_arrow_hours, glm::vec3(-0.3f, 0.0f, 0.0f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_arrow_hours));
	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, arrow1Verts.data());

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, arrow1Norms.data());

	glDrawElements(GL_TRIANGLES, arrow1Indices.size(), GL_UNSIGNED_INT, arrow1Indices.data()); //Narysuj obiekt

	glDisableVertexAttribArray(sp->a("vertex"));  //Wyłącz przesyłanie danych do atrybutu vertex
	glDisableVertexAttribArray(sp->a("normal"));  //Wyłącz przesyłanie danych do atrybutu vertex


	// ARROW2 LOADING
	M_arrow_minutes = glm::translate(M_arrow_minutes, glm::vec3(0.3f, 0.0f, 0.0f));
	M_arrow_minutes = glm::rotate(M_arrow_minutes, angle_arrows / 60, glm::vec3(0.0f, 0.0f, -1.0f)); //Wylicz macierz modelu
	M_arrow_minutes = glm::translate(M_arrow_minutes, glm::vec3(-0.3f, 0.0f, 0.0f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_arrow_minutes));
	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, arrow2Verts.data());

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, arrow2Norms.data());

	glDrawElements(GL_TRIANGLES, arrow2Indices.size(), GL_UNSIGNED_INT, arrow2Indices.data()); //Narysuj obiekt

	glDisableVertexAttribArray(sp->a("vertex"));  //Wyłącz przesyłanie danych do atrybutu vertex
	glDisableVertexAttribArray(sp->a("normal"));  //Wyłącz przesyłanie danych do atrybutu vertex
}

void drawGears(GLFWwindow* window, glm::mat4 M, float angle_arrows) {
	M = glm::scale(M, glm::vec3(3.0, 3.0, 3.0));
	glm::mat4 GearSmall1 = M;
	glm::mat4 GearSmall2 = M;
	glm::mat4 GearBig1 = M;
	glm::mat4 GearBig2 = M;


	// SMALL GEAR 1
	GearSmall1 = glm::rotate(GearSmall1, angle_arrows, glm::vec3(0.0f, 0.0f, 1.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(GearSmall1));
	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, gear4Verts.data());

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, gear4Norms.data());

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 4, GL_FLOAT, false, 0, gear4TexCoords.data());

	glUniform1i(sp->u("textureMap0"), 0);
	glUniform1i(sp->u("textureMap1"), 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texGold);

	glDrawElements(GL_TRIANGLES, gear4Indices.size(), GL_UNSIGNED_INT, gear4Indices.data()); //Narysuj obiekt
	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("normal"));
	glDisableVertexAttribArray(sp->a("texCoord0"));


	// BIG GEAR 1
	GearBig1 = glm::rotate(GearBig1, angle_arrows / 16, glm::vec3(0.0f, 0.0f, 1.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(GearBig1));
	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, gear2Verts.data());

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, gear2Norms.data());

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 4, GL_FLOAT, false, 0, gear2TexCoords.data());

	glUniform1i(sp->u("textureMap0"), 0);
	glUniform1i(sp->u("textureMap1"), 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texSilver);

	glDrawElements(GL_TRIANGLES, gear2Indices.size(), GL_UNSIGNED_INT, gear2Indices.data()); //Narysuj obiekt
	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("normal"));
	glDisableVertexAttribArray(sp->a("texCoord0"));

	// BIG GEAR 2
	GearBig2 = glm::translate(GearBig2, glm::vec3(1.35, 0, 0));
	GearBig2 = glm::rotate(GearBig2, angle_arrows / 4, glm::vec3(0.0f, 0.0f, -1.0f)); //Wylicz macierz modelu
	GearBig2 = glm::translate(GearBig2, glm::vec3(-1.35, 0, 0));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(GearBig2));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, gear3Verts.data());

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, gear3Norms.data());

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 4, GL_FLOAT, false, 0, gear3TexCoords.data());

	glUniform1i(sp->u("textureMap0"), 0);
	glUniform1i(sp->u("textureMap1"), 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texSilver);

	glDrawElements(GL_TRIANGLES, gear3Indices.size(), GL_UNSIGNED_INT, gear3Indices.data()); //Narysuj obiekt
	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("normal"));
	glDisableVertexAttribArray(sp->a("texCoord0"));



	// SMALL GEAR 2
	GearSmall2 = glm::translate(GearSmall2, glm::vec3(1.35, 0, 0));
	GearSmall2 = glm::rotate(GearSmall2, angle_arrows / 4, glm::vec3(0.0f, 0.0f, -1.0f)); //Wylicz macierz modelu
	GearSmall2 = glm::translate(GearSmall2, glm::vec3(-1.35, 0, 0));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(GearSmall2));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, gear1Verts.data());

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, gear1Norms.data());

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 4, GL_FLOAT, false, 0, gear1TexCoords.data());

	glUniform1i(sp->u("textureMap0"), 0);
	glUniform1i(sp->u("textureMap1"), 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texGold);

	glDrawElements(GL_TRIANGLES, gear1Indices.size(), GL_UNSIGNED_INT, gear1Indices.data()); //Narysuj obiekt
	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("normal"));
	glDisableVertexAttribArray(sp->a("texCoord0"));
}

//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float angle_x, float angle_y, float angle_arrows) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V = glm::lookAt(
		glm::vec3(0.0f, -7.0f, -25.0f),
		glm::vec3(0.0f, -7.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz widoku

	glm::mat4 P = glm::perspective(50.0f * PI / 180.0f, aspectRatio, 0.01f, 450.0f); //Wylicz macierz rzutowania
	
	//glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f);
	sp->use();//Aktywacja programu cieniującego
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));


	glm::mat4 M = glm::mat4(1.0f);
	
	/*M = glm::rotate(M, PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));
	M = glm::rotate(M, -PI / 2, glm::vec3(0.0f, 0.0f, 1.0f));*/

	M = glm::rotate(M, angle_y, glm::vec3(0.0f, 0.0f, 1.0f)); //Wylicz macierz modelu
	M = glm::rotate(M, angle_x, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	

	sp->use();//Aktywacja programu cieniującego
	//Przeslij parametry programu cieniującego do karty graficznej
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));

	//drawClock(window, M, angle_arrows);
	drawGears(window, M, angle_arrows);

	glfwSwapBuffers(window); //Przerzuć tylny bufor na przedni
}




int main(void) {
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno
	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów
	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}
	window = glfwCreateWindow(1000, 1000, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.
	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora
	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}
	initOpenGLProgram(window); //Operacje inicjujące

	//Główna pętla
	float angle_x = 0; //Aktualny kąt obrotu obiektu
	float angle_y = 0; //Aktualny kąt obrotu obiektu
	float angle_arrow = 0; //Aktualny kąt obrotu obiektu
	float angle_arrow_y = 0; //Aktualny kąt obrotu obiektu
	glfwSetTime(0); //Zeruj timer
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		angle_x += speed_x * glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
		angle_y += speed_y * glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
		angle_arrow += speed_arrows * glfwGetTime();
		

		glfwSetTime(0); //Zeruj timer
		drawScene(window, angle_x, angle_y,angle_arrow); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}