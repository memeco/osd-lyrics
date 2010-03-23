#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <glib.h>
#include <unistd.h>
#include "ol_lrc_parser.h"
#include "ol_utils.h"
#include "chardetect.h"
#include "ol_debug.h"

struct OlLrcParser
{
  char *buffer;
  char *filename;
  size_t offset;
  size_t buflen;
  chardet_t *charset;
};

static void _free_text_token (struct OlLrcTextToken *token);
static void _free_attr_token (struct OlLrcAttrToken *token);
static void _free_time_token (struct OlLrcTimeToken *token);
static union OlLrcToken *_parse_text (struct OlLrcParser *parser,
                                      size_t begin, size_t end);
/** 
 * @brief Parse the text between `[' and `]'
 *
 * There may be two types of the tag. One is the attribute tag, in the form of
 * `[attr:value]'. Another is the time tag, in the from of `[hh:mm:ss.ms]'.
 * This function will return an OlLrcAttrToken or OlLrcTimeToken according to
 * the form of the tag.
 *
 * @param parser A parser
 * @param begin The position of the '[' character in the buffer of the parser
 * @param end The position right after the ']' character in the buffer of ther parser
 * 
 * @return An OlLrcAttrToken if it is an attribute tag, or an OlLrcTimeToken if
 *         it is a time tag
 */
static union OlLrcToken *_parse_tag (struct OlLrcParser *parser,
                                     size_t begin, size_t end);
static union OlLrcToken *
_parse_text (struct OlLrcParser *parser,
             size_t begin, size_t end)
{
  struct OlLrcTextToken *token = g_new (struct OlLrcTextToken, 1);
  token->type = OL_LRC_TOKEN_TEXT;
  token->text = g_strndup (parser->buffer + begin, end - begin);
  return (union OlLrcToken*) token;
}

static union OlLrcToken *
_parse_tag (struct OlLrcParser *parser,
            size_t begin, size_t end)
{
  int istime = 1;
  size_t i;
  begin++; end--;
  for (i = begin; i < end; i++)
  {
    if (!isdigit (parser->buffer[i]) &&
        parser->buffer[i] != ':' &&
        parser->buffer[i] != '.')
    {
      istime = 0;
      break;
    }
  }
  if (istime)
  {
    struct OlLrcTimeToken *token = g_new (struct OlLrcTimeToken, 1);
    token->type = OL_LRC_TOKEN_TIME;
    double timepar[3] = {0.0, 0.0, 0.0}; /* hour, min, sec */
    i = end - 1;
    int idx = 0;
    while (i >= begin && idx < 3)
    {
      while (i > begin && parser->buffer[i - 1] != ':')
        i--;
      sscanf (parser->buffer + i, "%lf", &timepar[idx]);
      i -= 2;
      idx++;
    }
    token->time = ((timepar[2]*60+timepar[1])*60+timepar[0])*1000;
    return (union OlLrcToken*)token;
  }
  else
  {
    struct OlLrcAttrToken *token = g_new (struct OlLrcAttrToken, 1);
    token->type = OL_LRC_TOKEN_ATTR;
    for (i = begin; i < end; i++)
    {
      if (parser->buffer[i] == ':')
        break;
    }
    token->attr = g_strndup (parser->buffer + begin,
                             i - begin);
    if (i < end)
      token->value = g_strndup (parser->buffer + i + 1,
                                end - i - 1);
    else
      token->value = NULL;
    return (union OlLrcToken*)token;
  }
}

static void
_free_text_token (struct OlLrcTextToken *token)
{
  ol_assert (token != NULL);
  if (token->text != NULL)
    g_free (token->text);
  g_free (token);
}

static void
_free_attr_token (struct OlLrcAttrToken *token)
{
  ol_assert (token != NULL);
  if (token->attr != NULL)
    g_free (token->attr);
  if (token->value != NULL)
    g_free (token->value);
  g_free (token);
}

static void
_free_time_token (struct OlLrcTimeToken *token)
{
  ol_assert (token != NULL);
  g_free (token);
}

struct OlLrcParser *
ol_lrc_parser_new ()
{
  struct OlLrcParser *ret = g_new0 (struct OlLrcParser, 1);
  return ret;
}

