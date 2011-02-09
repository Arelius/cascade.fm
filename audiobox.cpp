#include <curl/curl.h>
#include <assert.h>
#include "utf.h"
#include "database.h"

void audiobox_upload_file(const char* hash, const wchar* file_name, const wchar* userpass)
{
    //FILE* file = fopen(file_name, "rb");
    //assert(file);

    CURL* curl = curl_easy_init();
    assert(curl);

    struct curl_httppost* post = NULL;
    struct curl_httppost* last = NULL;

    char* file_name_8 = utf_16_to_8(file_name);
    char* userpass_8 = utf_16_to_8(userpass);

    wprintf(L"filename16 %s\nuserpass16 %s\n", file_name, userpass);
    printf("filename %s\nuserpass %s\n", file_name_8, userpass_8);

    curl_formadd(&post, &last, CURLFORM_COPYNAME, "media",
                 CURLFORM_FILE, file_name_8, CURLFORM_END);

    curl_easy_setopt(curl, CURLOPT_VERBOSE, TRUE);
    curl_easy_setopt(curl, CURLOPT_URL, "http://audiobox.fm/api/tracks");
    //curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_USERPWD, userpass_8);
    curl_easy_setopt(curl, CURLOPT_POST, TRUE);
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);

    CURLcode r = curl_easy_perform(curl);
    if(r != CURLE_OK)
    {
        printf("Curl Error: %s\n", curl_easy_strerror(r));
    }

    free(userpass_8);
    free(file_name_8);

    //r = curl_easy_recv(curl, 

    curl_easy_cleanup(curl);   
}
