/*
 /   nmifetch 
 /   Copyright (c) 2020 NotManyIdeas
 /   GPL-3.0 License
*/

#include <iostream>
#include <string>
#include <sys/sysinfo.h>
#include <vector>
#include <algorithm>
#include "logo.h"
#include <iomanip>
#include <sstream>

#define ll long long

std::string GetOutputFromCommand(std::string cmd) {
    std::string data;
    FILE *stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");
    stream = popen(cmd.c_str(), "r");

    if (stream) {
        while(!feof(stream))
            if (fgets(buffer, max_buffer, stream) != NULL)
                data.append(buffer);
        pclose(stream);
    }

    return data;
}

int ExtractIntFromString(std::string str) {
    int i = 0;
    for (char c: str) {
        if (c >= '0' && c <= '9') {
            i = i * 10 + (c - '0');
        }
    }

    return i;
}

int ExtractIntFromCommand(std::string str) {
    return ExtractIntFromString(GetOutputFromCommand(str));
}

std::string GetUsername()
{
    std::string m_username = GetOutputFromCommand("echo $USER");
    m_username.erase(std::remove(m_username.begin(), m_username.end(), '\n'), m_username.end());
    return m_username;
}

std::string GetHostname(){
    return GetOutputFromCommand("cat /proc/sys/kernel/hostname");
}

std::string GetOS(){
    return GetOutputFromCommand("hostnamectl | grep -oP '(?<=Operating System: ).*'");
}

std::string GetArchitecture() {
    return GetOutputFromCommand("hostnamectl | grep -oP '(?<=Architecture: ).*'");
}

std::string GetKernelVersion(){
    return GetOutputFromCommand("hostnamectl | grep -oP '(?<=Kernel: ).*'");
}

struct TimeC {
    ll seconds, minutes, hours;

    TimeC(ll s, ll m, ll h) 
        : seconds(s), minutes(m), hours(h)
    {

    }
};

TimeC ConvertSeconds(const ll& seconds) {
     return TimeC(seconds, seconds / 60, (seconds / 60) / 60);
}

std::string GetUptime()
{
    struct sysinfo s_info;
    sysinfo(&s_info);

    TimeC m_time = ConvertSeconds(s_info.uptime);
    m_time.seconds -= m_time.minutes * 60;
    m_time.minutes -= m_time.hours * 60;

    return std::to_string( m_time.hours) + "h, " 
    + std::to_string(m_time.minutes) + "m, " 
    + std::to_string(m_time.seconds) + "s ";
}

std::string GetShell() {
    return GetOutputFromCommand("echo $SHELL");
}

std::string GetMemoryUsage() {
    ll totalmem = ExtractIntFromCommand("cat /proc/meminfo | grep MemTotal");
    ll freemem = ExtractIntFromCommand("cat /proc/meminfo | grep MemFree");
    ll buffers = ExtractIntFromCommand("cat /proc/meminfo | grep Buffers");
    ll cached = ExtractIntFromCommand("cat /proc/meminfo | grep -w Cached");
    ll sreclaimable = ExtractIntFromCommand("cat /proc/meminfo | grep SReclaimable");

    ll memusage = totalmem - freemem - buffers - cached - sreclaimable;

    ll finalusage = memusage / 1024;
    ll finaltotal = totalmem / 1024;

    return std::to_string(finalusage) + "MiB / " + std::to_string(finaltotal) + "MiB";
}

template<typename T>
std::string DetectBase(std::vector<T>& archBased, std::vector<T>& debianBased) {
    for (int i = 0; i < archBased.size(); i++) {
        if (GetOS().find(archBased[i]) != std::string::npos) {
            return "arch";
        }
    }

    for (int j = 0; j < debianBased.size(); j++)  {
        if (GetOS().find(debianBased[j]) != std::string::npos) {
            return "debian";
        }
    }

    return "Distro not supported.";
}

std::string GetPacmanPackages() {
    std::string m_packages = GetOutputFromCommand("pacman -Q | wc -l") + "(pacman)";
    m_packages.erase(std::remove(m_packages.begin(), m_packages.end(), '\n'), m_packages.end());
    return m_packages;
}

std::string GetDpkgPackages() {
    std::string m_packages = GetOutputFromCommand("dpkg-query -f '${binary:Package}\n' -W | wc -l") + "(dpkg)";
    m_packages.erase(std::remove(m_packages.begin(), m_packages.end(), '\n'), m_packages.end());
    return m_packages;
}

std::string GetCPUInfo() {
    return GetOutputFromCommand(R"(lscpu | grep "Model name:" | sed -r 's/Model name:\s{1,}//g')");
}


static std::vector<std::string> archBased = 
{
    "Alpine",
    "Anarchy",
    "Arch",
    "Arco",
    "Artix",
    "Endeavour",
    "Hyperbola",
    "KaOS",
    "LinHES",
    "Manjaro",
    "Ninja",
    "RaspArch",
    "Snal",
    "Talking",
    "UBOS"
};

static std::vector<std::string> debianBased = 
{
    "MX",
    "Mint",
    "Pop",
    "Ubuntu",
    "elementary",
    "KDE",
    "Zorin",
    "deepin",
    "Kali",
    "Lite",
    "Peppermint",
    "Sparky",
    "Lubuntu",
    "Kubuntu",
    "Xubuntu",
    "Parrot",
    "Endless",
    "MATE",
    "Pure",
    "Raspbian",
    "Raspberry Pi"   
};

void PrintAll(std::string logo) 
{
    std::istringstream f(logo);
    std::string line;

    std::vector<std::string> v;

    v.push_back("\n                   \033[1;36m  " + GetUsername() + "\e[0m@\033[1;36m" + GetHostname() + "\e[0m");
    v.push_back("  \033[1;34mos\e[0m: " + GetOS());
    v.push_back("  \033[1;34march\e[0m: " + GetArchitecture());
    v.push_back("  \033[1;34mkrnl\e[0m: " + GetKernelVersion());
    v.push_back("  \033[1;34muptm\e[0m: " + GetUptime() + "\n");
    v.push_back("  \033[1;34mshll\e[0m: " + GetShell());
    v.push_back("  \033[1;34mmem\e[0m: " + GetMemoryUsage() + "\n");
    v.push_back("  \033[1;34mcpu\e[0m: " + GetCPUInfo());

    if (DetectBase(archBased, debianBased) == "arch")
        v.push_back("  \033[1;34mpkgs\e[0m: " + GetPacmanPackages() + "\n");
    else if (DetectBase(archBased, debianBased) == "debian")
        v.push_back("  \033[1;34mpkgs\e[0m: " + GetDpkgPackages() + "\n");
    else if (DetectBase(archBased, debianBased) == "Distro not supported.")
        v.push_back("  \033[1;34mpkgs\e[0m: something went wrong. \n");

    int i = 0;

    while(std::getline(f, line)) {
        if (i != 0) {
            if (i < v.size() + 1)
                std::cout << "\033[1;34m" << line << "\e[0m" << std::right << v[i - 1];
            else
                std::cout << "\033[1;34m" << line << "\e[0m" << std::endl;
        }
        i++;
    }
}

int main() 
{
    PrintAll(arch_logo);
    std::cout << "\n";
}
