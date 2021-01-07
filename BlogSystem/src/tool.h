#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include <boost/algorithm/string.hpp>
class StringTools
{

     public:
       static void Split(const std::string& input,  const std::string& split_char, std::vector<std::string>* output)
       {
         boost::split(*output, input, boost::is_any_of(split_char), boost::token_compress_off);
       }
};
class UrlUtil
{
    //1.分割正文提交的数据
    public:
        static void PraseBody(const std::string& body, std::unordered_map<std::string, std::string>* param)
        {
            std::vector<std::string> output;
             StringTools::Split(body, "&", &output);
            for(const auto& token:output)
            {
                std::vector<std::string> kv;
                 StringTools::Split(token, "=", &kv);
                if(kv.size() != 2)
                {
                    continue;
                }
                (*param)[kv[0]] = UrlDecode(kv[1]);
            }
        }
    private:
        static unsigned char ToHex(unsigned char x) 
        { 
            return  x > 9 ? x + 55 : x + 48; 
        }

        static unsigned char FromHex(unsigned char x) 
        { 
            unsigned char y;
            if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
            else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
            else if (x >= '0' && x <= '9') y = x - '0';
            else assert(0);
            return y;
        }

       static std::string UrlEncode(const std::string& str)
        {
            std::string strTemp = "";
            size_t length = str.length();
            for (size_t i = 0; i < length; i++)
            {
                if (isalnum((unsigned char)str[i]) || 
                        (str[i] == '-') ||
                        (str[i] == '_') || 
                        (str[i] == '.') || 
                        (str[i] == '~'))
                    strTemp += str[i];
                else if (str[i] == ' ')
                    strTemp += "+";
                else
                {
                    strTemp += '%';
                    strTemp += ToHex((unsigned char)str[i] >> 4);
                    strTemp += ToHex((unsigned char)str[i] % 16);
                }
            }
            return strTemp;
        }

        static std::string UrlDecode(const std::string& str)
        {
            std::string strTemp = "";
            size_t length = str.length();
            for (size_t i = 0; i < length; i++)
            {
                if (str[i] == '+') strTemp += ' ';
                else if (str[i] == '%')
                {
                    assert(i + 2 < length);
                    unsigned char high = FromHex((unsigned char)str[++i]);
                    unsigned char low = FromHex((unsigned char)str[++i]);
                    strTemp += high*16 + low;
                }
                else strTemp += str[i];
            }
            return strTemp;
        }
};

