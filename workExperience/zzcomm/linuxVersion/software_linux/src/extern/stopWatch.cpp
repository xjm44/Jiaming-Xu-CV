#include "../include/include_gui/stopWatch.h"
#include <iostream>



stopWatch::stopWatch(){
    StartTime = {};
    ElapsedTime = {};
}
stopWatch::~stopWatch(){}

bool stopWatch::IsRunning() const {
    return StartTime != clock::time_point{};
}

void stopWatch::Start() {
    if(!IsRunning()) {
        StartTime = clock::now();
    }else{
        std::cout<<"unable to start timer"<<std::endl;
    }
}

void stopWatch::Stop() {
    if(IsRunning()) {
        ElapsedTime += clock::now() - StartTime;
        StartTime = {};
    }else{
        std::cout<<"unable to stop timer"<<std::endl;
    }
}

void stopWatch::Reset() {
    StartTime = {};
    ElapsedTime = {};
}

stopWatch::clock::duration stopWatch::GetElapsed() {
    auto result = ElapsedTime;
    if(IsRunning()) {
        result += clock::now() - StartTime;
    }

    return result;
}