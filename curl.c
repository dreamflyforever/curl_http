/*------------------------------load_date--------------------------------------------

 *

 *func:receive the date from the server and save it to the userp

 *@param userp :the buffer used to save the date receive from server

 *return the size of the string receive

 --------------------------------------------------*/

size_t load_date( void *buffer, size_t size, size_t nmemb, char *userp ) 
{
	//处理接收到数据的回调函数，其中buffer就是	curl_easy_setopt(curl,CURLOPT_WRITEDATA,
	//wr_buf); 这句传进来的参数wr_buf用来保存数据

	int  wr_index=0; 

	size_t segsize = size * nmemb; 

	if ( wr_index + segsize > MAX_BUF ) { 

		userp[0] = 0; 

		return 0; 

	} 



	/* copy the data from the curl buffer into our buffer */ 

	memcpy( static_cast<void *>(&userp[wr_index]), buffer, segsize ); 



	/* update the write index */ 

	wr_index += static_cast<int>(segsize); 



	/* null terminate the buffer */ 

	userp[wr_index] = 0;

	printf("%s",userp);

	/* return the number of bytes received, indicating to curl that all is okay */ 

	return segsize; 

} 



/*------------------------------load_header--------------------------------------------

 *

 *func:receive the header date from the server and save it to the file stream

 *@param stream :the temp file used to save the header date receive from the server

 *return the size of the string receive--------------------------------------------------*/
size_t load_header( void *buffer, size_t size, size_t nmemb, FILE *stream ) 
{ //这个是处理接收到的HTTP头的处理回调函数我这里是把头写到文件当中

	return fwrite(buffer,size,nmemb,stream);
} 

/*------------------------------send_https--------------------------------------------
 *
 *@func:encapsulate the package and send it to the server 
 *@param url :the server address
 *@param http_headers :the header of the request
 *@param poststring :the post parameter of the request
 *@param timeout :the timeout
 *@return the error code
 --------------------------------------------------*/
int send_https(const char *url, curl_slist *http_headers, const char *poststring,unsigned long timeout,char * wr_buf)
{
	CURLcode ret;
	int error_code=SUCEED_OK;
	static const char *headerfilename = "head.out";  
	FILE *headerfile;
	headerfile = fopen(headerfilename,"wb");
	if (headerfile == NULL) {
		return -1;
	}

	if(!curl)  return ERROR_CURL_INITIALFALIED;
	//调置HTTP请求包的头，这里当参数传入 
	curl_easy_setopt(curl,CURLOPT_HTTPHEADER,http_headers);
	//开启OPENSSL通道，允许接收HTTPS协议 
	curl_easy_setopt(curl,CURLOPT_SSL_VERIFYPEER,0L);
	curl_easy_setopt(curl,CURLOPT_SSL_VERIFYHOST,0L);	
	//设置要访问的URL 
	curl_easy_setopt(curl,CURLOPT_URL, url);
	//设置要post的数据 
	curl_easy_setopt(curl,CURLOPT_POSTFIELDS,poststring);
	//设置连接超时	
	curl_easy_setopt(curl,CURLOPT_CONNECTTIMEOUT,timeout);
	//设置数据接收缓冲区，这个要看回调函数处理 
	curl_easy_setopt(curl,CURLOPT_WRITEDATA, wr_buf); 
	//设置数据接收回调函数 
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,load_date); 
	//设置HTTP头数据处理回调函数 
	curl_easy_setopt(curl,CURLOPT_HEADERFUNCTION,load_header);
	//设置HTTP头数据接收缓冲，这里是文件 
	curl_easy_setopt(curl,CURLOPT_WRITEHEADER,headerfile);
	ret = curl_easy_perform(curl);
	if ( ret == 0 ) 
	{
		int error_code=SUCEED_OK;
	}
	else 
	{
		switch(ret)
		{
			case CURLE_COULDNT_CONNECT:
				{
					printf("ERROR:TimeOut can't connect to the host.\n");
					error_code=ERROR_CURL_TIMEOUT;
					break;
				}
			case CURLE_HTTP_RETURNED_ERROR:
				{
					printf("ERROR:HTTP return false.");
					error_code=ERROR_CURL_HTTPFALSE;
					break;
				}
			case CURLE_SSL_ENGINE_INITFAILED:
				{
					printf("ERROR:SSL can't be initialized.");
					error_code=ERROR_CURL_SSLINITFAILED;
					break;
				}
			case CURLE_COULDNT_RESOLVE_HOST:
				{
					printf("ERROR:SSL can't create ssl connection.");
					error_code=ERROR_CURL_SSLINITFAILED;
					break;
				}
			default:
				{
					printf("ERROR:correspond failed.");
					error_code=ERROR_FAILED_SEND;
				}
		}
	}
	fclose(headerfile);
	if(error_code == SUCEED_OK)
		error_code =parase_rev(wr_buf);//这一句是处理接收到的数据 
	strCheckSum = getChecksum(strUserID.c_str(),strPID.c_str(),strFirmStr.c_str());	

	return error_code;
}
int main()
{
	char wr_buf[65530];
	//初始化HTTP请求头 
	curl_slist *headers=NULL; /* init to NULL is important */
	headers = curl_slist_append(headers, "Accept-Language: zh-cn");
	headers = curl_slist_append(headers, "User-Agent: Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.30)");
	headers = curl_slist_append(headers, "Content-Type: text/xml");
	headers = curl_slist_append(headers, "Connection:");
	send_https("www.baidu.com",headers,"eeeeeeeeeee",1000,wr_buf);
}
