// Author: Peiyao Li
// Date:   Jan 8 2023
#pragma once

#include<vector>
#include"Model.h"


// ��������
struct ClassifiedPolygon
{
	float a, b, c, d; // ���������ƽ��ķ���ϵ��
	int id; // ����εı��
	int dy; // ����ο�Խ��ɨ������Ŀ
	Color3f color; // ����ε���ɫ
};

// �����
struct ClassifiedEdge
{
	float x; // ���϶˵��x����
	float dx; // ��������ɨ���߽����x�����
	int dy; // �߿�Խ��ɨ������Ŀ
	int id; // ����������εı��
};

// ���
struct ActiveEdge
{
	float xl, xr; //���ҽ����x����
	float dxl, dxr; // (���ҽ������)������ɨ���߽����x����֮��
	float dyl, dyr; // �Ժ����ҽ������ڱ��ཻ��ɨ������Ϊ��ֵ������ÿ����һ��ɨ����
	float zl; // ���ҽ��㴦���������ƽ������ֵ��
	float dzx; // ��ɨ���������߹�һ������ʱ�����������ƽ������������
	float dzy; // ��y���������ƹ�һ��ɨ����ʱ�����������ƽ������������
	int id; // ��������ڵĶ���εı��
	Color3f color; // ����ε���ɫ
};


// ɨ����ZBuffer�㷨
// ��Ϊ����㷨�Ǹ���һ��ʼ�����õı����ģ����Բ����õ�Fragment��
class ScanLineZBuffer
{
public:
	ScanLineZBuffer() = delete;
	ScanLineZBuffer(int w, int h);
	~ScanLineZBuffer();
	void initialize();
	bool buildTable(const Model& model); // �����������α�ͷ���߱�
	void scan(); // ���ݽ����ı����ν���ɨ���ߵļ���
	int** idBuffer;
	
	Color3f* getFrameBuffer()
	{
		return frameBuffer;
	}

private:
	int width;
	int height;
	float* zBuffer;
	Color3f* frameBuffer;
	std::vector<ClassifiedEdge> relatedEdge; // ��¼����߱��к͵�ǰ��߻��߻�����id��ͬ��
	std::vector<std::vector<ClassifiedPolygon> > classifiedPolygonTable;
	std::vector<std::vector<ClassifiedEdge> > classifiedEdgeTable;
	std::vector<ClassifiedPolygon> activePolygonTable;
	std::vector<ActiveEdge> activeEdgeTable;	
	
	void addActiveEdgeFromActivePolygon(int y, ClassifiedPolygon& activePolygon); // �Ե�ǰ�Ļ����Σ�������Ҫ����Ļ��
	bool findReplaceEdgeFromActivePolygon(int y, ActiveEdge& activeEdge); // �����ĳһ�ε�dy����0ʱѰ��һ������ߣ������»��
	void release();
};



