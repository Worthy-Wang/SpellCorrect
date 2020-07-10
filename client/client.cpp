#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <json/json.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
using namespace std;

int main()
{
    int ret;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        exit(-1);
    }
    struct sockaddr_in sockinfo;
    bzero(&sockinfo, sizeof(sockinfo));
    sockinfo.sin_addr.s_addr = inet_addr("172.21.0.7");
    sockinfo.sin_port = htons(8888);
    sockinfo.sin_family = AF_INET;
    ret = connect(sockfd, (sockaddr *)&sockinfo, sizeof(sockinfo));
    if (-1 == ret)
    {
        perror("connect");
        exit(-1);
    }

    char buf[1000];
    recv(sockfd, buf, sizeof(buf), 0);
    cout << buf << endl;

    while (1)
    {
        bzero(buf, sizeof(buf));
        read(0, buf, sizeof(buf));
        send(sockfd, buf, strlen(buf)-1, 0);

        bzero(buf, sizeof(buf));
        recv(sockfd, buf, sizeof(buf), 0);

        //接收到json格式的数据,解析
        Json::Reader reader;
        Json::Value value;
        if (reader.parse(buf, value)) // json字符串转为json对象
        {
            for (unsigned int i = 0; i < value["array"].size(); i++)
            {
                string word = value["array"][i]["word"].asString();
                int frequency = value["array"][i]["frequency"].asInt();
                int min_distance = value["array"][i]["min_distance"].asInt();
                
                cout << "单词\t词频\t最小编辑距离" << endl;
                cout << word << "\t" << frequency << "\t" << min_distance << endl; 
            }
        }
        cout << "-------------------------------------------------" << endl;
    }

    close(sockfd);
    return 0;
}
