// Author: Peiyao Li
// Date:   Jan 8 2023
#include "ScanZBuffer.h"
#include <omp.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <assert.h>
#include "Timer.h"
//#define NUM_THREADS 8


ScanLineZBuffer::ScanLineZBuffer(int w, int h)
{
	width = w;
	height = h;

	initialize();
}

ScanLineZBuffer::~ScanLineZBuffer()
{
	release();
}

void ScanLineZBuffer::initialize()
{
	release();

	zBuffer = new float[width];
	frameBuffer = new Color3f[width * height];
	for (int i = 0; i < width * height; i++)
	{
		frameBuffer[i] = Color3f(0, 0, 0);
	}
	idBuffer = new int* [height];
	for (int i = 0; i < height; ++i)
		idBuffer[i] = new int[width];

}

void ScanLineZBuffer::release()
{
	if (zBuffer != NULL)
	{
		delete[] zBuffer;
		zBuffer = NULL;
	}
	if (idBuffer != NULL)
	{
		for (int i = 0; i < height; ++i)
		{
			delete[] idBuffer[i];
			idBuffer[i] = NULL;
		}
	}
	delete[] idBuffer;
	idBuffer = NULL;
}

// ����ȡ���ĵ�ǰ������еķ���ߵıȽϺ���������
static bool classifiedEdgeSortCmp(const ClassifiedEdge& lEdge, const ClassifiedEdge& rEdge)
{
	if (lEdge.x < rEdge.x)return true;
	else if (lEdge.x == rEdge.x)
	{
		if (lEdge.dx < rEdge.dx)
			return true;
	}
	return false;
}

bool ScanLineZBuffer::buildTable(const Model& model)
{
	Timer timer;
	timer.start();

	classifiedPolygonTable.clear();
	classifiedPolygonTable.resize(height);
	classifiedEdgeTable.clear();
	classifiedEdgeTable.resize(height);

	omp_lock_t mylock;
	omp_init_lock(&mylock);

	int faces_size = model.faces.size();
	for (int i = 0; i < faces_size; i++)
	{
		const Face& face = model.faces[i];

		float max_y = -0xfffffff, min_y = 0xfffffff;
		//��������߱�
		const std::vector<int>& vertexIdx = model.faces[i].vIdx;
		for (int j = 0, vsize = vertexIdx.size(); j < vsize; j++)
		{
			Point3f p1 = model.vertices[vertexIdx[j]].p;
			Point3f p2 = model.vertices[vertexIdx[(j + 1) % vsize]].p;
			
			if (p1.y < p2.y)
			{
				Point3f tmp = p1;
				p1 = p2;
				p2 = tmp;
			}

			max_y = std::max(max_y, p1.y);
			min_y = std::min(min_y, p2.y);

			// problem:
			// ������һ��ʼ����round��������᲻���һ��
			// ����floor��ceil
			ClassifiedEdge edge;
			edge.x = floor(p1.x);
			edge.id = i;
			edge.dy = floor(p1.y) - floor(p2.y);
			if (isEqualf(edge.dy,0)) continue; //��ͼ��ռ�ƽ����x��

			//edge.dx = -(floor(p1.x) - floor(p2.x)) / (floor(p1.y) - floor(p2.y));
			edge.dx = -(floor(p1.x) - floor(p2.x)) / edge.dy;

			// problem: ����һ���߼���ƽ����x��
			//if (edge.dx > 10)
			//{
			//	std::cout << "error" << std::endl;
			//	std::cout << "test" << edge.dx << std::endl;
			//}

			omp_set_lock(&mylock);
			classifiedEdgeTable[floor(p1.y)].push_back(edge);
			omp_unset_lock(&mylock);
		}


		ClassifiedPolygon polygon;
		polygon.id = i;
		polygon.dy = floor(max_y) - floor(min_y);
		Point3f pos = model.vertices[face.vIdx[0]].p;
		polygon.a = face.faceNormal.x;
		polygon.b = face.faceNormal.y;
		polygon.c = face.faceNormal.z;
		polygon.d = -(polygon.a * pos.x + polygon.b * pos.y + polygon.c * pos.z);
		polygon.color = face.faceColor;

		omp_set_lock(&mylock);
		classifiedPolygonTable[floor(max_y)].push_back(polygon);
		omp_unset_lock(&mylock);
	}

	timer.end();
	timer.printTimeCost("ScanLine build");
	return true;
}

