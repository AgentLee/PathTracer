#pragma once
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QList>
#include <string>
#include <raytracing/film.h>
#include <scene/scene.h>
#include <scene/camera.h>
#include <scene/transform.h>

class JSONReader
{
public:
    void LoadSceneFromFile(QFile &file, const QStringRef &local_path, Scene &scene);
    bool LoadGeometry(QJsonObject &geometry, QMap<QString, std::shared_ptr<Material>> mtl_map, const QStringRef &local_path, QList<std::shared_ptr<Primitive>> *primitives, QList<std::shared_ptr<Drawable> > *drawables);
    bool LoadLights(QJsonObject &geometry, QMap<QString, std::shared_ptr<Material>> mtl_map, const QStringRef &local_path, QList<std::shared_ptr<Primitive>> *primitives, QList<std::shared_ptr<Light>> *lights, QList<std::shared_ptr<Drawable>> *drawables);
    bool LoadMaterial(QJsonObject &material, const QStringRef &local_path, QMap<QString, std::shared_ptr<Material> > *mtl_map);
    Camera LoadCamera(QJsonObject &camera);
    Transform LoadTransform(QJsonObject &transform);
    glm::vec3 ToVec3(const QJsonArray &s);
    glm::vec3 ToVec3(const QStringRef &s);

	bool Compare(QString str1, std::string str2)
	{
		return QString::compare(str1, QString(str2.c_str())) == 0;
	}

	//template<typename T>
	//T GetValue(QJsonObject& material, std::string name, T defaultValue)
	//{
	//	
	//}

	glm::vec3 GetValue(QJsonObject& material, std::string name, glm::vec3 defaultValue)
	{
		auto qName = QString(name.c_str());
		return material.contains(qName) ? ToVec3(material[qName].toArray()) : defaultValue;
	}

	double GetValue(QJsonObject& material, std::string name, float defaultValue)
	{
		auto qName = QString(name.c_str());
		return material.contains(qName) ? material[qName].toDouble() : defaultValue;
	}
};
