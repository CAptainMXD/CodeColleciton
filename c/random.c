#include <stdio.h>
#include <stdlib.h>

int Min(int x,int y)
{
    return x<y? x:y;
}

long long Random()
{
    FILE *m_pFile;
    long long result=0;
    m_pFile=fopen("/dev/urandom","r");
    if(m_pFile==NULL) return -1;
    fread(&result,sizeof(result),1,m_pFile);
    fclose(m_pFile);
    return result;
}


int randomx(int start,int end)
{
    if(start==end) return start;
    unsigned int r=Random()%0xffffffff;
    return (r%abs(end-start))+Min(start,end);
}


int main()
{
    int num=randomx(1,100);
    printf("num=%d\n",num);
    return 0;
}