struct OlLrcParser *
ol_lrc_parser_new_from_file (const char *filename)
{
  ol_assert_ret (filename != NULL, NULL);
  if (!ol_path_is_file (filename))
    return NULL;
  ssize_t len = ol_file_len (filename);
  if (len < 0)
    return NULL;
  FILE *flrc = fopen (filename, "rb");
  if (flrc == NULL)
    return NULL;
  char *buffer = g_new (char, len + 1);
  buffer[len] = '\0';
  size_t cnt = 0, curr = 0;
  while ((cnt = fread (buffer + curr, len - curr, sizeof (char), flrc)) > 0)
  {
    curr += cnt;
  }
  fclose (flrc);
  struct OlLrcParser *ret = ol_lrc_parser_new ();
  if (ret != NULL)
  {
    ol_lrc_parser_set_buffer (ret, buffer);
  }
  g_free (buffer);
  ret->filename = g_strdup (filename);
  return ret;
}

void
ol_lrc_parser_set_buffer (struct OlLrcParser *parser,
                          const char *buffer)
{
  /* TODO: detect charset and convert to UTF-8 */
  ol_assert (parser != NULL);
  if (parser->buffer != NULL)
    g_free (parser->buffer);
  if (buffer == NULL)
  {
    parser->buffer = NULL;
    parser->buflen = 0;
  }
  else
  {
    parser->buflen = strlen (buffer);
    parser->buffer = g_strdup (buffer);
  }
  ol_lrc_parser_reset (parser);
}

void
ol_lrc_parser_reset (struct OlLrcParser *parser)
{
  ol_assert (parser != NULL);
  parser->offset = 0;
}

union OlLrcToken *
ol_lrc_parser_next_token (struct OlLrcParser *parser)
{
  ol_assert_ret (parser != NULL, NULL);
  ol_assert_ret (parser->buffer != NULL, NULL);
  if (parser->offset == parser->buflen)
    return NULL;
  int tag = 0;
  int start = parser->offset;
  if (parser->buffer[parser->offset] == '[')
  {
    tag = 1;                    /* It may be a tag */
    parser->offset++;
  }
  while (parser->offset < parser->buflen)
  {
    if (tag && parser->buffer[parser->offset] == ']')
    {
      /* A tag found */
      tag = 2;
      parser->offset++;
      break;
    }
    else if (parser->buffer[parser->offset] == '\r' ||
             parser->buffer[parser->offset] == '\n')
    {
      parser->offset++;
      break;
    }
    parser->offset++;
  }
  if (tag == 2)
  {
    return _parse_tag (parser, start, parser->offset);
  }
  else
  {
    size_t end = parser->offset - 1;
    /* Deal with \r\n */
    if (parser->offset < parser->buflen &&
        parser->offset > 0 &&
        parser->buffer[parser->offset - 1] == '\r' &&
        parser->buffer[parser->offset] == '\n')
      parser->offset++;
    return _parse_text (parser, start, end);
  }
}

void
ol_lrc_parser_free (struct OlLrcParser *parser)
{
  ol_assert (parser != NULL);
  if (parser->buffer != NULL)
    g_free (parser->buffer);
  if (parser->filename != NULL)
    g_free (parser->filename);
  g_free (parser);
}

void
ol_lrc_token_free (union OlLrcToken *token)
{
  ol_assert (token != NULL);
  switch (ol_lrc_token_get_type (token))
  {
  case OL_LRC_TOKEN_TEXT:
    _free_text_token (&token->text);
    break;
  case OL_LRC_TOKEN_ATTR:
    _free_attr_token (&token->attr);
    break;
  case OL_LRC_TOKEN_TIME:
    _free_time_token (&token->time);
    break;
  default:
    ol_error ("Invalid token type");
    g_free (token);
    break;
  }
}

enum OlLrcTokenType
ol_lrc_token_get_type (union OlLrcToken *token)
{
  ol_assert_ret (token, OL_LRC_TOKEN_INVALID);
  if (token->type < OL_LRC_TOKEN_INVALID)
    return token->type;
  else
    return OL_LRC_TOKEN_INVALID;
}
