#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "wm01.h"

/* -------------------------------------------------------------------*/


struct Header header;

FILE *in;
FILE *out;

char dataname[] = "wmdbdata.danish";
char inname[] = "dansk02.txt";

extern int errno;


int main(int argc, char **argv)
{
	int i;
	size_t fsize = 0;
	unsigned char *data;
		
	for (i = 0; i < MAXLEN;i++)
	{
		header.listhead[i] = (WORDRECPTR) malloc(sizeof(WORDREC));
	}
	
	puts("Reading words");
	
	fsize = read_infile(&data, inname);
	puts("\nfile is opened");
	if (fsize == -1)
		return 3;
	
	read_words(&data, fsize);

	puts("\nwords read");
	
	//write_outfile(&header, sizeof(header), dataname);
	
	return 0;
}

/**********************************************************************
 * Name:	read_infile
 * 
 * Purpose:	open a file in read-only mode, allocate memory and point
 * 			it to the pointer 'data'
 * 
 * Return:	number of bytes read
 * *******************************************************************/

size_t read_infile(unsigned char **data, const char * name) 
{
	
	size_t fsize = 0;
	
	if ((in = fopen(name, "r")) == NULL)
	{
		fprintf(stderr, "Cannot open %s.\n",inname);
		getc(stdin);
		return -1;
	}

	fseek(in, 0L, SEEK_END);
	
	fsize = ftell(in);
	
	if (!fsize)
		return -1;
		
	*data = malloc(fsize);
	
	if (!data)
		return -1;
		
	rewind(in);
	
	fread(&data, fsize, 1, in);
	
	if (errno)
		return -1;
		
	fclose(in);
	
	return fsize;
}

int write_outfile(unsigned char **data, size_t size, const char * name) 
{
	
	//int i = 0;
	
	if ((out = fopen(name, "w")) == NULL)
	{
		fprintf(stderr, "Cannot open %s.\n",name);
		getc(stdin);
		return 1;
	}
	
	fwrite(data, size, 1, out);
	
	fclose(out);
	
	return 0;
}

int load_words(unsigned char **data, size_t size)
{
	return 1;
}

int read_words(unsigned char **data, size_t size)
{
	size_t i = 0, j = 0, len=0;
	unsigned char word[MAXLEN], c;
	
	for (i=0;i < 6;i++)
	{
		c = *data[i];
		j++;
		if (c == ' ' || c == '\n' || c == '.' || c == ',')
			break;
		else if (c == '\'')
		{
			while (c != '\n' && c != ' ')
			{
				c = *data[i++];
				break;
			}
		}
		else 
			word[j] = c;
	}
		*data[5] = '\0';
		printf("%s\n", *data);
	//len = my_strsize(MAXLEN, word);
	//header.listhead[len] = add_word_to_list(word, len, header.listhead[len]);
	return 1;
}


WORDRECPTR add_word_to_list(unsigned char *w, size_t len, WORDRECPTR head)
{
   WORDRECPTR new = NULL;
   WORDRECPTR tmp = NULL;
   WORDRECPTR prev = NULL;

   new = (WORDRECPTR) malloc(sizeof(WORDREC));
   if (!new)
   {
      fprintf(stderr, "\nUnable to allocate memory for WORDRECPTR\nin function add_word_to_list!\n");
      return 0;
   }

   /* set new link's data */

   new->str = (unsigned char *) malloc(len);
   
   my_strcpy(new->str, w, len);

   if (new->str == NULL)
   {
      fprintf(stderr, "\nUnable to allocate memory for new->str\nin function add_word_to_list!\n");
      return 0;
   }

   new->next = NULL;
   
   if (head == NULL)
   {
		head = new;
   }
   else
   {
     /* see if it goes before the first link */

      if (my_strcmp(new->str, head->str) < 0)
      {
         new->next = head;
         head = new;
      }
      else   /* it is being added to the middle or end */
      {
         tmp = head->next;
         prev = head;

         /* Check to see where link is added. */

         if (tmp == NULL)
         {
             /* we are adding second record to end */
             prev->next = new;
         }
         else
         {
            /* check to see if adding in middle */
            while ((tmp->next != NULL))
            {
               if(my_strcmp(new->str, tmp->str) < 0)
               {
                  new->next = tmp;
                  prev->next = new;
                  break;   /* link is added; exit while */
               }
               else
               {
                 tmp = tmp->next;
                 prev = prev->next;
               }
            }

            /* check to see if adding to the end */
            if (tmp->next == NULL)
            {
               if (my_strcmp(new->str, tmp->str) < 0) 
               {
                  new->next = tmp;
                  prev->next = new;
               }
               else  /* at the end */
               {
                  tmp->next = new;
						new->next = NULL;  /* redundant */
               }
            }
         }
      }
   }
   return(head);
}


int free_wordlist(WORDRECPTR head)
{
	WORDRECPTR cur,next;
	cur = head;                 /* Start at beginning */
	
	while (cur->str != NULL)          /* Go while not end of list */
	{
		next = cur->next; 		/* Get address of next record */
		free(cur->str);               /* Free current record */
		free(cur);
		cur = next;           	/* Adjust current record*/
	
   }
   return 0;
}

int my_strsize(int maxlen, unsigned char * str)
{
	unsigned int r = 0;
	unsigned int i = 0;
			
	while (i <= maxlen && str[i])
	{
		if (str[i] < 128)
		{
			i+=1;
		}
		else if (str[i] > 193 && \
				 str[i] < 224 && \
				 str[i+1] >127)
		{
			if (i+1 >= maxlen)
				//error!
				break;
			i+=2;
		}
		else if (str[i] > 223 && str[i] < 240)
		{
			if (i+2 >= maxlen)
				//error!
				break;
			if (str[i+1] > 127 && str[i+2] > 127)
			{
				i+=3;
			}
		}
		r+=1;
	}
	return r;	
}

int my_strcmp(unsigned char * tmp1, unsigned char * tmp2)
{
	
	do {
		if (tmp1 > tmp2)
			return 1;
		else if (tmp1 < tmp2)
			return -1;
	} while (tmp1++ && tmp2++);
	
	return 0;
}

int my_strcpy(unsigned char * dest, unsigned char * source, int len)
{
	int l = 0;
	while (dest && source && (l <= len)){
		memmove (dest++, source++, sizeof (unsigned char));
		l++;
	}
	
	return l;
}
