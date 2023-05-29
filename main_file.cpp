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
bool show_clock = true;

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


std::vector<glm::vec4> coneVerts;
std::vector<glm::vec4> coneNorms;
std::vector<glm::vec2> coneTexCoords;
std::vector<unsigned int> coneIndices;

std::vector<glm::vec4> gearMainBigVerts;
std::vector<glm::vec4> gearMainBigNorms;
std::vector<glm::vec2> gearMainBigTexCoords;
std::vector<unsigned int> gearMainBigIndices;

std::vector<glm::vec4> gearSmall20Verts;
std::vector<glm::vec4> gearSmall20Norms;
std::vector<glm::vec2> gearSmall20TexCoords;
std::vector<unsigned int> gearSmall20Indices;

std::vector<glm::vec4> gearBig5Verts;
std::vector<glm::vec4> gearBig5Norms;
std::vector<glm::vec2> gearBig5TexCoords;
std::vector<unsigned int> gearBig5Indices;

std::vector<glm::vec4> gearSmall5Verts;
std::vector<glm::vec4> gearSmall5Norms;
std::vector<glm::vec2> gearSmall5TexCoords;
std::vector<unsigned int> gearSmall5Indices;

std::vector<glm::vec4> gearBig20Verts;
std::vector<glm::vec4> gearBig20Norms;
std::vector<glm::vec2> gearBig20TexCoords;
std::vector<unsigned int> gearBig20Indices;

std::vector<glm::vec4> gearMainSmallVerts;
std::vector<glm::vec4> gearMainSmallNorms;
std::vector<glm::vec2> gearMainSmallTexCoords;
std::vector<unsigned int> gearMainSmallIndices;

std::vector<glm::vec4> gearSmall60Verts;
std::vector<glm::vec4> gearSmall60Norms;
std::vector<glm::vec2> gearSmall60TexCoords;
std::vector<unsigned int> gearSmall60Indices;

std::vector<glm::vec4> shaftVerts;
std::vector<glm::vec4> shaftNorms;
std::vector<glm::vec2> shaftTexCoords;
std::vector<unsigned int> shaftIndices;



ShaderProgram* sp;
GLuint texWood;
GLuint texFace;
GLuint texBingBong;
GLuint texSilver;
GLuint texGold;
GLuint texArrow;


float bingBong_pos = 0.5;
float bingBong_dir = 0.1;

float calc_acceleration(float t){
	return -t*(t-1);
}

