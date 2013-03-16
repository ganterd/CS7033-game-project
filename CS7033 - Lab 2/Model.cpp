#include "Model.h"

Model::Model(){
	yUp = false;
	modelScale = 1.0;

	MODEL_ROT_MAT = aiMatrix3x3(
		1, 0, 0,
		0, 0, 1,
		0, -1, 0
	);

	gettingVertsAndNormals = false;
}

Model::~Model(){
	for(int i = 0; i < numMeshes; i++){
		delete(meshes[i]);
	}
	delete meshes;
	importer.FreeScene();
}

bool Model::loadModel(const char* modelPath){
	std::cout << "[Model] Importing: " << modelPath << std::endl;
	model = importer.ReadFile(
		modelPath, 
		aiProcess_RemoveComponent | aiComponent_NORMALS | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs
	);

	if(!model){
		std::cout << "[Model] ERROR - " << modelPath << ": " << importer.GetErrorString() << std::endl;
		return false;
	}
	
	std::cout << "[Model] Model: " << modelPath << std::endl;
	std::cout << "[Model] Meshes: " << model->mNumMeshes << std::endl;
	std::cout << "[Model] Materials: " << model->mNumMaterials << std::endl;
	std::cout << "[Model] Animations: " << model->mNumAnimations << std::endl;
	std::cout << "[Model] Lights: " << model->mNumLights << std::endl;

	this->initModel(model);
	
	return true;
}

void Model::setLighting(){
	if(model->mNumLights == 0)
		return;

	for(unsigned int i = 0; i < model->mNumLights && i < 6; i++){
		unsigned int L;
		switch(i){
		case 0:
			L = GL_LIGHT0;
			break;
		case 1:
			L = GL_LIGHT1;
			break;
		case 2:
			L = GL_LIGHT2;
			break;
		case 3:
			L = GL_LIGHT3;
			break;
		case 4:
			L = GL_LIGHT4;
			break;
		case 5:
			L = GL_LIGHT5;
			break;
		}

		aiLight* l = model->mLights[i];
		aiNode* node = findNodeRecurse(model->mRootNode, l->mName);
		aiMatrix4x4 transformation = node->mTransformation;
		aiVector3D pos = MODEL_ROT_MAT * (transformation * l->mPosition);

		float fPos[] = {pos.x, pos.y, pos.z, 1};
		float fDiffuse[] = {l->mColorDiffuse[0], l->mColorDiffuse[1], l->mColorDiffuse[2]};
		//float fSpecular[] = {l->mColorSpecular[0], l->mColorSpecular[1], l->mColorSpecular[2]};
		//float fAmbient[] = {l->mColorAmbient[0], l->mColorAmbient[1], l->mColorAmbient[2]};
		float fSpecular[] = {0, 0, 0};
		float fAmbient[] = {0, 0, 0};
	
		glLightfv(L, GL_POSITION, fPos);
		glLightfv(L, GL_DIFFUSE, fDiffuse);
		glLightfv(L, GL_SPECULAR, fSpecular);
		glLightfv(L, GL_AMBIENT, fAmbient);
		glEnable(L);
	}
}

void Model::initTextures(const aiScene* scene) {
	ILboolean success;
	/* initialization of DevIL */
	ilInit(); 
	/* scan scene's materials for textures */
	for (unsigned int m=0; m < scene->mNumMaterials; ++m) {
		int texIndex = 0;
		aiString path;  // filename
		aiReturn texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
		while (texFound == AI_SUCCESS) {
			//fill map with textures, OpenGL image ids set to 0

			textureIdMap[path.data] = 0; 
			// more textures?
			texIndex++;
			texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
		}
	}

	int numTextures(textureIdMap.size());

	/* Because we're using static libs to load textures */
	/* we need to mutex lock this section               */
	pthread_mutex_lock(&_loadTextureMutex);

	/* create and fill array with DevIL texture ids */
	ILuint* imageIds = new ILuint[numTextures];
	ilGenImages(numTextures, imageIds); 

	/* create and fill array with GL texture ids */
	GLuint* textureIds = new GLuint[numTextures];
	glGenTextures(numTextures, textureIds); /* Texture name generation */

	/* get iterator */
	std::map<std::string, GLuint>::iterator itr = textureIdMap.begin();
	int i=0;
	for (; itr != textureIdMap.end(); ++i, ++itr) {
		//save IL image ID
		std::string filename = (*itr).first;  // get filename
		(*itr).second = textureIds[i];    // save texture id for filename in map
		ilBindImage(imageIds[i]); /* Binding of DevIL image name */
		ilEnable(IL_ORIGIN_SET);
		ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

		success = ilLoadImage((ILstring)filename.c_str());

		if (success) {
			/* Convert image to RGBA */
			ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE); 

			/* Create and load textures to OpenGL */
			glBindTexture(GL_TEXTURE_2D, textureIds[i]); 
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH),
				ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE,
				ilGetData()); 
		}
		else 
			printf("[Model] :: Couldn't load Image: %s\n", filename.c_str());
		
	}
	/* Because we have already copied image data into texture data  we can release memory used by image. */
	ilDeleteImages(numTextures, imageIds); 
	//Cleanup
	delete [] imageIds;
	delete [] textureIds;

	std::cout << "Exiting mutex lock." << std::endl;
	pthread_mutex_unlock(&_loadTextureMutex);
}

