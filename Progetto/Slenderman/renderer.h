#pragma once
#include "shader_m.h"
#include "constants.h"
#include <glm/gtc/matrix_transform.hpp>
#include "render_text.h"
#include <sstream>
#include <string>
#include "light_utils.h"
#include <math.h>

class Renderer {

public:
	Renderer(vector<glm::vec3> poiTranslations) {
		pointOfinterestTranslationVec = poiTranslations;
		lightUtils = LightUtils(pointOfinterestTranslationVec);
		posViewedPage = -1;
	};
	bool lightOn;
	glm::mat4 view;
	glm::mat4 projection;
	vector<glm::vec3> pointOfinterestTranslationVec;
	LightUtils lightUtils;
	int posViewedPage;
	float renderPageTimer;

	void renderFloor(Shader& floorShader, unsigned int& floorTexture, unsigned int& floorVAO);
	void renderPages(Shader& pageShader, Shader& shaderSingleColor, vector<unsigned int>& pageTextures, vector<int>& pageIndexPosition, vector<bool>& collectedPagesIndices,  unsigned int& pageVAO);
	void renderForest(Shader& forestShader, Model& treeModel, Camera& camera, vector<int>& positionsPointOfinterest);
	void renderFence(Shader& fenceShader, unsigned int& fenceTexture, Model& fenceModel);
	void renderGrass(Shader& grassShader, Model& grassModel, Camera& camera);
	void renderSlenderman(Shader& slenderShader, unsigned int& slenderTexture, Model& slenderModel, glm::vec3& translationMatrix);
	void renderStreetlight(Shader& streetlightShader, unsigned int& streetlightTexture, Model& streetlightModel);
	void renderPointsOfInterest(Shader& pointOfInterestShader, vector<Model>& pointOfInterestModels, vector<unsigned int>& pointOfInterestTexture, vector<glm::mat4>& modelPoiMatrices);
	void renderFlashlight(Shader& flashlightShader, unsigned int& flashlightTexture, Model& flashlightModel);
	void renderInfo(Camera& camera, int fps);
	void findLookingPage(Camera& camera, vector<int> pageIndexPosition);
	void renderPageMessage(int& actualPage);
	void buildMiniMap(unsigned int& framebuffer, Shader& shaderSingleColor, unsigned int& minimapWoodVAO, unsigned int& woodMinimapTexture, Shader& circleMinimapShader, unsigned int& circleVAO);
	void renderMiniMap(Shader& minimapShader, unsigned int& minimapVAO, unsigned int& textureColorBuffer);

private:
	void renderDynamicMap(Shader& shader, Model& modelObj, vector<int>& VAO_indexes, int quadSide, int vaoObjectSide);
	vector<int> getVaoIndexesFromCamera(Camera& camera, float offset, int quadSide, int vaoObjectSide);

};

