#include "DataStreamClient.h"
#include <iostream>
#include <typeinfo>
#include <csignal>

void signalHandler(int signal_num)
{
    std::cout << "Interrupted: SIG: " << signal_num << std::endl;
    exit(signal_num);
}

int main()
{
    signal(SIGINT, signalHandler);

    std::string hostname = "192.168.50.56";
    ViconDataStreamSDK::CPP::Client StackClient;
    std::cout << "Estabilishing connection to " << hostname << " ..." << std::endl;
    StackClient.Connect(hostname);

    while(!StackClient.IsConnected().Connected)
    {
        std::cout << "Estabilishing connection to " << hostname << " ..." << std::endl;
        StackClient.Connect(hostname);
        sleep(1);   
    }

    StackClient.EnableSegmentData();
    StackClient.EnableMarkerData();
    StackClient.EnableUnlabeledMarkerData();
    StackClient.EnableMarkerRayData();
    StackClient.EnableDeviceData();
    StackClient.EnableDebugData();
    StackClient.SetStreamMode(ViconDataStreamSDK::CPP::StreamMode::ClientPull);
    StackClient.SetBufferSize(1024);

    std::cout << "Connected. Initialization complete" << std::endl;

    while(true)
    {
        auto frame = StackClient.GetFrame();
        if(frame.Result == ViconDataStreamSDK::CPP::Result::Success)
        {
            unsigned int subject_count = StackClient.GetSubjectCount().SubjectCount;
            for(unsigned int subject_index = 0; subject_index < subject_count; ++subject_index)
            {
                std::string subject_name = StackClient.GetSubjectName(subject_index).SubjectName;
                unsigned int segment_count = StackClient.GetSegmentCount(subject_name).SegmentCount;
                for(unsigned int segment_index = 0; segment_index < segment_count; ++segment_index)
                {
                    std::string segment_name = StackClient.GetSegmentName(subject_name, segment_index).SegmentName;
                    auto trans = StackClient.GetSegmentGlobalTranslation(subject_name, segment_name);
                    auto rot = StackClient.GetSegmentGlobalRotationEulerXYZ(subject_name, segment_name);

                    std::cout << "Segment Name: " << segment_name << " -> Translation: (" << trans.Translation[0] << " ," << trans.Translation[1] << " ," << trans.Translation[2] << ")" << std::endl;
                    std::cout << "Segment Name: " << segment_name << " -> Rotation: (" << rot.Rotation[0] << " ," << rot.Rotation[1] << " ," << rot.Rotation[2] << ")"<< std::endl;
                }
            }
        }
    }
    return 0;
}