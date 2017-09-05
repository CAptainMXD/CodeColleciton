const size_t c_pool_max_size = 100 * 1024;
const size_t c_pool_hashstep = 32;
const size_t c_pool_hasharray = c_pool_max_size / c_pool_hashstep;
const uint32_t c_pool_magic_head = 0xDEADBEEF;
const uint32_t c_pool_magic_tail = 0xBADDDADD;
const uint32_t c_pool_pagesize = 4096;

static inline void * SLL_Next(void *t) 
{
	return *(reinterpret_cast<void**>(t));
}

static inline void SLL_SetNext(void *t, void *n) 
{
	*(reinterpret_cast<void**>(t)) = n;
}

static inline void SLL_Push(void **list, void *element) 
{
	SLL_SetNext(element, *list);
	*list = element;
}

static inline void * SLL_Pop(void **list) 
{
	void * result = *list;
	*list = SLL_Next(*list);
	return result;
}

extern int g_pool_enable;
extern int g_pool_total_malloc;
extern int g_pool_size;
extern int g_pool_malloc_num;


class PoolList
{
public:
	PoolList() 
		: m_list(0), 
		m_use_num(0), 
		m_free_num(0), 
		m_total_num(0),
		m_ele_size(0)
	{

	}
	~PoolList()
	{

	}
	
	void Fetch(size_t size)
	{
		size_t pagealign=((m_ele_size * size) + c_pool_pagesize) & (~(c_pool_pagesize - 1));
		size=pagealign / m_ele_size;
		
		void * p = mmap(0, 3 * pagealign, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		memset(p, 0xFF, 3 * pagealign);
		mprotect(p, c_pool_pagesize, PROT_NONE);
		mprotect((char *)p + c_pool_pagesize, c_pool_pagesize, PROT_READ | PROT_WRITE);
		mprotect((char *)p + c_pool_pagesize + pagealign, c_pool_pagesize, PROT_NONE);
		p = (char *)p + c_pool_pagesize;
		g_pool_total_malloc += pagealign;
		assert(p);
		if (!p)
		{
			return;
		}
		for (size_t i = 0; i < size; i++)
		{
			SLL_Push(&m_list, p);
			p = ((int8_t*)p + m_ele_size);
			m_free_num++;
		}
		m_total_num += size;
		
	}
	

	void Setele(size_t size)
	{
		m_ele_size = size;
	}

	size_t Getele() const
	{
		return m_ele_size;
	}

	bool Empty() const
	{
		return m_list == 0;
	}

private:
	void * m_list;
	size_t m_use_num;
	size_t m_free_num;
	size_t m_total_num;
	size_t m_ele_size;
};










struct PoolAlign
{
	size_t begin;
	size_t end;
	size_t align;
};

static const PoolAlign g_pool_align[] = {
	{1,256,32},
	{256+1,1024,64},
	{1024+1,4*1024,128},
	{4*1024+1,16*1024,256},
	{16*1024+1,64*1024,512},
	{64*1024+1,256*1024,1024},
	{256*1024+1,size_t(-1),2048}
};

class Pool
{
public:
	Pool()
	{
		for(size_t i=1;i<c_pool_max_size;i++)
		{
			int index=0;
			for(int j=0;j<_countof(g_pool_align);j++)
			{
				if(g_pool_align[j].begin<=i&&i<=g_pool_align[j].end)
				{
					index+=(i-g_pool_align[j].begin)/g_pool_align[j].align;
					break;
				}
				index+=(g_pool_align[j].end-g_pool_align[j].begin)/g_pool_align[j].align;
			        m_size_index[i / c_pool_hashstep] = index;
                            
			}
			
		}
		
	}
private:

private:
	size_t m_size_index[c_pool_hasharray];
	PoolList m_pool_list[c_pool_hasharray];

};


template <typename T>
class OPool
{
public:
	T *New()
	{
		if(g_pool_enable)
		{
			T * _object=(T*)P
		}
	}

};
