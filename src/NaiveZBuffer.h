// Author: Peiyao Li
// Date:   Jan 8 2023
#pragma once

#include "Fragment.h"

// �������ZBuffer
class NaiveZBuffer
{
public:
	NaiveZBuffer() = delete;
	NaiveZBuffer(int w, int h);
	~NaiveZBuffer();
	void initialize(); // ��ʼ��������ռ�
	void renderPolygons(Polygons& fragments); // ��������������Ƭ
	void renderPolygon(Fragment& fragment); // ��Ȳ��Բ����Ƶ�����Ƭ
	Color3f* getFrameBuffer() { return frameBuffer; }
	void release();
private:
	int width, height;
	Color3f* frameBuffer;
	float* zBuffer;
};