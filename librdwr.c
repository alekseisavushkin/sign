#include "librdwr.h"

char* intochar(int num)
{
    int i = 0;
    int minus= 0;
    int stay = 1;
    char* ans = (char*)calloc(50, sizeof(char));
    int max = 0;
    if(num < 0)
    {
        num = num - 2 * num;
        minus = 1;
    }
    int current = num;
    while(current != 0)
    {
        current = current / 10;
        max++;
    }
    if(num == 0)
    {
        ans[0] = '0';
        ans[1] = ' ';
    }
    if(minus == 0)
    {
        for(i = 1; num != 0; i++)
        {
            ans = (char*)realloc(ans, i + 1);
            stay = num % 10;
            num = num / 10;
            ans[max - i] = stay + '0';
        }
    }
    else
    {
        ans[0] = '-';
        for(i = 1; num != 0; i++)
        {
            stay = num % 10;
            num = num / 10;
            ans[max - i + 1] = stay + '0';
        }
    }
    strcat(ans, " ");
    return ans;
}
