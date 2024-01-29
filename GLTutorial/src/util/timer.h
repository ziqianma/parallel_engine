#pragma once
#include <iostream>
#include <string>
#include <chrono>

class AutoProfiler {
public:
	AutoProfiler(std::string name)
		: m_name(std::move(name)),
		m_beg(std::chrono::high_resolution_clock::now()) { }
	~AutoProfiler() {
		auto end = std::chrono::high_resolution_clock::now();
		auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - m_beg);
		std::cout << m_name << " : " << (dur.count() / 1000000.0f) << " sec\n";
	}
private:
	std::string m_name;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_beg;
};