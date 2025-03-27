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
void Controller::reset()
{
	_model->reset();
}
CallerController::CallerController(std::shared_ptr<NetworkCoordinator> m, QObject* parent)
	:Controller(parent)
	,_manager(m) 
{
	Group::Api::Create::handle(_manager, [this](Group::ExtendedData&& group) {
		model()->insertRow(0);
		model()->setData(model()->index(0), std::move(group));
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
		model()->insertRow(0);
		model()->setData(model()->index(0), std::move(res));
		return newid;
		});
}
QFuture<void> CallerController::initialize()
{
	Group::Api::GetAll req;
	return req.exec(_manager).then([=](std::vector<Group::ExtendedData>&& rooms)
		{
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
	return req.exec(_manager).then([this](std::vector<Group::ExtendedData> res)
		{
			model()->insertRange(0, std::make_move_iterator(res.begin()), 
				std::make_move_iterator(res.end()));
		});
}