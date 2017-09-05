/*************************************************************************
> File Name: main.cpp
> Author: 
> Mail: 
> Created Time: 2017年08月25日 星期五 11时34分08秒
************************************************************************/
/*
*
*<?xml version="1.0" ?> //TiXmlDeclaration,声明
<MyApp>    //TiXmlElement,元素
    <!-- Settings for MyApp -->//TiXmlComment，注释
    <Messages>//TiXmlElement,元素
        <Welcome>Welcome to MyApp</Welcome>
        //<Welcome>是元素TiXmlElement ，“Welcome to MyApp”是TiXmlText,文本
        <Farewell>Thank you for using MyApp</Farewell>//同上
    </Messages>


    <Windows>//TiXmlElement,元素
        <Window name="MainFrame" x="5" y="15" w="400" h="250" />
        // Window是元素TiXmlElement ，name、x、y、h是TiXmlAttribute
    </Windows>

    <Connection ip="192.168.0.1" timeout="123.456000" />
</MyApp>
* */



#include<iostream>
#include"./TinyXml.h"
using namespace std;

int main()
{

    TiXmlDocument *doc=new TiXmlDocument("hahha.xml");
    
    TiXmlDeclaration *declaration=new TiXmlDeclaration("1.0","","");
    doc->LinkEndChild(declaration);

    TiXmlElement *ele=new TiXmlElement("shapes");
    doc->LinkEndChild(ele);

    TiXmlElement *cir=new TiXmlElement("circle");
    cir->SetAttribute("name","circle0");

    TiXmlText *txt=new TiXmlText("circle is round");
    cir->LinkEndChild(txt);
    
    ele->LinkEndChild(cir);

    TiXmlPrinter printer;
    
    doc->Accept(&printer);

    string devparam=string(printer.CStr());
    cout<<devparam<<endl;


    
    /*    
    const TiXmlNode *lpItemNode=NULL;
    lpItemNode=doc->RootElement()->IterateChildren("circle",lpItemNode);
    if(lpItemNode==NULL)
    {
        cout<<"not find"<<endl;
    }
    else
    {
        string str=lpItemNode->ToElement()->Attribute("name");
        cout<<str<<endl;
    }

*/








    doc->SaveFile();
    
    /* 
    TiXmlDocument xml;

    if(!xml.LoadFile("./hi.xml"))
    {
        cout<<"load eror"<<endl;
        return -1;
    }

    xml.Print();
    */

    /*
    TiXmlDocument doc;

    TiXmlDeclaration *dec=new TiXmlDeclaration("1.0","","");

    //添加新的节点元素
    TiXmlElement *ele=new TiXmlElement("hello");
    TiXmlText *text=new TiXmlText("world");
    ele->LinkEndChild(text);

    doc.LinkEndChild(dec);
    doc.LinkEndChild(ele);
    
    doc.SaveFile("hihi.xml");


*/



    return 0;
}



