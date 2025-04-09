#include "messagecontroller.h"

using namespace Message;

CallerController ::CallerController(std::shared_ptr<NetworkCoordinator> m,
	QObject* parent)
	:Controller(parent)
	,_manager(m)
	,_tempMessageCounter(1)

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
QFuture<int> CallerController::create(const QString& body, int roomID)
{
	if (!_history.contains(roomID))
	{
		_history[roomID] = new Message::Model(this);
	}
	Message::Model* model = _history[roomID];
	model->lock();
	auto g = qScopeGuard([=]() {model->unlock(); });

	if (model->insertRow(0))
	{
		int tempMessageId = -(++_tempMessageCounter);
		auto index = model->index(0);
		model->setData(index, body, Message::Model::BodyRole);
		model->setData(index, tempMessageId, Message::Model::IdRole);
		model->setData(index, _manager->currentUser(), Message::Model::UserIdRole);
		model->setData(index, QDateTime::currentDateTime(), Message::Model::TimeRole);
		model->setData(index, -1, Message::Model::MessageIndexRole);
		Message::Api::Create req;
		req.body = std::move(body.toStdString());
		req.roomID = roomID;
		return req.exec(_manager).then([=](Message::Data&& data){
			model->lock();
			auto g = qScopeGuard([=]() {model->unlock(); });
			auto index = model->idToIndex(tempMessageId);
			if (index.isValid()&& model->setData(index, data))
				return data.id;
			return Message::invalidID;

			

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
