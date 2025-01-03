#include "sdp/main.h"

#include "mq_sch_app/mq_scheduler_appmanager.h"

int main(int argc, const char* argv[])
{
    return SDP::SdpMain<CMqSchedulerAppManager>(argc, argv);
}