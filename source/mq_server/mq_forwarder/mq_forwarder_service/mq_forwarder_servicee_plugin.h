#pragma once

#include "sdp/plugin/i_plugin.h"

class CMqForwarderServicePlugIn final : public SDP::IPlugin
{
public:
    std::int32_t OnInit() override;
};