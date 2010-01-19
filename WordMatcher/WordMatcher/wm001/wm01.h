#include <stdlib.h>
#include <stdio.h>

#define MAXLEN 50
#define WM_VERSION 1
 
struct WordRec {
	unsigned char *str;
	struct WordRec *next;
};

typedef struct WordRec WORDREC;
typedef WORDREC * WORDRECPTR;

struct Header {
	unsigned char name[4];
	int version;
	WORDRECPTR listhead[MAXLEN];
};

size_t read_infile(unsigned char **data, const char * name);
int write_outfile(unsigned char **data, size_t size, const char * name);
int read_words(unsigned char **data, size_t size);
int load_words(unsigned char **data, size_t size);
WORDRECPTR add_word_to_list(unsigned char * w, size_t len, WORDRECPTR head);

int my_strcpy(unsigned char * dest, unsigned char * source, int len);
int my_strcmp(unsigned char * tmp1, unsigned char * tmp2);
int my_strsize(int maxlen, unsigned char * str);
