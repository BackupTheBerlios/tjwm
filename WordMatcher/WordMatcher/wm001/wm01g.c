//#include <stdio.h>
//#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <glib/gstdio.h>
/* --------------------------------------------------------------------*/

#define MAXLEN 50
#define WM_VERSION 1

struct WordRec {
	guchar *str;
	struct WordRec *next;
};

typedef struct WordRec WORDREC;
typedef WORDREC * WORDRECPTR;

struct Header {
	guchar name[4];
	gint version;
	WORDRECPTR listhead[MAXLEN];
};

struct Header header;

FILE *in;
FILE *out;

gchar dataname[] = "wmdbdata.danish";
gchar inname[] = "dansk02.txt";
	
WORDRECPTR add_word_to_list(guchar * w, gsize len, WORDRECPTR head);
void free_wordlist(WORDRECPTR head);
gint get_word(guchar * s, gsize len, FILE * fp);
gint my_strsize(gint maxlen, guchar * str);
gint read_infile(void);
gint write_outfile(void);
gint my_strcmp(guchar * tmp1, guchar * tmp2);
gint my_strcpy(guchar * dest, guchar * source, gint len);

int main(int argc, char **argv)
{
	gint i = 0;
		
	for (i = 0; i < MAXLEN;i++)
	{
		header.listhead[i] = (WORDRECPTR) g_malloc(sizeof(WORDREC));
	}
	
	puts("Reading words");
	
	if (!read_infile()){
		for (i = 0; i < MAXLEN;i++)
		{
			free_wordlist(header.listhead[i]);
		}
	
		return 1;
	}
		
	puts("\nWriting words");
	
	//if (!write_outfile())
	//	return 1;
	

	
	return 0;
}

gint read_infile(void) {
	
	guchar str[MAXLEN];
	
	if ((in = fopen(inname, "r")) == NULL)
	{
		fprintf(stderr, "Cannot open %s.\n",inname);
		getc(stdin);
		return 1;
	}
	
	while (get_word(str, MAXLEN, in))
	{ 
		//get_word(str, MAXLEN, in);
		gsize l = my_strsize(MAXLEN, str);
		add_word_to_list(str,l,header.listhead[l]);

	}
	fclose(in);
	return 0;
}

gint write_outfile(void) {
	
	gint i = 0;
	
	if ((out = fopen(dataname, "w+")) == NULL)
	{
		fprintf(stderr, "Cannot open %s.\n",dataname);
		getc(stdin);
		return 1;
	}
	
	fwrite(&header, sizeof(struct Header), 1, out);
	
	for (i=0;i<MAXLEN;i++)
	{
		if (header.listhead[i])
		{
			while (header.listhead[i]->next)
			{
				fwrite(header.listhead[i]->str, sizeof(header.listhead[i]->str), 1, out);
				header.listhead[i] = header.listhead[i]->next;
			}
			free_wordlist(header.listhead[i]);
		}
	}
	fclose(out);
	return 0;
}

gint get_word(guchar * s, gsize len, FILE * fp)
{
	guchar w[len];
	gint c;
	gint i;
		
	for (i=0;i<MAXLEN;i++)
	{
		c = (gint) fgetc(fp);
		
		if (c == EOF)
			return 0;
		else if (c == ' ' || c == '\n' || c == EOF || c == '.' || c == ',')
			break;
		else if (c == '\'')
		{
			while (c != '\n' && c != ' ')
			{
				if (c == EOF)
					return 0;
				else
				{
					c = (gint) fgetc(fp);
					break;
				}
			}
		}
		else 
			w[i] = c;
	}
	w[i] = '\0';
	my_strcpy(s, w, len);
	return 1;
}

WORDRECPTR add_word_to_list(guchar *w, gsize len, WORDRECPTR head)
{
   WORDRECPTR new = NULL;
   WORDRECPTR tmp = NULL;
   WORDRECPTR prev = NULL;

   new = (WORDRECPTR) g_malloc(sizeof(WORDREC));
   if (!new)
   {
      fprintf(stderr, "\nUnable to allocate memory for WORDRECPTR\nin function add_word_to_list!\n");
      return 0;
   }

   /* set new link's data */

   new->str = (guchar *) g_malloc(len);
   
   my_strcpy(new->str, w, len);
   printf("w = %s, new->str = %s, len = %d\n", w, new->str, len);
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


void free_wordlist(WORDRECPTR head)
{
	WORDRECPTR cur,next;
	cur = head;                 /* Start at beginning */
	
	while (cur->str != NULL)          /* Go while not end of list */
	{
		next = cur->next; 		/* Get address of next record */
		g_free(cur->str);               /* Free current record */
		g_free(cur);
		cur = next;           	/* Adjust current record*/

   }
}

int my_strsize(gint maxlen, guchar * str)
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

gint my_strcmp(guchar * tmp1, guchar * tmp2)
{
	
	do {
		if (tmp1 > tmp2)
			return 1;
		else if (tmp1 < tmp2)
			return -1;
	} while (tmp1++ && tmp2++);
	
	return 0;
}

gint my_strcpy(guchar * dest, guchar * source, gint len)
{
	gint l = 0;
	while (dest && source && (l <= len)){
		g_memmove (dest++, source++, sizeof (guchar));
		l++;
	}
	
	return l;
}
