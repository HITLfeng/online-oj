
//tail.cpp不给用户看 最终编译时会把用户提交的代码和tail.cpp合并再编译
//#include "header.cpp"

void Test1(){
  bool ret = Solution().isPalindrome(121);
  if(ret)
    std::cout<<"Test1 OK"<<std::endl;
  else
    std::cout<<"Test1 failed!!!!!!!!!!!!!!"<<std::endl;
}

void Test2(){
  bool ret = Solution().isPalindrome(-121);
  if(!ret)
    std::cout<<"Test2 OK"<<std::endl;
  else
    std::cout<<"Test2 failed!!!!!!!!!!!!!!"<<std::endl;
}

int main(){
  Test1();
  Test2();
  return 0;
}
