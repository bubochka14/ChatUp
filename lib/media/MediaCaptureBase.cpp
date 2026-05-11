#include "MediaCaptureBase.h"
using namespace chatup;

bool MediaDevice::operator!=(const MediaDevice& other) const
{
	return m_name != other.m_name;
}

bool MediaDevice::operator==(const MediaDevice& other) const
{
	return !(*this != other);
}

std::optional<chatup::SourceConfig> MediaCaptureBase::Open(const MediaDevice& device)
{
	if (m_openedDevice == device)
	{
		return m_lastSourceConfig;
	}
	return OpenInternal(device);
}

std::optional<MediaDevice> MediaCaptureBase::GetOpenedDevice()
{
	return m_openedDevice;
}

std::shared_ptr<PacketPipe> chatup::MediaCaptureBase::GetOutput() const
{
	return m_output;
}

MediaCaptureBase::MediaCaptureBase(std::shared_ptr<PacketPipe> output) : m_output(std::move(output)){}
