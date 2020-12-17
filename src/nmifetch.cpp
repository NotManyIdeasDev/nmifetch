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

void PrintAll(std::string logo) {
    std::istringstream f(logo);
    std::string line;

    std::vector<std::string> v;
}



int main() 
{
    std::cout << GetUsername() << "@" << GetHostname() << std::endl;
    std::cout << "os  : " << GetOS();
    std::cout << "arch: " << GetArchitecture();
    std::cout << "krnl: " << GetKernelVersion();
    std::cout << "uptm: " << GetUptime() << std::endl;
    std::cout << "shll: " << GetShell();
    std::cout << "mem : " << GetMemoryUsage() << std::endl;

    if (DetectBase(archBased, debianBased) == "arch")
        std::cout << "pkgs: " << GetPacmanPackages() << std::endl;
    else if (DetectBase(archBased, debianBased) == "debian")
        std::cout << "pkgs: " << GetDpkgPackages() << std::endl;
    else if (DetectBase(archBased, debianBased) == "Distro not supported.")
        std::cout << "pkgs: something went wrong." << std::endl;

    std::cout << "cpu : " <<  GetCPUInfo() << std::endl;
}