void Renderer::renderFloor(Shader& floorShader, unsigned int& floorTexture, unsigned int& floorVAO) {
	lightUtils.initLightShader(floorShader, lightOn, camera);
	floorShader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floorTexture);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -4.0f, 0.0f));
	floorShader.setMat4("model", model);
	floorShader.setMat4("view", view);
	floorShader.setMat4("projection", projection);
	glBindVertexArray(floorVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::renderPages(Shader& pageShader, Shader& shaderSingleColor, vector<unsigned int>& pageTextures, vector<int>& pageIndexPosition, vector<bool>& collectedPagesIndices, unsigned int& pageVAO) {
	lightUtils.initLightShader(pageShader, lightOn, camera);
	pageShader.use();
	pageShader.setMat4("view", view);
	pageShader.setMat4("projection", projection);

	//DEBUG TODO: Commentare
	if (DEBUG) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, pageTextures[0]);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(STREETLIGHT_POI_OFFSET, 0.0f, STREETLIGHT_POI_OFFSET));
		model = glm::translate(model, glm::vec3(40.0f, -4.0f, 40.0f));
		model = glm::translate(model, glm::vec3(0.7f, 4.3f, 0.0f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.4f));
		model = glm::rotate(model, (float)glm::radians(5.0), glm::vec3(0.0f, 1.0f, 1.0f));
		model = glm::rotate(model, (float)glm::radians(270.0), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(0.0f, 0.0f, 1.0f));
		pageShader.setMat4("model", model);
		glBindVertexArray(pageVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	//
	
	for (int i = 0; i < pageIndexPosition.size(); i++) {
		if (collectedPagesIndices[i]) {
			// Disegna solo le pagine non collezionate
			continue;
		}
		int pageIndex = pageIndexPosition[i];
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, pageTextures[i]);
		glm::mat4 model = glm::mat4(1.0f);
		//TODO: Decidere se la pagina deve essere attaccata al lampione o al punto di interesse
		model = glm::translate(model, glm::vec3(STREETLIGHT_POI_OFFSET, 0.0f, STREETLIGHT_POI_OFFSET));
		model = glm::translate(model, pointOfinterestTranslationVec[pageIndex]);
		model = glm::translate(model, glm::vec3(0.7f, 4.3f, 0.0f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.4f));
		model = glm::rotate(model, (float)glm::radians(5.0), glm::vec3(0.0f, 1.0f, 1.0f));
		model = glm::rotate(model, (float)glm::radians(270.0), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(0.0f, 0.0f, 1.0f));
		pageShader.setMat4("model", model);
		if (i == posViewedPage) {
			glEnable(GL_STENCIL_TEST);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
		}
		
		glBindVertexArray(pageVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		if (i == posViewedPage) {
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

			glStencilMask(0x00);
			glDisable(GL_DEPTH_TEST);
			shaderSingleColor.use();
			shaderSingleColor.setMat4("view", view);
			shaderSingleColor.setMat4("projection", projection);
			float scale = 1.15f;
			model = glm::scale(model, glm::vec3(scale, scale, scale));
			shaderSingleColor.setMat4("model", model);
			glBindVertexArray(pageVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glStencilMask(0xFF);
			glStencilFunc(GL_ALWAYS, 0, 0xFF);
			
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_STENCIL_TEST);
		}
	}
	if (posViewedPage != -1 && !collectedPagesIndices[posViewedPage]) {
		RenderText("Click left mouse button to collect the page", (SCR_WIDTH / 2) - 250.0f, 200.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
	}
}

void Renderer::renderForest(Shader& forestShader, Model& treeModel, Camera& camera, vector<int>& positionsPointOfinterest) {

	lightUtils.initLightShader(forestShader, lightOn, camera);
	forestShader.use();
	forestShader.setMat4("projection", projection);
	forestShader.setMat4("view", view);
	forestShader.setFloat("alphaValue", 0.7f);

	vector<int> VAO_indexes = getVaoIndexesFromCamera(camera, TREE_OFFSET, TREE_QUAD_SIDE, VAO_OBJECTS_SIDE_TREE);
	for (int i = 0; i < positionsPointOfinterest.size(); i++) {
		int val = positionsPointOfinterest[i];
		auto it = find(VAO_indexes.begin(), VAO_indexes.end(), val);
		if (it != VAO_indexes.end()) {
			int deleteIndex = it - VAO_indexes.begin();
			VAO_indexes.erase(VAO_indexes.begin() + deleteIndex);
		}
	}

	//TODO Creare metodo per rifattorizzare il codice
	for (int i = 0; i < K_MAP_TO_EXCLUDE.size(); i++) {
		int val = K_MAP_TO_EXCLUDE[i];
		auto it = find(VAO_indexes.begin(), VAO_indexes.end(), val);
		if (it != VAO_indexes.end()) {
			int deleteIndex = it - VAO_indexes.begin();
			VAO_indexes.erase(VAO_indexes.begin() + deleteIndex);
		}
	}

	renderDynamicMap(forestShader, treeModel, VAO_indexes, TREE_QUAD_SIDE, VAO_OBJECTS_SIDE_TREE);
}

void Renderer::renderFence(Shader& fenceShader, unsigned int& fenceTexture, Model& fenceModel) {
	lightUtils.initLightShader(fenceShader, lightOn, camera);
	fenceShader.use();
	fenceShader.setMat4("projection", projection);
	fenceShader.setMat4("view", view);
	fenceShader.setFloat("alphaValue", 0.7f);
	int num_VAO = NUM_FENCES_FOR_SIDE * 4;
	//TODO: Renderizzare solo un sottoinsieme delle fence come per la foresta
	for (int k = 0; k < num_VAO; k++) {
		for (int i = 0; i < fenceModel.meshes.size(); i++) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, fenceTexture);
			glBindVertexArray(fenceModel.meshes[i].VAOs[k]);
			glDrawElements(GL_TRIANGLES, fenceModel.meshes[i].indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}
}

void Renderer::renderGrass(Shader& grassShader, Model& grassModel, Camera& camera) {
	lightUtils.initLightShader(grassShader, lightOn, camera);
	grassShader.use();
	grassShader.setMat4("projection", projection);
	grassShader.setMat4("view", view);
	grassShader.setFloat("alphaValue", 0.4f);

	vector<int> VAO_indexes = getVaoIndexesFromCamera(camera, GRASS_OFFSET, GRASS_QUAD_SIDE, VAO_OBJECTS_SIDE_GRASS);
	renderDynamicMap(grassShader, grassModel, VAO_indexes, GRASS_QUAD_SIDE, VAO_OBJECTS_SIDE_GRASS);
}


void Renderer::renderSlenderman(Shader& slenderShader, unsigned int& slenderTexture, Model& slenderModel, glm::vec3& translationMatrix) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, slenderTexture);
	lightUtils.initLightShader(slenderShader, lightOn, camera);
	slenderShader.use();
	slenderShader.setMat4("view", view);
	slenderShader.setMat4("projection", projection);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, translationMatrix);
	model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	slenderShader.setMat4("model", model);
	slenderModel.Draw(slenderShader);
}

