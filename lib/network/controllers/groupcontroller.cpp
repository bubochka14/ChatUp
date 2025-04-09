#include "groupcontroller.h"
using namespace Group;
void Controller::setModel(Group::Model* other)
{
	if (_model == other)
		return;
	_model = other;
	_model->setParent(this);
	emit modelChanged();
}
bool CallerController::incrementMessageCount(int roomID,size_t count)
{
	auto roomModel = model();
	auto g= qScopeGuard([=]() {roomModel->unlock(); });
	roomModel->lock();
	auto index = roomModel->idToIndex(roomID);
	if (!index.isValid())
		return false;
	int messageCount = roomModel->data(index, Group::Model::MessageCountRole).toInt();
    return roomModel->setData(index, qint32(count+ messageCount), Group::Model::MessageCountRole);
}
void Controller::reset()
{
	_model->reset();
}
CallerController::CallerController(std::shared_ptr<NetworkCoordinator> m, QObject* parent)
	:Controller(parent)
	,_manager(m) 
	,_updateReadingsTimer(new QTimer(this))
{
	_updateReadingsTimer->setInterval(1000);
	_updateReadingsTimer->setSingleShot(true);
	QObject::connect(_updateReadingsTimer, &QTimer::timeout, this, [this]() {
		auto groupModel = model();
		while (_readingsUpdateQueue.size())
		{
			int roomID = _readingsUpdateQueue.front();
			_readingsUpdateQueue.pop();
			auto g= qScopeGuard([=]() {groupModel->unlock(); });
			groupModel->lock();
			auto index = groupModel->idToIndex(roomID);
			if (!index.isValid())
				continue;
			Message::Api::MarkRead req;
			req.count = groupModel->data(index,Group::Model::LocalReadings).toInt();
			req.roomID = roomID;
			req.exec(_manager);

		}
	});
	using namespace Group::Api;
	Create::handle(_manager, [this](ExtendedData&& group) {
		auto groupModel = model();
		auto g= qScopeGuard([=]() {groupModel->unlock(); });
		groupModel->lock();
		groupModel->insertRow(0);
		groupModel->setData(groupModel->index(0), std::move(group));
	});
	Message::Api::Create::handle(_manager, [this](Message::Data&& mess) {

		auto roomModel = model();
		auto g = qScopeGuard([=]() {roomModel->unlock(); });
		roomModel->lock();
		auto index = roomModel->idToIndex(mess.roomID);
		if (!index.isValid())
			return;
		int messageCount = roomModel->data(index, Group::Model::MessageCountRole).toInt();
		roomModel->setData(index, messageCount+1, Group::Model::MessageCountRole);
		roomModel->setData(index, std::move(mess.body), Group::Model::LastMessageBody);
		roomModel->setData(index, std::move(mess.time), Group::Model::LastMessageTime);
		roomModel->setData(index, mess.userID, Group::Model::LastMessageSender);
	});
	Update::handle(_manager, [this](Update::GroupUpdate&& upd){
		auto groupModel = model();
		auto g = qScopeGuard([=]() {groupModel->unlock(); });
		groupModel->lock();
		auto index = groupModel->idToIndex(upd.id);
		if (!index.isValid())
			return;
		if (upd.tag.has_value())
			groupModel->setData(index,QString::fromStdString(std::move(*upd.tag)),Group::Model::TagRole);
		if (upd.name.has_value())
			groupModel->setData(index, QString::fromStdString(std::move(*upd.name)), Group::Model::NameRole);
		if (upd.foreignReadings.has_value())
			groupModel->setData(index, *upd.foreignReadings, Group::Model::ForeignReadings);
		if (upd.localReadings.has_value())
			groupModel->setData(index, *upd.localReadings, Group::Model::LocalReadings);
		if (upd.messageCount.has_value())
			groupModel->setData(index, *upd.messageCount, Group::Model::MessageCountRole);
		});
	/*_manager->addClientHandler("addRoom", [this](json&& roomJson) 
		{
			model()->insertRow(0);
			model()->setData(model()->index(0), std::move(roomJson));
		});
	_manager->addClientHandler("updateRoom", [this](json&& roomJson) 
		{
			Group::Data group(roomJson);
			model()->setData(model()->idToIndex(group.id), hash, roommodel::hashrole);
			});*/
}

