#pragma once
#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>

#define DLOG_INIT Dove::LogManager::GetInstance()
#define DLOG_CLOSE delete Dove::LogManager::GetInstance()

#define DLOG_ERROR(...) {char buff[256];\
sprintf_s(buff, __VA_ARGS__);\
Dove::LogManager::GetInstance()->PushMsg({Dove::LogType::DLT_ERROR,buff,__FILE__,__FUNCTION__,__LINE__});}
#define DLOG_WARN(...) {char buff[256];\
sprintf_s(buff, __VA_ARGS__);\
Dove::LogManager::GetInstance()->PushMsg({Dove::LogType::DLT_WARN,buff,__FILE__,__FUNCTION__,__LINE__});}
#define DLOG_INFO(...) {char buff[256];\
sprintf_s(buff, __VA_ARGS__);\
Dove::LogManager::GetInstance()->PushMsg({Dove::LogType::DLT_INFO,buff,__FILE__,__FUNCTION__,__LINE__});}
#define DLOG_DEBUG(...) {char buff[256];\
sprintf_s(buff, __VA_ARGS__);\
Dove::LogManager::GetInstance()->PushMsg({Dove::LogType::DLT_DEBUG,buff,__FILE__,__FUNCTION__,__LINE__});}
#define DLOG_TRACE(...) {char buff[256];\
sprintf_s(buff, __VA_ARGS__);\
Dove::LogManager::GetInstance()->PushMsg({Dove::LogType::DLT_TRACE,buff,__FILE__,__FUNCTION__,__LINE__});}

#define DLOG_ON_PUSH Dove::OnPushMsg
#define DLOG_ON_INIT Dove::OnInit
#define DLOG_ON_CLOSE Dove::OnClose

namespace Dove {

	using DMSG_FLAG = unsigned char;
	extern DMSG_FLAG DMSG_FLAG_TYPE;
	extern DMSG_FLAG DMSG_FLAG_CONTENT;
	extern DMSG_FLAG DMSG_FLAG_FILE;
	extern DMSG_FLAG DMSG_FLAG_FUNC;
	extern DMSG_FLAG DMSG_FLAG_LINE;
	extern DMSG_FLAG DMSG_FLAG_SIMPLE;
	extern DMSG_FLAG DMSG_FLAG_ALL;

	enum class LogType : unsigned char{
		DLT_ERROR = 0,
		DLT_WARN = 1,
		DLT_INFO = 2,
		DLT_DEBUG = 3,
		DLT_TRACE = 4
	};

	extern std::string LogTypes[5];

	struct LogMsg {
		LogType Type;
		std::string Content;
		std::string File;
		std::string Function;
		long Line;
	};
	
	std::string msg_to_string(const LogMsg& msg, DMSG_FLAG flag = Dove::DMSG_FLAG_SIMPLE);

	class LogManager {
	private:
		LogManager() {
			msgs.reserve(100);
			typedMsgs.reserve(100);
		}
	public:
		~LogManager() {
			Instance = nullptr;
		}
		static LogManager* GetInstance();
	public:
		inline std::vector<LogMsg>& GetLogMsgs() { return msgs; }
		inline std::vector<LogMsg*>& GetTypedMsgs(const LogType& type) { return typedMsgs[type]; };
		LogMsg* PushMsg(LogMsg msg);
	private:
		static LogManager* Instance;
		std::vector<LogMsg> msgs;
		std::unordered_map<LogType, std::vector<LogMsg*>> typedMsgs;
	};
	
	inline LogManager* DLOG_GET_MANAGER() {
		return Dove::LogManager::GetInstance();
	};
	inline std::vector<LogMsg>& DLOG_GET_MSGS() {
		return Dove::LogManager::GetInstance()->GetLogMsgs(); 
	};
	inline std::vector<LogMsg*>& DLOG_GET_MSGS_TYPED(const LogType& type) {
		return Dove::LogManager::GetInstance()->GetTypedMsgs(type);
	};

	using CallBackOnPushMsg =  void (*)(LogMsg);
	using CallBackOnInit = void (*)();
	using CallBackOnClose = void (*)();

	extern CallBackOnPushMsg OnPushMsg;
	extern CallBackOnInit OnInit;
	extern CallBackOnClose OnClose;
}