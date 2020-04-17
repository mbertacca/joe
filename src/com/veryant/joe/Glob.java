/*
 *
 * This source file is part of the "Java Objects Executor" open source project
 *
 * Copyright 2017 Veryant and Marco Bertacca
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

package com.veryant.joe;

public class Glob {
   private final char[] glob;
   private final boolean caseInsensitive;

   public Glob (String glob, boolean caseInsensitive) {
      this.caseInsensitive = caseInsensitive;
      this.glob = caseInsensitive ? glob.toUpperCase().toCharArray() :
                                    glob.toCharArray(); 
   }
   public boolean matches (String s) {
      final char[] ca = caseInsensitive ? s.toUpperCase().toCharArray() :
                                          s.toCharArray();
      return recurMatch (ca, 0, glob, 0);
   }

   private final boolean recurMatch (char[] s, int si, char[] m, int mi) {
      final int slen = s.length, mlen = m.length;
      for ( ; mi < mlen && si < slen; si++) {
         switch (m[mi]) {
         case '*':
            for (mi++; mi < mlen && m[mi] == '*'; mi++)
               ;
            if (mi == mlen)
               return true;
            for ( ; si < slen; si++)
               if (recurMatch (s, si, m, mi))
                  return true;
            break;
         case '[':
            {
               boolean negate = false;
               boolean correct = false;
               boolean result = false;

               int i = mi;
               i++;
               if (i < m.length) {
                  char r0 = m[i];
                  if (r0 == '!') {
                     i++;
                     if (i < m.length &&  m[i] == ']') {
                        result = s[si] == r0;
                        correct = true;
                        mi = i;
                     } else
                        negate = true;
                  }
                  while (i < m.length && !correct) {
                     r0 = m[i];
                     i++;
                     if (r0 == '\\' && i < m.length)
                        r0 = m[i++];
                     if (i < m.length) {
                        if (m[i] == ']') {
                           if (r0 == s[si])
                              result = true;
                           correct = true;
                           mi = i;
                        } else if (m[i] == '-') {
                           i++;
                           if (i < m.length) {
                              if (m[i] == ']') {
                                 if (s[si] == '-' || s[si] == r0)
                                    result = true;
                                 correct = true;
                                 mi = i;
                              } else {
                                 if (m[i] == '\\' && i + 1 < m.length)
                                    i++;
                                 if (s[si] >= r0 && s[si] <= m[i])
                                    result = true;
                              }
                           }
                        } else {
                           if (r0 == s[si])
                              result = true;
                        }
                     }
                  }
               }
               if (correct) {
                  if (negate ^ result)
                     mi++;
                  else
                     return false;
               } else {
                  if (m[mi] != s[si])
                     return false;
                  else
                     mi++;
               }
            }
            break;
         case '?':
            mi++;
            break;
         case '\\':
            mi++;
            if (mi == mlen)
               break;
            // PASSTHRU
         default:
            if (m[mi] != s[si])
               return false;
            else
               mi++;
             break;
         }
      }
      for ( ; mi < mlen && m[mi] == '*'; mi++)
         ;

      if (mi == mlen && si == slen)
         return true;
      else
         return false;
   }
}