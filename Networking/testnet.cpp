#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include "../CursesWrapper/wrapper.hpp"

#include "../utility.h"
#include "net_hdr.h"

// Only for testing purposes
// Serves to test the running ability of the networking library
int main() {
	unsigned char* buffer1 = new unsigned char[256];
	running_hdr* rhdr1 = new running_hdr(&buffer1, 256);

	unsigned char* buffer2 = new unsigned char[256];
	running_hdr* rhdr2 = new running_hdr(&buffer2, 256);

	startup_hdr* shdr1 = new startup_hdr("127.0.0.1", 5012, 5014);
	startup_hdr* shdr2 = new startup_hdr("127.0.0.1", 5014, 5012);

	std::thread submain1(netint_submain, std::ref(shdr1), std::ref(rhdr1));
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::thread submain2(netint_submain, std::ref(shdr2), std::ref(rhdr2));

	std::this_thread::sleep_for(std::chrono::milliseconds(1500));

	rhdr1->recv_mtx.lock();

#if defined(ISWIN)
	sprintf_s((char*)buffer1, 256, "network test");
#else
	sprintf((char*)buffer1, "network test");
#endif

	rhdr1->msg_code = msg_codes::sendBuffer;

	rhdr1->recv_mtx.unlock();

	while (true) {
		rhdr1->sent_mtx.lock();
		if (rhdr1->bufferSent)
			break;
		rhdr1->sent_mtx.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	rhdr1->bufferSent = false;
	rhdr1->sent_mtx.unlock();

	rhdr1->recv_mtx.lock();

	rhdr1->stop_mtx.lock();
	rhdr1->stopRequested = true;
	rhdr1->stop_mtx.unlock();

	rhdr1->recv_mtx.unlock();

	while (true) {
		rhdr2->hrecv_mtx.lock();

		if (rhdr2->hasReceived) {
			rhdr2->hrecv_mtx.unlock();
			break;
		}
		rhdr2->hrecv_mtx.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}

	rhdr2->recv_mtx.lock();

	rhdr2->msg_code = msg_codes::treatReceivedBuffer;
	rhdr2->stop_mtx.lock();
	rhdr2->stopRequested = true;
	rhdr2->stop_mtx.unlock();

	rhdr2->recv_mtx.unlock();

	submain1.join();
	submain2.join();

	nstd::ncout << "RECEIVED: " << std::string((char*)buffer2) << nstd::nendl;

	delete[] buffer1, buffer2;
	delete rhdr1, rhdr2, shdr1, shdr2;

	return 0;
}