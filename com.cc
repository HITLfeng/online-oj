#include "httplib.h"
#include "com.hpp"
#include <jsoncpp/json/json.h>
#include <unordered_map>

int main(){
  using namespace httplib;
  Server server;
  //叫做路由 什么样的路径对应什么样的处理函数
  server.Post("/oj",[](const Request& req, Response& resp){
    //(void)req;//没处理 req 会报警告，无所谓
    Json::Value req_json;
    //req and req_json如何联动
    //HTTP --- JSON
    //code=#include...&stdin=hello
    //此处由于提交的代码可能含有一些特殊符号，想要正确传输就要转义 浏览器帮我们完成了 URLENCODING
    //服务器收到数据(req.body)先urldecode 再解析整理为json
    std::unordered_map<std::string,std::string> body_kv;
    std::cout<<req.body<<std::endl;
    UrlUtil::ParseBody(req.body, body_kv);
    Json::Value resp_json;
    for(auto& e:body_kv){
      req_json[e.first] = e.second;
    }
    Compiler::CompilerAndRun(req_json,resp_json);
    //把json::value序列化 才能返回 
    Json::FastWriter writer;
    resp.set_content(writer.write(resp_json), "text/plain");//数据 数据类型    
  });//注册一个回调函数 处理get请求时自动调用
  server.set_base_dir("./wwwroot");
  server.listen("0.0.0.0", 8888);//创建socket 设置端口复用 bind listen 一条龙

  return 0;
}
