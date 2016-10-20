#include <stdlib.h>
#include <stdio.h>

int foo(const char** p) {
  printf("Hello.\n");
  return 0;
}

int array[] = {23, 34, 12, 17, 204, 99, 16};
#define TOTAL_ELEMENTS (sizeof(array) / sizeof(array[0]))
  
/*test 7*/
generate_initializer(char* string) {
  static char separator = ' ';
  printf("%c %s \n", separator, string);
  separator = ',';
}  
 
int main(int argc, const char* argv[]) {
  /*test 1*/
  char* cp;
  const char* ccp;
  ccp = cp;
  //cp = ccp;
  
  /*test 2*/
  const int limit = 10;
  const int* limitp = &limit;
  int i = 27;
  limitp = &i;
  //limit = 20;
  
  /*test 3*/
  foo(argv);
  
  /*test 4*/
  int d = -1, x;
  if (d <= TOTAL_ELEMENTS - 2) {
    x = array[d + 1];
  }
  printf("x is %d\n", x);

  /*test 5*/
  switch(2) {
    case 1: printf("case 1.\n");
    case 2: printf("case 2.\n");
    case 3: printf("case 3.\n");
    case 4: printf("case 4.\n");
    default: printf("default.\n");
  }

  /* test 6*/
  // network code() {
  //   switch(line) {
  //     case THING1: { doit1(); break; }
  //     case THING2: {
  //       if (x == STUFF) {
  //         do_first_stuff();
  //         if (y == OTHER_STUFF) { break; }
  //         do_later_stuff();
  //       } /* meant to jump to here */
  //       initialize_modes_pointer();
  //       break;
  //     default: { processing(); }
  //     } /* but it jumped to here */
  //     use_modes_pointer(); /* but modes_pointer wasn't initialized */
  //   }
  // }

  return 0;
}