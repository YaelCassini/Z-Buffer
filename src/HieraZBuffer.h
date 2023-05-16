// Author: Peiyao Li
// Date:   Jan 8 2023

#pragma once
#include "Model.h"
#include "Fragment.h"
#include "Quadtree.h"
#include <cmath>

// ʵ�ּ�ģʽ�Ĳ��ZBuffer
class HieraZBuffer
{
public:
	HieraZBuffer() = delete;
	HieraZBuffer(int w, int h);
	~HieraZBuffer();

	void initialize(); // ��ʼ��
	QuadNode* buildZPyramid(int minX, int maxX, int minY, int maxY); // ���䴢��ռ䲢ʹ���Ĳ����ڵ�ݹ齨�����Zbuffer�ṹ

	bool rejectPolygon(Fragment& fragment, QuadNode* qNode); // ��Ȳ���
	bool containPolygon(Fragment& fragment, QuadNode* qNode); // ��ǰ���ZBuffer�鷶Χ�Ƿ����������Ƭ��������Ƭ�и
	
	void renderPolygons(Polygons& polygons); // ���㳡����������Ƭ
	void renderPyramidRange(Fragment& fragment, QuadNode* qNode);  // �Ӷ��㿪ʼ�ֱ���Ȳ��ԣ��ҵ�������Ƭ�Ҳ��ܾܾ�����СZBuffer���ڴ˲㿪ʼ���»���
	float renderNodeRange(Fragment& fragment, QuadNode* qNode); // �ڵ�ǰ���ZBuffer�ڵ�ݹ��ҵ���ײ㣬����Ⱦ��Ƭ

	Color3f* getFrameBuffer() { return frameBuffer; }
	void release();

private:
	int width, height;
	Color3f* frameBuffer;

	QuadNode* zPyramid;
	QuadNode* zBuffer;

};