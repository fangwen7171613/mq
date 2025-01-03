#include "sdp/pch.h"
#ifdef USING_SSL
#include "sdp/license.h"
#endif

#include "mq_fwd_app/mq_forwarder_appmanager.h"

int32_t CMqForwarderAppManager::OnInit()
{
    SDP_FWD_CALL(SDP::CAppMng::OnInit());

    // #ifdef USING_SSL
    //	SDP::CSdpLicense license;
    //
    //	if (-1 == license.VerifyLicense())
    //	{
    //		return -1;
    //	}
    // #endif

    return 0;
}

int32_t CMqForwarderAppManager::OnExit()
{
    SDP_FWD_CALL(SDP::CAppMng::OnExit());
    // todo add service logic

    return 0;
}