#include "UpdateHelper.h"

#include <stdio.h>
#include <sys/stat.h>
#include <vector>
#include <algorithm>    // std::replace


#include "unzip/unzip.h"

#ifdef WIN32
//do nothing
#else  
#include <utime.h> 
#include <sys/time.h> 
#include <dirent.h>  
#endif



#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)  
const char FILE_TOKEN = '\\'; 
#else  
const char FILE_TOKEN = '/'; 
#endif 






TempFile::~TempFile() {
    if (m_fp) {
        fclose(m_fp);
    }
    if (m_path.size() > 0) {
        remove(m_path.c_str());
    }
    //delete m_mdContext;
}

const std::string TempFile::GetSign() {
    unsigned char buf[10240];
    Close();
    Open("rb");
    MD5 md5;
    int n = fread(buf, 1, 10240, m_fp);
    md5.update(buf, n);
    while (n == 10240) {
        n = fread(buf, 1, 10240, m_fp);
        md5.update(buf, n);
    }
    md5.finalize();
    return md5.hexdigest();
}

size_t TempFile::Write(char *ptr, size_t size, size_t nmemb) {
    return fwrite(ptr, size, nmemb, m_fp);
}

bool TempFile::Open(const char * mode) {
    Close();
    m_fp = fopen(m_path.c_str(), mode);
    return m_fp != NULL;
}

void TempFile::Close() {
    if (m_fp) {
        fclose(m_fp);
        m_fp = NULL;
    }
}


//////////////////////zip file///////////////////////


FZipFile::FZipFile(const char * path, int* iProgress) {
	m_file = unzOpen(path);
	m_bIsHaveSo = false;
	m_piProgress = iProgress;
	*m_piProgress = 0;
}
FZipFile::~FZipFile() {
	unzClose(m_file);
}

bool FZipFile::IsOpen() {
	return m_file != NULL;
}
bool FZipFile::getIsHaveSo() {
	return m_bIsHaveSo;
}

