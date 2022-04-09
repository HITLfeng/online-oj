#pragma once 
//(MVC) 经典软件设计方式  MVVM 现代


//一个题目对应一个目录 里面包含
//header.cpp 代码框架
//tail.cpp 代码测试用例
//desc.txt 题目详细描述
//
//oj_config.cfg 作为总的入口文件 行文本文件
//这一行包括 id,name,hard,dir（对应的目录）

//把题目中加载的文件信息加载起来供服务器使用

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <fstream>
#include "Util.hpp"

struct Question{
  std::string id;
  std::string name;
  std::string degree;
  std::string dir;

  //根据dir获取
  std::string header_cpp;
  std::string tail_cpp;
  std::string desc;
};


class OjModel{
private:
  std::map<std::string, Question> m;//id:question
public:
  bool GetAllQuestions(std::vector<Question>* questions) const{
    questions->clear();
    for(const auto& kv : m){
      questions->push_back(kv.second);
    }
    return true;
  }
  
  bool GetQuestion(const std::string& id, Question* q){
    std::map<std::string, Question>::iterator pos = m.find(id);
    if(pos == m.end()){
      return false;
    }
    *q = pos->second;
    return true;
  }

  bool Load(){
    //1.打开 oj_config.cfg
    std::ifstream file("./oj_data/oj_config.cfg");
    //2.按行读取
    std::string line;
    while(std::getline(file,line)){
      std::vector<std::string> tokens;
      //StringUtil::Split(line,"\t",tokens);
      StringUtil::Split(line," ",tokens);
      if(tokens.size() != 4){
        LOG(ERROR,"config file format error, num = "+std::to_string(tokens.size()));
        continue;
      }
      Question q;
      q.id = tokens[0];
      q.name = tokens[1];
      q.degree = tokens[2];
      q.dir = tokens[3];
      FileUtil::Read(q.dir + "/header.cpp", q.header_cpp);
      FileUtil::Read(q.dir + "/tail.cpp", q.tail_cpp);
      FileUtil::Read(q.dir + "/desc.txt", q.desc);
      m[q.id] = q;
    }
    file.close();
    LOG(INFO,"Load title number : "+std::to_string(m.size()));
    return true;
  }

};

