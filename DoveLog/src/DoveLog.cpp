#include "DoveLog.h"
#include <mutex>

namespace Dove {
	DMSG_FLAG DMSG_FLAG_TYPE = 1 << 0;
	DMSG_FLAG DMSG_FLAG_CONTENT = 1 << 1;
	DMSG_FLAG DMSG_FLAG_FILE = 1 << 2;
	DMSG_FLAG DMSG_FLAG_FUNC = 1 << 3;
	DMSG_FLAG DMSG_FLAG_LINE = 1 << 4;

	DMSG_FLAG DMSG_FLAG_SIMPLE = 1 << 0 | 1 << 1;
	DMSG_FLAG DMSG_FLAG_ALL = 0xFF;

	LogManager* LogManager::Instance = nullptr;

	CallBackOnPushMsg OnPushMsg = nullptr;
	CallBackOnInit OnInit = nullptr;
	CallBackOnClose OnClose = nullptr;

	std::string LogTypes[5] = {
		"ERROR",
		"WARN",
		"INFO",
		"DEBUG",
		"TRACE"
	};

	std::string Dove::msg_to_string(const LogMsg& msg, DMSG_FLAG flag) {
		std::string str = "";
		if (flag & DMSG_FLAG_TYPE)
			str.append(LogTypes[(unsigned char)msg.Type]+":");
		if (flag & DMSG_FLAG_CONTENT)
			str.append("\t" + msg.Content);
		if (flag & DMSG_FLAG_FILE)
			str.append(" -<" + msg.File + "> ");
		if (flag & DMSG_FLAG_FUNC)
			str.append(" -function: " + msg.Function);
		if (flag & DMSG_FLAG_LINE)
			str.append(" -line: " + std::to_string(msg.Line));
		return str;
	}
	LogManager* LogManager::GetInstance() {
		if (Instance == nullptr)
		{
			Instance = new LogManager();
		}
		return Instance;
	}
	LogMsg* LogManager::PushMsg(LogMsg msg) {
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

		LogMsg* pushedMsg = &msgs[msgs.size() - 1];

		mtx.unlock();
		return pushedMsg;
	}
}