int FZipFile::ExtractTo(const char * root) {
   
    char buf[16*1024];
    CCLOG("[trace]ExtractTo, root %s", root);
    unz_global_info64 stGlobalFileInfo;
    stGlobalFileInfo.number_entry = 0;
    stGlobalFileInfo.size_comment = 0;
    int iCurDoFileNum = 0; //当前处理的文件数
    unzGetGlobalInfo64(m_file, &stGlobalFileInfo); //获取总文件数
    int ret = unzGoToFirstFile(m_file);
    int retval = 0;
	std::string pathtmp;
    std::string lastFolder;
    string strWinPath;
    string strFileOne;

    while (ret == UNZ_OK) {
        FILE * fs = NULL;
		bool isHaveSo = false;
        iCurDoFileNum++;
        *m_piProgress = int(iCurDoFileNum*100/stGlobalFileInfo.number_entry); //进度

        std::string folder;
        unzGetCurrentFileInfo64(m_file, NULL, buf, 1024, 
            NULL, 0, NULL, 0);
        if (unzOpenCurrentFile(m_file) != UNZ_OK) {
            CCLOG("[error]ZipFile::ExtraceTo,Fail to open current file");
            retval = -1;
            goto HANDLE_FILE_ERROR;
        }
		
       // snprintf(pathtmp, 1024, "%s/%s", root, buf);
		pathtmp = StringUtils::format("%s/%s", root, buf);
        strFileOne = buf;
        if(string::npos != strFileOne.find(".so")) {
			CCLOG("[trace]ExtractTo ,Exist so");
            m_bIsHaveSo = true;
			isHaveSo = true;
        }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) 
        //strWinPath = pathtmp;
        //strWinPath = UpdateHelper::ConvertWinPath(strWinPath);
        //snprintf(pathtmp, 1024, "%s", strWinPath.c_str());
		pathtmp = UpdateHelper::ConvertWinPath(pathtmp.c_str());
#endif
		CCLOG("[trace]ExtractTo pathtmp:%s",pathtmp.c_str());
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		if (isHaveSo && FileUtils::getInstance()->isFileExist(pathtmp.c_str()))
		{
			//如果包含so更新.且已经存在so,则重命名新的so
			int index = strFileOne.find(".so");
			CCLOG("[trace]ExtractTo ,index:%d",index);
			strFileOne.insert(index, "1");
			index = pathtmp.find(".so");
			pathtmp.insert(index, "1");
		    snprintf(buf, 1024, "%s", strFileOne.c_str());
			CCLOG("[trace]ExtractTo  ,Exist so,---pathtmp:%s,buf:%s",pathtmp.c_str(),buf);
		}
#endif
		CCLOG("[trace]ExtractTo  .....");
		folder = UpdateHelper::GetPath(pathtmp.c_str());
		CCLOG("[trace]ExtractTo  ,folder:%s",folder.c_str());
        if (folder != lastFolder) {
            UpdateHelper::MakeDirs(folder.c_str());
        }

        //neil add check
        if(buf[strlen(buf) - 1] == '/') {

            CCLOG("[warn]ExtractTo, dir: %s,created but do not open.",  buf);
            ret = unzGoToNextFile(m_file);
            continue;
        }
        //CCLOG("[trace]ExtractTo, Get folder %s", folder.c_str());
        CCLOG("[trace]ExtractTo, file %s", buf);
		


		fs = fopen(pathtmp.c_str(), "wb");
		CCLOG("[trace]file,100");

        if (fs == NULL) {
			CCLOG("[error]ExtractTo, Fail to open file %s", pathtmp.c_str());
            retval = -1;
            goto HANDLE_FILE_ERROR;
        }
        while (true) {
            ret = unzReadCurrentFile(m_file, buf, 16*1024);
            if (ret > 0) {
                fwrite(buf, ret, 1, fs);
            } else {
                break;
            } 
        }
        if (ret == 0) {
			CCLOG("[trace]ExtractTo, finish extract file %s", pathtmp.c_str());
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32) 
            //chmod(pathtmp, 755); //全加吧
            chmod(pathtmp.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH  );
#endif
        } else {
            retval = -1;
        }
        fclose(fs);
HANDLE_FILE_ERROR:
        unzCloseCurrentFile(m_file); 
        if (ret == UNZ_OK) {
            ret = unzGoToNextFile(m_file);
        }
    }
    return retval;
}

//////////////////////////

std::string UpdateHelper::GetPath(const char * filepath) {
    const char * begin = filepath;
    const char * end = filepath;
    while (*end != '\0') {
        if (*end == FILE_TOKEN) {
            begin = end;
        }
        end++;
    }
    return std::string(filepath, begin-filepath);
}


std::string UpdateHelper::ConvertWinPath(const string& str) {
    //将str路径中的斜杠替换成反斜杠
    //str.replace(str.begin(), str.end(), )
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) 
    string tmpStr = str;
    std::replace(tmpStr.begin(), tmpStr.end(), '/', '\\');
    return tmpStr;
#endif
    return str;
}



size_t UpdateHelper::saveDataToFD(char *ptr, size_t size, size_t nmemb, void *userdata) {
    FILE * file = (FILE*)userdata;
    return fwrite(ptr, size, nmemb, file);
}


size_t UpdateHelper::saveData(char *ptr, size_t size, size_t nmemb, void *userdata) {
    TempFile * file = (TempFile*) userdata;
    if (file != NULL) {
        size_t n = file->Write(ptr, size, nmemb);
        if (n != size * nmemb) {
            CCLOG("Fail to serialize to local disk");
        }
        //CCLOG("[trace]saveData,Write %d byte data", (int)n);
        return n;
    }
    return 0;
}

void UpdateHelper::HexMd5(const unsigned char * input, char * output) {
    static const char T[] = "0123456789ABCDEF";
    for (size_t i = 0; i < 16; ++i) {
        //sprintf(output+i*2, "%02x", input[i]);
        output[i*2+1] = T[input[i] & 0x0F];
        output[i*2] = T[(input[i]>>4) & 0x0F];
    }
}

