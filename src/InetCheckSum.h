#ifndef __INETCHECKSUM__
#define __INETCHECKSUM__

namespace Net{
namespace Raw{

unsigned short CheckSum(unsigned short *data, int size)
{
    register long sum;
    unsigned short oddbyte;
    register short answer;
 
    sum=0;
    while(size>1) 
	{
        sum+=*data++;
        size-=2;
    }
    if(size==1) 
	{
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)data;
        sum+=oddbyte;
    }
 
    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;
     
    return(answer);
}

}
}
#endif
