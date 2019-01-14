// mavlinkPrase.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "MAVLINK/mavlink_avoid_errors.h"
#include "MAVLINK/common/mavlink.h"
#include "MAVLINK/mavlink_helpers.h"
using namespace std;

std::vector<std::string> split(std::string str, std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str += pattern;//扩展字符串以方便操作
	int size = str.size();

	for (int i = 0; i<size; i++)
	{
		pos = str.find(pattern, i);
		if (pos<size)
		{
			std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}

uint8_t ChangeNum(char* str, int length = 2)
{
	char  revstr[3] = { 0 };  //根据十六进制字符串的长度，这里注意数组不要越界  
	int   num[3] = { 0 };
	int   count = 1;
	uint8_t   result = 0;
	strcpy_s(revstr, str);
	for (int i = length - 1; i >= 0; i--)
	{
		if ((revstr[i] >= '0') && (revstr[i] <= '9'))
			num[i] = revstr[i] - 48;//字符0的ASCII值为48  
		else if ((revstr[i] >= 'a') && (revstr[i] <= 'f'))
			num[i] = revstr[i] - 'a' + 10;
		else if ((revstr[i] >= 'A') && (revstr[i] <= 'F'))
			num[i] = revstr[i] - 'A' + 10;
		else
			num[i] = 0;
		result = result + num[i] * count;
		count = count * 16;//十六进制(如果是八进制就在这里乘以8)      
	}
	return result;
}

int  ChangeNum2int(char* str)
{
	int length = 2;
	char  revstr[3] = { 0 };  //根据十六进制字符串的长度，这里注意数组不要越界  
	int   num[3] = { 0 };
	int   count = 1;
	int    result = 0;
	strcpy_s(revstr, str);
	for (int i = length - 1; i >= 0; i--)
	{
		if ((revstr[i] >= '0') && (revstr[i] <= '9'))
			num[i] = revstr[i] - 48;//字符0的ASCII值为48  
		else if ((revstr[i] >= 'a') && (revstr[i] <= 'f'))
			num[i] = revstr[i] - 'a' + 10;
		else if ((revstr[i] >= 'A') && (revstr[i] <= 'F'))
			num[i] = revstr[i] - 'A' + 10;
		else
			num[i] = 0;
		result = result + num[i] * count;
		count = count * 16;//十六进制(如果是八进制就在这里乘以8)      
	}
	return result;
}


vector<uint8_t> string2uint8(vector<string> &Snum)
{
	vector<uint8_t> ResNum;
	for (int i = 0; i<Snum.size(); i++)
	{
		if (Snum[i].length() < 2)continue;
		char stt[3];
		stt[0] = Snum[i][0];
		stt[1] = Snum[i][1];
		stt[2] = '\0';
		uint8_t tmpp = ChangeNum(stt,2);
		ResNum.push_back(tmpp);
	}
	return ResNum;
}

vector<int> string2int(vector<string> &Snum)
{
	vector<int> ResNum;
	for (int i = 0; i<Snum.size(); i++)
	{
		if (Snum[i].length() < 2)continue;
		char stt[3];
		stt[0] = Snum[i][0];
		stt[1] = Snum[i][1];
		stt[2] = '\0';
		int tmpp = ChangeNum2int(stt);
		ResNum.push_back(tmpp);
	}
	return ResNum;
}





int main()
{
	string filebase = "att";
	string att_num = "30";
	string file_name = filebase + att_num + ".txt";
	fstream f(file_name,ios::in);
	stringstream ss;
	ss << f.rdbuf();
	std::string contents(ss.str());
	vector<std::string> Snum = split(contents, " ");
	f.close();
	char tmp[3] = "FE";
	cout << ChangeNum(tmp) << endl;
	vector<uint8_t> ResNum = string2uint8(Snum);
	vector<int> IntNum = string2int(Snum);
	vector<mavlink_attitude_t> mavatt;
	
	mavlink_message_t msg;

	mavlink_status_t status;

	mavlink_channel_t chan;

	string out_file = "result" + att_num + ".txt";
	fstream of(out_file, ios::out);

	for (int i = 0; i < ResNum.size(); i++)
	{
		if (mavlink_parse_char(MAVLINK_COMM_0, ResNum[i], &msg, &status))

		{

			mavlink_attitude_t   attitude;

			if (msg.msgid == MAVLINK_MSG_ID_ATTITUDE)
			{
				mavlink_msg_attitude_decode(&msg, &attitude);
				mavatt.push_back(attitude);

				of << attitude.time_boot_ms << " " << attitude.roll << " "
					<< attitude.pitch << " " << attitude.yaw << " "
					<< attitude.rollspeed << " " << attitude.pitchspeed << " "
					<< attitude.yawspeed << endl;

				cout << "666" << endl;
			}
		}
	}
	of.close();

    return 0;
}

