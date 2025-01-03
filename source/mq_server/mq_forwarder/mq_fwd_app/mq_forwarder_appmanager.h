#pragma once

#include "sdp/app_mng.h"
#include "mq_common/mq_id_def.h"

class CMqForwarderAppManager final : public SDP::CAppMng
{
public:
    static constexpr std::uint16_t EN_NODE_TYPE = MQ_ID::EN_MQ_FORWARDER_NODE_TYPE;

protected:
    std::int32_t OnInit() override;
    std::int32_t OnExit() override;
};