void Renderer::renderStreetlight(Shader& streetlightShader, unsigned int& streetlightTexture, Model& streetlightModel) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, streetlightTexture);
	lightUtils.initLightShader(streetlightShader, lightOn, camera);
	streetlightShader.use();
	streetlightShader.setMat4("view", view);
	streetlightShader.setMat4("projection", projection);

	//DEBUG TODO: Commentare
	if (DEBUG) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(STREETLIGHT_POI_OFFSET, 0.0f, STREETLIGHT_POI_OFFSET));
		model = glm::translate(model, glm::vec3(40.0f, -4.0f, 40.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		streetlightShader.setMat4("model", model);
		streetlightModel.Draw(streetlightShader);
	}
	//

	for (int i = 0; i < pointOfinterestTranslationVec.size(); i++) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(STREETLIGHT_POI_OFFSET, 0.0f, STREETLIGHT_POI_OFFSET));
		model = glm::translate(model, pointOfinterestTranslationVec[i]);
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		streetlightShader.setMat4("model", model);
		streetlightModel.Draw(streetlightShader);
	}
}

void Renderer::renderPointsOfInterest(Shader& pointOfInterestShader, vector<Model>& pointOfInterestModels, vector<unsigned int>& pointOfInterestTexture, vector<glm::mat4>& modelPoiMatrices) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, pointOfInterestTexture[0]);
	lightUtils.initLightShader(pointOfInterestShader, lightOn, camera);
	pointOfInterestShader.use();
	pointOfInterestShader.setMat4("view", view);
	pointOfInterestShader.setMat4("projection", projection);

	//DEBUG TODO: Commentare
	int poiDebugPosition = 7;
	if (DEBUG) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, pointOfInterestTexture[poiDebugPosition]);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(40.0f, -4.0f, 40.0f));
		model = glm::translate(model, glm::vec3(-60.0f, 3.8f, 60.0f));
		model = glm::scale(model, glm::vec3(0.008f, 0.008f, 0.008f));
		model = glm::rotate(model, (float)glm::radians(270.0), glm::vec3(1.0f, 0.0f, 0.0f));
		pointOfInterestShader.setMat4("model", model);
		pointOfInterestModels[poiDebugPosition].Draw(pointOfInterestShader);
	}
	//

	for (int i = 0; i < modelPoiMatrices.size(); i++) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, pointOfInterestTexture[i]);
		pointOfInterestShader.setMat4("model", modelPoiMatrices[i]);
		pointOfInterestModels[i].Draw(pointOfInterestShader);
	}

}

void Renderer::renderFlashlight(Shader& flashlightShader, unsigned int& flashlightTexture, Model& flashlightModel) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, flashlightTexture);
	lightUtils.initLightShader(flashlightShader, lightOn, camera);
	flashlightShader.use();
	view = glm::mat4(1.0f);
	flashlightShader.setMat4("view", view);
	flashlightShader.setMat4("projection", projection);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	model = glm::translate(model, glm::vec3(-1.2f, -0.6f, -2.0f));
	model = glm::rotate(model, (float)glm::radians(30.0), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, (float)glm::radians(70.0), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, (float)glm::radians(50.0), glm::vec3(1.0f, 0.0f, 0.0f));
	flashlightShader.setMat4("model", model);
	flashlightModel.Draw(flashlightShader);
}

