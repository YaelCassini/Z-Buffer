// Author: Peiyao Li
// Date:   Jan 8 2023
#include "Raseterizer.h"
#include "Vector.h"
#include <iostream>


// Ϊ�˷��㲻ͬ�߶Ȳ�ͬ�ռ������Mesh������������ʾ����Ļ��
// �˴��Ĺ�դ����ʵ�Ǹ���ģ�͵�����ֵ��Χ�ʹ��ڵĴ�С������һ��scale��transform
// ��դ����Model�ж��������任Ϊ����Ļ�����ϵĶ�ά����ֵ
void Rasterizer::rasterize(Model& model)
{
	int vertices_size = model.vertices.size();

	Point3f min_pos(0xfffffff, 0xfffffff, 0xfffffff),
		max_pos(-0xfffffff, -    0xfffffff, -0xfffffff);
	for (int i = 0; i < vertices_size; i++)
	{
		const Point3f& vertex = model.vertices[i].p;
		min_pos = min(min_pos, vertex);
		max_pos = max(max_pos, vertex);
	}

	Point3f center_pos = (min_pos + max_pos) / 2;
	float model_width = max_pos.x - min_pos.x;
	float model_height = max_pos.y - min_pos.y;

	float scale = (std::min(width, height) - 1) / std::max(model_width, model_height);
	scale *= 0.8;


	for (int i = 0; i < vertices_size; ++i)
	{
		Vec3f& vertex = model.vertices[i].p;
		//���õ������м�
		Vec3f rasterVertex;
		vertex.x = (vertex.x - center_pos.x) * scale + width / 2;
		vertex.y = (vertex.y - center_pos.y) * scale + height / 2;
		vertex.z = (vertex.z - center_pos.z) * scale;
	}
	model.center = Point3f(width / 2, height / 2, 0);
}