Controller::Controller(QObject* parent)
	:AbstractController(parent)
	,_model(new Group::Model(this))
{}
Model* Controller::model() const
{
	return _model;
}

QFuture<int> CallerController::create(const QString& name)
{
	Group::Api::Create req;
	req.name = name.toStdString();
	return req.exec(_manager).then([this](Group::ExtendedData&& res) {
		if (!res.id)
			throw QString("Room id receive error");
		int newid = res.id;
		auto groupModel = model();
		auto g= qScopeGuard([=]() {groupModel->unlock(); });
		groupModel->lock();
		groupModel->insertRow(0);
		groupModel->setData(model()->index(0), std::move(res));
		return newid;
		});
}
QFuture<void> CallerController::initialize()
{
	Group::Api::GetAll req;
	return req.exec(_manager).then([=](std::vector<Group::ExtendedData>&& rooms){
			model()->insertRange(0, std::make_move_iterator(rooms.begin()),
				std::make_move_iterator(rooms.end()));
		});
}
QFuture<void> CallerController::addUser(int userID, int roomID)
{
	Api::AddUser req;
	req.roomID = roomID;
	req.userID = userID;
	return req.exec(_manager);
}
QFuture<void> CallerController::remove(int id)
{
	Api::Delete req;
	req.roomID = id;
	return req.exec(_manager);
}
QFuture<void> CallerController::update(const QVariantHash& data)
{
	Api::Update req;
	return req.exec(_manager).then([](Group::Data&&){});
}
QFuture<void> CallerController::load()
{
	Api::GetAll req;
    return req.exec(_manager).then([this](std::vector<Group::ExtendedData> res){
        model()->insertRange(0, std::make_move_iterator(res.begin()),
                                       std::make_move_iterator(res.end()));
    });
}
QFuture<void> CallerController::setLocalReadings(int roomID, size_t count)
{
	auto roomModel = model();
	auto g= qScopeGuard([=]() {roomModel->unlock(); });
	roomModel->lock();
	auto index = roomModel->idToIndex(roomID);
	if (!index.isValid())
	{
		return QtFuture::makeExceptionalFuture(std::make_exception_ptr(std::string("Unknow roomID received")));
	}
	if (roomModel->data(index,Group::Model::LocalReadings).toInt() >= count)
	{
		return QtFuture::makeReadyVoidFuture();
	}
    roomModel->setData(index, quint32(count), Group::Model::LocalReadings);
	_readingsUpdateQueue.push(roomID);
	if (!_updateReadingsTimer->isActive())
		_updateReadingsTimer->start();
	return QtFuture::makeReadyVoidFuture();

}
void CallerController::setLastSender(int roomID,int id)
{
	auto roomModel = model();
	auto g = qScopeGuard([=]() {roomModel->unlock(); });
	roomModel->lock();
	auto index = roomModel->idToIndex(roomID);
	if (!index.isValid())
		return;
	roomModel->setData(index, id, Group::Model::LastMessageSender);

}
void CallerController::setLastMessageBody(int roomID,const QString& body)
{
	auto roomModel = model();
	auto g = qScopeGuard([=]() {roomModel->unlock(); });
	roomModel->lock();
	auto index = roomModel->idToIndex(roomID);
	if (!index.isValid())
		return;
	roomModel->setData(index,body, Group::Model::LastMessageBody);

}
void CallerController::setLastMessageTime(int roomID,const QDateTime& time)
{
	auto roomModel = model();
	auto g = qScopeGuard([=]() {roomModel->unlock(); });
	roomModel->lock();
	auto index = roomModel->idToIndex(roomID);
	if (!index.isValid())
		return;
	roomModel->setData(index, time, Group::Model::LastMessageTime);

}
