/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2011, Daniel Stenberg, <daniel@haxx.se>, et al.
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
/* An example source code that issues a HTTP POST and we provide the actual
 * data through a read callback.
 */ 
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

const char data[] = "[ \'name\': \'James Bond\" \"id\": 20489217\" ]";
const char user_agent[] = "MI6VPN/1.3";
const char login[] = "jmesbond:goldfinger";

struct WriteThis {
  const char *readptr;
  int sizeleft;
};

static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
  struct WriteThis *pooh = (struct WriteThis *)userp;

  if(pooh->sizeleft) {
    *(char *)ptr = pooh->readptr[0]; /* copy one single byte */ 
    pooh->readptr++;                 /* advance pointer */ 
    pooh->sizeleft--;                /* less data left */ 
    return 1;                        /* we return 1 byte at a time! */ 
  }

  return 0;                          /* no more data left to deliver */ 
}

int main(int argc, const char* argv[])
{
	if (argc != 2) {
		printf("Usage: secret_agent_client http://host_ip:port\n\n");
		return 0;
	}
	
  CURL *curl;
  CURLcode res;

  struct WriteThis pooh;

  pooh.readptr = data;
  pooh.sizeleft = strlen(data);

  curl = curl_easy_init();
  if(curl) {
    /* First set the URL that is about to receive our POST. */ 
		
		char url[1024];
		sprintf(url, "%s/agents/login.json", argv[1]);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		
		/* HTTP Basic auth */
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
		curl_easy_setopt(curl, CURLOPT_USERPWD, login);

    /* Now specify we want to POST data */ 
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    /* we want to use our own read function */ 
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);

    /* pointer to pass to our read function */ 
    curl_easy_setopt(curl, CURLOPT_READDATA, &pooh);

    /* get verbose debug output please */ 
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);

		curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent);
		
		struct curl_slist *headers=NULL; // init to NULL is important
		headers = curl_slist_append(headers, "Accept: application/json"); 
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		
    /*
      If you use POST to a HTTP 1.1 server, you can send data without knowing
      the size before starting the POST if you use chunked encoding. You
      enable this by adding a header like "Transfer-Encoding: chunked" with
      CURLOPT_HTTPHEADER. With HTTP 1.0 or without chunked transfer, you must
      specify the size in the request.
    */ 
#ifdef USE_CHUNKED
    {
      struct curl_slist *chunk = NULL;

      chunk = curl_slist_append(chunk, "Transfer-Encoding: chunked");
      res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
      /* use curl_slist_free_all() after the *perform() call to free this
         list again */ 
    }
#else
    /* Set the expected POST size. If you want to POST large amounts of data,
       consider CURLOPT_POSTFIELDSIZE_LARGE */ 
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (curl_off_t)pooh.sizeleft);
#endif

#ifdef DISABLE_EXPECT
    /*
      Using POST with HTTP 1.1 implies the use of a "Expect: 100-continue"
      header.  You can disable this header with CURLOPT_HTTPHEADER as usual.
      NOTE: if you want chunked transfer too, you need to combine these two
      since you can only set one list of headers with CURLOPT_HTTPHEADER. */ 

    /* A less good option would be to enforce HTTP 1.0, but that might also
       have other implications. */ 
    {
      struct curl_slist *chunk = NULL;

      chunk = curl_slist_append(chunk, "Expect:");
      res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
      /* use curl_slist_free_all() after the *perform() call to free this
         list again */ 
    }
#endif

    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);

    /* always cleanup */ 
    curl_easy_cleanup(curl);

		long http_code = 0;
		curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
		printf("\n\nResponse: HTTP %ld\n\n", http_code);


  }
  return 0;
}