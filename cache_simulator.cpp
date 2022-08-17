#include <iostream>
#include  <iomanip>
#include  <math.h>
#include <vector>

using namespace std;

#define		DBG				1
#define		DRAM_SIZE		(64*1024*1024)
#define		CACHE_SIZE		(64*1024)
#define		CASHE_LINE_SIZE	(16)
#define		NO_WAYS			(4)


int no_lines = (CACHE_SIZE / CASHE_LINE_SIZE) / NO_WAYS; // 4  8 lines  --> 4
int bits_index = log2(no_lines); // 2 3 bits
int no_byte_offset = log2(CASHE_LINE_SIZE); // 2  3


// First ---> Validity Bit ,  Second ----> TAG
vector < vector<pair<int, int>>> set_associative_cache(no_lines, vector< pair<int, int>>(NO_WAYS, { 0,0 }));

enum cacheResType { MISS = 0, HIT = 1 };

/* The following implements a random number generator */
unsigned int m_w = 0xABCCAB99;    /* must not be zero, nor 0x464fffff */
unsigned int m_z = 0xDEAD6902;    /* must not be zero, nor 0x9068ffff */
unsigned int rand_()
{
	m_z = 36969 * (m_z & 65535) + (m_z >> 16);
	m_w = 18000 * (m_w & 65535) + (m_w >> 16);
	return (m_z << 16) + m_w;  /* 32-bit result */
}


unsigned int memGen6()
{
	static unsigned int addr=0;
	return (addr+=32)%(64*4*1024);
}



struct cache_line
{
	bool validity;
	int index;
	int tag;
};

unsigned int memGenA()
{
	static unsigned int addr = 0;
	return (addr++) % (DRAM_SIZE);
}

unsigned int memGenB()
{
	static unsigned int addr = 0;
	return  rand_() % (64 * 1024);
}

unsigned int memGenC()
{
	static unsigned int a1 = 0, a0 = 0;
	a0++;
	if (a0 == 512) { a1++; a0 = 0; }
	if (a1 == 128) a1 = 0;
	return(a1 + a0 * 128);
}

unsigned int memGenD()
{
	return  rand_() % (16 * 1024);
}

unsigned int memGenE()
{
	static unsigned int addr = 0;
	return (addr++) % (1024 * 64);
}

unsigned int memGenF()
{
	static unsigned int addr = 0;
	return (addr += 64) % (64 * 4 * 1024);
}


// Direct Mapped Cache Simulator
cacheResType cacheSimDM(unsigned int addr)
{
	// This function accepts the memory address for the memory transaction and
	// returns whether it caused a cache miss or a cache hit

	// The current implementation assumes there is no cache; so, every transaction is a miss
	cache_line L;
	// Exclude offset bits
	int temp_addr = addr >> no_byte_offset;
	// Get index bits
	L.index = temp_addr % (1 << bits_index);
	temp_addr /= (1 << bits_index);
	// Get tag bits
	L.tag = temp_addr;

	int i = 0;
	for (; i < NO_WAYS; i++)
	{
		if (set_associative_cache[L.index][i].first)
		{
			if (set_associative_cache[L.index][i].second == L.tag)
				return HIT;
		}
		else break;

	}
	// This case means we will replace the fisrt address in the set by the new address
	if (i == NO_WAYS)
	{
		set_associative_cache[L.index][rand() % NO_WAYS].second = L.tag;
	}
	// This case means that there is still empty place in the set to store the address
	else
	{
		set_associative_cache[L.index][i].first = 1;
		set_associative_cache[L.index][i].second = L.tag;
	}
	return MISS;
}


char* msg[2] = { (char*)"Miss", (char*)"Hit" };

#define		NO_OF_Iterations	5	// CHange to 1,000,000
int main()
{
	unsigned int hit = 0;
	cacheResType r;

	unsigned int addr;
	cout << "Cache Simulator\n";

	for (int inst = 0; inst < NO_OF_Iterations; inst++)
	{
		// comment when testing
		addr = memGen6();
		// Uncomment in testing to input addresses
		//cin>>addr;
		r = cacheSimDM(addr);
		if (r == HIT) hit++;
		//cout << "0x" << setfill('0') << setw(8) << hex << addr << " (" << msg[r] << ")\n";
	}

    // validation --> Uncomment when testing
    /*
	for(int i=0; i<no_lines; i++){
        cout << "index " << i << ": ";
        for(int j=0; j<NO_WAYS; j++){
            cout<< "v: "<< set_associative_cache[i][j].first << " tag: "<< set_associative_cache[i][j].second << "  ,  ";
        }
        cout<<endl;
	}
    */
	cout << "Hit ratio = " << (float)(100.0 * hit / NO_OF_Iterations) <<" %" <<endl;
	cout << "Miss ratio = " << 100.0 - (float)(100.0 * hit / NO_OF_Iterations) <<" %" <<endl;
}
