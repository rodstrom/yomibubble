#ifndef _VN_SYSTEM_H_
#define _VN_SYSTEM_H_

class System
{
public:
	virtual ~System(void){}
	static void Notify(const std::string& p_message, const std::string& p_caption);
	static void Break();
	static void SetLogFile(const std::string& p_filename, const std::string& p_extension);
	static void Log(const std::string& p_message);
	static void Msg(const std::string& p_message);

protected:
	static std::ofstream m_logfile;
	static std::string m_filename;
};

#endif // _VN_SYSTEM_H_