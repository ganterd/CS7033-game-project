#ifndef IMPORTER_H
#define IMPORTER_H

#include <iostream>
#include <Windows.h>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

class Importer{
public:
	Importer();

	aiScene* import(const char*);
};

#endif