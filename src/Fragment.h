// Author: Peiyao Li
// Date:   Jan 8 2023
#pragma once

#include "Model.h"
#include <vector>
#include <cmath>
#include <cfloat>
#include <cinttypes>
class Fragment
{
// �����դ��֮�����Ƭ��֧����������Ƭ���ı�����Ƭ��
// ����ͨ�����������ֵ�ķ�ʽ���㵱ǰ���ص�Zֵ
// �����ı�����Ƭ���������ǲ������Σ�ÿ�β������������һ�������μ��㣬��ѡȡ�����������Ҫ��Ľ��
public:
	Fragment(Model& model, int faceid);
	void addPoint(Point3f p);
	std::vector<Point3f> vPos;
	Color3f faceColor;
	int id;
	float caculateZ(Vec2f pixel);
	float caculateZTriangle(Vec2f pixel, int aIdx, int bIdx, int cIdx);	

	int getMinX() { return minX; }
	int getMaxX() { return maxX; }
	int getMinY() { return minY; }
	int getMaxY() { return maxY; }
	float getMinZ() { return minZ; }
	float getMaxZ() { return maxZ; }
	int getDeltaX() { return maxX - minX; }
	int getDeltaY() { return maxY - minY; }
	float getDeltaZ() { return maxZ - minZ; }
private:
	int minX = INT_MAX, maxX = INT_MIN;
	int minY = INT_MAX, maxY = INT_MIN;
	float minZ = FLT_MAX, maxZ = -FLT_MAX;
};


class Polygons
{
// ����һ��vector�������������е���������ƬFragment
public:
	std::vector<Fragment> fragments;
	float minZ = FLT_MAX, maxZ = -FLT_MAX;

	void addModel(Model& model);
	int getSize() { return fragments.size(); }
};