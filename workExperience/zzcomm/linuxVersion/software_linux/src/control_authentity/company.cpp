// companyLib.cpp : Defines the functions for the static library.
//
#include "company.hpp"
// TODO: This is an example of a library function

void company::getCpuInfoMd5()
{
    char buff[17]{};
    unsigned level{1}, eax{0}, ebx{0}, ecx{0}, edx{0};
    /*
     * Return cpuid data for requested cpuid leaf, as found in returned
     * eax, ebx, ecx and edx registers.  The function checks if cpuid is
     * supported and returns 1 for valid cpuid information or 0 for
     * unsupported cpuid leaf.  All pointers are required to be non-null.
     */
    __get_cpuid(level, &eax, &ebx, &ecx, &edx);
    // CPU ID is the same as "sudo dmidecode -t 4 | grep ID"
    sprintf(buff, "%08X%08X",  edx, eax);
	cpuInfoMd5 = encoder.Encode(buff);
	std::transform(cpuInfoMd5.begin(), cpuInfoMd5.end(), cpuInfoMd5.begin(), ::toupper);
}

void company::getcompanyMd5()
{
	companyMd5 = encoder.Encode(R"(company)");
	std::transform(companyMd5.begin(), companyMd5.end(), companyMd5.begin(), ::toupper);

    zzkjMd5 = encoder.Encode(R"(cccc)");
	std::transform(zzkjMd5.begin(), zzkjMd5.end(), zzkjMd5.begin(), ::toupper);
}

void company::getSerialNum()
{
	for (std::size_t i = 0; i < cpuInfoMd5.size(); i++)
	{
		serialNum.push_back(static_cast<char>(cpuInfoMd5[i] ^ companyMd5[i]));
	}
	serialHex = string_to_hex(serialNum);
	std::transform(serialHex.begin(), serialHex.end(), serialHex.begin(), ::toupper);
}


std::string company::getRegistrationCode(const std::string& _serial, const time_t& _startTime, const time_t& _endTime)
{
	if (_serial.size() != 64)
	{
		throw std::runtime_error("Invalid serial length ...");
	}
	auto serial = hex_to_string(_serial);
	auto startTime = time_t_to_string(_startTime);
	auto endTime = time_t_to_string(_endTime);
	std::string code;
	for (int i = 0; i < 8; i++)
	{
		code.push_back(static_cast<char>(serial[i] ^ companyMd5[i] ^ zzkjMd5[i] ^ endTime[i]));
	}
	for (int i = 8; i < 16; i++)
	{
		code.push_back(static_cast<char>(serial[i] ^ companyMd5[i] ^ zzkjMd5[i] ^ startTime[i]));
	}
	for (int i = 16; i < 24; i++)
	{
		code.push_back(static_cast<char>(serial[i] ^ companyMd5[i] ^ zzkjMd5[i] ^ endTime[i]));
	}

	for (int i = 24; i < 32; i++)
	{
		code.push_back(static_cast<char>(serial[i] ^ companyMd5[i] ^ zzkjMd5[i] ^ startTime[i]));
	}

	
	auto ret = string_to_hex(code);
	std::transform(ret.begin(), ret.end(), ret.begin(), ::toupper);
	return ret;
}

bool company::tryRegister(const std::string& _regCode, time_t& _startTime, time_t& _endTime)
{
	if (_regCode.size() != 64)
	{
		throw std::runtime_error("Invalid serial lenth ...");
	}

	auto code = hex_to_string(_regCode);
	std::string time;
	for (std::size_t i = 0; i < code.size(); i++)
	{
		time.push_back(static_cast<char>(code[i] ^ cpuInfoMd5[i] ^ zzkjMd5[i]));
	}

	return string_to_time_t(time, _startTime, _endTime);
}


company::company()
{
	getCpuInfoMd5();
	getcompanyMd5();
	getSerialNum();
}

company::~company()
{

}

std::string company::querySerialNum() const {
    // return serialNum in hex format
    return serialHex;
}


std::string string_to_hex(const std::string& _in)
{
	std::stringstream ss;
	ss << std::hex << std::setfill('0');
	for (char i : _in)
	{
		ss << std::setw(2) << static_cast<unsigned int>(static_cast<unsigned char>(i));
	}
	return ss.str();
}

std::string hex_to_string(const std::string& _in)
{
	std::string ret;
	if ((_in.size() % 2) != 0) {
		throw std::runtime_error("Invalid string length ...");
	}

	auto cnt = _in.size() / 2;
	for (size_t i = 0; i < cnt; i++)
	{
		uint32_t s{};
		std::stringstream ss;
		ss << std::hex << _in.substr(i * 2, 2);
		ss >> s;
		ret.push_back(static_cast<char>(s));
	}
	return ret;
}

static int Time2Map[32]
{ 7, 5, 3, 2, 1, 6, 0, 4,
//0, 1, 2, 3, 4, 5, 6, 7
  1, 3, 7, 4, 0, 6, 5, 2,
//8, 9, 10,11,12,13,14,15
  6, 2, 0, 5, 7, 1, 4, 3,
//16,17,18,19,20,21,22,23
  4, 5, 1, 6, 2, 7, 3, 0
//24,25,26,27,28,29,30,31
};

static int Map2Time[32]
{
	6, 4, 3, 2, 7, 1, 5, 0,
	12, 8, 15, 9, 11, 14, 13, 10,
	18, 21, 17, 23, 22, 19, 16, 20,
	31, 26, 28, 30, 24, 25, 27, 29
};

std::string time_t_to_string(const time_t& _time)
{
	const char* read = (const char*)&_time;
	std::string ret;
	for (int i : Time2Map)
	{
		ret.push_back(read[i]);
	}
	return ret;
}

time_t string_to_time_t(const std::string& _in)
{
	if (_in.size() != 32)
	{
		throw std::runtime_error("Invalid string length ...");
	}

	time_t t[4]{};
	int pos{ 0 };
	for (long & i : t)
	{
		char* read = (char*)&i;
		for (int j = 0; j < 8; j++)
		{
			read[j] = _in[Map2Time[pos++]];
		}
	}

	for (int i = 1; i < 4; i++)
	{
		if (t[i] != t[i - 1]) return 0;
	}

	return t[0];
}

bool string_to_time_t(const std::string& _in, time_t& _startTime, time_t& _endTime)
{
	if (_in.size() != 32)
	{
		throw std::runtime_error("Invalid string length ...");
	}

	time_t t[4]{};
	int pos{ 0 };
	for (long & i : t)
	{
		char* read = (char*)&i;
		for (int j = 0; j < 8; j++)
		{
			read[j] = _in[Map2Time[pos++]];
		}
	}

	if (t[0] != t[2]) return false;
	if (t[1] != t[3]) return false;
	_endTime = t[0];
	_startTime = t[1];

	return true;
}