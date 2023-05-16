// Author: Peiyao Li
// Date:   Jan 8 2023
#pragma once

#include "Model.h"

// ��դ��
class Rasterizer
{
public:
	Rasterizer(int w, int h) { width = w; height = h; }
	inline void setSize(int w, int h) { width = w; height = h; }
	void rasterize(Model& model); // �Ե���Model���й�դ����������ֱ�Ӵ�����ԭ����Model����
private:
	int width, height;
};