/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2013, Daniel Stenberg, <daniel@haxx.se>, et al.
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
/* Stream-parse a document using the streaming Expat parser.
 * Written by David Strauss
 *
 * Expat => http://www.libexpat.org/
 *
 * gcc -Wall -I/usr/local/include xmlstream.c -lcurl -lexpat -o xmlstream
 *
 */

#include "xmlparser.h"
#include "aliqr.h"
char stqrcode[QRRESULTSTR]={0};
char timemark[32]={0};
char qrout_trade_no[65]={0};

void startElement(void *userData, const XML_Char *name, const XML_Char **atts)
{
  struct ParserStruct *state = (struct ParserStruct *) userData;
  state->tags++;
  state->depth++;

  /* Get a clean slate for reading in character data. */
  free(state->characters.memory);
  state->characters.memory = NULL;
  state->characters.size = 0;
}

void characterDataHandler(void *userData, const XML_Char *s, int len)
{
  struct ParserStruct *state = (struct ParserStruct *) userData;
  struct MemoryStruct *mem = &state->characters;

  mem->memory = realloc(mem->memory, mem->size + len + 1);
  if(mem->memory == NULL) {
    /* Out of memory. */
    fprintf(stderr, "Not enough memory (realloc returned NULL).\n");
    state->ok = 0;
    return;
  }

  memcpy(&(mem->memory[mem->size]), s, len);
  mem->size += len;
  mem->memory[mem->size] = 0;
}

void endElement(void *userData, const XML_Char *name)
{
    struct ParserStruct *state = (struct ParserStruct *) userData;
    state->depth--;
   // printf("%5lu    %5lu   %10lu   %s %s\n",state->tags, state->depth, state->characters.size, name, state->characters.memory);
    if( strcmp(name,"result") == 0) {
        //get the qrout_trade_no should be sync with server and request
        if(state->tags == 11) {
            memcpy(qrout_trade_no,state->characters.memory,state->characters.size);
            printf("qrout_trade_no:%5lu   %10lu   %s %s\n", state->depth, state->characters.size, name, state->characters.memory);
        }
        //get the qrcode 
        if(strstr(state->characters.memory, "qr.alipay.com")) {
            memcpy(stqrcode,state->characters.memory,state->characters.size);
            printf("qrgenerate:%5lu   %10lu   %s %s\n", state->depth, state->characters.size, name, state->characters.memory);
        }
    }
}

void endElement1(void *userData, const XML_Char *name)
{
    struct ParserStruct *state = (struct ParserStruct *) userData;
    state->depth--;
    if( strcmp(name,"response") == 0){
        if(strstr(state->characters.memory, "TRADE_SUCCESS")) {
            memset(stqrcode,0, sizeof(stqrcode));
            memcpy(stqrcode,state->characters.memory,state->characters.size);
            printf("%5lu   %10lu   %s %s\n", state->depth, state->characters.size, name, state->characters.memory);
        }
        else
            strcpy(stqrcode,"TRADE_FAILURE");
  }
}

#if 1
void endElement2(void *userData, const XML_Char *name)
{
    struct ParserStruct *state = (struct ParserStruct *) userData;
    state->depth--;
    if( strcmp(name,"time_mark") == 0){
        if(state->characters.memory != NULL) {
            memcpy(timemark,state->characters.memory,state->characters.size);
            printf("%5lu   %10lu   %s %s\n", state->depth, state->characters.size, name, state->characters.memory);
        }
    }
    if( strcmp(name,"order") == 0){
        if(state->characters.memory != NULL) {
            memset(stqrcode,0, sizeof(stqrcode));
            memcpy(stqrcode,state->characters.memory,state->characters.size);
            printf("%5lu   %10lu   %s %s\n", state->depth, state->characters.size, name, state->characters.memory);
        }
    }
}
#endif

void endElement3(void *userData, const XML_Char *name)
{
    struct ParserStruct *state = (struct ParserStruct *) userData;
    state->depth--;
    if(strcmp(name,"result") == 0){
        if(state->characters.memory != NULL&&strstr(state->characters.memory, "TRADE_SUCCESS")) { 
            memset(stqrcode,0, sizeof(stqrcode));
            memcpy(stqrcode,state->characters.memory,state->characters.size);
            printf("%5lu   %10lu   %s %s\n", state->depth, state->characters.size, name, state->characters.memory);
        }else {
            strcpy(stqrcode,"TRADE_FAILURE");
        }
    }
}

size_t parseStreamCallback(void *contents, size_t length, size_t nmemb, void *userp)
{
  XML_Parser parser = (XML_Parser) userp;
  size_t real_size = length * nmemb;
  struct ParserStruct *state = (struct ParserStruct *) XML_GetUserData(parser);

  /* Only parse if we're not already in a failure state. */
  if (state->ok && XML_Parse(parser, contents, real_size, 0) == 0) {
    int error_code = XML_GetErrorCode(parser);
    fprintf(stderr, "Parsing response buffer of length %lu failed with error code %d (%s).\n",
            real_size, error_code, XML_ErrorString(error_code));
    state->ok = 0;
  }

  return real_size;
}
