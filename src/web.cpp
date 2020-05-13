#include "web.h"

#include "log.h"


size_t cURLWriteFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*) ptr, size * nmemb);
    return size * nmemb;
}

std::string curl_get(std::string request_url, std::string data){
    // from https://gist.github.com/whoshuu/2dc858b8730079602044
    auto curl = curl_easy_init();
    CURLcode res;

    std::string response_string;
    std::string header_string;

    if (curl) {
        LOG(debug) << _format("fetching {}", request_url.c_str());
        curl_easy_setopt(curl, CURLOPT_URL, request_url.c_str());
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        //curl_easy_setopt(curl, CURLOPT_USERPWD, "user:pass");

        if( !data.empty() ) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length());
        }
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.42.0");
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cURLWriteFunction);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

        curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");

        char* url;
        long response_code;
        double elapsed;

        res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);

        LOG(debug) << header_string;
        LOG(debug) << _format("got {}: {} in {} ms", response_code, curl_easy_strerror(res), elapsed);
        //        LOG(debug) << _format("got {} in {} ms", response_code, elapsed);
        curl_easy_cleanup(curl);
        curl = NULL;
    }

    return response_string;
}
