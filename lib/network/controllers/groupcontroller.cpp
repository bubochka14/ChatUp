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
CallerController::CallerController(NetworkManager*m, QObject* parent)
	:Controller(parent)
	,_manager(m)
{}

Controller::Controller(QObject* parent)
	:AbstractController(parent)
	,_model(new Group::Model(this))
{}
Model* Controller::model() const
{
	return _model;
}
void CallerController::connectToDispatcher() {
	//_manager->dispatcher()->addHandler("addRoom", [=](const QVariantHash& hash) {
	//	userGroups()->insertRow(0);
	//	userGroups()->setData(userGroups()->index(0), hash, RoomModel::HashRole);
	//	});
	//_manager->dispatcher()->addHandler("updateRoom", [=](const QVariantHash& hash) {
	//	if (!hash.contains("roomID"))
	//	{
	//		qCWarning(LC_CMESSAGE_CONTROLLER) << "received updateRoom data doesn`t have roomID field";
	//		return;
	//	}
	//	RoomModel* model = findModel(hash["roomID"].toInt());
	//	if (!model)
	//	{
	//		qCWarning(LC_CMESSAGE_CONTROLLER) << "Unknown roomID received";
	//		return;
	//	}
	//	model->setData(model->idToIndex(hash["roomID"].toInt()), hash, RoomModel::HashRole);
	//	});
}
QFuture<int> CallerController::create(const QString& name)
{
	Group::Api::Create req;
	req.name = name;
	return req.exec(_manager).then([this](Group::Data&& res) {
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
	return req.exec(_manager).then([=](QList<Group::Data>&& rooms)
		{
			int rows = rooms.size();
			model()->insertRows(0, rooms.count());
			for (size_t i = 0; i < rows; i++)
			{
				model()->setData(model()->index(0 + i), rooms.takeFirst());
			}
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
QFuture<void> CallerController::load(int count)
{
	return QtFuture::makeReadyVoidFuture();
}