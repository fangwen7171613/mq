#include "sdp/pch.h"
#include "sdp/configure/configure.h"
#include "sdp/dispatcher/dispatcher_center.h"

#include "mq_forwarder_service/mq_forwarder_service.h"
#include "mq_forwarder_service/mq_forwarder_scheduler.h"
#include "mq_forwarder_service/mq_forwarder_servicee_plugin.h"

std::int32_t CMqForwarderServicePlugIn::OnInit()
{
	std::int32_t nMqForwardNum = 1;
	std::int32_t nTcpNodelayFlag = 0;

	if (-1 == SDP::CFG::GetProfileInteger("mq", "mq_forwarder_num", nMqForwardNum))
	{
		SDP_RUN_LOG_WARNING("Failed to read 'mq_forwarder_num' configuration item.");

		return -1;
	}

	if (-1 == SDP::CFG::GetProfileInteger("mq", "tcp_nodelay_flag", nTcpNodelayFlag))
	{
		SDP_RUN_LOG_WARNING("Failed to read 'tcp_nodelay_flag' configuration item.");

		return -1;
	}

	for (std::uint16_t i = 1; i <= nMqForwardNum; ++i)
	{
		if (const auto pForwarderService = new (std::nothrow) CMqForwarderService(nTcpNodelayFlag); SDP::CDispatcherCenterDemon::Instance()->RegisterTask(pForwarderService, CMqForwarderService::EN_SERVICE_TYPE, i, true))
		{
			SDP_RUN_LOG_ERROR("MQ Forwarder Service registration failed.");

			return -1;
		}

		if (nMqForwardNum == i)
		{
			if (const auto pForwarderScheduler = new (std::nothrow) CMqForwarderScheduler(nMqForwardNum); SDP::CDispatcherCenterDemon::Instance()->RegisterTask(pForwarderScheduler, CMqForwarderScheduler::EN_SERVICE_TYPE, 1, true))
			{
				SDP_RUN_LOG_ERROR("mq scheduler agent service registration failed");

				return -1;
			}
		}
	}

	SDP_RUN_LOG_FINE("mq forwarder service registration successfully");

	return 0;
}

#ifdef USE_PLUGIN
#include "sdp/plugin/plugin_export.h"

extern "C" PLUGIN_EXPORT SDP::IPlugin* CreateSdpPlugin(void)
{
	return new (std::nothrow) CMqForwarderServicePlugIn;
}
#endif