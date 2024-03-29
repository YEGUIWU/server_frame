/**
 * @file httpclient_parser.h
 * @brief ragel 生成的httpclient_parser
 * @author YeGuiWu
 * @email yeguiwu@qq.com
 * @version 1.0
 * @date 2022-04-14
 * @copyright Copyright (c) 2020年 guiwu.ye All rights reserved www.yeguiwu.xyz
 */

#ifndef httpclient_parser_h
#define httpclient_parser_h

#include "http11_common.h"

typedef struct httpclient_parser { 
  int cs;
  size_t body_start;
  int content_len;
  int status;
  int chunked;
  int chunks_done;
  int close;
  size_t nread;
  size_t mark;
  size_t field_start;
  size_t field_len;

  void *data;

  field_cb http_field;
  element_cb reason_phrase;
  element_cb status_code;
  element_cb chunk_size;
  element_cb http_version;
  element_cb header_done;
  element_cb last_chunk;
  
  
} httpclient_parser;
typedef httpclient_parser HttpClientParser;
int httpclient_parser_init(httpclient_parser *parser);
int httpclient_parser_finish(httpclient_parser *parser);
int httpclient_parser_execute(httpclient_parser *parser, const char *data, size_t len, size_t off);
int httpclient_parser_has_error(httpclient_parser *parser);
int httpclient_parser_is_finished(httpclient_parser *parser);

#define httpclient_parser_nread(parser) (parser)->nread 

#endif
