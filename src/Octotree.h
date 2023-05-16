// Author: Peiyao Li
// Date:   Jan 8 2023
#pragma once

#include<vector>
#include "Vector.h"
#include "Fragment.h"


// �˲����ڵ�
class OctoNode
{
public:
	OctoNode() {}
	OctoNode(int minX, int maxX, int minY, int maxY, float minZ, float maxZ, Polygons polygons)
	{
		this->minX = minX;
		this->maxX = maxX;
		this->minY = minY;
		this->maxY = maxY;
		this->minZ = minZ;
		this->maxZ = maxZ;
		this->z = -FLT_MAX;
	}
	// ��ǰ�˲����ڵ�Ŀռ�
	float z = -FLT_MAX;
	int minX, maxX;
	int minY, maxY;
	float minZ, maxZ;

	Polygons polygons;
	std::vector<OctoNode*> children;

	void release() {
		for (OctoNode* child : children) {
			if (child != NULL) child->release();
		}
		children.clear();
	}
private:
};