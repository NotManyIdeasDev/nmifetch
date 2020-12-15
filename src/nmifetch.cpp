/*
 /   nmifetch 
 /   Copyright (c) 2020 NotManyIdeas
 /   GPL-3.0 License
*/

#include <iostream>
#include <string>
#include <sys/sysinfo.h>

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
    return GetOutputFromCommand("echo $USER");
}

std::string GetHostname(){
    return GetOutputFromCommand("cat /proc/sys/kernel/hostname");
}

std::string GetOS(){
    return GetOutputFromCommand("hostnamectl | grep -oP '(?<=Operating System: ).*");
}

std::string GetKernelVersion(){
    return GetOutputFromCommand("hostnamectl | grep -oP '(?<=Kernel: ).*");
}

struct TimeC {
    long long seconds, minutes, hours;

    TimeC(long long s, long long m, long long h) 
        : seconds(s), minutes(m), hours(h)
    {

    }
};

TimeC ConvertSeconds(const long long& seconds) {
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

int main() {
    std::cout << GetUptime() << std::endl;
}
