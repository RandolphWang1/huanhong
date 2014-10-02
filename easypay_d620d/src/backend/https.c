/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2012, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/
#include <stdio.h>
#include "../include/curl/curl.h"
#include "aliqr.h"
#include "xmlparser.h"
extern char stqrcode[];
extern char timemark[];
extern char *str_timemark;
extern char time_mark[32];

int alipay_main(struct qr_result *query_result, struct payInfo* order_info, int order_type)
{
  CURL *curl;
  CURLcode res;
  char https_req[1024*3];
  int req_len = 0;

  XML_Parser parser;
  struct ParserStruct state;

  /* Initialize the state structure for parsing. */
  memset(&state, 0, sizeof(struct ParserStruct));
  state.ok = 1;

  /* Initialize a namespace-aware parser. */
  parser = XML_ParserCreateNS(NULL, '\0');
  XML_SetUserData(parser, &state);
  if (order_type == ALI_PRECREATE_ORDER)
  XML_SetElementHandler(parser, startElement, endElement);
  else if (order_type == ALI_PRECREATE_QUERY)
  XML_SetElementHandler(parser, startElement, endElement2);
  XML_SetCharacterDataHandler(parser, characterDataHandler);

  curl_global_init(CURL_GLOBAL_DEFAULT);

  curl = curl_easy_init();
  if(curl) {
    alipay_precreate(https_req, &req_len, order_info, order_type);
    printf("https_req:\n%s\n, len:%d\n", https_req, req_len);
    curl_easy_setopt(curl, CURLOPT_URL, https_req);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, parseStreamCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)parser);

#if 1 //def SKIP_PEER_VERIFICATION
    /*
     * If you want to connect to a site who isn't using a certificate that is
     * signed by one of the certs in the CA bundle you have, you can skip the
     * verification of the server's certificate. This makes the connection
     * A LOT LESS SECURE.
     *
     * If you have a CA cert for the server stored someplace else than in the
     * default bundle, then the CURLOPT_CAPATH option might come handy for
     * you.
     */
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#if 1//def SKIP_HOSTNAME_VERIFICATION
    /*
     * If the site you're connecting to uses a different host name that what
     * they have mentioned in their server certificate's commonName (or
     * subjectAltName) fields, libcurl will refuse to connect. You can skip
     * this check, but this will make the connection less secure.
     */
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    } else if (state.ok) {
        /* Expat requires one final call to finalize parsing. */
        if (XML_Parse(parser, NULL, 0, 1) == 0) {
            int error_code = XML_GetErrorCode(parser);
            fprintf(stderr, "Finalizing parsing failed with error code %d (%s).\n",
                    error_code, XML_ErrorString(error_code));
        }
        else {
            printf("                     --------------\n");
            printf("                     %lu tags total\n", state.tags);
            //memcpy(qr_result,stqrcode,strlen(stqrcode));
            if(stqrcode[0] != '\0' && time_mark[0] != '\0') {
            memcpy(query_result->qr_string,stqrcode,strlen(stqrcode));
            printf("the qr_result is %s\n, the stqrcode is %s\n",query_result->qr_string,stqrcode);
            }
            memset(stqrcode,0,QRLEN);
            memcpy(query_result->time_mark,timemark,strlen(timemark));
        }

    }
    /* Clean up. */
    free(state.characters.memory);
    XML_ParserFree(parser);
    /* always cleanup */
    curl_easy_cleanup(curl);
  }

  curl_global_cleanup();

  return 0;
}
