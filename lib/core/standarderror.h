#pragma once
#include "core_include.h"
#include <qqmlengine.h>
class StandardError
{
	QML_ELEMENT;
	Q_GADGET;
public:
	StandardError() = default;
	StandardError(int c, QString desc)
		:code(c)
		,description(std::move(desc))
	{}
	int code = defaultErrorCode;
	QString description = "unknown";
	static constexpr int defaultErrorCode = 0;
};