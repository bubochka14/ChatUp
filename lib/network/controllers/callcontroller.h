#pragma once
#include <QFuture>
#include <network_include.h>
#include <qvideosink.h>
#include <qqmlengine.h>
#include "participatemodel.h"
#include "abstractcontroller.h"
#include "api/call.h"
#include "networkmanager.h"
#include "rtcservice.h"
#include "rtppacketizer.h"
#include "encoder.h"
#include <qaudiooutput.h>
#include <qaudiodevice.h>
#include <qaudiosink.h>
#include <qmediadevices.h>
#include "media.h"
#include "audiooutput.h"
#include <qtimer>
//#include "audiooutput.h"
Q_DECLARE_LOGGING_CATEGORY(LC_CALL_CONTROLLER);

namespace Call {
	class Controller;
	class Handler;
	class CC_NETWORK_EXPORT Handler : public QObject
	{
		Q_OBJECT;
		QML_NAMED_ELEMENT(CallHandler);
		QML_UNCREATABLE("");
	public:
		enum State
		{
			Disconnected,
			InsideTheCall
		};Q_ENUM(State)
		explicit Handler(Controller* controller, int roomID);
		State state() const;
		Participate::Model* participants();
		int roomID() const;
		bool hasVideo();
		bool hasAudio();
		Q_INVOKABLE QFuture<void> openVideo(Media::Video::StreamSource* source);
		Q_INVOKABLE QFuture<void> openAudio(Media::Audio::StreamSource* source);
		//Q_INVOKABLE QFuture<void> connectAudioSink(int userID, QAudioSi);
		Q_INVOKABLE QFuture<void> disconnect();
		Q_INVOKABLE void closeVideo();
		Q_INVOKABLE void closeAudio();
		Q_INVOKABLE QFuture<void> join();
		Q_INVOKABLE void connectVideoSink(int userID, QVideoSink*);
		Q_INVOKABLE void release();
	signals:
		void participantsChanged();
		void isMutedChanged();
		void stateChanged();
		void hasAudioChanged();
		void hasVideoChanged();
	protected:
		void setState(State other);
		void setHasAudio(bool);
		void setHasVideo(bool);
	private:
		Q_PROPERTY(Participate::Model* participants READ participants NOTIFY participantsChanged);
		Q_PROPERTY(State state READ state NOTIFY stateChanged);
		Q_PROPERTY(int roomID READ roomID CONSTANT)
		Q_PROPERTY(bool hasVideo READ hasVideo NOTIFY hasVideoChanged)
		Q_PROPERTY(bool hasAudio READ hasAudio NOTIFY hasAudioChanged)

		Controller* _controller;
		Participate::Model* _prt;
		const int _roomID;
		bool _isMuted;
		State _state;
		friend class Controller;
	};
	struct StreamContext
	{
		Media::Video::StreamSource* videoSource = nullptr;
		std::unique_ptr<Media::Video::SinkConnector> videoSinkConnector = nullptr;
		Media::Audio::StreamSource* audioSource = nullptr;
		//Media::StreamSource* audio = nullptr;
		//Media::Audio::Source audioSource;
	};



	class CC_NETWORK_EXPORT Controller : public AbstractController
	{
		Q_OBJECT;
		QML_NAMED_ELEMENT(CallController)
		QML_UNCREATABLE("");
	public:
		explicit Controller(std::shared_ptr<NetworkCoordinator> manager,
			QObject* parent = nullptr);
		Q_INVOKABLE Handler* handler(int roomID);

		QFuture<void> disconnect(Handler* h);
		QFuture<void> join(Handler* h);
		QFuture<void> openVideo(Handler* h, Media::Video::StreamSource* st);
		QFuture<void> openAudio(Handler* h, Media::Audio::StreamSource* st);
		void closeVideo(Handler* h);
		void closeAudio(Handler* h);
		void connectVideoSink(Handler* h,int userID, QVideoSink*);
		void setAudio(bool st, Handler* h);
		void setVideo(bool st, Handler* h);
		bool hasAudio(Handler* h);
		bool hasVideo(Handler* h);
		void release(Handler* h);

	private:
		QtEventLoopEmplacer* _guiEmplacer;
		QHash<int, Handler*> _handlers;
		std::shared_ptr<NetworkCoordinator> _manager;
		std::shared_ptr<rtc::Service> _rtc;
		std::unordered_map<int, StreamContext> _userContexts;
		std::optional<int> _activeCallRoomID;
	};
}