void ScanLineZBuffer::addActiveEdgeFromActivePolygon(int y, ClassifiedPolygon& activePolygon)
{
	if (isEqualf(activePolygon.c, 0))return;

	relatedEdge.clear();
	// �Ѹö������oxyƽ���ϵ�ͶӰ��ɨ�����ཻ�ı߼��뵽��߱���
	for (std::vector<ClassifiedEdge>::iterator it = classifiedEdgeTable[y].begin(),
		end = classifiedEdgeTable[y].end(); it != end; it++)
	{
		if (it->id != activePolygon.id) continue;

		relatedEdge.push_back(*it);
		// classifiedEdgeTable[y].erase(it); // problem������ɾȥ�ᵼ��it�����������޷�����ȫ������
		it->id = -1;
	}
	// �Ե�ǰ�����εĻ�߱���x��������(x���ʱ��ʹ��dx)
	if (relatedEdge.size() % 2 != 0)
	{
		std::cout << "Error to find even number of edges intersecting the current scan line" << std::endl;
		assert(relatedEdge.size() % 2 == 0);
	}
	// problem: ��һ����ƽ����x�� ���������ڵ�һ�ξͱ�������
	if (relatedEdge.size() == 3)
	{
		std::cout << "There is an edge parallel to the x-axis." << std::endl;
	}
	sort(relatedEdge.begin(), relatedEdge.end(), classifiedEdgeSortCmp);

	ActiveEdge activeEdge;
	activeEdge.id = activePolygon.id;
	activeEdge.dzx = -(activePolygon.a / activePolygon.c);
	activeEdge.dzy = activePolygon.b / activePolygon.c;
	activeEdge.color = activePolygon.color;
	for (std::vector<ClassifiedEdge>::iterator it = relatedEdge.begin(),
		end = relatedEdge.end(); it != end; it++)
	{
		activeEdge.xl = it->x;
		activeEdge.dxl = it->dx;
		activeEdge.dyl = it->dy;
		activeEdge.zl = -(activePolygon.d + activePolygon.a * it->x + activePolygon.b * y) / activePolygon.c;
		it++;
		activeEdge.xr = it->x;
		activeEdge.dxr = it->dx;
		activeEdge.dyr = it->dy;

		activeEdgeTable.push_back(activeEdge);
	}
}

bool ScanLineZBuffer::findReplaceEdgeFromActivePolygon(int y, ActiveEdge& activeEdge)
{
	std::vector<ClassifiedEdge> relatedEdge;

	for (std::vector<ClassifiedEdge>::iterator it = classifiedEdgeTable[y].begin(),
		end = classifiedEdgeTable[y].end(); it != end; it++)
	{
		if (it->id == activeEdge.id)
		{
			relatedEdge.push_back(*it);
		}
	}
	if (relatedEdge.size() == 0)
	{
		return false; // �û�������û�и�ɾ���û��
	}
	else
	{
		if (activeEdge.dyl == 0 && activeEdge.dyr == 0)
		{
			// ���һ����ƽ����x�ᣬ����Ҫ��������Ϊ��һ�ֵ�������Զ�����
			// ��Ҫ������
			if (relatedEdge.size() == 1) {
				activeEdge.dxl = activeEdge.dxr = 0;
				activeEdge.dyl = activeEdge.dyr = 1;
			}
			else if (relatedEdge.size() == 2) // �������ı����У�������ͬʱȡ�����ڵ�������
			{
				sort(relatedEdge.begin(), relatedEdge.end(), classifiedEdgeSortCmp);
				for (std::vector<ClassifiedEdge>::iterator it = relatedEdge.begin(),
					end = relatedEdge.end(); it != end; it++)
				{
					activeEdge.xl = it->x;
					activeEdge.dxl = it->dx;
					activeEdge.dyl = it->dy;

					it++;
					activeEdge.xr = it->x;
					activeEdge.dxr = it->dx;
					activeEdge.dyr = it->dy;
				}
			}
		}
		else if (activeEdge.dyl == 0)
		{
			activeEdge.xl = relatedEdge[0].x;
			activeEdge.dxl = relatedEdge[0].dx;
			activeEdge.dyl = relatedEdge[0].dy;
		}
		else
		{
			activeEdge.xr = relatedEdge[0].x;
			activeEdge.dxr = relatedEdge[0].dx;
			activeEdge.dyr = relatedEdge[0].dy;
		}
	}


}

