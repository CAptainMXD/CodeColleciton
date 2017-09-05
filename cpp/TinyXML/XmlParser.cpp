/*************************************************************************
> File Name: XmlParaser.cpp
> Author: 
> Mail: 
> Created Time: 2017年08月25日 星期五 16时38分33秒
************************************************************************/
#include "XmlParser.h"

    XmlParser::XmlParser()
    : m_bValid(false)
    {

    }

    XmlParser::~XmlParser()
    {

    }




int XmlParser::LoadFromFile(const std::string& file)
{
    TiXmlDocument doc(file.c_str());
    m_bValid=doc.LoadFile(TIXML_ENCODING_UTF8);

    if(m_bValid==false)
    {
        return -1;
    }

    MyTiVisitor myVisitor;
    if(doc.Accept(&myVisitor)<0)
    {
        return -1;
    }

    m_mapValues=myVisitor.m_params.m_mapValues;

    return 0;
}
std::string XmlParser::GetValue(const std::string& path,const std::string& defaultValue)
{
    CollectionMapValuesT::iterator iter = m_mapValues.find(path);
    if(iter == m_mapValues.end())
    return defaultValue;

    ListValuesT& values = iter->second;
    if (values.empty())
    {
        return defaultValue;

    }

    return iter->second.front();

}

ListValuesT* XmlParser::GetValues( const std::string& path )
{
    CollectionMapValuesT::iterator iter = m_mapValues.find(path);
    if(iter == m_mapValues.end())
    {
        return NULL;

    }
    return &iter->second;

}

        int XmlParser::CleanUp()
        {
            m_mapValues.clear();
            m_bValid = false;
            return 0;

        }


//这种从没有考虑一个element有参数的情况,只有text这种情况
bool MyTiVisitor::VisitEnter( const TiXmlElement& element, const TiXmlAttribute* firstAttribute  )
{
    m_params.m_vParents.push_back(element.Value());
    return true;
}




bool MyTiVisitor::VisitExit( const TiXmlElement& element  )
{
    m_params.m_vParents.pop_back();
    return true;        
}



bool isSpaces(const char* szStr)
{
    if(szStr==NULL) return false;
    while (*szStr != '\0')
    {
        char c = *szStr;
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
        ++szStr;
        else
        return false;

    }

    return true;

}

bool MyTiVisitor::Visit( const TiXmlText& text  )
{
    if(text.Value()==NULL) return false;

    if(isSpaces(text.Value()))return false;

    string val(text.Value());

    string key = m_params.GetParents();

    m_params.m_mapValues[key].push_back(val);

    return true;
}