void Renderer::renderInfo(Camera& camera, int fps) {
	std::stringstream ssx;
	ssx << "x: " << camera.Position.x;
	std::string x = ssx.str();
	RenderText(x, SCR_WIDTH - 200.0f, 50.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
	std::stringstream ssz;
	ssz << "z: " << camera.Position.z;
	std::string z = ssz.str();
	RenderText(z, SCR_WIDTH - 200.0f, 70.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

	int numVAOForSide = TREE_QUAD_SIDE / VAO_OBJECTS_SIDE_TREE;
	vector<int> indexes = getVaoIndexesFromCamera(camera, TREE_OFFSET, TREE_QUAD_SIDE, VAO_OBJECTS_SIDE_TREE);
	int k_index = indexes[(indexes.size() - 1) / 2];
	int x_index = k_index / numVAOForSide;
	int z_index = k_index % numVAOForSide;

	std::stringstream ssk;
	ssk << "k_index: " << k_index;
	std::string k = ssk.str();
	RenderText(k, SCR_WIDTH - 200.0f, 120.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

	std::stringstream ssxindex;
	ssxindex << "x_index: " << x_index;
	std::string x_ind = ssxindex.str();
	RenderText(x_ind, SCR_WIDTH - 200.0f, 140.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

	std::stringstream sszindex;
	sszindex << "z_index: " << z_index;
	std::string z_ind = sszindex.str();
	RenderText(z_ind, SCR_WIDTH - 200.0f, 160.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

	std::stringstream ssfrontinfo;
	ssfrontinfo << "x_v: " << camera.Front.x << " y_v: " << camera.Front.y << " z_v: " << camera.Front.z;
	std::string front_info = ssfrontinfo.str();
	RenderText(front_info, 100.0f, 50.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

	std::stringstream ssfps;
	ssfps << "fps: " << fps;
	std::string fps_str = ssfps.str();
	RenderText(fps_str, SCR_WIDTH - 200.0f, SCR_HEIGHT - 50.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

	std::stringstream ssCollectPage;
	ssCollectPage << "Viewed Page: " << posViewedPage;
	std::string collectPage_str = ssCollectPage.str();
	RenderText(collectPage_str, 50.0f, SCR_HEIGHT - 50.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void Renderer::findLookingPage(Camera& camera, vector<int> pageIndexPosition) {
	for (int k = 0; k < pageIndexPosition.size(); k++) {
		int indexPoi = pageIndexPosition[k];
		glm::vec3 translationVec = pointOfinterestTranslationVec[indexPoi];
		float x_lampione = translationVec.x + STREETLIGHT_POI_OFFSET;
		float z_lampione = translationVec.z + STREETLIGHT_POI_OFFSET;

		float distanza_lampione = sqrt(pow(x_lampione - camera.Position.x, 2) + pow(z_lampione - camera.Position.z, 2));
		bool collectPosition = (camera.Position.x > x_lampione && distanza_lampione <= PAGE_SELECTION_DISTANCE);

		float componente_z = z_lampione - camera.Position.z;
		float angolo_alpha = componente_z / distanza_lampione;
		bool collectVision = (camera.Front.x > X_V_MIN_PAGE && camera.Front.x < X_V_MAX_PAGE)
			&& (camera.Front.y > Y_V_MIN_PAGE && camera.Front.y < Y_V_MAX_PAGE)
			&& (camera.Front.z > (Z_V_MIN_PAGE + angolo_alpha) && camera.Front.z < (Z_V_MAX_PAGE + angolo_alpha))
			&& (abs(angolo_alpha) < MAX_ANGLE_PAGE);
		if (collectPosition && collectVision) {
			posViewedPage = k;
			return;
		}
	}
	posViewedPage = -1;
}

void Renderer::renderPageMessage(int& actualPage) {
	if (renderPageTimer + PAGE_COLLECTED_MESSAGE_SECONDS > glfwGetTime()) {
		std::stringstream ssPageInfo;
		ssPageInfo << "Collected Page: " << actualPage << "/" << NUM_PAGES;
		std::string ssPageInfoStr = ssPageInfo.str();
		RenderText(ssPageInfoStr, (SCR_WIDTH / 2) - 150.0f, SCR_HEIGHT - 200.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
	}
}

void Renderer::buildMiniMap(unsigned int& framebuffer, Shader& minimapWoodShader, unsigned int& minimapWoodVAO, unsigned int& woodMinimapTexture, Shader& circleMinimapShader, unsigned int& circleVAO) {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.137f, 0.09f, 0.035f, 0.8f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	minimapWoodShader.use();
	glBindTexture(GL_TEXTURE_2D, woodMinimapTexture);
	glBindVertexArray(minimapWoodVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	circleMinimapShader.use();
	glm::mat4 transform = glm::mat4(1.0f);
	for (int i = 0; i < pointOfinterestTranslationVec.size(); i++) {
		glm::vec3 poitv = pointOfinterestTranslationVec[i];
		transform = glm::mat4(1.0f);
		transform = glm::translate(transform, glm::vec3(-poitv.x / MAX_W_QUAD_MAP, -poitv.z / MAX_W_QUAD_MAP, 0.0f));
		transform = glm::scale(transform, glm::vec3(0.03f, 0.03f, 0.03f));
		circleMinimapShader.setMat4("transform", transform);
		circleMinimapShader.setVec3("circleColor", glm::vec3(1.0f, 0.8f, 0.0f));
		glBindVertexArray(circleVAO);
		glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES_CIRCLE / 2);
	}

	transform = glm::mat4(1.0f);
	transform = glm::translate(transform, glm::vec3(-camera.Position.x / MAX_W_QUAD_MAP, -camera.Position.z / MAX_W_QUAD_MAP, 0.0f));
	transform = glm::scale(transform, glm::vec3(0.04f, 0.04f, 0.04f));
	circleMinimapShader.setMat4("transform", transform);
	circleMinimapShader.setVec3("circleColor", glm::vec3(1.0f, 0.0f, 0.0f));
	glBindVertexArray(circleVAO);
	glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES_CIRCLE / 2);

	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::renderMiniMap(Shader& minimapShader, unsigned int& minimapVAO, unsigned int& textureColorBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	minimapShader.use();
	glBindVertexArray(minimapVAO);
	glBindTexture(GL_TEXTURE_2D ,textureColorBuffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::renderDynamicMap(Shader& shader, Model& modelObj, vector<int>& VAO_indexes, int quadSide, int vaoObjectSide) {
	unsigned int num_VAO = (quadSide / vaoObjectSide) * (quadSide / vaoObjectSide);
	unsigned int num_element_for_VAO = (quadSide * quadSide) / num_VAO;

	for (unsigned int k = 0; k < VAO_indexes.size(); k++) {
		for (unsigned int i = 0; i < modelObj.meshes.size(); i++) {
			for (unsigned int j = 0; j < modelObj.meshes[i].textures.size(); j++) {
				glActiveTexture(GL_TEXTURE0 + j);
				string name = modelObj.meshes[i].textures[j].type;
				glUniform1i(glGetUniformLocation(shader.ID, (name + std::to_string(j + 1)).c_str()), j);
				glBindTexture(GL_TEXTURE_2D, modelObj.meshes[i].textures[j].id);
			}
			int vao_index = max(VAO_indexes[k], 0);
			glBindVertexArray(modelObj.meshes[i].VAOs[vao_index]);
			glDrawElementsInstanced(GL_TRIANGLES, modelObj.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, num_element_for_VAO);
			glBindVertexArray(0);
		}
	}
}

vector<int> Renderer::getVaoIndexesFromCamera(Camera& camera, float offset, int quadSide, int vaoObjectSide) {
	vector<int> result;

	float x_camera = camera.Position.x + (offset * quadSide / 2) + offset / 2;
	if (x_camera < 0.0f) {
		x_camera = 0.0f;
	}
	if (x_camera >= offset * quadSide) {
		x_camera = offset * quadSide - 0.1f;
	}

	float z_camera = camera.Position.z + (offset * quadSide / 2) + offset / 2;
	if (z_camera < 0.0f) {
		z_camera = 0.0f;
	}
	if (z_camera >= offset * quadSide) {
		z_camera = offset * quadSide - 0.1f;
	}

	int x_index = floor(x_camera / (vaoObjectSide * offset));
	int z_index = floor(z_camera / (vaoObjectSide * offset));


	for (int i = (x_index - INT_OFFSET_VAO_INDEXES); i <= (x_index + INT_OFFSET_VAO_INDEXES); i++) {
		for (int j = (z_index - INT_OFFSET_VAO_INDEXES); j <= (z_index + INT_OFFSET_VAO_INDEXES); j++) {
			int vao_index = (i * (quadSide / vaoObjectSide)) + j;
			result.push_back(vao_index);
		}
	}

	return result;
}


