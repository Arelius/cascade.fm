#include <curl/curl.h>
#include <assert.h>
#include "utf.h"
#include "database.h"
#include "scan.h"

struct free_handler
{
    void* data;
    free_handler(void* d) : data(d) {}
    ~free_handler() { free(data); };
private:
    free_handler(free_handler&) {}
};

#define scope_free(x) free_handler _##x##_## __LINE__(x)

char* mstrcat(const char* l, const char* r)
{
    char* buffer = (char*)malloc((strlen(l) + strlen(r) + 1) * sizeof(char));

    strcpy(buffer, l);
    strcat(buffer, r);

    return buffer;
}

bool audiobox_check_exists_hash(const char* hash, const wchar* userpass)
{
    CURL* curl = curl_easy_init();
    assert(curl);

    char* userpass_8 = utf_16_to_8(userpass);
    scope_free(userpass_8);
    char* url = mstrcat("http://audiobox.fm/api/tracks/", hash);
    scope_free(url);

    curl_easy_setopt(curl, CURLOPT_VERBOSE, TRUE);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_USERPWD, userpass_8);

    curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);

    CURLcode r = curl_easy_perform(curl);

    curl_easy_cleanup(curl);

    //FIXME: all errors will return a file not found.
    return r == CURLE_OK;
}

void audiobox_upload_file(const wchar* file_name, const wchar* userpass)
{

    // Bail early if already uploaded.
    char hash[Hash_Buffer_Len];
    hash_file(file_name, hash);
    if(audiobox_check_exists_hash(hash, userpass))
        return;

    CURL* curl = curl_easy_init();
    assert(curl);

    struct curl_httppost* post = NULL;
    struct curl_httppost* last = NULL;

    char* file_name_8 = utf_16_to_8(file_name);
    char* userpass_8 = utf_16_to_8(userpass);
    scope_free(file_name_8);
    scope_free(userpass_8);

    curl_formadd(&post, &last, CURLFORM_COPYNAME, "media",
                 CURLFORM_FILE, file_name_8, CURLFORM_END);

    curl_easy_setopt(curl, CURLOPT_VERBOSE, TRUE);
    curl_easy_setopt(curl, CURLOPT_URL, "http://audiobox.fm/api/tracks");
    curl_easy_setopt(curl, CURLOPT_USERPWD, userpass_8);
    curl_easy_setopt(curl, CURLOPT_POST, TRUE);
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);

    CURLcode r = curl_easy_perform(curl);
    if(r != CURLE_OK)
    {
        printf("Curl Error: %s\n", curl_easy_strerror(r));
    }

    //r = curl_easy_recv(curl, 

    curl_easy_cleanup(curl);   
}
