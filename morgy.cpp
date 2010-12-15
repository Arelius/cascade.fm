#include <curl/curl.h>
#include <assert.h>
#include "utf.h"

size_t received_data(void* ptr, size_t size, size_t nmemb, void* userdata)
{
    
}

void morgy_upload_file(const char* hash, const wchar* file_name)
{
    CURL* curl = curl_easy_init();
    assert(curl);

    curl_easy_setopt(curl, CURLOPT_URL, "http://morgy.arelius.com/upload");
    //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, received_data);
    //curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
    CURLcode r = curl_easy_perform(curl);

    assert(r == CURLE_OK);

    //r = curl_easy_recv(curl, 

    curl_easy_cleanup(curl);
}