void Model::initModel(const aiScene* scene){
	numMeshes = scene->mNumMeshes;
	meshes = new Mesh*[numMeshes];

	std::map<unsigned int, bool> vertexEncountered;

	/***** Start Loading Meshes *****/
	for(int i = 0; i < numMeshes; i++){
		aiMesh* mesh = scene->mMeshes[i];
		
		meshes[i] = new Mesh();
		int nVertices = mesh->mNumVertices;
		int nFaces = mesh->mNumFaces;

		meshes[i]->mNumVertices = nVertices;
		meshes[i]->vertices = new glm::vec3[nVertices];
		meshes[i]->faceVertexIndices = new int*[nFaces];
		meshes[i]->numFaces = nFaces;

		meshes[i]->materialIndex = mesh->mMaterialIndex;

		for(int k = 0; k < nFaces; k++){
			const struct aiFace* face = &mesh->mFaces[k];
			meshes[i]->faceVertexIndices[k] = new int[3];
			meshes[i]->faceVertexIndices[k][0] = face->mIndices[0];
			meshes[i]->faceVertexIndices[k][1] = face->mIndices[1];
			meshes[i]->faceVertexIndices[k][2] = face->mIndices[2];
		}
		
		for(int j = 0; j < nVertices; j++){
			aiVector3D v = mesh->mVertices[j];
			meshes[i]->vertices[j].x = v.x;
			meshes[i]->vertices[j].y = v.y;
			meshes[i]->vertices[j].z = v.z;
		}
	}

	this->initTextures(scene);
}

void Model::initDisplayList(){
	std::cout << "[Model] Creating display list..." << std::endl;
	displayListID = glGenLists(1);
	glNewList(displayListID, GL_COMPILE);
	this->drawModel();
	glEndList();
	std::cout << "[Model] Display list created..." << std::endl;
}

void Model::drawFromDisplayList(){
	LOGGER_ENTER("Model", "drawFromDisplayList");
	glCallList(displayListID);
	LOGGER_EXIT;
}

aiNode* Model::findNodeRecurse(aiNode* node, aiString name){
	LOGGER_ENTER("Model", "findNodeRecurse");
	if(node->mName == name){
		LOGGER_EXIT;
		return node;
	}
	for(unsigned int i = 0; i < node->mNumChildren; i++){
		aiNode* n = findNodeRecurse(node->mChildren[i], name);
		if(n != NULL){
			LOGGER_EXIT;
			return n;
		}
	}
	LOGGER_EXIT;
	return NULL;
}

