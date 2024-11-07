#pragma once

#include "check.h"

int sht_parse_error(const char* parsed, int i, const char* i_name, const char* format, const char* keyword, size_t keyword_n)
{
   fwrite(parsed, 1, i, stdout);
   printf("%s%c%s%s\n", SHT_RED_HIGHLIGHT, parsed[i], SHT_RESET, parsed + i + 1);
   printf("%s%s%*s^%s\n", SHT_RED_HIGHLIGHT, SHT_BLINK, i, "", SHT_RESET);
   printf("%*s%s%s%*s", i, "", i_name, SHT_RESET, SHT_IND_LEVEL, "");
   if (keyword && keyword_n > 0)
   {
      if (format)
      {
         int last_k = 0;
         int matched = 0;
         for (int f = 0; format[f] != '\0'; f++)
         {
            if (strncmp("%k", &format[f], 2) == 0)
            {
               fwrite(&format[last_k], 1, f, stdout);
               printf("%s", SHT_RED_HIGHLIGHT);
               fwrite(keyword, 1, keyword_n, stdout);
               printf("%s", SHT_RESET);
               last_k = f+2;
               f++;
               matched++;
            }
            else if (format[f] == '%')
            {
               char pe_buff[64];

               int spaces_nested = i + strlen(i_name) + SHT_IND_LEVEL + f + 1;
               char* nested_format = malloc(strlen(format) + 1);
               if (nested_format == NULL)
               {
                  perror("Error: failed to malloc nested format string");
                  return -1;
               }
               strcpy(nested_format, format);

               char* ch = strchr(nested_format, '\n');
               if (ch)
                  (*ch) = '\0';

               snprintf(pe_buff, 63, "%sError:%s Unsupported format specifier\n", SHT_RED_HIGHLIGHT, SHT_RESET);
               sht_parse_error(nested_format, spaces_nested, "f", pe_buff, 0, 0);;

               spaces_nested += 1;
               printf("%*s", spaces_nested, "");
               printf("%*s%s", SHT_IND_LEVEL + SHT_IND_LEVEL, "", "(try using \"%k\" instead)\n");
               return -1;
            }
         }
         if (last_k)
            printf("%s", &format[last_k]);

         return matched;
      }
      else
      {
         printf("\"");
         fwrite(keyword, 1, keyword_n, stdout);
         printf("\" invalid keyword\n");
      }
   }
   if (format)
      printf("%s", format);
   else
   {
      printf("\"");
      fwrite(parsed, 1, i, stdout);
      printf("\" invalid keyword\n");
   }

   return 0;
}

int sht_parse_filename(const char* arg)
{
   if (arg == NULL)
   {
      fprintf(stderr, "Error: must specify arg to parse filename\n");
      return -1;
   }

   int dot_found = 0;
   for (int i = 0; arg[i] != '\0'; i++)
   {
      char c = isalnum(arg[i]) ? 'A' : arg[i];
      switch (c)
      {
         case 'A':
            break;
         case '.':
            dot_found++;
            if (dot_found > 1)
            {
               if (sht_parse_error(arg, i, "i", "additional '.' found in extension of file \"%k\"\n", arg, i) != 1)
               {
                  fprintf(stderr, "%sError%s: failed to match arguments while printing parse error\n", SHT_RED_HIGHLIGHT, SHT_RESET);
                  return -1;
               }
               return -1;
            }
            continue;
         default:
            if (sht_parse_error(arg, i, "i", "Note: file name must only contain alphanumeric or '_'\n", 0, 0) != 1)
            {
               fprintf(stderr, "%sError%s: failed to match arguments while printing parse error\n", SHT_RED_HIGHLIGHT, SHT_RESET);
               return -1;
            }
            fprintf(stderr, "Hi\n");
         case '-':
            /*
            if (i == 0)
               // ask user if they are trying to specify argument flag as filename
            else
               // ask user if they are trying to specify tag as filename
            */
            return -1;
      }
   }
}

