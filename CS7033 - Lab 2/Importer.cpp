#include "Importer.h"

Importer::Importer(){
	std::cout << "IMPORTER :: Initializing..." << std::endl;
}

aiScene* Importer::import(const char* file){
	std::cout << "IMPORTER :: Importing " << file << std::endl;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		file, 
		aiProcess_MakeLeftHanded |
		aiProcess_Triangulate
	);

	if(!scene){
		std::cout << "ERROR :: IMPORTER :: " << file << ": " << importer.GetErrorString() << std::endl;
		return NULL;
	}

	return (aiScene*)scene;
}