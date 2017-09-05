/*************************************************************************
> File Name: main.cpp
> Author: 
> Mail: 
> Created Time: 2017年08月25日 星期五 11时34分08秒
************************************************************************/


#include<iostream>
#include"XmlParser.h"

using namespace std;

enum ServerType
{
        Client_Type = 0,
        Conn_Type,
        Login_Type,
        Scene_Type,
        World_Type,
        Data_Type,
        Cross_Type,
        Log_Type,
        Server_Max,

};

template<class T>
string tostring(const T& data)
{
        stringstream ss;
        ss << data;
        return ss.str();                
}

int main()
{

    XmlParser g_objSrvConfig;
    g_objSrvConfig.LoadFromFile("srvconf.xml");
    const string s_strSrvcfg[] = {
                "/srvconf/temp/",
                "/srvconf/conn/",
                "/srvconf/login/",
                "/srvconf/scene/",
                "/srvconf/world/",
                "/srvconf/data/",
                "/srvconf/cross/",
                "/srvconf/log/"      
    };
    
    string keyAddr=s_strSrvcfg[Conn_Type];
   
    keyAddr+="addr";

    keyAddr+=tostring(1);

    string strAddr = g_objSrvConfig.GetValue(keyAddr.c_str(), "0");
    
    cout<<strAddr<<endl;

    return 0;
}
