#include "messagecontroller.h"

using namespace Message;

CallerController ::CallerController(std::shared_ptr<NetworkCoordinator> m,
	QObject* parent)
	:Controller(parent)
	,_manager(m)

{
	Api::Create::handle(_manager, [this](Message::Data&& data) {
		QtFuture::makeReadyFuture().then(this, [this, data = std::move(data)]() {
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
		});
}
Controller::Controller(QObject* parent)
	:AbstractController(parent)
{
}
QFuture<void> CallerController::initialize()
{

	return QtFuture::makeReadyVoidFuture();
}

QFuture<void> CallerController::updateMessageBody(const QString& body, int roomID)
{
	return QtFuture::makeReadyVoidFuture();
}
void Message::CallerController::reset()
{
	_history.clear();
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
		model->setData(model->index(0), _manager->currentUser(), Message::Model::UserIdRole);
		model->setData(model->index(0), QDateTime::currentDateTime(), Message::Model::TimeRole);
		Message::Api::Create req;
		req.body = body.toStdString();
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
	return req.exec(_manager).then([this,roomID,row](std::vector<Message::Data>&& res) {
		auto model = _history[roomID];
		model->insertRange(row, std::make_move_iterator(res.rbegin()),
			std::make_move_iterator(res.rend()));
		});

}

QFuture<void> CallerController::remove(int roomid, int messId)
{
	return QtFuture::makeReadyVoidFuture();
}
QFuture<void> CallerController::markAsRead(int roomID, size_t count)
{
	auto msgModel = model(roomID);
	if (msgModel->userReadings() >= count)
		return QtFuture::makeReadyVoidFuture();
	msgModel->setUserReadings(count);
	Api::MarkRead req;
	req.count = count;
	req.roomID = roomID;
	return req.exec(_manager).then([]() {});
}