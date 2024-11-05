#pragma once

#include <stdbool.h>
#include <string.h>

#define DEBUG_MODE 0

#ifndef min
int min(int x, int y) {
	return x < y ? x : y;
}
#endif

typedef enum {
  WAIT = 0,
  TURN = 1
} MoveNotice;

typedef enum {
  OUTDATED_VERSION = -1,
  INVALID_MOVE = 0,
  MOVE_SUCCESS = 1,
} MoveStatus;

typedef enum {
  CONTINUE = 3,
  DRAW = 2,
  GREEN_WIN = 0,
  BLUE_WIN = 1
} GameState;

//https://stackoverflow.com/questions/791982/determine-if-a-string-is-a-valid-ipv4-address-in-c
int isValidIp4 (char *str) {
    int segs = 0;   /* Segment count. */
    int chcnt = 0;  /* Character count within segment. */
    int accum = 0;  /* Accumulator for segment. */

    /* Catch NULL pointer. */

    if (str == NULL)
        return 0;

    /* Process every character in string. */

    while (*str != '\0') {
        /* Segment changeover. */

        if (*str == '.') {
            /* Must have some digits in segment. */

            if (chcnt == 0)
                return 0;

            /* Limit number of segments. */

            if (++segs == 4)
                return 0;

            /* Reset segment values and restart loop. */

            chcnt = accum = 0;
            str++;
            continue;
        }
        /* Check numeric. */

        if ((*str < '0') || (*str > '9'))
            return 0;

        /* Accumulate and check segment. */

        if ((accum = accum * 10 + *str - '0') > 255)
            return 0;

        /* Advance other segment specific stuff and continue loop. */

        chcnt++;
        str++;
    }

    /* Check enough segments and enough characters in last segment. */

    if (segs != 3)
        return 0;

    if (chcnt == 0)
        return 0;

    /* Address okay. */

    return 1;
}
