#pragma once
#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

#define DLOG_INIT                    \
	Dove::LogManager::GetInstance(); \
	if (Dove::OnInit != nullptr)     \
	Dove::OnInit()
#define DLOG_CLOSE                \
	if (Dove::OnClose != nullptr) \
		Dove::OnClose();          \
	delete Dove::LogManager::GetInstance()

#define DLOG_ERROR(...)                                                                                     \
	{                                                                                                       \
		char buff[256];                                                                                     \
		sprintf_s(buff, __VA_ARGS__);                                                                       \
		Dove::g_dlog_instance->PushMsg({Dove::LogType::DLT_ERROR, buff, __FILE__, __FUNCTION__, __LINE__}); \
	}
#define DLOG_WARN(...)                                                                                     \
	{                                                                                                      \
		char buff[256];                                                                                    \
		sprintf_s(buff, __VA_ARGS__);                                                                      \
		Dove::g_dlog_instance->PushMsg({Dove::LogType::DLT_WARN, buff, __FILE__, __FUNCTION__, __LINE__}); \
	}
#define DLOG_INFO(...)                                                                                     \
	{                                                                                                      \
		char buff[256];                                                                                    \
		sprintf_s(buff, __VA_ARGS__);                                                                      \
		Dove::g_dlog_instance->PushMsg({Dove::LogType::DLT_INFO, buff, __FILE__, __FUNCTION__, __LINE__}); \
	}
#define DLOG_DEBUG(...)                                                                                     \
	{                                                                                                       \
		char buff[256];                                                                                     \
		sprintf_s(buff, __VA_ARGS__);                                                                       \
		Dove::g_dlog_instance->PushMsg({Dove::LogType::DLT_DEBUG, buff, __FILE__, __FUNCTION__, __LINE__}); \
	}
#define DLOG_TRACE(...)                                                                                     \
	{                                                                                                       \
		char buff[256];                                                                                     \
		sprintf_s(buff, __VA_ARGS__);                                                                       \
		Dove::g_dlog_instance->PushMsg({Dove::LogType::DLT_TRACE, buff, __FILE__, __FUNCTION__, __LINE__}); \
	}

#define DLOG_ON_PUSH Dove::OnPushMsg
#define DLOG_ON_INIT Dove::OnInit
#define DLOG_ON_CLOSE Dove::OnClose

namespace Dove
{
	using DMSG_FLAG = unsigned char;
	inline DMSG_FLAG DMSG_FLAG_TYPE = 1 << 0;
	inline DMSG_FLAG DMSG_FLAG_CONTENT = 1 << 1;
	inline DMSG_FLAG DMSG_FLAG_FILE = 1 << 2;
	inline DMSG_FLAG DMSG_FLAG_FUNC = 1 << 3;
	inline DMSG_FLAG DMSG_FLAG_LINE = 1 << 4;

	inline DMSG_FLAG DMSG_FLAG_SIMPLE = 1 << 0 | 1 << 1;
	inline DMSG_FLAG DMSG_FLAG_ALL = 0xFF;

	inline std::string LogTypes[5] = {
		"ERROR",
		"WARN",
		"INFO",
		"DEBUG",
		"TRACE"
	};


	enum class LogType : unsigned char
	{
		DLT_ERROR = 0,
		DLT_WARN = 1,
		DLT_INFO = 2,
		DLT_DEBUG = 3,
		DLT_TRACE = 4
	};

	struct LogMsg
	{
		LogType Type;
		std::string Content;
		std::string File;
		std::string Function;
		long Line;
	};

	using CallBackOnPushMsg = void (*)(LogMsg);
	using CallBackOnInit = void (*)();
	using CallBackOnClose = void (*)();

	inline CallBackOnPushMsg OnPushMsg = nullptr;
	inline CallBackOnInit OnInit = nullptr;
	inline CallBackOnClose OnClose = nullptr;


	inline std::string msg_to_string(const LogMsg& msg, DMSG_FLAG flag = Dove::DMSG_FLAG_SIMPLE)
	{
		std::string str = "";
		if (flag & DMSG_FLAG_TYPE)
			str.append(LogTypes[(unsigned char)msg.Type] + ":");
		if (flag & DMSG_FLAG_CONTENT)
			str.append("\t" + msg.Content);
		if (flag & DMSG_FLAG_FILE)
			str.append(" -<" + msg.File + "> ");
		if (flag & DMSG_FLAG_FUNC)
			str.append(" -function: " + msg.Function);
		if (flag & DMSG_FLAG_LINE)
			str.append(" -line: " + std::to_string(msg.Line));
		return str;
	};

	class LogManager;
	inline LogManager* g_dlog_instance = nullptr;

	class LogManager
	{
	private:
		LogManager()
		{
			msgs.reserve(100);
			typedMsgs.reserve(100);
		}

	public:
		~LogManager()
		{
			g_dlog_instance = nullptr;
		}
		static LogManager *GetInstance()
		{
			if (g_dlog_instance == nullptr)
			{
				g_dlog_instance = new LogManager();
			}
			return g_dlog_instance;
		}

	public:
		std::vector<LogMsg> &GetLogMsgs() { return msgs; }
		std::vector<LogMsg *> &GetTypedMsgs(const LogType &type) { return typedMsgs[type]; };
		LogMsg *PushMsg(LogMsg msg)
		{
			static std::mutex mtx;
			mtx.lock();

			if (msgs.capacity() == (msgs.size() + 1))
				msgs.reserve(msgs.capacity() + 300);
			if (typedMsgs[msg.Type].capacity() == (typedMsgs.size() + 1))
				typedMsgs.reserve(typedMsgs[msg.Type].capacity() + 300);

			msgs.emplace_back(msg);
			typedMsgs[msg.Type].emplace_back(&msgs[msgs.size() - 1]);

			if (OnPushMsg != nullptr)
				OnPushMsg(msg);

			LogMsg *pushedMsg = &msgs[msgs.size() - 1];

			mtx.unlock();
			return pushedMsg;
		}

	private:
		//static LogManager *Instance;
		std::vector<LogMsg> msgs;
		std::unordered_map<LogType, std::vector<LogMsg *>> typedMsgs;
	};

	inline LogManager *DLOG_GET_MANAGER()
	{
		return Dove::LogManager::GetInstance();
	};
	inline std::vector<LogMsg> &DLOG_GET_MSGS()
	{
		return Dove::LogManager::GetInstance()->GetLogMsgs();
	};
	inline std::vector<LogMsg *> &DLOG_GET_MSGS_TYPED(const LogType &type)
	{
		return Dove::LogManager::GetInstance()->GetTypedMsgs(type);
	};

}