#include <jsoncpp/json/json.h>
#include "com.hpp"

int main(){
  //int fd = open("a/b/c",O_RDWR|O_CREAT,0666);
  //close(fd);
  
  Json::Value req;
  req["code"] = "#include<stdio.h>\n int main(){  printf(\"hehe da de bu cuo you\");return 0;}";
  req["stdin"] = "";
  Json::Value resp;
  Compiler::CompilerAndRun(req,resp);

  Json::FastWriter writer;
  LOG(INFO,writer.write(resp));

  return 0;
}
