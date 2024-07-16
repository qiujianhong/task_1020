#include "baseCommon.h"

#include "rs.h"

const unsigned char GF[16]={0,1,2,4,8,3,6,12,11,5,10,7,14,15,13,9};
const unsigned char ID[16]={0,1,2,5,3,9,6,11,4,15,10,8,7,14,12,13};
const unsigned char pow2[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

unsigned char mul_rs(unsigned char a,unsigned char b);
unsigned char div_rs(unsigned char a,unsigned char b);
unsigned char rs_check(unsigned char a);
void rs_coder(unsigned char a[]);
void rs_xcoder(unsigned char a[]);
void mod_rs(unsigned char a[],unsigned char b[]);

#if MSK_RS_EN == 1
//RS编码标识: 0 标识无RS编码; 1 表示RS11/15; 2 表示RS9/15
unsigned int rs_code_pack(unsigned char *pbuff, unsigned int len, uint8 rs_code)
{
	int pos_data, pos_code;
	char step = 0;

	if(rs_code == 2)	//2 表示RS9/15
		step = 9;
	else if(rs_code == 1)	//1 表示RS11/15
		step = 11;
	else
		return len;

	while(len%step != 0)		/*- 按step对齐 -*/
	{
		pbuff[len] = 0x77;
		len++;
	}

	pos_code = len*15/step - 15;
	pos_data = len - step;

	while(pos_data>=0)
	{
		rs_encode(pbuff+pos_data,pbuff+pos_code,rs_code);
		pos_code -= 15;
		pos_data -= step;
	}

	if(rs_code == 2)	//2 表示RS9/15
		return (len/9)*15;
	else if(rs_code == 1)	//1 表示RS11/15
		return (len/11)*15;
	else
		return len;
}

unsigned int rs_code_unpack(unsigned char *pbuff, unsigned int len, uint8 rs_code)
{
	int pos_data,pos_code;
	char step = 0;

	if(rs_code == 2) // RS9/15
		step = 9;
	else if(rs_code == 1) // RS11/15
		step = 11;
	else
		return len;

	while(len%15 != 0)		/*- 按15对齐 -*/
	{
		pbuff[len] = 0x77;
		len ++;
	}

	pos_code = 0;
	pos_data = 0;

	while(pos_code < len)
	{
		rs_decode(pbuff+pos_code,pbuff+pos_data,rs_code);
		pos_code += 15;
		pos_data += step;
	}

	if(rs_code == 2)// RS9/15
		return (len/15)*9;
	else if(rs_code == 1) // RS11/15
		return (len/15)*11;
	else
		return len;
}

#else

unsigned int rs_code_pack(unsigned char *pbuff, unsigned int len)
{
//	int i;
	int pos_data,pos_code;
	
	#if rs_9_2_4
	char step = 9;
	#elif rs_11_0_4
	char step = 11;
	#endif
	
	while(len%step != 0)		/*- 按step对齐 -*/
	{
  	pbuff[len] = 0x77;
  	len++;
	}
	
	pos_code = len*15/step - 15;
	pos_data = len - step;

	while(pos_data>=0)
	{
	  	rs_encode(pbuff+pos_data,pbuff+pos_code);
	  	pos_code -= 15;
	  	pos_data -= step;
	}
	
	#if rs_9_2_4
		return (len/9)*15;
	#elif rs_11_0_4
		return (len/11)*15;
	#endif
}

unsigned int rs_code_unpack(unsigned char *pbuff, unsigned int len)
{
//	struct cch_radio *pRd;
//	int i;
	int pos_data,pos_code;
	#if rs_9_2_4
	char step = 9;
	#elif rs_11_0_4
	char step = 11;
	#endif
	
	while(len%15 != 0)		/*- 按15对齐 -*/
	{
  	pbuff[len] = 0x77;
  	len ++;
	}
	
	pos_code = 0;
	pos_data = 0;
	
	while(pos_code < len)
	{
  	rs_decode(pbuff+pos_code,pbuff+pos_data);
  	pos_code += 15;
  	pos_data += step;
	}
	
	#if rs_9_2_4
		return (len/15)*9;
	#elif rs_11_0_4
		return (len/15)*11;
	#endif
}
#endif

void mod_rs(unsigned char a[],unsigned char b[])
{
	signed int k,n;
	signed int i,j;
	for(i=0;i<11;i++)
	{
	  if(a[i]!=0)
	  {
	    if(ID[a[i]]==ID[b[0]])
	      k=0;
	    else if(ID[a[i]]>ID[b[0]])
	      k=1+ID[a[i]]-ID[b[0]];
	    else
	      k=16-(ID[a[i]]-ID[b[0]]);

	     for(j=0;j<5;j++)
	     {
		if((k==0)||(ID[b[j]]==0))
		  n=ID[b[j]];
		else
		{
		  n=(k-1+ID[b[j]]-1)%15;
		  if(n<0) n+=15;
		  n++;
		}
		a[i+j] = a[i+j]^GF[n];
	     }
	  } /* if */
	} /* for i */
}

unsigned char mul_rs(unsigned char a,unsigned char b)
{
	signed int k;
	if((ID[a]==0)||(ID[b]==0))
		k=0;
	else
	{
		k=(ID[a]-1+ID[b]-1)%15;
		if(k<0) k += 15;
		k++;
	}
	return GF[k];
}

unsigned char div_rs(unsigned char a,unsigned char b)
{
	signed int k;
	int m,n;
	m = ID[a];
	n = ID[b];
	if((ID[a]==0)||(ID[b]==0))
		k=0;
	else
	{
		k= (m-n)%15;
		if(k<0) k+=15;
		k++;
	}
	return GF[k];
}

unsigned char rs_check(unsigned char b)
{
  signed int i;
  signed int count;
  count = 0;
  for(i=0;i<8;i++)
  {
    if(b&pow2[i]) count ++;
  } /* for i */

  return count%2 ;
}

#if MSK_RS_EN == 1
//RS编码标识	0 标识无RS编码  1 表示RS11/15  2 表示RS9/15
void rs_decode(unsigned char *src,unsigned char *dest, uint8 rs_code)
{
	unsigned char b[15]={0};
	unsigned char c[30]={0};
	unsigned char d[15]={0};
	unsigned char e[30]={0};
	signed int i;
	for(i=0;i<15;i++)
		b[i]=src[i];
	for(i=0;i<15;i++)
	{
		if(rs_code == 2)	//2 表示RS9/15
		{
			c[2*i] = b[i]/16;
			c[2*i+1] = b[i]%16;
		}
		else if(rs_code == 1)	//1 表示RS11/15
		{
			c[2*i] = b[i]%16;
			c[2*i+1] = b[i]/16;
		}
		else
			return;
	}
	//-c[]中最大的为15-
	for(i=0;i<11;i++)
		d[i]=c[i];
	for(i=11;i<15;i++)
		d[i]=c[11+i];

	rs_xcoder(d);
	for(i=0;i<11;i++)
		e[i]=d[i];

	for(i=0;i<11;i++)
		d[i]=c[11+i];
	for(i=11;i<15;i++)
		d[i]=c[15+i];

	rs_xcoder(d);
	for(i=0;i<11;i++)
		e[11+i]=d[i];

	for(i=0;i<11;i++)
	{
		if(rs_code == 2)	//2 表示RS9/15
		{
			dest[i]=e[2*i]*16 + e[2*i+1];
		}
		else if(rs_code == 1)	//1 表示RS11/15
		{
			dest[i]=e[2*i] + e[2*i+1]*16;
		}
		else
			return;
	}
}

void rs_encode(unsigned char *src,unsigned char *dest, uint8 rs_code)
{
	signed int i;
	unsigned char b[15]={0};
	unsigned char c[22]={0};
	unsigned char d[15]={0};

	if(rs_code == 2)	//2 表示RS9/15
	{
		b[9]=0;
		for(i=0;i<9;i++)
		{
			b[i] = src[i];
			if( (i<8)&&rs_check(b[i]) )
				b[9] = b[9] + pow2[i];
		}
		b[10]=rs_check(b[8]);

		for(i=0;i<11;i++)
		{
			c[2*i] = b[i]/16;
			c[2*i+1] = b[i]%16;
		}
		for(i=0;i<11;i++)
			d[i] = c[i];
		rs_coder(d);

		b[11] = d[11]*16 + d[12];
		b[12] = d[13]*16 + d[14];

		for(i=11;i<22;i++)
			d[i-11] = c[i];
		rs_coder(d);

		b[13] = d[11]*16 + d[12];
		b[14] = d[13]*16 + d[14];

		for(i=0;i<15;i++)
			dest[i]=b[i];
	}
	else if(rs_code == 1)	//1 表示RS11/15
	{
		for(i=0;i<11;i++)
		{
			b[i] = src[i];
		}

		for(i=0;i<11;i++)
		{
			c[2*i] = b[i]%16;
			c[2*i+1] = b[i]/16;
		}
		for(i=0;i<11;i++)
			d[i] = c[i];
		rs_coder(d);

		b[11] = d[12]*16 + d[11];
		b[12] = d[14]*16 + d[13];

		for(i=11;i<22;i++)
			d[i-11] = c[i];
		rs_coder(d);

		b[13] = d[12]*16 + d[11];
		b[14] = d[14]*16 + d[13];

		for(i=0;i<15;i++)
			dest[i]=b[i];
	}
	else
		return;
}

#else

void rs_decode(unsigned char *src,unsigned char *dest)
{
  unsigned char b[15]={0};
  unsigned char c[30]={0};
  unsigned char d[15]={0};
  unsigned char e[30]={0};
  signed int i;
  for(i=0;i<15;i++)
    b[i]=src[i];
  for(i=0;i<15;i++)
  {
  	#if rs_9_2_4
    c[2*i] = b[i]/16;
    c[2*i+1] = b[i]%16;
    #elif rs_11_0_4
    c[2*i] = b[i]%16;
    c[2*i+1] = b[i]/16;
    #endif
  }
	//-c[]中最大的为15-
  for(i=0;i<11;i++)
    d[i]=c[i];
  for(i=11;i<15;i++)
    d[i]=c[11+i];

  rs_xcoder(d);
  for(i=0;i<11;i++)
    e[i]=d[i];

  for(i=0;i<11;i++)
    d[i]=c[11+i];
  for(i=11;i<15;i++)
    d[i]=c[15+i];

  rs_xcoder(d);
  for(i=0;i<11;i++)
    e[11+i]=d[i];

  for(i=0;i<11;i++)
    #if rs_9_2_4
    dest[i]=e[2*i]*16 + e[2*i+1];
    #elif rs_11_0_4
    dest[i]=e[2*i] + e[2*i+1]*16;
    #endif
}

void rs_encode(unsigned char *src,unsigned char *dest)
{
  signed int i;
  unsigned char b[15]={0};
  unsigned char c[22]={0};
  unsigned char d[15]={0};
  
  #if rs_9_2_4
  b[9]=0;
  for(i=0;i<9;i++)
  {
    b[i] = src[i];
    if( (i<8)&&rs_check(b[i]) )
      b[9] = b[9] + pow2[i];
  }
  b[10]=rs_check(b[8]);
  #elif rs_11_0_4
  for(i=0;i<11;i++)
  {
    b[i] = src[i];
  }
  #endif
  
  for(i=0;i<11;i++)
  {
  	#if rs_9_2_4
    c[2*i] = b[i]/16;
    c[2*i+1] = b[i]%16;
    #elif rs_11_0_4
    c[2*i] = b[i]%16;
    c[2*i+1] = b[i]/16;
    #endif
  }
  for(i=0;i<11;i++)
    d[i] = c[i];
  rs_coder(d);
  
  #if rs_9_2_4
  b[11] = d[11]*16 + d[12];
  b[12] = d[13]*16 + d[14];
  #elif rs_11_0_4
  b[11] = d[12]*16 + d[11];
  b[12] = d[14]*16 + d[13];
  #endif
  for(i=11;i<22;i++)
    d[i-11] = c[i];
  rs_coder(d);
  
  #if rs_9_2_4
  b[13] = d[11]*16 + d[12];
  b[14] = d[13]*16 + d[14];
  #elif rs_11_0_4
  b[13] = d[12]*16 + d[11];
  b[14] = d[14]*16 + d[13];
  #endif
  
  for(i=0;i<15;i++)
    dest[i]=b[i];
}
#endif

void rs_coder(unsigned char a[])
{
  unsigned char b[5]={1,13,12,8,7};
  unsigned char c[15]={0};
  unsigned char i;

  for(i=0;i<11;i++)
    c[i] = a[i];
  for(i=11;i<15;i++)
    c[i] = 0;

  mod_rs(c,b);

  for(i=11;i<15;i++)
    a[i]=c[i];
}

void rs_xcoder(unsigned char a[])
{
  unsigned char i,j;
  unsigned char s[4]={0};
  unsigned char dz[3]={0};
  unsigned char ew[2]={0};
  unsigned char x[2]={0};
  int k=0;
  for(i=0;i<4;i++)
  {
    s[i]=0x0;
    for(j=0;j<15;j++)
    {
      if(a[j]!=0){
        k = ID[a[j]]-1+((i+1)*(14-j));
        k = k%15;
        if(k<0) k+=15;
        k++;
        s[i]=s[i]^GF[k];
      }  /* if a!=0 */
    }  /* for j */
  }  /* for i */

  j = 0;
  for(i=0;i<4;i++)
  {
    if(s[i]!=0)
    {
      j = 1;
      break;
    }
  } /* for */

  dz[2] = mul_rs(s[0],s[2]) ^ mul_rs(s[1],s[1]);
  dz[1] = mul_rs(s[0],s[3]) ^ mul_rs(s[1],s[2]);
  dz[0] = mul_rs(s[2],s[2]) ^ mul_rs(s[1],s[3]);

  if(dz[2]==0)
  {
    if( (dz[1]!=0)||(s[0]==0)||
        (s[1]==0)||(s[2]==0)||(s[3]==0) ) ;
    else{
      x[0] = div_rs(s[1],s[0]);
      ew[0]= mul_rs(s[0],s[0]);
      ew[0]= div_rs(ew[0],s[1]);
      i = ID[x[0]] - 1;
      a[14-i] ^= ew[0];
    }
  }
  else{
    if( (dz[1]==0)||(dz[0]==0) ){
      ;
    }
    else{
      k = 0;
      for(i=0;i<16;i++)
      {
        j = mul_rs(dz[2],mul_rs(GF[i],GF[i]));
        j = j ^ mul_rs(dz[1],GF[i]);
        j = j ^ dz[0];
        if(j==0)
        {
        	if(i+1>15)
        	{
        		return;
        	}
          	x[0] = GF[i+1];
          	break;
        }
      }
      x[1] = div_rs(dz[1],dz[2]) ^ x[0];
    }
    i = mul_rs(x[0],x[1]) ^ mul_rs(x[0],x[0]);
    ew[0] = mul_rs(s[0],x[1]) ^ s[1];
    ew[0] = div_rs(ew[0],i);

    i = mul_rs(x[0],x[1]) ^ mul_rs(x[1],x[1]);
    ew[1] = mul_rs(s[0],x[0]) ^ s[1];
    ew[1] = div_rs(ew[1],i);
	
	if(x[0]>15||x[1]>15)
	{
		return;
	}
    if(ID[x[0]]<1) i = 0;
    else i = ID[x[0]] - 1;

    a[14-i] ^= ew[0];
    if(ID[x[1]]<1) i = 0;
    else i = ID[x[1]] - 1;
    a[14-i] ^= ew[1];
  }
}



