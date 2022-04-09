#pragma once 
#include <stdint.h>
#include <sys/time.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <assert.h>


//获取当前的时间戳
class TimeUtil{
  public:
    static int64_t TimeStamp(){
      struct timeval tv;
      gettimeofday(&tv,NULL);
      return tv.tv_sec;
    }
    static int64_t TimeStampMS() // 获取时间戳毫秒级别
    {
      struct timeval tv;
      gettimeofday(&tv,NULL);
      return tv.tv_sec * 1000 + tv.tv_usec;
    }
};


//打印日志工具

/*
enum Level{
  INFO,
  WARNING,
  ERROR,
  FATAL
};*/
#define INFO    1
#define WARNING 2
#define ERROR   3
#define FATAL   4
#define LOG(level,msg) Log(#level,msg,__FILE__,__LINE__)

inline void Log(const std::string& level, const std::string& msg, const std::string& file_name,int line_num){
  std::cout<<"["<<level<<"]"<<"["<<TimeUtil::TimeStamp()<<"]"<<"["<<msg<<"]"<<"["<<file_name<<"]"<<"["<<line_num<<"]"<<std::endl;
  //std::cout<<"["<<level<<"]"<<"["<<time(NULL)<<"]"<<"["<<msg<<"]"<<"["<<file_name<<"]"<<"["<<line_num<<"]"<<std::endl;
}


//文件相关工具
class FileUtil{
  public:
    //传入一个文件路径，帮我们把所有内容读取到 Content字符串
    static bool Read(const std::string& file_path, std::string& Content){
      Content.clear();
      std::ifstream file(file_path.c_str());
      if(!file.is_open()){
        return false;
      }
      std::string line;
      //getline 会去掉 \n
      while(std::getline(file,line)){
        Content += line + "\n";
      }
      file.close();
      return true;
    }

    static bool Write(const std::string& file_path, const std::string& Content){
      std::ofstream file(file_path.c_str());
      if(!file.is_open()){
        return false;
      }
      file.write(Content.c_str(),Content.size());
      file.close();
      return true;
    }
};

class StringUtil{
public:
  static void Split(const std::string& input,const std::string& split_char,
      std::vector<std::string>& output){
    boost::split(output,input,boost::is_any_of(split_char),boost::token_compress_on);
    //aaa  bb cc  3 切成部分还是四部分 on 是支持分隔符压缩
  }
};

class HttpUtility{
  public:
    static unsigned char ToHex(unsigned char x) 
    { 
      return x > 9 ? x + 55 : x + 48; 
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
 
    static std::string URLEncode(const std::string& str) 
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
          strTemp += HttpUtility::ToHex((unsigned char)str[i] >> 4); 
          strTemp += HttpUtility::ToHex((unsigned char)str[i] % 16); 
        } 
      } 
      return strTemp; 
    } 
 
    static std::string URLDecode(const std::string& str) 
    { 
      std::string strTemp = ""; 
      size_t length = str.length(); 
      for (size_t i = 0; i < length; i++) 
      { 
        if (str[i] == '+') strTemp += ' '; 
        else if (str[i] == '%') 
        { 
          assert(i + 2 < length); 
          unsigned char high = HttpUtility::FromHex((unsigned char)str[++i]); 
          unsigned char low = HttpUtility::FromHex((unsigned char)str[++i]); 
          strTemp += high*16 + low; 
        } 
        else strTemp += str[i]; 
      } 
      return strTemp; 
    } 
};

class UrlUtil{
public:
    static std::string UrlDecode(const std::string& str){
			return HttpUtility::URLDecode(str);
    } 
    static void ParseBody(const std::string& body,std::unordered_map<std::string,std::string>& body_kv){
    std::vector<std::string> kvs;
    StringUtil::Split(body,"&",kvs);
    for(int i = 0; i < kvs.size(); ++i){
      std::vector<std::string> kv;
      StringUtil::Split(kvs[i],"=",kv);
      if( kv.size() != 2 ){
        continue;
      }
      body_kv[kv[0]] = UrlUtil::UrlDecode(kv[1]);//kv[0] 一般是 code  stdin 不用转义
    }
    //set paste / nopaste
  }
};

/*
class HttpUtility
{
public:
    typedef unsigned char BYTE;
 
    inline BYTE toHex(const BYTE &x)
    {
        return x > 9 ? x -10 + 'A': x + '0';
    }
 
    inline BYTE fromHex(const BYTE &x)
    {
        return isdigit(x) ? x-'0' : x-'A'+10;
    }
 
    inline static std::string URLEncode(const std::string &sIn)
    {
        std::string sOut;
        for( size_t ix = 0; ix < sIn.size(); ix++ )
        {      
            BYTE buf[4];
            memset( buf, 0, 4 );
            if( isalnum( (BYTE)sIn[ix] ) )
            {      
                buf[0] = sIn[ix];
            }
            //else if ( isspace( (BYTE)sIn[ix] ) ) //貌似把空格编码成%20或者+都可以
            //{
            //    buf[0] = '+';
            //}
            else
            {
                buf[0] = '%';
                buf[1] = toHex( (BYTE)sIn[ix] >> 4 );
                buf[2] = toHex( (BYTE)sIn[ix] % 16);
            }
            sOut += (char *)buf;
        }
        return sOut;
    };
 
    inline static std::string URLDecode(const std::string &sIn)
    {
        std::string sOut;
        for( size_t ix = 0; ix < sIn.size(); ix++ )
        {
            BYTE ch = 0;
            if(sIn[ix]=='%')
            {
                ch = (fromHex(sIn[ix+1])<<4);
                ch |= fromHex(sIn[ix+2]);
                ix += 2;
            }
            else if(sIn[ix] == '+')
            {
                ch = ' ';
            }
            else
            {
                ch = sIn[ix];
            }
            sOut += (char)ch;
        }
        return sOut;
    }
};*/

