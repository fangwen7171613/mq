#pragma once

#include "sdp/plugin/i_plugin.h"

class CMqSchedulerServicePlugIn final : public SDP::IPlugin
{
public:
    int32_t OnInit() override;
};