void Model::animate(float t){
	LOGGER_ENTER("Model", "animate");
	if(!model->HasAnimations())
		return;

	const aiAnimation* anim = model->mAnimations[0];
	time = glm::mod(t, (float)anim->mDuration);

	for(unsigned int i = 0; i < anim->mNumChannels; i++){
		const aiNodeAnim* chan = anim->mChannels[i];
		aiVector3D currPos;
		aiQuaternion currRot;
		aiVector3D currScale;

		aiNode* targetNode = findNodeRecurse(model->mRootNode, chan->mNodeName);

		unsigned int scaleIndex = 0;
		while(true){
			if(scaleIndex + 1 >= chan->mNumScalingKeys){
				currScale = chan->mScalingKeys[scaleIndex].mValue;
				break;
			}
			if(chan->mScalingKeys[scaleIndex].mTime <= time && time < chan->mScalingKeys[scaleIndex + 1].mTime){
				double pFactor = (time - chan->mScalingKeys[scaleIndex].mTime) / (chan->mScalingKeys[scaleIndex + 1].mTime - chan->mScalingKeys[scaleIndex].mTime);
				currScale.x = (chan->mScalingKeys[scaleIndex].mValue.x * (1 - pFactor)) + (chan->mScalingKeys[scaleIndex + 1].mValue.x * pFactor);
				currScale.y = (chan->mScalingKeys[scaleIndex].mValue.y * (1 - pFactor)) + (chan->mScalingKeys[scaleIndex + 1].mValue.y * pFactor);
				currScale.x = (chan->mScalingKeys[scaleIndex].mValue.z * (1 - pFactor)) + (chan->mScalingKeys[scaleIndex + 1].mValue.z * pFactor);

				break;
			}
			if(chan->mScalingKeys[scaleIndex + 1].mTime > time){
				currScale = chan->mScalingKeys[scaleIndex].mValue;
				break;
			}
			scaleIndex++;
		}

		/* Find the right POSITION key frame for the time */
		unsigned int posIndex = 0;
		while(true){
			if(posIndex + 1 >= chan->mNumPositionKeys){
				currPos = chan->mPositionKeys[posIndex].mValue;
				break;
			}
			if(chan->mPositionKeys[posIndex].mTime <= time && time < chan->mPositionKeys[posIndex + 1].mTime){
				float pFactor = (time - chan->mPositionKeys[posIndex].mTime) / (chan->mPositionKeys[posIndex + 1].mTime - chan->mPositionKeys[posIndex].mTime);
				currPos.x = (chan->mPositionKeys[posIndex].mValue.x * (1 - pFactor)) + (chan->mPositionKeys[posIndex + 1].mValue.x * pFactor);
				currPos.y = (chan->mPositionKeys[posIndex].mValue.y * (1 - pFactor)) + (chan->mPositionKeys[posIndex + 1].mValue.y * pFactor);
				currPos.z = (chan->mPositionKeys[posIndex].mValue.z * (1 - pFactor)) + (chan->mPositionKeys[posIndex + 1].mValue.z * pFactor);

				break;
			}
			if(chan->mPositionKeys[posIndex + 1].mTime > time){
				currPos = chan->mPositionKeys[posIndex].mValue;
				break;
			}
			posIndex++;
		}

		/* Find the right POSITION key frame for the time */
		unsigned int rotIndex = 0;
		while(true){
			if(rotIndex + 1 >= chan->mNumRotationKeys){
				currRot = chan->mRotationKeys[rotIndex].mValue;
				break;
			}
			if(chan->mRotationKeys[rotIndex].mTime <= time && time < chan->mRotationKeys[rotIndex + 1].mTime){
				float pFactor = (time - chan->mRotationKeys[rotIndex].mTime) / (chan->mRotationKeys[rotIndex + 1].mTime - chan->mRotationKeys[rotIndex].mTime);
				chan->mRotationKeys[rotIndex].mValue.Interpolate(
					currRot, 
					chan->mRotationKeys[rotIndex].mValue,
					chan->mRotationKeys[rotIndex + 1].mValue,
					pFactor
				);

				break;
			}
			if(chan->mRotationKeys[rotIndex + 1].mTime > time){
				currRot = chan->mRotationKeys[rotIndex].mValue;
				break;
			}
			rotIndex++;
		}
		

		aiMatrix4x4 transformMatrix;
		transformMatrix.a1 = currRot.GetMatrix().a1;
		transformMatrix.a2 = currRot.GetMatrix().a2;
		transformMatrix.a3 = currRot.GetMatrix().a3;
		transformMatrix.b1 = currRot.GetMatrix().b1;
		transformMatrix.b2 = currRot.GetMatrix().b2;
		transformMatrix.b3 = currRot.GetMatrix().b3;
		transformMatrix.c1 = currRot.GetMatrix().c1;
		transformMatrix.c2 = currRot.GetMatrix().c2;
		transformMatrix.c3 = currRot.GetMatrix().c3;
		transformMatrix.a4 = currPos.x;
		transformMatrix.b4 = currPos.y;
		transformMatrix.c4 = currPos.z;

		aiMatrix4x4 scalingMatrix;
		scalingMatrix.Scaling(currScale, scalingMatrix);
		
		/* For some reason, the scaling matrix for the Z scale is zero? */
		scalingMatrix.c3 = 1;
		

		targetNode->mTransformation = transformMatrix * scalingMatrix;
	}
	LOGGER_EXIT;
}

