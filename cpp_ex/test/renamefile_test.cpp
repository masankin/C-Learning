#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

struct FileParameter {
  char *directory;
  char *extension;
};

int append_jpg(char *basedir, char *fileext);
int slash_jpg(char *basedir, char *fileext);
char *config_profile_string(char *buffer, unsigned long buflen, char *keyword);
int config_find(char * buffer, unsigned long len, char *keyword);
int show_menu(char *basedir, char *ext);
unsigned long getfilesize(FILE *stream);
FileParameter ini_file_read(char *para_file);


int main(int argc, char *argv[]) {
  printf("Hello, world\n");
  char *param_filename = (char *) "test/sns.ini";
  printf("param_filename:%s \n", param_filename);

  char *directory = NULL, *extension = NULL;
  FileParameter fparam = ini_file_read(param_filename);
  printf("directory:%s \t extension:%s\n", fparam.directory, fparam.extension);
  int sel = show_menu(fparam.directory, fparam.extension);
  if (sel == 1)
    append_jpg(fparam.directory, fparam.extension);
  else if (sel == 2)
    slash_jpg(fparam.directory, fparam.extension);
  return 0;
}
int show_menu(char *basedir, char *ext) {
  printf("Current Directory : %s\n", basedir);
  printf("1.append %s extension to all images\n", ext);
  printf("2.slash %s extension of all images\n", ext);
  int i;
  scanf("%d", &i);
  return i;
}

//重命名文件,如果没有后缀.jpg,加上.jpg后缀
int append_jpg(char *basedir, char *fileext) {
  struct dirent *dp;
  DIR *dfd;
  char oldname[255], newname[255];
  if ((dfd = opendir(basedir)) == NULL) {
    printf("open dir failed! dir: %s\n", basedir);
    return -1;
  }
  struct stat s; /*include sys/stat.h if necessary */

  for (dp = readdir(dfd); NULL != dp; dp = readdir(dfd)) {  //dp->d_name只有名字没有路径
    sprintf(oldname, "%s/%s", basedir, dp->d_name);
    stat(oldname, &s);  //(s.st_mode & S_IFDIR)
    //if(dp->d_type == DT_DIR&& ( strcmp(dp->d_name,".")!=0 && strcmp(dp->d_name,"..")!=0 ) )//如果是目录且不是.和..
    if ((s.st_mode & S_IFDIR)
        && (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)) {
      //printf("directory:%s\n",oldname);
      append_jpg(oldname, fileext);
    } else	//否则是文件则都给改名
    {
      if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)	//如果不是.和..
          {
        if (strstr(dp->d_name, fileext) == NULL)  //如果后缀名不是.jpg
        {
          sprintf(oldname, "%s/%s", basedir, dp->d_name);
          printf("%s\n", oldname);
          sprintf(newname, "%s/%s.html", basedir, dp->d_name);	//追加后缀名
          rename(oldname, newname);
        }
      }
    }
  }
  closedir(dfd);
  return 0;
}

//从拥有后缀名.jpg的文件名里，砍掉.jpg后缀名
int slash_jpg(char *basedir, char *fileext) {
  struct dirent *dp;
  DIR *dfd;
  char oldname[255], newname[255], newfn[255];
  if ((dfd = opendir(basedir)) == NULL) {
    printf("open dir failed! dir: %s\n", basedir);
    return -1;
  }
  struct stat s;
  for (dp = readdir(dfd); NULL != dp; dp = readdir(dfd)) {
    sprintf(oldname, "%s/%s", basedir, dp->d_name);
    stat(oldname, &s);
    //if(dp->d_type == DT_DIR&& ( strcmp(dp->d_name,".")!=0 && strcmp(dp->d_name,"..")!=0 ) )//如果是目录且不是.和..
    if (((s.st_mode & S_IFMT) == S_IFDIR)
        && (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)) {
      slash_jpg(oldname, fileext);
    } else	//否则是文件则都给改名
    {
      if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)	//如果不是.和..
          {
        if (strstr(dp->d_name, fileext) != NULL)  //如果文件名中包含.jpg
        {
          sprintf(oldname, "%s/%s", basedir, dp->d_name);
          int startpos = config_find(dp->d_name, strlen(dp->d_name), fileext);  //startpos不为-1
          strncpy(newfn, dp->d_name, startpos);
          newfn[startpos] = '\0';
          sprintf(newname, "%s/%s", basedir, newfn);	//减去后缀名
          printf("%s to %s\n", oldname, newname);  //只有名字没有路径
          rename(oldname, newname);
        }
      }
    }
  }
  return 0;
}

int config_find(char * buffer, unsigned long len, char *keyword) {
  int i = 0;
  for (i = 0; i < len; i++) {
    if (strncmp((buffer + i), keyword, strlen(keyword)) == 0) {
      return i;
    }
  }
  return -1;
}

char *config_profile_string(char *buffer, unsigned long buflen, char *keyword)  //读取字符
                            {
  int firstpos = config_find(buffer, buflen, keyword);
  if (firstpos == -1)
    return (char *) "";
  firstpos += strlen(keyword);
  unsigned long kwlen = config_find((buffer + firstpos), buflen - firstpos,
                                    (char *) "\r");  //先找windows回车开始字符
  if (kwlen == -1)	//可能是linux文件的回车
    kwlen = config_find((buffer + firstpos), buflen - firstpos, (char *) "\n");  //找linux回车开始字符
  if (kwlen == -1)	//可能是文件末尾没有回车
    kwlen = buflen - firstpos;
  char *val1 = new char[kwlen + 1];
//    memcpy(val1,(buffer+firstpos),kwlen);
  strncpy(val1, (buffer + firstpos), kwlen);
  val1[kwlen] = '\0';  //字符串结尾一定要加\0,不然会产生不可预知的问题.某些linux会初始化内存空间为全0;而mac不会
//    printf("wocao:\n%s",val1);
  return val1;
}

FileParameter ini_file_read(char *para_file) {
  FILE *fp = fopen(para_file, "rb");
  FileParameter fparam;
  if (fp != NULL) {
    unsigned long thefz = getfilesize(fp);
    char * buffer = new char[thefz];
    fread(buffer, thefz, 1, fp);
    fparam.directory = config_profile_string(buffer, thefz,
                                             (char *) "directory=");
    fparam.extension = config_profile_string(buffer, thefz,
                                             (char *) "extension=");
    fclose(fp);
  } else
    printf("Can not open config file %s", para_file);
  return fparam;
}
/**获取文件大小*/
unsigned long getfilesize(FILE *stream) {
  long curpos, length;
  curpos = ftell(stream);
  fseek(stream, 0L, SEEK_END);
  length = ftell(stream);
  fseek(stream, curpos, SEEK_SET);
  return length;
}
//Current Directory : /Users/masankin/Documents/jee-workspace/z_jifen/web/member
//1.append .html extension to all images 
//2.slash .html extension of all images