void ScanLineZBuffer::scan()
{
	Timer timer;
	timer.start();

	for (int y = height - 1; y >= 0; y--)
	{
		memset(idBuffer[y], -1, sizeof(int) * width);
		std::fill(zBuffer, zBuffer + width, -0xfffffff);

		for (std::vector<ClassifiedPolygon>::iterator it = classifiedPolygonTable[y].begin(),
			end = classifiedPolygonTable[y].end(); it != end; it++)
		{
			activePolygonTable.push_back(*it);
			addActiveEdgeFromActivePolygon(y, *it);
		}

		for (std::vector<ActiveEdge>::iterator it = activeEdgeTable.begin(),
			end = activeEdgeTable.end(); it != end; it++)
		{
			ActiveEdge tmpActiveEdge = *it;
			float zx = tmpActiveEdge.zl;
			for (int x = round(tmpActiveEdge.xl); x <= round(tmpActiveEdge.xr); x++)
			{
				if (zx > zBuffer[x])
				{
					zBuffer[x] = zx;
					idBuffer[y][x] = tmpActiveEdge.id;
					frameBuffer[y * width + x] = tmpActiveEdge.color;
				}
				zx += tmpActiveEdge.dzx;
			}
			
			if (tmpActiveEdge.dyl == 0 || tmpActiveEdge.dyr == 0)
			{
				bool flag = findReplaceEdgeFromActivePolygon(y, tmpActiveEdge);
			}
			tmpActiveEdge.dyl--; tmpActiveEdge.dyr--;
			
			tmpActiveEdge.xl += tmpActiveEdge.dxl;
			tmpActiveEdge.xr += tmpActiveEdge.dxr;
			tmpActiveEdge.zl += tmpActiveEdge.dzx * tmpActiveEdge.dxl + tmpActiveEdge.dzy;
			// problem: ��ʼ����д��dyl��
			
			*it = tmpActiveEdge; // ��ʼû����һ�䵼�¼������
		}

		// ����erase������
		//for (std::vector<ClassifiedPolygon>::iterator it = activePolygonTable.begin(),
		//	end = activePolygonTable.end(); it != end; it++)
		//{
		//	it->dy--;
		//	if (it->dy < 0)
		//		activePolygonTable.erase(it);
		//}

		// û�и���ActivePolygon��dy�ᵼ��
		for (std::vector<ClassifiedPolygon>::iterator it = activePolygonTable.begin(),
			end = activePolygonTable.end(); it != end; it++)
		{
			it->dy--;
		}
		//for (std::vector<ActiveEdge>::iterator it = activeEdgeTable.begin(),
		//	end = activeEdgeTable.end(); it != end; it++)
		//{
		//	it->dyl--; it->dyr--;
		//}

		// ����dy ���Ƴ� dy < 0 �Ļ�����
		int last = 0;
		int actPolySize = activePolygonTable.size();
		for (int i = 0; i < actPolySize; ++i, ++last) {
			while (activePolygonTable[i].dy < 0) {
				++i;
				if (i >= actPolySize)break;
			}
			if (i >= actPolySize)break;
			activePolygonTable[last] = activePolygonTable[i];
		}
		activePolygonTable.resize(last);

		// ����dy ���Ƴ� dy < 0 �Ļ��
		last = 0;
		int actEdgeSize = activeEdgeTable.size();
		for (int i = 0; i < actEdgeSize; ++i, ++last) {
			// problem: �߽����⣬���˺ܾ�
			// �����<0 ����һЩ�������������̫Сֻռ��һ��Ԫ��
			// �����<=0 ���ܻ�����������ı߽�
			// ˼������ΪӦ����<0 ֻռһ��Ԫ�ص����Ӧ�ó�ʼdy=0
			while (activeEdgeTable[i].dyl < 0 || activeEdgeTable[i].dyr < 0){
				//||floor(activeEdgeTable[i].xl) > floor(activeEdgeTable[i].xr) ) {
				++i;
				if (i >= actEdgeSize)break;
			}
			if (i >= actEdgeSize)break;
			activeEdgeTable[last] = activeEdgeTable[i];
		}
		activeEdgeTable.resize(last);
	}
	timer.end();
	timer.printTimeCost("ScanLine");
}

