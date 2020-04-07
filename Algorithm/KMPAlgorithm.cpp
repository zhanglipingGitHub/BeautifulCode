#include<iostream>
#include<string>

using namespace std;

unique_ptr<int[]> CalNextArray(const string&);

bool KMPAlgorithm(const string& str, const string& pat)
{
	int sLength = str.length();
	int pLength = pat.length();

	
	unique_ptr<int[]> pNextArray = CalNextArray(pat);

	int i = 0, j = 0;
	while (i < sLength && j < pLength)
	{
		if (j == -1 || str[i] == pat[j])
		{
			i++;
			j++;
		}
		else
		{
			j = pNextArray[j];
		}
	}
	
	return j == pLength;
}

unique_ptr<int[]> CalNextArray(const string& pattern)
{
	int pLength = pattern.length();
	unique_ptr<int[]> pNextArray(new int[pLength]);

	pNextArray[0] = -1;

	//当前计算next值的索引
	int j = 0;
	//当前索引的next值
	int k = -1;

	//计算截止 j 位置的最长相同前后缀，然后赋值给索引 j+1 的next值，所以 j 只计算到倒数第二位
	//比如说有字符串 abcabca，计算 j = 5的字符串 abcabc 的最长相同前后缀，然后赋值给 next[6]
	while (j < pLength - 1)
	{
		if (k == -1 || pattern[j] == pattern[k])
		{
			j++;
			//当前长度 + 1
			k++;

			//本来 pNextArray[j] 应该直接 = k 的，但是这里做了一个优化。
			//当在 j 处失配时，本来应该根据 next 数组将 j 移动到 k 处，
			//但是，如果 pattern[j] == pattern[k]，那么移动到 k处必然仍会导致失配，
			//所以 j 往前继续移动，令 pNextArray[j] = pNextArray[k]
			pNextArray[j] = pattern[j] == pattern[k] ? pNextArray[k] : k;
		}
		else
		{
			k = pNextArray[k];
		}
	}

	return pNextArray;
}