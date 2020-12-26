#include<stdio.h>
#include<stdlib.h>

int state[4][4], Key[4][4];

int rnd, rndCnstnt[10] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36};

int sbox[16][16], T[256], INV[256];

char *str1, key[16], str[16];

int mulCol[4][4], hello=0x11B;
int g;
int uree=0;

void gVal()
{
	g=3;
	int i;
	for (i=0;i<0;i++)
		g=(g*g)%256;
}

void ginv()
{
	int i, j, h=1, s;
	INV[1]=0;
	for(i=1;i<256;i++)
	{
		s=0;
		for (j=0;j<8;j++)
		{
			if((g>>j)&1==1)
				s^=(h<<j);
		}
		for(j=7;j>-1;j--)
		{
			if((s>>(j+8))>0)
				s^=(hello<<j);
		}
		h=s;
		INV[h]=i;
	}
	h=1;
	T[0]=-(1<<20);
	for(i=1;i<255;i++)
	{
		s=0;
		for (j=0;j<8;j++)
		{
			if((g>>j)&1==1)
				s^=(h<<j);
		}
		for(j=7;j>-1;j--)
		{
			if((s>>(j+8))&1==1)
				s^=(hello<<j);
		}
		h=s;
		s^=1;
		T[i]=INV[s];
	}
/*	printf("\n");
	for(i=0;i<255;i++)
	{
		printf("%5x",T[i]);
		if((i+1)%16==0)
			printf("\n   ");
	}
	printf("    -\n");*/
}

int xor(int x, int y)
{
	if (x==y)
		return 255;
	if (y<-260 || y==255)
		return x%255;
	if (x<-260 || x==255)
		return y%255;
	if (y<x)
	{
		int qq=y;
		y=x;
		x=qq;
	}
	return (x+T[(y-x)])%255;
}

int fieldInv(int a)
{
	int j,i,b=0,mod=283;
	for (i=1;i<256;i++)
	{
		b=0;
		for(j=0;j<8;j++)
		{
			int temp=(a>>j)&1;
			if(temp==1)
				b=b^(i<<j);	//b=xor(b,(i<<j)); 
		}
		int h=(b>>8);
		while(h>0)
		{
			int l=8,temp=(b>>l)&1;
			while(temp!=1)
			{
				l++;
				temp=(b>>l)&1;
			}
			int temp1=(mod<<(l-8));
			b=b^temp1; //b=xor(b,temp1); 
			h=(b>>8);
		}
		if (b==1)
			return i;
	}
	return -1;
}

int inv(int k)
{
	int c=0x63,i;
	if (k==0)
		return c;
	int a;
	a=fieldInv(k);
	int b=0;
	for (i=0;i<8;i++)
	{
		int b1,b2,b3,b4,b5,b6,b7;
		b1=(a>>i)&1;
		b4=(a>>((i+4)%8))&1;
		b5=(a>>((i+5)%8))&1;
		b6=(a>>((i+6)%8))&1;
		b7=(a>>((i+7)%8))&1;
		b2=(c>>i)&1;
		b+=((b1+b4+b5+b6+b7+b2)%2)<<i;
	}
	return b;
}

void addRoundKey()
{
	int i,j,k,l,i1,j1,k1;
	for (i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
			state[i][j]=xor(state[i][j],Key[i][j]); //state[i][j]=state[i][j]^Key[i][j]; //
	}
	int temp[4][4],tmp[4];
	for(j=3;j>=0;j--)
	{
		if(j==3)
		{
			for(k=0;k<4;k++)
				tmp[k]=Key[(k+1)%4][j];
			for(k=0;k<4;k++)
			{
					l=tmp[k];
					i1=(l>>4);
					k1=(i1<<4);
					j1=l^k1; //j1=xor(l,k1); 
					temp[k][j]=sbox[i1][j1];
					if(k==0)
						temp[k][j]=xor(temp[k][j],rndCnstnt[rnd]); //temp[k][j]=temp[k][j]^rndCnstnt[rnd]; //
			}
			for (i=0;i<4;i++)
				temp[i][0]=xor(temp[i][j],Key[i][0]); //temp[i][0]=temp[i][j]^Key[i][0]; //
		}
		else
		{
			for (i=0;i<4;i++)
				temp[i][3-j]=xor(temp[i][2-j],Key[i][3-j]); //temp[i][3-j]=temp[i][2-j]^Key[i][3-j]; //
		}
	}
	for (i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
			Key[i][j]=temp[i][j];
	}
	rnd++;
}

void subBytes()
{
	int i,j,k,i1,j1,k1;
	for (i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			k=state[i][j];
			i1=(k>>4);
			k1=(i1<<4);
			j1=k^k1; //j1=xor(k,k1); 
			state[i][j]=sbox[i1][j1];
		}
	}
}