glm::mat4 calc_bingBong(glm::mat4 bingBong) {

	if (bingBong_pos < 0.0) bingBong_dir = 0.1;
	else if (bingBong_pos > 1.0) bingBong_dir = -0.1;

	float movement;
	if (speed_arrows > 0)
		movement = std::max(calc_acceleration(bingBong_pos), 0.005f) * bingBong_dir;
	else
		movement = 0;

	//std::cout << bingBong_pos << " " << movement << std::endl;

	bingBong = glm::rotate(bingBong, (bingBong_pos + movement)/5, glm::vec3(0.0, 0.0, 1.0));
	bingBong_pos += movement;
	
	return bingBong;
}

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
	//cone
	mesh = scene->mMeshes[5];
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i];
		coneVerts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		aiVector3D normal = mesh->mNormals[i];
		coneNorms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		coneTexCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace& face = mesh->mFaces[i];

		for (int j = 0; j < face.mNumIndices; j++) {
			coneIndices.push_back(face.mIndices[j]);
		}
	}
	



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
		gearMainBigVerts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		aiVector3D normal = mesh->mNormals[i];
		gearMainBigNorms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		gearMainBigTexCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace& face = mesh->mFaces[i];

		for (int j = 0; j < face.mNumIndices; j++) {
			gearMainBigIndices.push_back(face.mIndices[j]);
		}
	}

	mesh = scene->mMeshes[1];
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i];
		gearSmall20Verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		aiVector3D normal = mesh->mNormals[i];
		gearSmall20Norms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		gearSmall20TexCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace& face = mesh->mFaces[i];

		for (int j = 0; j < face.mNumIndices; j++) {
			gearSmall20Indices.push_back(face.mIndices[j]);
		}
	}

	mesh = scene->mMeshes[2];
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i];
		gearBig5Verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		aiVector3D normal = mesh->mNormals[i];
		gearBig5Norms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		gearBig5TexCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace& face = mesh->mFaces[i];

		for (int j = 0; j < face.mNumIndices; j++) {
			gearBig5Indices.push_back(face.mIndices[j]);
		}
	}

	mesh = scene->mMeshes[3];
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i];
		gearSmall5Verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		aiVector3D normal = mesh->mNormals[i];
		gearSmall5Norms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		gearSmall5TexCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace& face = mesh->mFaces[i];

		for (int j = 0; j < face.mNumIndices; j++) {
			gearSmall5Indices.push_back(face.mIndices[j]);
		}
	}

	mesh = scene->mMeshes[4];
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i];
		gearBig20Verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		aiVector3D normal = mesh->mNormals[i];
		gearBig20Norms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		gearBig20TexCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace& face = mesh->mFaces[i];

		for (int j = 0; j < face.mNumIndices; j++) {
			gearBig20Indices.push_back(face.mIndices[j]);
		}
	}

	mesh = scene->mMeshes[5];
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i];
		gearMainSmallVerts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		aiVector3D normal = mesh->mNormals[i];
		gearMainSmallNorms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		gearMainSmallTexCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace& face = mesh->mFaces[i];

		for (int j = 0; j < face.mNumIndices; j++) {
			gearMainSmallIndices.push_back(face.mIndices[j]);
		}
	}

	mesh = scene->mMeshes[6];
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i];
		gearSmall60Verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		aiVector3D normal = mesh->mNormals[i];
		gearSmall60Norms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		gearSmall60TexCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace& face = mesh->mFaces[i];

		for (int j = 0; j < face.mNumIndices; j++) {
			gearSmall60Indices.push_back(face.mIndices[j]);
		}
	}

	mesh = scene->mMeshes[7];
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i];
		shaftVerts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		aiVector3D normal = mesh->mNormals[i];
		shaftNorms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		shaftTexCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace& face = mesh->mFaces[i];

		for (int j = 0; j < face.mNumIndices; j++) {
			shaftIndices.push_back(face.mIndices[j]);
		}
	}
	
}

void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) speed_x = -PI / 2;
		if (key == GLFW_KEY_RIGHT) speed_x = PI / 2;
		if (key == GLFW_KEY_UP) speed_y = PI / 2;
		if (key == GLFW_KEY_DOWN) speed_y = -PI / 2;
		if (key == GLFW_KEY_D) speed_arrows = PI / 30;
		if (key == GLFW_KEY_SPACE) show_clock = false;
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT) speed_x = 0;
		if (key == GLFW_KEY_RIGHT) speed_x = 0;
		if (key == GLFW_KEY_UP) speed_y = 0;
		if (key == GLFW_KEY_DOWN) speed_y = 0;
		if (key == GLFW_KEY_D) speed_arrows = 0;
		if (key == GLFW_KEY_SPACE) show_clock = true;

	}
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	if (height == 0) return;
	aspectRatio = (float)width / (float)height;
	glViewport(0, 0, width, height);
}

void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0.3, 0.3, 0.7, 1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);

	sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
	texFace = readTexture("face.png");
	texWood = readTexture("wood.png");
	texSilver = readTexture("silver.png");
	texGold = readTexture("gold.png");
	texArrow = readTexture("arrow.png");




	loadModel("clock.obj");
	loadGears("gears1_60.obj");
}

void freeOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************
	delete sp;
}