int UpdateHelper::MakeDirs(const char * path) {
    const char * p = path;
    const char * p1 = path;
    vector<string> stack;
    if (*p == '/') {
        stack.push_back("/");
        p++;
    }
    while (*p != '\0') {
        while (*p1 != '/' && *p1 != '\0') {
            p1++;
        }
        if (p < p1) {
            string val = string(p, p1-p);
            if (val == "..") {
                stack.pop_back();
            } else if (val == ".") {

            } else {
                stack.push_back(val);
            }
        }
        if (*p1 != '\0') {
            p = ++p1;
        } else {
            break;
        }
    }
    if (stack.size() == 0) {
        stack.push_back(".");
    }
    std::string checkPath;
    checkPath.reserve(1024);
    size_t i = 0;
    if (stack[0] == "/") {
        checkPath += "/";
        i = 1;
    } else {
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)  
        //linux
        checkPath += ".";
#endif
    }
    struct stat tmpstat;
    for (; i < stack.size(); ++i) {
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)  
        //linux
        checkPath += "/";
#else
        //win32
		if (i != 0){
			checkPath += "\\"; //neilwu mod   late modify by weihua
		}
#endif
        checkPath += stack[i];
        if (stat(checkPath.c_str(), &tmpstat) == 0) {
            if (!(tmpstat.st_mode & S_IFDIR)) {
                CCLOG("expect directory %s", checkPath.c_str());
                return -1;
            }
        } else {
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)  
            //linux
            DIR *pDir = NULL;  
            pDir = opendir(checkPath.c_str());  
            if (!pDir)  
            {  
                if (mkdir(checkPath.c_str(), 0777) != 0) {
                    CCLOG("[error]MakeDirs,Fail to create directory %s", checkPath.c_str());
                    return -1;
                } else {
                    CCLOG("[trace]create dir %s", checkPath.c_str());
                }
            } 
#else
            if ((GetFileAttributesA(checkPath.c_str())) == INVALID_FILE_ATTRIBUTES)  
            {  
                int isSuccess = CreateDirectoryA(checkPath.c_str(), 0);  
                if(!isSuccess) {
                    CCLOG("[error]MakeDirs,fail to make dir,ret:%d,dir=%s", GetLastError(), checkPath.c_str());
                } else {
                    CCLOG("[trace]create dir %s", checkPath.c_str());
                }
            } 
#endif 
        }
    }
    return 0;
}


int UpdateHelper::RemoveDir(const char *path)
{
    CCLOG("[trace]UpdateHelper::RemoveDir,[%s]", path);
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)  
    //linux
    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    int r = -1;

    if (d)
    {
        struct dirent *p;

        r = 0;

        while (!r && (p=readdir(d)))
        {
            int r2 = -1;
            char *buf;
            size_t len;

            /* Skip the names "." and ".." as we don't want to recurse on them. */
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
            {
                continue;
            }

            len = path_len + strlen(p->d_name) + 2; 
            buf = (char*)malloc(len);

            if (buf)
            {
                struct stat statbuf;

                snprintf(buf, len, "%s/%s", path, p->d_name);

                if (!stat(buf, &statbuf))
                {
                    if (S_ISDIR(statbuf.st_mode))
                    {
                        r2 = RemoveDir(buf);
                    }
                    else
                    {
                        r2 = unlink(buf);
                    }
                }

                free(buf);
            }

            r = r2;
        }

        closedir(d);
    }

    if (!r)
    {
        r = rmdir(path);
    }
    return r;
#endif
    CCLOG("[trace]UpdateHelper::RemoveDir,done.[%s]", path);
    return 0;
}




void UpdateHelper::IntToString(std::string &out, const int value)
{
	char buf[32];
	snprintf(buf, sizeof (buf), "%d", value);  // snprintf is thread safe #include <stdio.h>
	out.append(buf);
}
