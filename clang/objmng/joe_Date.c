/*
 * This source file is part of the "Joe Objects Executor" open source project
 *
 * Copyright 2023 Marco Bertacca (www.bertacca.eu)
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

# include <time.h>
# include "joe_Date.h"
# include "joe_Integer.h"
# include "joe_String.h"
# include "joe_Boolean.h"
# include "joe_Exception.h"
# include <stdio.h>

struct s_Timestamp {
   long julian;
   unsigned int year;
   unsigned int month;
   unsigned int day;
   unsigned int hour;
   unsigned int minute;
   unsigned int second;
   unsigned int millis;
};

const static int64_t zeroMillis = 86400000 * 6;

const static char *weekDays[7] = {
   "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

const static int dayInMonth[13][2] = {
   { 0,  0},
   {31, 31},
   {28, 29},
   {31, 31},
   {30, 30},
   {31, 31},
   {30, 30},
   {31, 31},
   {31, 31},
   {30, 30},
   {31, 31},
   {30, 30},
   {31, 31}
};

inline static int
isLeapYear (int y)
{
   if ((y) <= 1752)
      return y % 4 == 0 ? 1 : 0;
   else
      return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0 ? 1 : 0;
}

int64_t
toMilliseconds(struct s_Timestamp * ts)
{
   int64_t Return = 0;
   register int i;
   
   Return = ts->day;
   for (i = 1; i < ts->month; i++)
      Return += dayInMonth[i][isLeapYear(ts->year)];
   for (i = 1; i < ts->year; i++)
      if (isLeapYear(i))
         Return += 366;
      else
         Return += 365;
   if (ts->year > 1752 ||
       (ts->year == 1752 && ts->month > 9) ||
       (ts->year == 1752 && ts->month == 9 && ts->day >= 14))
      Return -= 11;
   Return += 5;  /* 01/01/0001 era sabato (?); cosi' j % 7 da domenica = 0 */
   ts->julian = Return;

   Return *= 86400000;
   Return += (ts->hour   * 3600000) + 
             (ts->minute *   60000) +
             (ts->second *    1000) +
              ts->millis;
   return Return;
}

static void
fromMilliseconds (struct s_Timestamp * ts, int64_t millis)
{
   register int i;
   long julian = millis / 86400000;
   long time = millis % 86400000;

   ts->day = ts->month = ts->year = 0;
   ts->julian = julian;
   if (julian > 639803L) /* giuliano del 2/9/1752 */
      julian += 11; /* Compensazione gregoriana */
   julian -= 5;  /* 01/01/0001 era sabato (?); cosi' j % 7 da domenica = 0 */
   if ( julian > 0) {
      for (i = 1; ; i++)
         if (isLeapYear(i)) {
            julian -= 366;
            if (julian <= 0) {
               ts->year = i;
               julian += 366;
               break;
            }
         } else {
            julian -= 365;
            if (julian <= 0) {
               ts->year = i;
               julian += 365;
               break;
            }
         }
      for (i = 1; ; i++) {
         julian -= dayInMonth[i][isLeapYear(ts->year)];
         if (julian <= 0) {
            ts->month = i;
            ts->day = julian + dayInMonth[i][isLeapYear(ts->year)];
            break;
         } 
      }
   }
   ts->hour = time / 3600000;
   time %= 3600000;
   ts->minute = time / 60000;
   time %= 60000;
   ts->second = time / 1000;
   time %= 1000;
   ts->millis = time;
}

static char tsbuff[48];

static char*
asc (const struct s_Timestamp * ts)
{
   sprintf (tsbuff,"%.04d-%.02d-%.02dT%.02d:%.02d:%.02d.%.03d %s",
                   (int) ts->year,
                   (int) ts->month,
                   (int) ts->day,
                   (int) ts->hour,
                   (int) ts->minute,
                   (int) ts->second,
                   (int) ts->millis,
                   weekDays[ts->julian % 7]);

   return tsbuff;
}

static int
isTimeStampOk (struct s_Timestamp *ts)
{
   return ts->month >= 1 && ts->month <= 12 &&
          ts->day >= 1 &&
          ts->day <= dayInMonth[ts->month][isLeapYear(ts->year)] &&
          ts->hour < 24 &&
          ts->minute < 60 &&
          ts->second < 60 &&
          ts->millis < 1000;
}


