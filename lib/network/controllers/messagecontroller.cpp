#include "messagecontroller.h"

using namespace Message;
CallerController ::CallerController(NetworkManager* m,
	QObject* parent)
	:Controller(parent)
	,_manager(m)

{

}
Controller::Controller(QObject* parent)
	:AbstractController(parent)
{
}
QFuture<void> CallerController::initialize()
{
	Api::Create::handle(_manager, [this](Message::Data&& data) {
		if (data.id == Message::invalidID || data.roomID == Group::invalidID)
			return;
		Message::Model* model;
		if (!_history.contains(data.roomID))
		{
			model = new Message::Model(this);
			_history[data.roomID] = model;
		}
		else
			model = _history[data.roomID];
		model->insertData(0, std::move(data));
		});
	return QtFuture::makeReadyVoidFuture();
}

QFuture<void> CallerController::updateMessageBody(const QString& body, int roomID)
{
	return QtFuture::makeReadyVoidFuture();
}
QFuture<void> CallerController::create(const QString& body, int roomID)
{
	if (!_history.contains(roomID))
	{
		_history[roomID] = new Message::Model(this);
	}
	Message::Model* model = _history[roomID];
	if (model->insertRow(0))
	{
		int tempMessageId = -(++_tempMessageCounter);
		model->setData(model->index(0), body, Message::Model::BodyRole);
		model->setData(model->index(0), tempMessageId, Message::Model::IdRole);
		model->setData(model->index(0), _manager->userID(), Message::Model::UserIdRole);
		model->setData(model->index(0), QDateTime::currentDateTime(), Message::Model::TimeRole);
		Message::Api::Create req;
		req.body = body;
		req.roomID = roomID;
		return req.exec(_manager).then([=](Message::Data&& data)
			{
				if (!model->idToIndex(tempMessageId).isValid())
					return;
				model->setData(model->idToIndex(tempMessageId), data);

			});
	}

}
Model* CallerController::model(int roomID)
{
	if (!_history.contains(roomID))
		_history[roomID] = new Message::Model;
	return _history[roomID];
}

QFuture<void> CallerController::load(int roomID,int row, int from, int to)
{
	Api::GetByIndex req;
	req.roomID = roomID;
	req.startIndex = from;
	req.endIndex = to;
	return req.exec(_manager).then([this,roomID,row](QList<Message::Data>&& res) {
		auto model = _history[roomID];
		model->insertDataList(row, std::move(res));
		});

}

QFuture<void> CallerController::remove(int roomid, int messId)
{
	return QtFuture::makeReadyVoidFuture();
}
QFuture<void> CallerController::markAsRead(int roomID, size_t count)
{
	return QtFuture::makeReadyVoidFuture();
}