void drawClock(GLFWwindow* window, glm::mat4 M, float angle_arrows) {
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

	glm::mat4 M_arrow_hours = M;
	glm::mat4 M_arrow_minutes = M;
	glm::mat4 BingBong = M;

	// CLOCK LOADING
	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, clockVerts.data());

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, clockNorms.data());

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, clockTexCoords.data());

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texWood);

	glDrawElements(GL_TRIANGLES, clockIndices.size(), GL_UNSIGNED_INT, clockIndices.data()); //Narysuj obiekt


	// FACE LOADING
	glEnableVertexAttribArray(sp->a("vertex"));  //Włącz przesyłanie danych do atrybutu vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, faceVerts.data()); //Wskaż tablicę z danymi dla atrybutu vertex

	glEnableVertexAttribArray(sp->a("normal"));  //Włącz przesyłanie danych do atrybutu vertex
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, faceNorms.data()); //Wskaż tablicę z danymi dla atrybutu vertex

	glEnableVertexAttribArray(sp->a("texCoord0"));  //Włącz przesyłanie danych do atrybutu vertex
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, faceTexCoords.data()); //Wskaż tablicę z danymi dla atrybutu vertex

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texFace);

	glDrawElements(GL_TRIANGLES, faceIndices.size(), GL_UNSIGNED_INT, faceIndices.data()); //Narysuj obiekt


	// BINGBONG LOADING
	BingBong = glm::rotate(BingBong, -0.1f, glm::vec3(0.0, 0.0, 1.0));

	BingBong = calc_bingBong(BingBong);
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(BingBong));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, bingBongVerts.data());

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, bingBongNorms.data());

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, bingBongTexCoords.data());

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texGold);

	glDrawElements(GL_TRIANGLES, bingBongIndices.size(), GL_UNSIGNED_INT, bingBongIndices.data()); //Narysuj obiekt


	// ARROW1 LOADING
	M_arrow_hours = glm::rotate(M_arrow_hours, angle_arrows * 60, glm::vec3(0.0f, 0.0f, 1.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_arrow_hours));
	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, arrow1Verts.data());

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, arrow1Norms.data());

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, arrow1TexCoords.data());

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texArrow);

	glDrawElements(GL_TRIANGLES, arrow1Indices.size(), GL_UNSIGNED_INT, arrow1Indices.data()); //Narysuj obiekt


	// ARROW2 LOADING
	M_arrow_minutes = glm::rotate(M_arrow_minutes, angle_arrows, glm::vec3(0.0f, 0.0f, 1.0f)); //Wylicz macierz modelu
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_arrow_minutes));
	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, arrow2Verts.data());

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, arrow2Norms.data());

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, arrow2TexCoords.data());

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texArrow);

	glDrawElements(GL_TRIANGLES, arrow2Indices.size(), GL_UNSIGNED_INT, arrow2Indices.data()); //Narysuj obiekt


	// Cone LOADING
	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, coneVerts.data());

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, coneNorms.data());

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, coneTexCoords.data());

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texArrow);

	glDrawElements(GL_TRIANGLES, coneIndices.size(), GL_UNSIGNED_INT, coneIndices.data()); //Narysuj obiekt
	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("normal"));
	glDisableVertexAttribArray(sp->a("texCoord0"));
}

