#include "httplib.h"
#include "Util.hpp"
#include "oj_model.hpp"
#include "view.hpp"
#include <jsoncpp/json/json.h>
#include "com.hpp"

int main(){
  OjModel model;
  model.Load();
  using namespace httplib;
  Server server;
  server.Get("/all_questions",[&model](const Request& req, Response& resp){
        (void)req;
        std::vector<Question> all_questions;
        model.GetAllQuestions(&all_questions);
        //构建 resp vQ --> HTML view.hpp
        std::string html;
        OjView::RenderAllQuestions(all_questions,&html);
        resp.set_content(html,"text/html");
      });
  //  R"()" c++11  语法 原始字符串 忽略字符串中的转义字符
  //  \d+ 正则表达式 
  server.Get(R"(/question/(\d+))",[&model](const Request& req, Response& resp){
      //std::cout<<req.matches[0].str()<<std::endl;// /question/1
      //std::cout<<req.matches[1].str()<<std::endl;// 1 
      Question question;
      model.GetQuestion(req.matches[1].str(),&question);
      std::string html;
      OjView::RenderQuestion(question,&html);
      
      resp.set_content(html,"text/html");
      });
  server.Post(R"(/oj/(\d+))",[&model](const Request& req, Response& resp){
      Question question;
      model.GetQuestion(req.matches[1].str(),&question);
      //std::cout<<"ret = "<<ret<<std::endl;
      //std::cout<<question.id<<std::endl;
      //std::cout<<question.tail_cpp<<std::endl;
      
      std::unordered_map<std::string,std::string> body_kv;
      UrlUtil::ParseBody(req.body, body_kv);
      const std::string& user_code = body_kv["code"];
      //跟 tailcpp 拼接
      Json::Value req_json;
      //std::cout<<question.tail_cpp<<std::endl;
      req_json["code"] = user_code + question.tail_cpp;
      Json::Value resp_json;
      //调用编译模板进行编译
      Compiler::CompilerAndRun(req_json,resp_json);
      //根据编译结果构造最终网页
      std::string html;
      OjView::RenderResult(resp_json["stdout"].asString(),resp_json["reason"].asString(),&html);
      resp.set_content(html,"text/html");
      });
  server.set_base_dir("./wwwroot");
  server.listen("0.0.0.0", 8888);



  return 0;
}