static int64_t
currentTime (struct s_Timestamp *ts)
{
   struct tm *now;
   struct timespec tms;

   clock_gettime (CLOCK_REALTIME, &tms);
   now = localtime(&tms.tv_sec);

   ts->year = now->tm_year + 1900;
   ts->month = now->tm_mon + 1;
   ts->day = now->tm_mday;
   ts->hour = now->tm_hour;
   ts->minute = now->tm_min;
   ts->second = now->tm_sec;
   ts->millis = tms.tv_nsec / 1000000;

   return toMilliseconds(ts);
}

static int
ctor (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   struct s_Timestamp ts;

   if (argc == 0) {
      *((int64_t *) joe_Object_getMem(self)) = currentTime(&ts);
      return JOE_SUCCESS;
   } else if (argc == 1 && joe_Object_instanceOf(argv[0], &joe_Integer_Class)) {
      int64_t millis = *((int64_t *) joe_Object_getMem(argv[0]));
      if (millis < zeroMillis)
         millis = zeroMillis;
      *((int64_t *) joe_Object_getMem(self)) = millis;
      return JOE_SUCCESS;
   }
   joe_Object_assign(retval,
                     joe_Exception_New ("joe_Date ctor: invalid argument(s)"));
   return JOE_FAILURE;
}

static int
equals (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf(argv[0], &joe_Date_Class)) {
      if (*((int64_t *) joe_Object_getMem (self)) ==
          *((int64_t *) joe_Object_getMem (argv[0])))
         joe_Object_assign(retval, joe_Boolean_New_true());
      else
         joe_Object_assign(retval, joe_Boolean_New_false());
      return JOE_SUCCESS;
   }
   joe_Object_assign(retval,
                     joe_Exception_New ("joe_Date equals: invalid argument(s)"));
   return JOE_FAILURE;
}

static int
lt (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf(argv[0], &joe_Date_Class)) {
      if (*((int64_t *) joe_Object_getMem (self)) <
          *((int64_t *) joe_Object_getMem (argv[0])))
         joe_Object_assign(retval, joe_Boolean_New_true());
      else
         joe_Object_assign(retval, joe_Boolean_New_false());
      return JOE_SUCCESS;
   }
   joe_Object_assign(retval, joe_Exception_New ("joe_Date equals: invalid argument"));
   return JOE_FAILURE;
}

static int
gt (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf(argv[0], &joe_Date_Class)) {
      if (*((int64_t *) joe_Object_getMem (self)) >
          *((int64_t *) joe_Object_getMem (argv[0])))
         joe_Object_assign(retval, joe_Boolean_New_true());
      else
         joe_Object_assign(retval, joe_Boolean_New_false());
      return JOE_SUCCESS;
   }
   joe_Object_assign(retval, joe_Exception_New ("joe_Date equals: invalid argument"));
   return JOE_FAILURE;
}

static int
toString (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      int64_t millis = *((int64_t *) joe_Object_getMem(self));
      struct s_Timestamp ts;
      fromMilliseconds (&ts, millis);
      joe_Object_assign(retval, joe_String_New (asc (&ts)));
      return JOE_SUCCESS;
   } else {
      joe_Object_assign(retval,
                   joe_Exception_New("joe_Date toString: invalid argument(s)"));
      return JOE_FAILURE;
   }
}

static int
getDate (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      int64_t millis = *((int64_t *) joe_Object_getMem(self));
      struct s_Timestamp ts;
      fromMilliseconds (&ts, millis);
      joe_Object_assign(retval, joe_Integer_New (ts.day));
      return JOE_SUCCESS;
   }
   joe_Object_assign(retval,
                   joe_Exception_New("joe_Date getDate: invalid argument(s)"));
   return JOE_FAILURE;
}

static int
getDay (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      int64_t millis = *((int64_t *) joe_Object_getMem(self));
      struct s_Timestamp ts;
      fromMilliseconds (&ts, millis);
      joe_Object_assign(retval, joe_Integer_New (ts.julian % 7));
      return JOE_SUCCESS;
   }
   joe_Object_assign(retval,
                   joe_Exception_New("joe_Date getDay: invalid argument(s)"));
   return JOE_FAILURE;
}

static int
getHours (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      int64_t millis = *((int64_t *) joe_Object_getMem(self));
      struct s_Timestamp ts;
      fromMilliseconds (&ts, millis);
      joe_Object_assign(retval, joe_Integer_New (ts.hour));
      return JOE_SUCCESS;
   }
   joe_Object_assign(retval,
                   joe_Exception_New("joe_Date getHours: invalid argument(s)"));
   return JOE_FAILURE;
}

