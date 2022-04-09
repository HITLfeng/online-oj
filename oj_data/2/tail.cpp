
//tail.cpp不给用户看 最终编译时会把用户提交的代码和tail.cpp合并再编译
//#include "header.cpp"

void Test1(std::vector<int> v){
  vector<vector<int>> vv;
  vv = Solution().permute(v);
  vector<vector<int>> stdvv = {{1,2,3},{1,3,2},{2,1,3},{2,3,1},{3,1,2},{3,2,1}};
  if(vv == stdvv)
    std::cout<<"Test1 OK"<<std::endl;
  else
    std::cout<<"Test1 failed!!!!!!!!!!!!!!"<<std::endl;
}

int main()
{ 
  vector<int> v{1,2,3};
  Test1(v);
  return 0;
}
