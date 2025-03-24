#pragma once
#include "core_include.h"
#include <unordered_map>
#include "nlohmann/json.hpp"
#include <QObject>
using json = nlohmann::json;
class CC_CORE_EXPORT QtEventLoopEmplacer : public QObject
{
	Q_OBJECT;
public:
	using Task = std::function<void()>;
	QtEventLoopEmplacer(QObject* parent = nullptr)
		:QObject(parent)
	{
		connect(this, &QtEventLoopEmplacer::emplaceTask, this, &QtEventLoopEmplacer::handleTask);

	}
signals:
	void emplaceTask(const Task&);
private slots:
	void handleTask(const Task& t)
	{
		t();
	}
};
namespace Core
{

	static void Init()
	{
		static bool inited = false;
		if (!inited)
		{
		}
	}
}