void Model::recursiveRender(const aiNode* node){
	LOGGER_ENTER("Model", "recursiveRender");
	glPushMatrix();
	int m = node->mNumMeshes;
	aiMatrix4x4 masterTransform = node->mTransformation;

	/* Iterate through the node's meshes */
	for(int i = 0; i < m; i++){
		aiMesh* mesh = model->mMeshes[node->mMeshes[i]];

		Mesh m;
		if(gettingVertsAndNormals){
			m.mNumVertices = mesh->mNumVertices;
			m.vertices = new glm::vec3[m.mNumVertices];
			m.normals = new glm::vec3[m.mNumVertices];
			m.numFaces = mesh->mNumFaces;
			m.faceVertexIndices = new int*[m.numFaces];
			m.name = mesh->mName.C_Str();
		}

		aiVector3D* vertices = mesh->mVertices;
		aiVector3D* normals = mesh->mNormals;

		/*** Calculate Skeleton Transformations and Resulting Vertex Trasformations ***/
		if(mesh->HasBones()){
			vertices = new aiVector3D[mesh->mNumVertices];
			normals = new aiVector3D[mesh->mNumVertices];
			
			for(unsigned int b = 0; b < mesh->mNumBones; b++){
				aiMatrix4x4 boneMatrix;
				const aiBone* bone = mesh->mBones[b];

				aiNode* boneNode = findNodeRecurse(model->mRootNode, bone->mName);

				boneMatrix = bone->mOffsetMatrix;
				const aiNode* tmpNode = boneNode;
				while(tmpNode){
					boneMatrix = tmpNode->mTransformation * boneMatrix;
					tmpNode = tmpNode->mParent;
				}

				const aiMatrix4x4& posTransform = boneMatrix;
				const aiMatrix3x3 normalTransform = aiMatrix3x3(posTransform);

				for(unsigned int w = 0; w < bone->mNumWeights; w++){
					const aiVertexWeight& weight = bone->mWeights[w];
					unsigned int vertexID = weight.mVertexId;
					const aiVector3D& srcPos = mesh->mVertices[vertexID];
					const aiVector3D& srcNormal = mesh->mNormals[vertexID];

					vertices[vertexID] += weight.mWeight * (posTransform * srcPos);
					normals[vertexID] += weight.mWeight * (normalTransform * srcNormal);
				}
			}
		}

		/* Bind the texture if a diffuse texture exists */
		aiMaterial* mat = model->mMaterials[mesh->mMaterialIndex];
		int texIndex = 0;
		aiString texPath;
		if(AI_SUCCESS == mat->GetTexture(aiTextureType_DIFFUSE, texIndex, &texPath))    {
			unsigned int texId = textureIdMap[texPath.data];
			glBindTexture(GL_TEXTURE_2D, texId);
		}
		
		aiString matName;
		mat->Get(AI_MATKEY_NAME,matName);

		aiColor3D matDiffuseColour(0,0,0);
		if(mat->Get(AI_MATKEY_COLOR_DIFFUSE,matDiffuseColour) == AI_SUCCESS){
			float fMatDiffuseColour[] = {matDiffuseColour.r, matDiffuseColour.g, matDiffuseColour.b};
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, fMatDiffuseColour);
		}

		aiColor3D matSpecularColour(0,0,0);
		if(mat->Get(AI_MATKEY_COLOR_SPECULAR,matSpecularColour) == AI_SUCCESS){
			float fMatSpecularColour[] = {matSpecularColour.r, matSpecularColour.g, matSpecularColour.b};
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, fMatSpecularColour);
		}

		aiColor3D matAmbientColour(0,0,0);
		if(mat->Get(AI_MATKEY_COLOR_AMBIENT,matAmbientColour) == AI_SUCCESS){
			float fMatAmbientColour[] = {matAmbientColour.r, matAmbientColour.g, matAmbientColour.b};
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, fMatAmbientColour);
		}
		
			/* Draw Mesh Faces (Textures) */
			aiVector3D* texCoords = mesh->mTextureCoords[0];
			for(unsigned int j = 0; j < mesh->mNumFaces; j++){
				unsigned int* indices = mesh->mFaces[j].mIndices;
				aiVector3D n1, n2, n3, v1, v2, v3;
				
				if(mesh->HasBones()){
					n1 = normals[indices[0]];
					n2 = normals[indices[1]];
					n3 = normals[indices[2]];
					v1 = vertices[indices[0]];
					v2 = vertices[indices[1]];
					v3 = vertices[indices[2]];
				}else{
					n1 = MODEL_ROT_MAT * (masterTransform * normals[indices[0]]);
					n2 = MODEL_ROT_MAT * (masterTransform * normals[indices[1]]);
					n3 = MODEL_ROT_MAT * (masterTransform * normals[indices[2]]);
					v1 = MODEL_ROT_MAT * (masterTransform * vertices[indices[0]]);
					v2 = MODEL_ROT_MAT * (masterTransform * vertices[indices[1]]);
					v3 = MODEL_ROT_MAT * (masterTransform * vertices[indices[2]]);
				}

				glm::vec3 tmpV1(v1.x,v1.y,v1.z);
				glm::vec3 tmpV2(v2.x,v2.y,v2.z);
				glm::vec3 tmpV3(v3.x,v3.y,v3.z);
				glm::vec3 faceNormal = glm::normalize(glm::cross(tmpV3 - tmpV2, tmpV1 - tmpV2));

				if(gettingVertsAndNormals){
					glm::vec3 tmpN1(n1.x,n1.y,n1.z);
					glm::vec3 tmpN2(n2.x,n2.y,n2.z);
					glm::vec3 tmpN3(n3.x,n3.y,n3.z);

					m.vertices[indices[0]] = tmpV1;
					m.vertices[indices[1]] = tmpV2;
					m.vertices[indices[2]] = tmpV3;
					m.normals[indices[0]] = faceNormal;
					m.normals[indices[1]] = faceNormal;
					m.normals[indices[2]] = faceNormal;

					m.faceVertexIndices[j] = new int[3];
					m.faceVertexIndices[j][0] = indices[0];
					m.faceVertexIndices[j][1] = indices[1];
					m.faceVertexIndices[j][2] = indices[2];

					meshesMap[matName.C_Str()] = m;
				}else{
					glBegin(GL_POLYGON);
					
					if(mesh->HasTextureCoords(0)) glTexCoord2f(texCoords[indices[0]].x, 1 - texCoords[indices[0]].y);
					glNormal3f(faceNormal.x, faceNormal.y, faceNormal.z);
					glVertex3f(v1.x, v1.y, v1.z);
					if(mesh->HasTextureCoords(0)) glTexCoord2f(texCoords[indices[1]].x, 1 - texCoords[indices[1]].y);
					glVertex3f(v2.x, v2.y, v2.z);
					if(mesh->HasTextureCoords(0)) glTexCoord2f(texCoords[indices[2]].x, 1 - texCoords[indices[2]].y);
					glVertex3f(v3.x, v3.y, v3.z);
					glEnd();
				}
			}

			/* Free the memory that was used for the animated vertices and normals */
			if(mesh->HasBones()){
				free(normals);
				free(vertices);
			}

		/* Remove any texture binding */
		glBindTexture(GL_TEXTURE_2D, NULL);
	}

	/* Recurse Through Child Nodes */
	for(unsigned int i = 0; i < node->mNumChildren; i++){
		recursiveRender(node->mChildren[i]);
	}
	glPopMatrix();
	LOGGER_EXIT;
}

void Model::drawModel(){
	LOGGER_ENTER("Model", "drawModel");
	glPushMatrix();
	glScalef(modelScale, modelScale, modelScale);
	recursiveRender(model->mRootNode);
	glPopMatrix();
	LOGGER_EXIT;
}

std::map<std::string, Mesh> Model::getMeshes(){
	gettingVertsAndNormals = true;
	this->drawModel();
	gettingVertsAndNormals = false;
	return std::map<std::string, Mesh>(meshesMap);
}