#ifndef company_H
#define company_H
#include "pch.h"
#include "framework.h"
#include <iostream>
#include <vector>
#include <bitset>
#include <array>
#include <string>
#include <intrin.h>
#include <algorithm>
#include <sstream>
#include <cstdint>
#include <string>
#include <iomanip>
#include <time.h>
#include "md5.hpp"

class company // code in .lib file
{
public:
	std::string cpuInfoMd5;
	std::string companyMd5;
	std::string zzzzMd5;
	std::string serialHex;
	std::string serialNum;
private:
	Md5Encode encoder;
public:
	company();
	~company();
private:
	void getCpuInfoMd5();
	void getcompanyMd5();
	void getSerialNum();
public:
	std::string getRegistrationCode(const std::string& _serial, const time_t& _startTime, const time_t& _endTime);
	bool tryRegister(const std::string& _regCode, time_t& _startTime, time_t& _endTime);
};

std::string string_to_hex(const std::string& _in);
std::string hex_to_string(const std::string& _in);
std::string time_t_to_string(const time_t& _time, bool isBegin);
time_t string_to_time_t(const std::string& _in);
bool string_to_time_t(const std::string& _in, time_t& _startTime, time_t& _endTime);

#endif // company_H