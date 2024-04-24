#include "DataStreamClient.h"
#include <iostream>
#include <typeinfo>
#include <csignal>
#include <chrono>
#include <thread>

unsigned long first_timestamp = 0;
unsigned long last_timestamp = 0;
unsigned int frame_received = 0;

//function to handle the SIGINT signal from keyboard (^C) used to stop the receiving of data
void signalHandler(int signal_num)
{
    std::cout << "Interrupted: SIG: " << signal_num << std::endl;
    last_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::cout << "Duration (ms): " << last_timestamp-first_timestamp << std::endl;
    std::cout << "Frames: " << frame_received << std::endl;
    std::cout << "Frequency (Hz): " << static_cast<double>(frame_received*1000)/static_cast<double>(last_timestamp-first_timestamp) << std::endl;
    exit(signal_num);
}

int main()
{
    //initialize the handler and signal
    signal(SIGINT, signalHandler);

    //IP address of the PC hosting VICON in same network, standard port is 801
    std::string hostname = "192.168.50.56";
    ViconDataStreamSDK::CPP::Client StackClient; //client initialization
    std::cout << "Establishing connection to " << hostname << " ..." << std::endl;
    StackClient.Connect(hostname);

    //retrying to connect if connection was unsuccessful
    while(!StackClient.IsConnected().Connected)
    {
        std::cout << "Estabilishing connection to " << hostname << " ...";
        StackClient.Connect(hostname);
        if(!StackClient.IsConnected().Connected)
        {
            std::cout << " failed!" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1)); //cross-platform support for sleep function
        }
    }

    //enablers 
    StackClient.EnableSegmentData();
    StackClient.EnableMarkerData();
    // StackClient.EnableUnlabeledMarkerData();
    StackClient.EnableMarkerRayData();
    StackClient.EnableDeviceData();
    StackClient.EnableDebugData();
    StackClient.SetStreamMode(ViconDataStreamSDK::CPP::StreamMode::ClientPull);
    StackClient.SetBufferSize(1024);

    std::cout << "Connected. Initialization complete." << std::endl;

    //main loop, data receiving stopped by SIGINT (^C)
    while(true)
    {
        auto frame = StackClient.GetFrame();
        //check if the frame was successfully acquired
        if(frame.Result == ViconDataStreamSDK::CPP::Result::Success)
        {
            ++frame_received;
            unsigned int subject_count = StackClient.GetSubjectCount().SubjectCount;
            for(unsigned int subject_index = 0; subject_index < subject_count; ++subject_index)
            {
                std::string subject_name = StackClient.GetSubjectName(subject_index).SubjectName;
                unsigned int segment_count = StackClient.GetSegmentCount(subject_name).SegmentCount;
                for(unsigned int segment_index = 0; segment_index < segment_count; ++segment_index)
                {
                    std::string segment_name = StackClient.GetSegmentName(subject_name, segment_index).SegmentName;
                    unsigned int marker_count = StackClient.GetMarkerCount(subject_name).MarkerCount;
                    for(unsigned int marker_index = 0; marker_index < marker_count; ++marker_index)
                    {
                        std::string marker_name = StackClient.GetMarkerName(subject_name, marker_index).MarkerName;
                        std::string marker_parent_name = StackClient.GetMarkerParentName(subject_name, marker_name).SegmentName;
                        auto marker_trans = StackClient.GetMarkerGlobalTranslation(subject_name, marker_name);
                        std::cout << "Marker Name: " << marker_name << " from: " << marker_parent_name << std::endl;
                        std::cout << "\tTranslation: (" << marker_trans.Translation[0] << " ," << marker_trans.Translation[1] << " ," << marker_trans.Translation[2] << ")" << std::endl;
                    }
                    auto trans = StackClient.GetSegmentGlobalTranslation(subject_name, segment_name);
                    auto rot = StackClient.GetSegmentGlobalRotationEulerXYZ(subject_name, segment_name);

                    std::cout << "Segment Name: " << segment_name << " -> Translation: (" << trans.Translation[0] << " ," << trans.Translation[1] << " ," << trans.Translation[2] << ")" << std::endl;
                    std::cout << "Segment Name: " << segment_name << " -> Rotation: (" << rot.Rotation[0] << " ," << rot.Rotation[1] << " ," << rot.Rotation[2] << ")"<< std::endl;
                    std::chrono::milliseconds timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
                    std::cout << "TimeStamp (ms): " << timestamp.count() << std::endl;
                    std::cout << "Subject name: " << subject_name << std::endl;
                    first_timestamp = first_timestamp == 0 ? timestamp.count() : first_timestamp;
                }
            }
        }
    }
    return 0;
}