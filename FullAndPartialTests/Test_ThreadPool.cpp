#include <iostream>
#include <string>
#include <thread>
#include <sstream>
#include <exception>

#include "../KozyLibrary_Complete.hpp"

using namespace std;

void foo(){
    stringstream str;
    str << this_thread::get_id();
    string out = "Hello from " + str.str() + '\n';
    cout << out;
    this_thread::sleep_for(chrono::microseconds(500));
}
void foo2(){
    stringstream str;
    str << this_thread::get_id();
    string out = "Goodbye from " + str.str() + '\n';
    cout << out;
    this_thread::sleep_for(chrono::microseconds(500));
}

int main(int argc, const char** args){
    KozyLibrary::ThreadPool pool;

    for (unsigned cnt = 100; cnt != 0; --cnt){
        pool.add_Workload(foo);
    }

    auto lam = []()->void{ cout << endl << "LAST WORK\n" << endl; };

    pool.add_Workload(lam);


    
    pool.start(20);
    for (unsigned cnt = 100; cnt != 0; --cnt){
        pool.add_Workload(foo2);
    }

    auto lam2 = []()->void{ cout << "\nIM SLEEPING\n" << endl; };
    pool.set_pausingWork(lam2);
    //pool.restart(20);
    this_thread::sleep_for(chrono::microseconds(500));

    constexpr unsigned sleepSeconds = 3;
    cout << (string("Terminating in ") + to_string(sleepSeconds) + " seconds!\n");
    this_thread::sleep_for(chrono::seconds(sleepSeconds));


    pool.stop();/**
    cout << boolalpha;
    for (string input; getline(cin,input) && input != "x"; getline(cin, input)){
        cout    << "\nIs Running: " << pool.is_running() << endl
                << "Workers: " << pool.get_workerCnt() << endl;
    }

*/
    cout << "\nTerminating!\n";

    cout << thread::hardware_concurrency() << endl;
    cout << pool.recommended_ThreadsCnt << endl;
    pool.wait_untilStopped();

    cout << "\nTerminated!\n";

    return 0;
}