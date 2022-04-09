#pragma once 
#include <ctemplate/template.h>
//#include "./ThirdPartLibForCpp/el7.x86_64/third_part/include/ctemplate/template.h"
#include "oj_model.hpp"

class OjView{
public:
  //根据数据生成html --- 网页渲染
  static void RenderAllQuestions(const std::vector<Question>& all_questions,std::string* html){
    //c++ 直接通过字符串拼接构造网页太麻烦
    //考虑通过网页模板解决问题
    // #include <ctemplate/template.h>
    // 模板类似与填空，把HTML中需要动态计算的数据挖空留下来 根据计算结果填空
    ctemplate::TemplateDictionary dict("all_questions");
    for(const auto& question : all_questions){
      ctemplate::TemplateDictionary* table_dict = dict.AddSectionDictionary("question"); 
      table_dict->SetValue("id", question.id);
      table_dict->SetValue("name", question.name);
      table_dict->SetValue("degree", question.degree);

      ctemplate::Template* tpl;
      tpl = ctemplate::Template::GetTemplate(
          "./template/all_questions.html",ctemplate::DO_NOT_STRIP);
      tpl->Expand(html,&dict);
    }
  }

  static void RenderQuestion(const Question& question,std::string* html){
    
    ctemplate::TemplateDictionary dict("question");
    dict.SetValue("id", question.id);
    dict.SetValue("name", question.name);
    dict.SetValue("degree", question.degree);
    dict.SetValue("desc", question.desc);
    dict.SetValue("header", question.header_cpp);

    ctemplate::Template* tpl;
    tpl = ctemplate::Template::GetTemplate(
          "./template/question.html",ctemplate::DO_NOT_STRIP);
    tpl->Expand(html,&dict);

  }

  static void RenderResult(const std::string& str_stdout,const std::string& reason,std::string* html){
    ctemplate::TemplateDictionary dict("result");
    dict.SetValue("stdout", str_stdout);
    dict.SetValue("reason", reason);

    ctemplate::Template* tpl;
    tpl = ctemplate::Template::GetTemplate(
          "./template/result.html",ctemplate::DO_NOT_STRIP);
    tpl->Expand(html,&dict);


  }

};
