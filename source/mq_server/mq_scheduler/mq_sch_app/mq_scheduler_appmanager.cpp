#include "sdp/pch.h"
#ifdef USING_SSL
#include "sdp/license.h"
#endif

#include "sdp/comm_def.h"
#include "mq_sch_app/mq_scheduler_appmanager.h"

int32_t CMqSchedulerAppManager::OnInit()
{
    SDP_FWD_CALL(SDP::CAppMng::OnInit());

    return 0;
}

int32_t CMqSchedulerAppManager::OnExit()
{
    SDP_FWD_CALL(SDP::CAppMng::OnExit());
    // todo add service logic

    return 0;
}