template<int N=32,typename T =unsigned char> 
class BitSet
{
	public:
		BitSet()
		{
			//初始化为0
			memset(m_szBits,0,sizeof(m_sBits));
			m_bBitsDirty=false;
		}

		inline int set(size_t i,bool set)
		{
			if(i<0||i>=N) return -1;
			//如果set为true就加
			if(set)
			{
				++m_szBits[i];
			}
			else
			{
				if(m_szBits[i]>0) --m_szBits[i];
			}

			m_bBitsDirty=true;
			return 0;
		}


		inline int Get(size_t i) const 
		{
			if(i<0||i>=N) return -1;
			return m_szBits[i]>0;	
		}


		inline int GetValue(size_t i)const 
		{
			if(i<0||i>=N) return -1;
			return m_szBits[i];
		}

		inline int SetValue(size_t i,int val)
		{
			m_szBits[i]=val;
			m_bBitsDirty=true;
			return 0;
		}

	        inline void Clean()
		{
			memset(m_szBits,0,sizeof(m_szBits));
			m_bBitsDirty = false;
		}

		inline bool IsDirty() const
		{
			return m_bBitsDirty;
		}

		inline bool IsZero() const
		{
			static T s_szBits[N] = {0};
			return memcmp(s_szBits, m_szBits, sizeof(s_szBits)) == 0 ? true : false;
		}	
	private:
		//用来存储状态
		T m_szBits[N];

		//表示状态改变
		bool m_bBitsDirty;
};





template<int N = 32> 
class BitSetOne
{
	public:

			BitSetOne()
		{
			memset(m_nStates,0,sizeof(m_nStates));
		}

	public:
		inline bool Get(size_t i) const
		{
			if(i>=N) return false;
			int bit=i%32;
			int idx=i/32;
				//1左移的位数，前后都是0 ?
			return m_nStates[idx] & (1<<bit);
		}
		
		inline void Set(size_t i,bool set)
		{
			if(i>=N) return false;
			int bit=i%32;
			int idx=i/32;
			if(set)
			 m_nStates[idx] |= (1<<bit);
			else
			 m_nStates[idx] &= ~(1<<bit);
		}
		
		inline void Clean()
		{
			memset(m_nStates,0,sizeof(m_nStates));
		}
			
		inline bool IsZero() const
		{
			static unsigned int s_nStates[(N/32)]={0};
		    return memcmp(m_nStates, s_nStates, sizeof(s_nStates)) == 0 ? true : false;
		}
	private:
		unsigned int m_nStates[(N/32)];
};


