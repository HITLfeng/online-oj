#pragma once 
#include <jsoncpp/json/json.h>
#include "Util.hpp"
#include <atomic>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

//head_only 风格 hpp 

/////////////////////////////////////////////
//此模块完成在线编译功能
//提供Compiler类 提供 CompilerAndRun 函数
//完成编译加运行功能
/////////////////////////////////////////////


class Compiler{
public:
  //static bool CompilerAndRun(std::string& req_json, std::string& resp_json)
  //Json::Value 是JAONcpp中的核心类 借助这个类可以完成序列化和反序列化 使用方法和map很像 键值对
  static bool CompilerAndRun(const Json::Value& req, Json::Value& resp){
    //根据json请求对象生成一份源代码文件
    //std::cout<<"req[\"code\"]="<<req["code"].asString()<<std::endl;
    if(req["code"].empty()){
      resp["error"] = "3";
      resp["reason"] = "code empty";
      LOG(ERROR,"code is empty");
      return false;
    }
    //req["code"] 类型也是 Json::Value
    const std::string code = req["code"].asString();// const 不涉及拷贝
    std::string file_name = WriteTmpFile(code,req["stdin"].asString());//将code 写入代码文件
    //标准输入也写入文件中 
    //#if(!req["stdin"].empty()){}
    //调用G++进行编译 （fork + exec）
    bool ret = Compile(file_name);
    if(!ret){
      resp["error"] = "1";
      std::string reason;
      FileUtil::Read(CompilerError(file_name), reason);
      resp["reason"] = reason;
      LOG(ERROR,"compiler failed");
      return false;
    }
    //    编译成功 生成可执行程序
    //    编译出错 记录出错原因  重定向到文件当中
    //若编译成功调用可执行程序运行，把标准输入记录到文件 文件内容重定向到
    //可执行程序，可执行程序的标准输出和标准错误也要重定向输出记录到文件中
    int sig = Run(file_name);
    if(sig != 0){
      resp["error"] = "2";
      resp["reason"] = "Progrm exit by signo:" + std::to_string(sig);
      LOG("INFO","Program exit by signo "+std::to_string(sig));
      return false;
    }
    //把程序最终结果进行返回 构造resp对象
    resp["error"] = "0";
    resp["reason"] = "";
    std::string str_stdout;
    FileUtil::Read(StdoutPath(file_name), str_stdout);
    resp["stdout"] = str_stdout;
    std::string str_stderr;
    FileUtil::Read(StderrPath(file_name), str_stderr);
    resp["stderr"] = str_stderr;
    LOG(INFO,"Program "+file_name+" done");
    
    return true;
  }
  //1.源代码文件 2.编译错误文件 3.可执行文件 
  //4.标准输入文件 5.标准输出文件 6.标准错误文件
  
  //1.源代码文件
  static std::string SrcPath(const std::string& name){
    return "./temp_files/"+name+".cpp";
    //请求与请求之间 name必须是不相同的
  }
  //2.编译错误文件
  static std::string CompilerError(const std::string& name){
    return "./temp_files/"+name+".compiler_error";
  }
  //3.可执行文件
  static std::string ExePath(const std::string& name){
    return "./temp_files/"+name+".exe";
  }
  //4.标准输入文件
  static std::string StdinPath(const std::string& name){
    return "./temp_files/"+name+".in";
  }
  //5.标准输出文件
  static std::string StdoutPath(const std::string& name){
    return "./temp_files/"+name+".out";
  }
  //6.标准错误文件
  static std::string StderrPath(const std::string& name){
    return "./temp_files/"+name+".err";
  }
private:
  //把代码写入文件里 为这个文件分配唯一 name
  //分配的名字形如 155099992.2
  static std::string WriteTmpFile(const std::string& code,const std::string& str_stdin){
    static std::atomic_int id(0);//加锁开销大
    ++id;
    std::string file_name = "tmp_"
      + std::to_string(TimeUtil::TimeStamp())
      + "." + std::to_string(id);
    //open(("temp_files/"+file_name).c_str(),O_RDONLY|O_CREAT,0666);  
    /*if(fork()==0){
      execl("mkdir","mkdir","-p",("./temp_files/"+file_name).c_str());
      exit(1);
    }*/
    FileUtil::Write(SrcPath(file_name), code);
    FileUtil::Write(StdinPath(file_name), str_stdin);
    return file_name;
  }

