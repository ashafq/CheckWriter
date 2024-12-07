#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static const char *UNIT[] = {
  "zero",
  "one",
  "two",
  "three",
  "four",
  "five",
  "six",
  "seven",
  "eight",
  "nine",
  "ten",
  "eleven",
  "tweleve",
  "thirteen",
  "fourteen",
  "fifteen",
  "sixteen",
  "seventeen",
  "eighteen",
  "nineteen",
};

static const char *TENS[] = {
  "",
  "ten",
  "twenty",
  "thirty",
  "fourty",
  "fifty",
  "sixty",
  "seventy",
  "eighty",
  "ninety",
};

// Limit of uint32_t is ~4 Billion
static const char *WEIGHT[] = {
  "",
  "thousand",
  "million",
  "billion",
};

#define FAIL_EARLY(err)         \
  do                            \
    {                           \
      if (err < 0)              \
        {                       \
          goto fail_and_return; \
        }                       \
    }                           \
  while (0);

static uint32_t
eng_log10 (uint32_t num)
{
  return floor (log10 (num)) / 3;
}

static uint32_t
eng_power (uint32_t power)
{
  return pow (10.0, power * 3);
}

int
num_to_words (char *dst, size_t len, uint32_t num)
{
  if (!dst)
    {
      return -1;
    }

  char *const start = dst;
  char *const end = dst + len;
  char *cur = dst;
  uint32_t ex, val, msd, rem;
  int written = 0;

  memset (dst, 'x', len);

  if (num >= 1000)
    {
      ex = eng_log10 (num);
      val = eng_power (ex);

      msd = num / val;
      rem = num % val;

      written = num_to_words (cur, end - cur, msd);
      FAIL_EARLY (written)
      cur += written;

      written = snprintf (cur, end - cur, " %s", WEIGHT[ex]);
      FAIL_EARLY (written)
      cur += written;

      if (rem)
        {
          *(cur) = ' ';
          ++cur;
          written = num_to_words (cur, end - cur, rem);
          FAIL_EARLY (written)
          cur += written;
        }

      return cur - start;
    }
  else if (num >= 100)
    {
      msd = num / 100;
      rem = num % 100;

      written = snprintf (cur, end - cur, "%s hundred", UNIT[msd]);
      FAIL_EARLY (written);
      cur += written;

      if (rem)
        {
          *(cur) = ' ';
          ++cur;
          written = num_to_words (cur, end - cur, rem);
          FAIL_EARLY (written)
          cur += written;
        }

      return cur - start;
    }
  else if (num >= 20)
    {
      msd = num / 10;
      rem = num % 10;

      written = snprintf (cur, end - cur, "%s", TENS[msd]);
      FAIL_EARLY (written)
      cur += written;

      if (rem)
        {
          written = snprintf (cur, end - cur, "-%s", UNIT[rem]);
          FAIL_EARLY (written)
          cur += written;
        }

      return cur - start;
    }
  else
    {
      written = snprintf (cur, end - cur, "%s", UNIT[num]);
      return written;
    }

fail_and_return:
  return written;
}