static int
getMinutes (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      int64_t millis = *((int64_t *) joe_Object_getMem(self));
      struct s_Timestamp ts;
      fromMilliseconds (&ts, millis);
      joe_Object_assign(retval, joe_Integer_New (ts.minute));
      return JOE_SUCCESS;
   }
   joe_Object_assign(retval,
                   joe_Exception_New("joe_Date getMinutes: invalid argument(s)"));
   return JOE_FAILURE;
}

static int
getMonth (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      int64_t millis = *((int64_t *) joe_Object_getMem(self));
      struct s_Timestamp ts;
      fromMilliseconds (&ts, millis);
      joe_Object_assign(retval, joe_Integer_New (ts.month));
      return JOE_SUCCESS;
   }
   joe_Object_assign(retval,
                   joe_Exception_New("joe_Date getMonth: invalid argument(s)"));
   return JOE_FAILURE;
}

static int
getSeconds (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      int64_t millis = *((int64_t *) joe_Object_getMem(self));
      struct s_Timestamp ts;
      fromMilliseconds (&ts, millis);
      joe_Object_assign(retval, joe_Integer_New (ts.second));
      return JOE_SUCCESS;
   }
   joe_Object_assign(retval,
                   joe_Exception_New("joe_Date getSeconds: invalid argument(s)"));
   return JOE_FAILURE;
}

static int
getTime (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      joe_Object_assign(retval,
                   joe_Integer_New (*((int64_t *) joe_Object_getMem(self))));
      return JOE_SUCCESS;
   }
   joe_Object_assign(retval,
                   joe_Exception_New("joe_Date getTime: invalid argument(s)"));
   return JOE_FAILURE;
}

static int
getYear (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 0) {
      int64_t millis = *((int64_t *) joe_Object_getMem(self));
      struct s_Timestamp ts;
      fromMilliseconds (&ts, millis);
      joe_Object_assign(retval, joe_Integer_New (ts.year));
      return JOE_SUCCESS;
   }
   joe_Object_assign(retval,
                   joe_Exception_New("joe_Date getYear: invalid argument(s)"));
   return JOE_FAILURE;
}

static int
setDate (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
      int64_t millis = *((int64_t *) joe_Object_getMem(self));
      struct s_Timestamp ts;
      fromMilliseconds (&ts, millis);
      ts.day = *((int64_t *) joe_Object_getMem(argv[0]));
      if (isTimeStampOk (&ts)) {
         millis = toMilliseconds (&ts);
         *((int64_t *) joe_Object_getMem(self)) = millis;
         joe_Object_assign(retval, self);
         return JOE_SUCCESS;
      }
   }
   joe_Object_assign(retval,
                   joe_Exception_New("joe_Date setDate: invalid argument(s)"));
   return JOE_FAILURE;
}

static int
setHours (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
      int64_t millis = *((int64_t *) joe_Object_getMem(self));
      struct s_Timestamp ts;
      fromMilliseconds (&ts, millis);
      ts.hour = *((int64_t *) joe_Object_getMem(argv[0]));
      if (isTimeStampOk (&ts)) {
         millis = toMilliseconds (&ts);
         *((int64_t *) joe_Object_getMem(self)) = millis;
         joe_Object_assign(retval, self);
         return JOE_SUCCESS;
      }
   }
   joe_Object_assign(retval,
                   joe_Exception_New("joe_Date setHours: invalid argument(s)"));
   return JOE_FAILURE;
}

static int
setMinutes (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
      int64_t millis = *((int64_t *) joe_Object_getMem(self));
      struct s_Timestamp ts;
      fromMilliseconds (&ts, millis);
      ts.minute = *((int64_t *) joe_Object_getMem(argv[0]));
      if (isTimeStampOk (&ts)) {
         millis = toMilliseconds (&ts);
         *((int64_t *) joe_Object_getMem(self)) = millis;
         joe_Object_assign(retval, self);
         return JOE_SUCCESS;
      }
   }
   joe_Object_assign(retval,
                   joe_Exception_New("joe_Date setMinutes: invalid argument(s)"));
   return JOE_FAILURE;
}

