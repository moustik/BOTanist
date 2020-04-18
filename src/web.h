#ifndef BOTA_WEB_H
#define BOTA_WEB_H

#include <string>

#include <curl/curl.h>


/** @fn cURLWriteFunction
 *  @brief fonction de callback permettant a cURL d'ecrire dans une std:string
 *
 */
size_t cURLWriteFunction(void *ptr, size_t size, size_t nmemb, std::string* data);

/** @fn curl_get
 *  @brief recupere la reponse d'une requete GET http
 *  @param request_url requette HTTP formattee pour le web
 *
 */
std::string curl_get(std::string request_url, std::string data = "");

#endif
