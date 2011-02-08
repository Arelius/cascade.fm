#include <curl/curl.h>
#include <assert.h>
#include "utf.h"
#include "database.h"

struct post_field
{
    const char* field;
    const wchar* data;
    post_field(const char* f, const wchar* d) : field(f), data(d) {}
};


// This needs to properlly encode the strings!!
// curl_easy_escape!!
char* alloc_post_data(post_field* fields)
{
    size_t post_len = -1;

    {
        post_field* curr = fields;
        while(curr->field != NULL)
        {
            post_len += strlen(curr->field);
            post_len += utf16_to_utf8(NULL, 0, curr->data);
            post_len += 2; // +:&=
            curr++;
        }
        post_len++; // \0
    }

    char* post_buf = (char*)malloc(post_len);
    
    {
        post_field* curr = fields;
        char* curr_buf = post_buf;
        while(curr->field != NULL)
        {
            if(curr_buf != post_buf)
                *(curr_buf++) = '&';

            strcpy(curr_buf, curr->field);
            curr_buf += strlen(curr->field);

            *(curr_buf++) = '=';

            curr_buf +=
                utf16_to_utf8(curr_buf,
                              post_len -
                              (curr_buf -
                               post_buf),
                              curr->data);

            curr++;

            assert(curr_buf-post_buf <= post_len);
        }
    }

    return post_buf;
}

size_t received_data(void* ptr, size_t size, size_t nmemb, void* userdata)
{

    return size;
}

void morgy_upload_file(const char* hash, const wchar* file_name, const wchar* sessionid)
{
    post_fields fields[] =
        { post_field("SESSION", sessionid),
          post_field("HASH", hash),
          post_field("LOCALPATH", file_name);
          post_field(NULL, NULL) };

    char* post_data = alloc_post_data(fields);

    FILE* file = fopen(file_name, "rb");
    assert(file);

    CURL* curl = curl_easy_init();
    assert(curl);

    curl_easy_setopt(curl, CURLOPT_URL, "http://morgy.arelius.com:8085/upload");
    //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, received_data);
    //curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
    CURLcode r = curl_easy_perform(curl);

    assert(r == CURLE_OK);

    //r = curl_easy_recv(curl, 

    curl_easy_cleanup(curl);

    free(post_data);
}

size_t morgy_login_recieve(void* ptr, size_t size, size_t nmemb, char** userdata)
{
    char* sessionid = new char[(size*nmemb)+1];
    memcpy(sessionid, ptr, size*nmemb);
    sessionid[(size*nmemb)+1] = '\0';
    *userdata = sessionid;
    return size*nmemb;
}

bool morgy_login(const wchar* username, const wchar* password, database* db)
{
    post_field fields[] =
        { post_field("USER", username),
          post_field("PASS", password),
          post_field("KEY", L"ret"),
          post_field(NULL, NULL) };

    char* post_data = alloc_post_data(fields);

    CURL* curl = curl_easy_init();
    assert(curl);
    curl_easy_setopt(curl, CURLOPT_URL, "http://morgy.arelius.com:8085/login");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, morgy_login_recieve);
    char* sessionid;
    sessionid = (char*)3;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &sessionid);

    CURLcode r = curl_easy_perform(curl);

    db_set_session_id(db, sessionid);

    delete [] sessionid;

    if(r != CURLE_OK)
    {
        printf("CURL Error Code:%d\n", r);
        return false;
    }

    curl_easy_cleanup(curl);

    free(post_data);

    return true;
}
