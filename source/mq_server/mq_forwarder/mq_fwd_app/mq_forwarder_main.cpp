#include "sdp/main.h"

#include "mq_fwd_app/mq_forwarder_appmanager.h"

int main(const int argc, const char* argv[])
{
    return SDP::SdpMain<CMqForwarderAppManager>(argc, argv);
}