int sht_parse_tag(const char* arg, const char** keyword_ptr)
{
   if (arg == NULL)
   {
      fprintf(stderr, "Error: must specify arg to parse tag\n");
      return -1;
   }
   if (keyword_ptr == NULL)
   {
      fprintf(stderr, "Error: must specify keyword ptr\n");
      return -1;
   }

   for (int i = 0; arg[i] != '\0'; i++)
   {
      char c = (isdigit(arg[i]) || islower(arg[i])) ? 'a' : arg[i];
      switch (c)
      {
         case 'a':
            break;
         case ':':
            if (strncmp("filename:", arg, i) == 0)
            {
               (*keyword_ptr) = arg + i + 1;
               return sht_parse_filename(*keyword_ptr);
            }
            else if (sht_parse_error(arg, i, "i", "\"%k\" is not a valid keyword\n", arg, i) != 1)
            {
               fprintf(stderr, "Error: failed to match arguments while printing parse error\n");
               return -1;
            }
            continue;
         case '-':
            if (i == 0)
            {
               if (sht_parse_error(arg, i, "i", "Note: tag name must not contain leading '-'\n", 0, 0) != 1)
               {
                  fprintf(stderr, "Error: failed to match arguments while printing parse error\n");
                  return -1;
               }
               return 0;
            }
            break;
         default:
            if (sht_parse_error(arg, i, "i","Note: tag name must only contain lowercase alphanumeric or '-'\n", 0, 0) != 1)
            {
               fprintf(stderr, "Error: failed to match arguments while printing parse error\n");
               return -1;
            }
            return 0;
      }
   }
}

int sht_normalize_files(int force_flag)
{
   sht_check_complain();

   if (sht_determine_sucks(0) == -1)
      return -1;

   FILE* to_norm_file = fopen(".sht/sucks.sht", "r");
   if (to_norm_file == NULL)
   {
      perror("Error: failed to open file for verification of what needs normalized");
      return -1;
   }

   char* line_buff;
   size_t line_len = 0;
   for (ssize_t nread; (nread = getline(&line_buff, &line_len, to_norm_file)) != -1; )
   {
      int force_ent = force_flag;

      char* line_buff_delim_byte = strrchr(line_buff, '\'');
      if (line_buff_delim_byte == NULL)
      {
         fprintf(stderr, "Error: could not read filename to be normalized from file\n");
         goto NORMALIZE_RET_ERROR;
      }

      (*line_buff_delim_byte) = '\0';
      line_buff[nread - 1] = '\0';

      if (force_flag == 0)
      {
         printf("Normalize file '%s' to %s?\n", line_buff, line_buff_delim_byte + 1);
         printf("%*s[a | Y/n | q]: ", SHT_IND_ITEM, "");
         char v[4];
         if (fgets(v, 4, stdin) == NULL)
         {
            fprintf(stderr, "Error: failed to read validation from user\n");
            goto NORMALIZE_RET_ERROR;
         }

         if (v[strcspn(v, "\n")] != '\n')
            for (int c; (c = getchar()) != '\n' && c != EOF; ) { }

         if (v[0] == 'y' || v[0] == 'Y')
            force_ent = 1;
         if (v[0] == 'a' || v[0] == 'A')
            force_ent = force_flag = 1;
         if (v[0] == 'q' || v[0] == 'Q')
            exit(EXIT_SUCCESS);
         //for (int c; (c = getchar()) != '\n' && c != EOF; ) { }
      }

      if (force_flag || force_ent)
      {
         if (rename(line_buff, line_buff_delim_byte + 1))
         {
            fprintf(stderr, "Error: failed to rename file \"%s\" to \"%s\" for normalization: %s\n",
                                                       line_buff, line_buff_delim_byte + 1, strerror(errno));
            goto NORMALIZE_RET_ERROR;
         }
      }
   }

   free(line_buff);
   return 0;

NORMALIZE_RET_ERROR:
   free(line_buff);
   return -1;
}
