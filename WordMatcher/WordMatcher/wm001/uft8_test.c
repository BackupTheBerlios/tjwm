/*
 *      utf8_test
 */

#include <stdio.h>
#include <string.h>



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


