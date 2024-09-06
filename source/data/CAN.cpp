#include <stdio.h>
#include <string.h>
#include <unistd.h>


#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include <fcntl.h>

#include "data/CAN.hpp"
#include "Timer.hpp"


#define RETRANSMISSION_TIME 10
#define MAX_RETRANSMISSION 4


// CONSTRUCTOR #################################################################


CAN::CAN(const char *port, std::shared_ptr<CanStructMapper> canStructMapper){ PROFILING_FUNCTION();
	this->canStructMapper = canStructMapper;
	// get port number
	char lastChar = port[strlen(port) - 1];
    this->canPortNr = atoi(&lastChar);
	if (canPortNr < 0 || canPortNr > 3){
		ECU_ERROR(CAN_IS_NOT_EXISTING, "CAN port '{}' is not existing.",port);
	}
	


	canFrameBuilder = std::make_unique<CanFrameBuilder>(canPortNr);
	init(port);
	pthread_create(&canThread, NULL, &CAN::run, this);
	ECU_INFO("CAN{}: started",canPortNr);

	//send init frames
	vector<can_frame>& initFrames = canFrameBuilder->buildInitFrames();
	for (auto frame : initFrames){
		if (write(canSocket, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
			ECU_WARN("{}: Init frame could not be transmitted.",port);
		}
	}

}

CAN::~CAN(){ PROFILING_FUNCTION();
	
	flagRun = false;

	if (close(canSocket) < 0) {
		ECU_ERROR(CAN0_CLOSE_SOCKET_ERROR+ canPortNr ,"Socket could not be closed.");
	}

	pthread_join(canThread, NULL);
	ECU_INFO("CAN: stopped");
}

// METHODS: PUBLIC #############################################################

void CAN::transmit(std::string frameName){ PROFILING_FUNCTION();
	std::size_t hashedFrameName = std::hash<std::string>{}(frameName);
	transmit(hashedFrameName);
}

void CAN::transmit(std::size_t frameName){ PROFILING_FUNCTION();
	can_frame_t frame;
	can_frame rawFrame;
	try{
		frame = canFrameBuilder->buildTransmitCanFrame(frameName);
		canStructMapper->mapOutputCanFrame(frame);
		rawFrame = canFrameBuilder->convertToRawFrame(frame);
		
	}catch(std::runtime_error &e){
		ECU_ERROR(CAN0_TRANSMIT_FRAME_BUILDER_ERROR + canPortNr,"Can{} Message could not be transmitted. {}", canPortNr ,e.what())
		return;
	}
	
	if(rawFrame.can_dlc){
		uint8_t retransmissionCounter = 0;
		while(write(canSocket, &rawFrame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
			if(retransmissionCounter < 20){
				retransmissionCounter++;
			}else{
				auto hexID = ecu::decToHexStr(rawFrame.can_id);
				ECU_ERROR((CAN0_TRANSMIT_ERROR + canPortNr),"Can{} Message with ID {} could not be transmitted.", canPortNr,hexID);
				break;
			}
		}
	}
}


int CAN::getBufferSize(void) const{ PROFILING_FUNCTION();

	return frameBuffer.size();
}

std::optional<can_frame_t> CAN::getFrame(void){ PROFILING_FUNCTION();

	std::optional<can_frame_t> frame;

	mtx.lock();

	if (!frameBuffer.empty()){		// when buffer is not empty
		frame = frameBuffer.front();	// read oldest element
		frameBuffer.pop_front();		// delete oldest element
	}else{
		frame = std::nullopt;
	}

	mtx.unlock();

	return frame;
}

// METHODS: PRIVATE ############################################################

void CAN::init(const char* port){ PROFILING_FUNCTION();
    
	struct sockaddr_can addr;
	struct ifreq ifr;

	if ((canSocket = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		ECU_ERROR(CAN0_INIT_SOCKET_ERROR + canPortNr, "Socket could not be initialised.");
	}

	strcpy(ifr.ifr_name, port);
	ioctl(canSocket, SIOCGIFINDEX, &ifr);

	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(canSocket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		ECU_ERROR(CAN0_SOCKET_BIND_ERROR + canPortNr, "Socket could not be bind.");
	}

	fcntl(canSocket, F_SETFL, fcntl(canSocket, F_GETFL, 0) | O_NONBLOCK);
}

void CAN::receive(void){ PROFILING_FUNCTION();

	int nBytes;
	int bufferIndex;
	struct can_frame readFrame;
	
	int i;
	
	try{
		// read data from CAN Bus
		nBytes = read(canSocket, &readFrame, sizeof(struct can_frame));

		if (nBytes > 0) {
			readFrame.can_id &= ~CAN_EFF_FLAG;
			can_frame_t builtFrame;
			builtFrame = canFrameBuilder->buildReceiveCanFrame(readFrame);
			frameToBuffer(builtFrame);
			#ifdef DEBUG
				builtFrame.print();		//? print the can frame for debugging
			#endif
		}
		else{
			std::this_thread::sleep_for(std::chrono::microseconds(3));  
		}
	}catch(std::runtime_error &e){
		//std::string canIdHex = ecu::decToHexStr(readFrame.can_id);
		//ECU_WARN("Warning in CAN{} receive: {}. The received Frame ID = {}",canPortNr, e.what(), canIdHex);
	}

	
}

//? pthread fix for slow SPI to CAN chips
void* CAN::run(void* arg) {
		CAN* can = static_cast<CAN*>(arg);
		can->flagRun = true;

		// Set thread as real-time thread
		struct sched_param params;
		params.sched_priority = 99;

		if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &params) != 0) {
			ECU_WARN("Failed to set thread as real-time thread.");
		}

		while (can->flagRun){
			can->receive();
		}
	return nullptr;
}
//? original run function for perfect working CAN
/*void CAN::run(){ PROFILING_FUNCTION();
	flagRun = true;
	while (flagRun){
		receive();
	}
}*/

void CAN::frameToBuffer(can_frame_t frame){ PROFILING_FUNCTION();
	mtx.lock();
	frameBuffer.push_back(frame);
	mtx.unlock();

}

