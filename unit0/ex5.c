#include <stdio.h>
#include <stdlib.h>

int main() {
  char string[3];
  gets(string);
  puts(string);
  
  /* Convert the source file timestamp into a localized date string */ 
  char * localized_time(char * filename)
  {
  struct tm *tm_ptr; 
  struct stat stat_block; 
  char buffer[120];
  /* get the sourcefile's timestamp in time_t format */ 
  stat(filename, &stat_block);
  /* convert UNIX time_t into a struct tm holding local time */
  tm_ptr = localtime(&stat_block.st_mtime);}
  /* convert the tm struct into a string in local format */ strftime(buffer, sizeof(buffer), "%a %b %e %T %Y", tm_ptr);
  return buffer;
  }
  
  
  return 0;
  
  
}