/*************************************************************************
	> File Name: XmlParser.h
	> Author: 
	> Mail: 
	> Created Time: 2017年08月25日 星期五 16时24分25秒
 ************************************************************************/

#ifndef _XMLPARSER_H
#define _XMLPARSER_H
using namespace std;
#include<list>
#include<vector>
#include<string>
#include<iostream>
#include<sstream>
#include<map>
#include "TinyXml.h"


typedef list<string> ListValuesT;

typedef map<string,ListValuesT> CollectionMapValuesT;


struct XmlParseParams
{
    vector<string> m_vParents; //用来存储所有的父类节点

    CollectionMapValuesT m_mapValues; //


    string GetParents()
    {
        string result;
        result.reserve(512);
        for(size_t i=0;i<m_vParents.size();i++)
            result.append("/").append(m_vParents[i]);

        return result;
    }
    
};

class MyTiVisitor:public TiXmlVisitor
{
    public:

        virtual bool VisitEnter(const TiXmlElement & element,const TiXmlAttribute* firstAttribute);
        virtual bool VisitExit( const TiXmlElement& element  );
        virtual bool Visit( const TiXmlText& text  );
        
        XmlParseParams m_params;
};


class XmlParser
{
    public:
        XmlParser();
        ~XmlParser();
        public:
            int CleanUp();
            int LoadFromFile(const std::string& file);
            int LoadFromStream(std::istream& stream);
            int LoadFromString(const std::string& text);
        public:
            bool IsValid() { return m_bValid;  }
    public:

            std::string GetValue(const std::string& path,const std::string& defaultValue = "");
            ListValuesT* GetValues(const std::string& path);
    public:
            CollectionMapValuesT m_mapValues;
            bool m_bValid;
};











#endif