void drawGears(GLFWwindow* window, glm::mat4 M, float angle_arrows) {
	//M = glm::scale(M, glm::vec3(3.0, 3.0, 3.0));
	glm::mat4 GearMainBig = M;
	glm::mat4 GearSmall20 = M;
	glm::mat4 GearBig5 = M;
	glm::mat4 GearSmall5 = M;
	glm::mat4 GearBig20 = M;
	glm::mat4 GearMainSmall = M;
	glm::mat4 GearSmall60 = M;
	glm::mat4 Shaft = M;

	// GEAR MAIN BIG
	GearMainBig = glm::rotate(GearMainBig, angle_arrows, glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(GearMainBig));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, gearMainBigVerts.data());

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, gearMainBigNorms.data());

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, gearMainBigTexCoords.data());

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texSilver);

	glDrawElements(GL_TRIANGLES, gearMainBigIndices.size(), GL_UNSIGNED_INT, gearMainBigIndices.data()); //Narysuj obiekt


	// GEAR SMALL 20
	GearSmall20 = glm::translate(GearSmall20, glm::vec3(1.3, -1.1, 0.0));
	GearSmall20 = glm::rotate(GearSmall20, angle_arrows * 20, glm::vec3(0.0f, 0.0f, 1.0f));
	GearSmall20 = glm::translate(GearSmall20, glm::vec3(-1.3, 1.1, 0.0));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(GearSmall20));
	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, gearSmall20Verts.data());

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, gearSmall20Norms.data());

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, gearSmall20TexCoords.data());

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texGold);

	glDrawElements(GL_TRIANGLES, gearSmall20Indices.size(), GL_UNSIGNED_INT, gearSmall20Indices.data()); //Narysuj obiekt


	// GEAR BIG 5
	GearBig5 = glm::translate(GearBig5, glm::vec3(1.3, 0, 0));
	GearBig5 = glm::rotate(GearBig5, angle_arrows * 5, glm::vec3(0.0f, 0.0f, -1.0f));
	GearBig5 = glm::translate(GearBig5, glm::vec3(-1.3, 0, 0));

	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(GearBig5));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, gearBig5Verts.data());

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, gearBig5Norms.data());

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, gearBig5TexCoords.data());

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texSilver);

	glDrawElements(GL_TRIANGLES, gearBig5Indices.size(), GL_UNSIGNED_INT, gearBig5Indices.data()); //Narysuj obiekt


	// GEAR SMALL 5
	GearSmall5 = glm::translate(GearSmall5, glm::vec3(1.3, 0, 0));
	GearSmall5 = glm::rotate(GearSmall5, angle_arrows * 5, glm::vec3(0.0f, 0.0f, -1.0f));
	GearSmall5 = glm::translate(GearSmall5, glm::vec3(-1.3, 0, 0));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(GearSmall5));
	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, gearSmall5Verts.data());

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, gearSmall5Norms.data());

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, gearSmall5TexCoords.data());

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texGold);

	glDrawElements(GL_TRIANGLES, gearSmall5Indices.size(), GL_UNSIGNED_INT, gearSmall5Indices.data()); //Narysuj obiekt


	// GEAR BIG 20
	GearBig20 = glm::translate(GearBig20, glm::vec3(1.3, -1.1, 0.0));
	GearBig20 = glm::rotate(GearBig20, angle_arrows * 20, glm::vec3(0.0f, 0.0f, 1.0f));
	GearBig20 = glm::translate(GearBig20, glm::vec3(-1.3, +1.1, 0.0));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(GearBig20));
	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, gearBig20Verts.data());

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, gearBig20Norms.data());

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, gearBig20TexCoords.data());

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texSilver);

	glDrawElements(GL_TRIANGLES, gearBig20Indices.size(), GL_UNSIGNED_INT, gearBig20Indices.data()); //Narysuj obiekt


	// GEAR MAIN SMALL
	GearMainSmall = glm::rotate(GearMainSmall, angle_arrows * 60, glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(GearMainSmall));
	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, gearMainSmallVerts.data());

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, gearMainSmallNorms.data());

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, gearMainSmallTexCoords.data());

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texGold);

	glDrawElements(GL_TRIANGLES, gearMainSmallIndices.size(), GL_UNSIGNED_INT, gearMainSmallIndices.data()); //Narysuj obiekt


	// GEAR SMALL 60
	GearSmall60 = glm::translate(GearSmall60, glm::vec3(0.07, -0.7, 0.0));
	GearSmall60 = glm::rotate(GearSmall60, angle_arrows * 60, glm::vec3(0.0f, 0.0f, -1.0f));
	GearSmall60 = glm::translate(GearSmall60, glm::vec3(-0.07, 0.7, 0.0));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(GearSmall60));
	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, gearSmall60Verts.data());

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, gearSmall60Norms.data());

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, gearSmall60TexCoords.data());

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texGold);

	glDrawElements(GL_TRIANGLES, gearSmall60Indices.size(), GL_UNSIGNED_INT, gearSmall60Indices.data()); //Narysuj obiekt


	// GEAR MAIN SMALL
	Shaft = glm::rotate(Shaft, angle_arrows * 60, glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Shaft));
	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, shaftVerts.data());

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, shaftNorms.data());

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, shaftTexCoords.data());

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texGold);

	glDrawElements(GL_TRIANGLES, shaftIndices.size(), GL_UNSIGNED_INT, shaftIndices.data()); //Narysuj obiekt
	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("normal"));
	glDisableVertexAttribArray(sp->a("texCoord0"));
}

//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float angle_x, float angle_y, float angle_arrows) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V = glm::lookAt(
		glm::vec3(0.0f, -5.0f, -25.0f),
		glm::vec3(0.0f, -5.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 P = glm::perspective(50.0f * PI / 180.0f, aspectRatio, 0.01f, 450.0f);
	
	sp->use();//Aktywacja programu cieniującego
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));

	glm::mat4 M = glm::mat4(1.0f);
	
	M = glm::rotate(M, angle_y, glm::vec3(1.0f, 0.0f, 0.0f)); //Wylicz macierz modelu
	M = glm::rotate(M, angle_x, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	
	sp->use();//Aktywacja programu cieniującego
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));

	if (show_clock) {
		drawClock(window, M, angle_arrows);
		drawGears(window, M, angle_arrows);
	}
	else {
		M = glm::scale(M, glm::vec3(3.0, 3.0, 3.0));
		drawGears(window, M, angle_arrows);
	}

	

	glfwSwapBuffers(window);
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