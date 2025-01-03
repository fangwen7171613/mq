#include "sdp/pch.h"
#include "sdp/dispatcher/dispatcher_center.h"

#include "mq_scheduler_service/mq_scheduler_service.h"
#include "mq_scheduler_service/mq_scheduler_service_plugin.h"

int32_t CMqSchedulerServicePlugIn::OnInit()
{
    const auto pScheduler = new(std::nothrow) CMqSchedulerService;
    const int32_t nRet = SDP::CDispatcherCenterDemon::Instance()->RegisterTask(pScheduler, CMqSchedulerService::EN_SERVICE_TYPE, CMqSchedulerService::EN_SERVICE_ID, true);

    if (nRet)
    {
        SDP_RUN_LOG_ERROR("mq scheduler service register failed");
    }
    else
    {
        SDP_RUN_LOG_FINE("mq scheduler service register successfully");
    }

    return nRet;
}

#ifdef USE_PLUGIN
#include "sdp/plugin/plugin_export.h"

extern "C" PLUGIN_EXPORT SDP::IPlugin* CreateSdpPlugin(void)
{
    return new(std::nothrow) CMqSchedulerServicePlugIn;
}
#endif