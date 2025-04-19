#pragma once
#include "abstractchatwindow.h"
#include <QQuickWindow>
#include <qurl.h>
#include <QtConcurrent/qtconcurrentrun.h>
#include <qqmlincubator.h>
#include <qqmlcontext.h>
#include <qqmlapplicationengine.h>
#include <qqmlcomponent.h>
#include "controllers/controllermanager.h"
#include <QTimer>
#include "pipelines.h"
#include <qloggingcategory.h>
#include "audiooutput.h"
Q_DECLARE_LOGGING_CATEGORY(LC_QML_CHAT_WINDOW);
struct MessageControllerWrapper
{
	Q_GADGET;
	QML_FOREIGN(Message::Controller);
	QML_SINGLETON;
	QML_NAMED_ELEMENT(MessageController);
public:
	inline static Message::Controller* singletonInstance = nullptr;

	static Message::Controller* create(QQmlEngine*, QJSEngine* engine)
	{
		if (s_engine)
		{
			if (engine == s_engine)
				qCFatal(LC_QML_CHAT_WINDOW) << "Received new qqmlengine";
		}
		else
			s_engine = engine;
		QJSEngine::setObjectOwnership(singletonInstance,
			QJSEngine::CppOwnership);
		return singletonInstance;
	}
private:
	inline static QJSEngine* s_engine = nullptr;
};
struct GroupControllerWrapper
{
	Q_GADGET;
	QML_FOREIGN(Group::Controller);
	QML_SINGLETON;
	QML_NAMED_ELEMENT(GroupController);
public:
	inline static Group::Controller* singletonInstance = nullptr;

	static Group::Controller* create(QQmlEngine*, QJSEngine* engine)
	{
		if (s_engine)
		{
			if (engine == s_engine)
				qCFatal(LC_QML_CHAT_WINDOW) << "Received new qqmlengine";
		}
		else
			s_engine = engine;
		QJSEngine::setObjectOwnership(singletonInstance,
			QJSEngine::CppOwnership);
		return singletonInstance;
	}
private:
	inline static QJSEngine* s_engine = nullptr;
};
struct CallControllerWrapper
{
	Q_GADGET;
	QML_FOREIGN(Call::Controller);
	QML_SINGLETON;
	QML_NAMED_ELEMENT(CallController);
public:
	inline static Call::Controller* singletonInstance = nullptr;

	static Call::Controller* create(QQmlEngine*, QJSEngine* engine)
	{
		if (s_engine)
		{
			if (engine == s_engine)
				qCFatal(LC_QML_CHAT_WINDOW) << "Received new qqmlengine";
		}
		else
			s_engine = engine;
		QJSEngine::setObjectOwnership(singletonInstance,
			QJSEngine::CppOwnership);
		return singletonInstance;
	}
private:
	inline static QJSEngine* s_engine = nullptr;
};
struct UserControllerWrapper
{
	Q_GADGET;
	QML_FOREIGN(User::Controller);
	QML_SINGLETON;
	QML_NAMED_ELEMENT(UserController);
public:
	inline static User::Controller* singletonInstance = nullptr;

	static User::Controller* create(QQmlEngine*, QJSEngine* engine)
	{
		if (s_engine)
		{
			if (engine == s_engine)
				qCFatal(LC_QML_CHAT_WINDOW) << "Received new qqmlengine";
		}
		else
			s_engine = engine;
		QJSEngine::setObjectOwnership(singletonInstance,
			QJSEngine::CppOwnership);
		return singletonInstance;
	}
private:
	inline static QJSEngine* s_engine = nullptr;
};
struct CurrentUserWrapper 
{
	Q_GADGET;
	QML_FOREIGN(User::Handle);
	QML_SINGLETON;
	QML_NAMED_ELEMENT(CurrentUser);
public:
	inline static User::Handle* singletonInstance = nullptr;

	static User::Handle* create(QQmlEngine* , QJSEngine* engine)
	{
		if (s_engine)
		{
			if (engine == s_engine)
				qCFatal(LC_QML_CHAT_WINDOW) << "Received new qqmlengine";
		}
		else
			s_engine = engine;
		QJSEngine::setObjectOwnership(singletonInstance,
			QJSEngine::CppOwnership);
		return singletonInstance;
	}
private:
	inline static QJSEngine* s_engine = nullptr;
};
struct CameraPipelineWrapper
{
	Q_GADGET;
	QML_FOREIGN(Media::Video::CameraPipeline);
	QML_SINGLETON;
	QML_NAMED_ELEMENT(CameraPipeline);
public:
	inline static Media::Video::CameraPipeline* singletonInstance = nullptr;

	static Media::Video::CameraPipeline* create(QQmlEngine*, QJSEngine* engine)
	{
		if (s_engine)
		{
			if (engine == s_engine)
				qCFatal(LC_QML_CHAT_WINDOW) << "Received new qqmlengine";
		}
		else
			s_engine = engine;
		QJSEngine::setObjectOwnership(singletonInstance,
			QJSEngine::CppOwnership);
		return singletonInstance;
	}
private:
	inline static QJSEngine* s_engine = nullptr;
};
struct AudioOutputWrapper
{
	Q_GADGET;
	QML_FOREIGN(Media::Audio::Output);
	QML_NAMED_ELEMENT(MyAudioOutput);
public:
	inline static Media::Audio::Output* singletonInstance = nullptr;

	static Media::Audio::Output* create(QQmlEngine*, QJSEngine* engine)
	{
		if (s_engine)
		{
			if (engine == s_engine)
				qCFatal(LC_QML_CHAT_WINDOW) << "Received new qqmlengine";
		}
		else
			s_engine = engine;
		QJSEngine::setObjectOwnership(singletonInstance,
			QJSEngine::CppOwnership);
		return singletonInstance;
	}
private:
	inline static QJSEngine* s_engine = nullptr;
};
struct MicrophonePipelineWrapper
{
	Q_GADGET;
	QML_FOREIGN(Media::Audio::MicrophonePipeline);
	QML_SINGLETON;
	QML_NAMED_ELEMENT(MicrophonePipeline);
public:
	inline static Media::Audio::MicrophonePipeline* singletonInstance = nullptr;

	static Media::Audio::MicrophonePipeline* create(QQmlEngine*, QJSEngine* engine)
	{
		if (s_engine)
		{
			if (engine == s_engine)
				qCFatal(LC_QML_CHAT_WINDOW) << "Received new qqmlengine";
		}
		else
			s_engine = engine;
		QJSEngine::setObjectOwnership(singletonInstance,
			QJSEngine::CppOwnership);
		return singletonInstance;
	}
private:
	inline static QJSEngine* s_engine = nullptr;
};
class QmlChatWindow final: public AbstractChatWindow
{
	Q_OBJECT;
	QML_ELEMENT;
	QML_UNCREATABLE("");
public:
	explicit QmlChatWindow(
		QQmlEngine* eng,
		std::shared_ptr<ControllerManager> manager,
		QObject* parent = nullptr
	);
	QFuture<void> initialize() override;
	bool hasError() const;
	QString errorString() const; 
	~QmlChatWindow();
public slots:
	void show() override;
	void hide() override;
private slots:
	void finalizeCreation();
private:
	bool _hasError;
	QString _error;
	std::shared_ptr<ControllerManager> _manager;
	std::unique_ptr<QPromise<void>> _initPromise;
	QQmlIncubator _inc;
	QTimer _creationChecker;
	QQuickWindow* _window;
	QQmlComponent _comp;
};
