// Author: Peiyao Li
// Date:   Jan 8 2023
#pragma once
#include "Vector.h"
#include "Model.h"
const float kd = 0.8;

// ��ɫģʽ��ȫ��ɫ����Ƭ�����ɫ�͵��ԴDiffuse����
enum ShaderPattern {
	WHITE = 0,
	RANDOM_COLOR = 1,
	LIGHT_DIFFUSE = 2
};

// ��Ҫ˵����ʱ��Ϊ��չʾ����ͬ��Ƭ�ı߽磬��ɫʱÿ����Ƭ����ͬ������ɫ����������ƽ��ģʽ�������ݵ����ɫ��ֵ
class Shader
{
public:
	Point3f lightPos = Point3f(400.0f, 300.0f, 500.0f);
	Color3f lightCol = Color3f(0.6, 0.6, 0.6);
	Color3f ambientCol = Color3f(0.3, 0.3, 0.3);

	void shaderModel(Model& model, ShaderPattern shaderpattern);
};