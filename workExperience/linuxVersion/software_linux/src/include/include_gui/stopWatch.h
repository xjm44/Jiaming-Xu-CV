#pragma once
#include <chrono>

class stopWatch {
    using clock = std::chrono::steady_clock;
    // clock::time_point StartTime = {};
    // clock::duration ElapsedTime = {};
public:
//   bool IsRunning() const {
//      return StartTime != clock::time_point{};
//   }

//   void Start() {
//     if(!IsRunning()) {
//       StartTime = clock::now();
//     }
//   }

//   void Stop() {
//     if(IsRunning()) {
//       ElapsedTime += clock::now() - StartTime;
//       StartTime = {};
//     }
//   }

//   void Reset() {
//     StartTime = {};
//     ElapsedTime = {};
//   }

//   clock::duration GetElapsed() {
//     auto result = ElapsedTime;
//     if(IsRunning()) {
//       result += clock::now() - StartTime;
//     }
//     return result;
//   }
    stopWatch();
    ~stopWatch();
    bool IsRunning() const ;
    void Start();
    void Stop();
    void Reset() ;
    clock::duration GetElapsed();

private:
    clock::time_point StartTime;
    clock::duration ElapsedTime;
};