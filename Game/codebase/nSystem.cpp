#include "stdafx.h"
#include "nSystem.h"

std::ofstream System::m_logfile;
std::string System::m_filename;

void System::Notify(const std::string& p_message, const std::string& p_caption) {
#ifdef _WIN32
	MessageBoxA(NULL, p_message.c_str(), p_caption.c_str(), MB_OK | MB_ICONEXCLAMATION);
#endif
}

void System::Break() {
	__debugbreak();
}

void System::SetLogFile(const std::string& p_filename, const std::string& p_extension) {
	m_filename = p_filename + "." + p_extension;
}

void System::Msg(const std::string& p_message) {
	std::cout << p_message << std::endl;
}

void System::Log(const std::string& p_message) {
	m_logfile.open(m_filename, std::ios::app);
	struct tm time;
	__time32_t long_time;
	_time32(&long_time);
	_localtime32_s(&time, &long_time);

	std::string line;
	line.append("[" + NumberToString(time.tm_year - 100));
	line.append("-" + NumberToString(time.tm_mon + 1));
	line.append("-" + NumberToString(time.tm_mday));
	line.append("_" + NumberToString(time.tm_hour));
	line.append(":" + NumberToString(time.tm_min));
	line.append(":" + NumberToString(time.tm_sec));
	m_logfile << line << "] " << p_message << std::endl;
#ifdef _DEBUG
	std::cout << p_message << std::endl;
#endif
	m_logfile.close();
}