static int
setMonth (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
      int64_t millis = *((int64_t *) joe_Object_getMem(self));
      struct s_Timestamp ts;
      fromMilliseconds (&ts, millis);
      ts.month = *((int64_t *) joe_Object_getMem(argv[0]));
      if (isTimeStampOk (&ts)) {
         millis = toMilliseconds (&ts);
         *((int64_t *) joe_Object_getMem(self)) = millis;
         joe_Object_assign(retval, self);
         return JOE_SUCCESS;
      }
   }
   joe_Object_assign(retval,
                   joe_Exception_New("joe_Date setMonth: invalid argument(s)"));
   return JOE_FAILURE;
}

static int
setSeconds (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
      int64_t millis = *((int64_t *) joe_Object_getMem(self));
      struct s_Timestamp ts;
      fromMilliseconds (&ts, millis);
      ts.second = *((int64_t *) joe_Object_getMem(argv[0]));
      if (isTimeStampOk (&ts)) {
         millis = toMilliseconds (&ts);
         *((int64_t *) joe_Object_getMem(self)) = millis;
         joe_Object_assign(retval, self);
         return JOE_SUCCESS;
      }
   }
   joe_Object_assign(retval,
                   joe_Exception_New("joe_Date setSeconds: invalid argument(s)"));
   return JOE_FAILURE;
}

static int
setTime (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
      int64_t millis = *((int64_t *) joe_Object_getMem (argv[0]));
      if (millis >= zeroMillis) {
         *((int64_t *) joe_Object_getMem(self)) = millis;
         joe_Object_assign(retval, self);
         return JOE_SUCCESS;
      }
   }
   joe_Object_assign(retval,
                   joe_Exception_New("joe_Date setTime: invalid argument(s)"));
   return JOE_FAILURE;
}

static int
setYear (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
      int64_t millis = *((int64_t *) joe_Object_getMem(self));
      struct s_Timestamp ts;
      fromMilliseconds (&ts, millis);
      ts.year = *((int64_t *) joe_Object_getMem(argv[0]));
      if (isTimeStampOk (&ts)) {
         millis = toMilliseconds (&ts);
         *((int64_t *) joe_Object_getMem(self)) = millis;
         joe_Object_assign(retval, self);
         return JOE_SUCCESS;
      }
   }
   joe_Object_assign(retval,
                   joe_Exception_New("joe_Date setYear: invalid argument(s)"));
   return JOE_FAILURE;
}

static int
addDays (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_Integer_Class)) {
      int64_t millis = *((int64_t *) joe_Object_getMem (argv[0]));
      millis *= 86400000;
      millis += *((int64_t *) joe_Object_getMem (self));
      if (millis >= zeroMillis) {
         int Return;
         joe_Object args[1] = { 0 };
         joe_Object_assign (args, joe_Integer_New (millis));
         Return = joe_Class_newInstance (&joe_Date_Class, 1, args, retval);
         joe_Object_assign (args, 0);
         return Return;
      }
   }
   joe_Object_assign(retval,
                   joe_Exception_New("joe_Date addDays: invalid argument(s)"));
   return JOE_FAILURE;
}

static int
diffDays (joe_Object self, int argc, joe_Object *argv, joe_Object *retval)
{
   if (argc == 1 && joe_Object_instanceOf (argv[0], &joe_Date_Class)) {
      int64_t millis = *((int64_t *) joe_Object_getMem (self)) -
                       *((int64_t *) joe_Object_getMem (argv[0]));
      millis /= 86400000;
      joe_Object_assign (retval, joe_Integer_New (millis));
      return JOE_SUCCESS;
   }
   joe_Object_assign(retval,
                   joe_Exception_New("joe_Date diffDays: invalid argument(s)"));
   return JOE_FAILURE;
}


static joe_Method mthds[] = {
   {"gt", gt },
   {"lt", lt },
   {"addDays", addDays },
   {"after", gt },
   {"before", lt },
   {"diffDays", diffDays },
   {"equals", equals },
   {"getDate", getDate },
   {"getDay", getDay },
   {"getHours", getHours },
   {"getMinutes", getMinutes },
   {"getMonth", getMonth },
   {"getSeconds", getSeconds },
   {"getTime", getTime },
   {"getYear", getYear },
   {"setDate", setDate },
   {"setHours", setHours },
   {"setMinutes", setMinutes },
   {"setMonth", setMonth },
   {"setSeconds", setSeconds },
   {"setTime", setTime },
   {"setYear", setYear },
   {"toString", toString },
  {(void *) 0, (void *) 0}
};

joe_Class joe_Date_Class = {
   "joe_Date",
   ctor,
   0,
   mthds,
   0,
   &joe_Object_Class,
   0
};