void shiftRow()
{
	int i,j,temp[4];
	for (i=0;i<4;i++)
	{
		if (i>0)
		{
			for (j=0;j<4;j++)
				temp[j]=state[i][(j+i)%4];
			for (j=0;j<4;j++)
				state[i][j]=temp[j];	
		}
	}
}

void mixColumns()
{
	int i,j,k,n,t,temp[4][4];
	for (i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			temp[i][j]=0;
			for(k=0;k<4;k++)
			{
				if(mulCol[i][k]==0x1)
					temp[i][j]=xor(temp[i][j],state[k][j]); //temp[i][j]^=state[k][j]; //
				else if(mulCol[i][k]==0x2)
				{
					n=state[k][j]<<1;
					t=n>>8;
					if (t>0)
						n=n^hello; //n=xor(n,hello); //
					temp[i][j]=xor(temp[i][j],n); //temp[i][j]^=n; //
				}
				else
				{
					n=state[k][j]<<1;
					t=n>>8;
					if (t>0)
						n=n^hello; //n=xor(n,hello); //
					n=xor(n,state[k][j]); //n=n^state[k][j]; //
					temp[i][j]=xor(temp[i][j],n); //temp[i][j]^=n; //
				}
			}
		}
	}
	for (i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
			state[i][j]=temp[i][j];
	}
}

void sBox()
{
	char i,j;
	int k,c=0x63;
	for (i=0;i<16;i++)
	{
		for(j=0;j<16;j++)
		{
			k=i;
			k=(k<<4);
			k=k+j;
			sbox[i][j]=inv(k);
		}
	}
}

int main()
{
	gVal();
	ginv();
	str1="Random number generators are widely used in";
//	str1="Random number generators are widely used in these days in the field of encryption, scientific and applied computing, information transmission, engineering, and finance. As a result, the evaluation of this random nature of outputs produced by various generators has become vital and necessary task for the communications and banking industries where digital signatures and key management are crucial for information processing and computer security. On the purpose of generating random numbers, in this project report, we are going to describe the statistical requirements, like, Test statistic, Hypothesis Testing, P-value etc. The Computer Security Division of the National Institute of Standards and Technology (NIST) initiated a study to assess the quality of different random number generators. This collection of tests was not designed to identify the best possible generator, but rather to provide a user with a characteristic that allows one to make an informed decision about the source. The test is being accepted which is optimal in the statistical point of view, being measured via. P-value, usually. Generally, a small P-values indicate a disagreement between the hypothesis and the observed data. Although larger P-values do not imply validity of the hypothesis at hand, when a string is being tested by a number of tests, they are necessary to continue the study of multifaceted aspects of non-randomness.";
	printf("Input string (PlainText) of 16 length string: %s\n",str1);
	int hey=0, lenn=strlen(str1);
	int arr[lenn];
	for (hey=0; hey<lenn;hey++)
	{
		if (lenn-hey>=16)
		{
			int i=0;
			for (i=0;i<16;i++)
			{
				str[i]=str1[0];
				str1=str1+1;
			}
		}
		else
		{
			int i=0;
			for (i=0;i<lenn-hey;i++)
			{
				str[i]=str1[0];
				str1=str1+1;
			}
			for (i=lenn-hey;i<16;i++)
			{
				str[i]=" ";
			}	
		}
		hey+=16;
		int ii=0;
		for (ii=0;ii<16;ii++)
		{
			key[ii]=rand()%128;
		}
		int i,j,k;
		sBox();
		rnd=0;
		for (i=0;i<4;i++)
		{
			for(j=0;j<4;j++)
			{
				if(j==i)
					mulCol[i][j]=0x2;
				else if((j-i)==1 || (j-i)==-3)
					mulCol[i][j]=0x3;
				else
					mulCol[i][j]=0x1;
			}
		}
		k=0;
		for(j=0;j<4;j++)
		{
			for (i=0;i<4;i++)
			{
				Key[i][j]=key[k];
				state[i][j]=str[k];
				k++;
			}
		}
		addRoundKey();
		for(i=1;i<11;i++)
		{
			subBytes();
			shiftRow();
			if (i<10)
				mixColumns();
			addRoundKey();
		}
		for (i=0;i<4;i++)
		{
			for(j=0;j<4;j++)
			{
				printf(" %x ",state[j][i]);
				if (uree<lenn)
				{
					arr[uree]=state[j][i];
					uree++;
				}
			}
		}
	}
	printf("\nHence the ciphertext is: ");
	int i=0;
	for (i=0;i<lenn;i++)
		printf("%4x",arr[i]);
	return 0;
}
