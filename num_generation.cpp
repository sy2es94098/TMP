#include<cstdlib>
#include<iostream>
#include<ctime>
#include<fstream>
#include<math.h>
#include<string> 

using namespace std;
#define BYTES_SIZE 8
#define DATA_SIZE 16
#define PAGE_SIZE 1
string random_num(int &size){
	int num=0;
	for(int i = 1 ; i < sizeof(int)*BYTES_SIZE ; ++i ){
		int random = rand()%2;
	        num = random == 1?(num | (1<<i)):(num | 0);
	}
	string result = to_string(num) + "\n";
	size = result.size();
	return result;
}

int main(){
	srand(time(NULL));
	ofstream file("input.txt");
	long long page = pow(2,30)*PAGE_SIZE;
	string data;
	long long size = 0;
	int round = DATA_SIZE/PAGE_SIZE;
	while(true)
	{
		int tmp = 0;
		data.append(random_num(tmp));
		size += tmp;
		if(data.size() >= page)
		{
			file << data;
			data.clear();
		}
		if(size >= page*round)
		{
			file << data;
			data.clear();
			break;
		}
	}
	file.close();
}

