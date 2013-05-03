#include "xml-message.h"
#include "crypto/crypto.h"

std::string XMLBuildMessage(const char* name, const char* content)
{
    TiXmlDocument doc;
    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
    TiXmlElement * element = new TiXmlElement( "message" );
    
    TiXmlElement * element_name = new TiXmlElement( "name" );
    TiXmlElement * element_content = new TiXmlElement( "content" );
    
    INFO("debug", "XML: writing name -> %s\n", name);
    TiXmlText * text_name = new TiXmlText( EncodeBase64(name) );
    INFO("debug", "XML: writing content -> %s\n", content);
    TiXmlText * text_content = new TiXmlText( EncodeBase64(content) );
    
    element->LinkEndChild( element_name );
    element->LinkEndChild( element_content );
    
    element_name->LinkEndChild( text_name );
    element_content->LinkEndChild( text_content );
    
    doc.LinkEndChild( decl );
    doc.LinkEndChild( element );

    stringstream ss;
    ss << doc;

    return ss.str(); 
}

void XMLReadMessage(const char *str, string& name, string& content)
{
    if(!str)
        return;
        
    TiXmlDocument doc;
    doc.Parse(str);
    
    TiXmlElement* root = doc.FirstChildElement();

    if(root == NULL)
    {
        cerr << "Failed to load file: No root element." << endl;
        doc.Clear();
        return;
    }
    
    for(TiXmlElement* elem = root->FirstChildElement();
        elem != NULL;
        elem = elem->NextSiblingElement())
    {
        const char* attr = NULL;
        string elemName = elem->Value();
        //cout << "elem: " << elemName << endl;
        if (elemName == "name")
        {
            attr = elem->Attribute("level");
            //if (attr != NULL){};
        }
        else if (elemName == "content")
        {
            attr = elem->Attribute("type");
            //if(attr != NULL){};
        }
        
        for(TiXmlNode* e = elem->FirstChild();
            e != NULL;
            e = e->NextSibling())
        {
            TiXmlText* text = e->ToText();
            if(text == NULL)
                continue;

            if(elemName == "name")
            {
                name = DecodeBase64(text->Value());
                INFO("debug", "XML: reading name -> %s\n", name.c_str());
            }
            else if (elemName == "content")
            {
                content = DecodeBase64(text->Value());
                INFO("debug", "XML: reading content -> %s\n", content.c_str());
            }
        }
     }
}

std::string XMLBuildUpdate(const char* name, const char* status)
{
    TiXmlDocument doc;
    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
    TiXmlElement * element = new TiXmlElement( "update" );
    
    TiXmlElement * element_name = new TiXmlElement( "name" );
    INFO("debug", "XML: writing name -> %s\n", name);
    TiXmlText * text_name = new TiXmlText( EncodeBase64(name) );

    element->LinkEndChild( element_name );
    element_name->LinkEndChild( text_name );
    
    INFO("debug", "XML: writing status -> %s\n", status);
    element_name->SetAttribute("status", EncodeBase64(status));
    
    doc.LinkEndChild( decl );
    doc.LinkEndChild( element );

    stringstream ss;
    ss << doc;
 
    return ss.str(); 
}

void XMLReadUpdate(const char *str, string& name, string& status)
{
    if(!str)
        return;
        
    TiXmlDocument doc;
    doc.Parse(str);
    
    TiXmlElement* root = doc.FirstChildElement();

    if(root == NULL)
    {
        cerr << "Failed to load file: No root element." << endl;
        doc.Clear();
        return;
    }
    
    for(TiXmlElement* elem = root->FirstChildElement();
        elem != NULL;
        elem = elem->NextSiblingElement())
    {
        const char* attr;
        string elemName = elem->Value();
        if(elemName == "name")
        {
            attr = elem->Attribute("status");
            if(attr != NULL)
            {
                status = DecodeBase64(attr);
                INFO("debug", "XML: reading status -> %s\n", status.c_str());
            }
        }
        
        for(TiXmlNode* e = elem->FirstChild();
            e != NULL;
            e = e->NextSibling())
        {
            TiXmlText* text = e->ToText();
            if(text == NULL)
                continue;

            if(elemName == "name")
            {
                name = DecodeBase64(text->Value());
                INFO("debug", "XML: reading name -> %s\n", name.c_str());
            }
        }
     }
}

/*
E' un esempio, la pwd potrebbe essere usata per decifrare il certificato
privato, o come segreto condiviso da passare in modo sicuro...
*/
std::string XMLBuildLogin(const char* username, const char* password)
{
    TiXmlDocument doc;
    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
    TiXmlElement * element = new TiXmlElement( "login" );
    
    TiXmlElement * element_name = new TiXmlElement( "username" );
    INFO("debug", "XML: writing username -> %s\n", username);
    TiXmlText * text_name = new TiXmlText( EncodeBase64(username) );

    element->LinkEndChild( element_name );
    element_name->LinkEndChild( text_name );
    
    string pwd_digest;
    int len = strlen(password);
    if (len)
        SHA256_digest(password, len, pwd_digest);
    element_name->SetAttribute("password", pwd_digest.c_str());
    
    doc.LinkEndChild( decl );
    doc.LinkEndChild( element );

    stringstream ss;
    ss << doc;
 
    return ss.str(); 
}

void XMLReadLogin(const char *str, string& username, string& password_digest)
{
    if(!str)
        return;
        
    TiXmlDocument doc;
    doc.Parse(str);
    
    TiXmlElement* root = doc.FirstChildElement();

    if(root == NULL)
    {
        cerr << "Failed to load file: No root element." << endl;
        doc.Clear();
        return;
    }
    
    for(TiXmlElement* elem = root->FirstChildElement();
        elem != NULL;
        elem = elem->NextSiblingElement())
    {
        const char* attr;
        string elemName = elem->Value();
        if(elemName == "username")
        {
            attr = elem->Attribute("password");
            if(attr != NULL)
            {
                password_digest = attr;
            }
        }
        
        for(TiXmlNode* e = elem->FirstChild();
            e != NULL;
            e = e->NextSibling())
        {
            TiXmlText* text = e->ToText();
            if(text == NULL)
                continue;

            if(elemName == "username")
            {
                username = DecodeBase64(text->Value());
                INFO("debug", "XML: reading username -> %s\n", username.c_str());
            }
        }
     }
}

