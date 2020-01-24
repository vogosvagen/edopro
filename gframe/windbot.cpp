#include "windbot.h"
#include "utils.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#ifdef __ANDROID__
#include "porting_android.h"
#endif
#include <fmt/format.h>
#include "bufferio.h"

namespace ygo {

bool WindBot::Launch(int port, bool chat, int hand) const {
#ifdef _WIN32
	auto args = fmt::format(
		L"./WindBot/WindBot.exe Deck=\"{}\" Port={} Version={} name=\"[AI] {}\" Chat={} {}",
		deck.c_str(),
		port,
		version,
		name.c_str(),
		chat,
		hand ? fmt::format(L"Hand={}", hand) : L"");
	STARTUPINFO si = {};
	PROCESS_INFORMATION pi = {};
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	if (CreateProcess(NULL, (TCHAR*)Utils::ParseFilename(args).c_str(), NULL, NULL, FALSE, 0, NULL, executablePath.c_str(), &si, &pi)) {
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return true;
	}
	return false;
#elif defined(__ANDROID__)
	std::string param = fmt::format("Deck={} Port={} Version={} Name='[AI] {}' Chat={} Hand={}", BufferIO::EncodeUTF8s(deck).c_str(), port, version, BufferIO::EncodeUTF8s(name).c_str(), chat ? 1 : 0, hand);
	porting::launchWindbot(param);
	return true;
#else
	int pid = fork();
	if (pid == 0) {
		std::string argDeck = fmt::format("Deck={}", BufferIO::EncodeUTF8s(deck).c_str());
		std::string argPort = fmt::format("Port={}", port);
		std::string argVersion = fmt::format("Version={}", version);
		std::string argName = fmt::format("name=[AI] {}", BufferIO::EncodeUTF8s(name).c_str());
		std::string argChat = fmt::format("Chat={}", chat);
		std::string argHand = fmt::format("Hand={}", hand);
		chdir("WindBot");
		execlp("mono", "WindBot.exe", "WindBot.exe", 
			argDeck.c_str(), argPort.c_str(), argVersion.c_str(), argName.c_str(), argChat.c_str(),
			hand ? argHand.c_str() : nullptr, nullptr);
		perror("Failed to start WindBot");
		exit(EXIT_FAILURE);
	}
	return pid > 0;
#endif
}

}