  static bool Compile(const std::string& file_name){
    // g++ file_name.cpp -o file_name -std=c++11 -lpthread
    char* command[20] = {0};
    char buf[20][50] = {{0}};
    for(int i = 0;i < 20;++i){
      command[i] = buf[i];//初始化
    }
    sprintf(command[0],"%s","g++");
    sprintf(command[1],"%s",SrcPath(file_name).c_str());
    sprintf(command[2],"%s","-o");
    sprintf(command[3],"%s",ExePath(file_name).c_str());
    sprintf(command[4],"%s","-std=c++11");
    sprintf(command[5],"%s","-lpthread");
    command[6] = NULL;
    pid_t pid  = fork();
    if(pid > 0){
      waitpid(pid,NULL,0);
    }
    else if(pid == 0){
      int fd = open(CompilerError(file_name).c_str(),O_WRONLY|O_CREAT,0666);
      if(fd < 0){
        LOG("ERROR","Open compiler file error");//文件描述符不够？
        return false;
      }
      dup2(fd,2);// 编译错误信息直接打印到 stderr 就可以写入文件
      execvp(command[0],command);
      LOG(ERROR,"exec error");
      exit(1);
    }
    else{
      LOG(ERROR,"Fork error");
      return false;
    }
    //父进程逻辑 如何判断编译成功没有 stat 
    struct stat st;
    int ret = stat(ExePath(file_name).c_str(),&st);//看有没有生成
    if(ret < 0){
      // file didnt exist
      LOG(INFO,"Compiler.exe didnt exist");
      return false;
    }
    LOG(INFO,"Compiler success");
    return true;
  }

  static int Run(const std::string& file_name){
    //1.创建子进程
    //2.父进程等待
    //3.子进程进行程序替换
    pid_t pid = fork();
      int status = 0;
    if(pid > 0){
      waitpid(pid,&status,0);
    }
    else if(pid == 0){
      //重定向
      int fd_stdin = open(StdinPath(file_name).c_str(),O_RDONLY);
      dup2(fd_stdin,0);
      int fd_stdout = open(StdoutPath(file_name).c_str(),O_WRONLY|O_CREAT,0666);
      dup2(fd_stdout,1);
      int fd_stderr = open(StderrPath(file_name).c_str(),O_RDONLY|O_CREAT,0666);
      dup2(fd_stderr,2);

      execl(ExePath(file_name).c_str(),ExePath(file_name).c_str(),NULL);
      exit(1);
    }
    else{}
    return status & 0x7f;

  }


};

  /*HTTP请求应该包含哪些信息 响应应该包含哪些信息
   *请求 request 
   * 1.代码
   * 2.标准输入的内容
   
   *响应 reponse
   * 1.error,0 正确编译运行
   * 2.error,1 编译出错 返回reason
   * 3.error,2 运行出错 返回reason
   * 4.error,3 其他错误（code为空）
   * 标准输出
   * 标准错误
   *
   *--- 序列化 json ---
   * jsoncpp 第三方库
   * [root@VM-0-14-centos ~]# yum list|grep jsoncpp
   * Repository epel is listed more than once in the configuration
   * jsoncpp.x86_64                             0.10.5-2.el7           epel          
   * jsoncpp-devel.x86_64                       0.10.5-2.el7           epel          
   * jsoncpp-doc.noarch                         0.10.5-2.el7           epel     
   *
   *
   * request 
   * {
   *    "code":"#include...",
   *    "stdin":"1,2,3"
   * }
   * response
   * {
   *    "error":"0",
   *    "reason":"",
   *    "stdout":"6",
   *    "stderr":""
   * }
   *
   * */
