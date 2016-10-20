#include <stdio.h>

void func1(){
  printf("Inside func1!\n");
}

void func2(){
  printf("Inside func2!\n");
}

int main (int argc, const char* argv[]) {
  void (*func[2])() = {&func1, &func2};
  func[0]();
  func[1]();
  
  return 0;
}


