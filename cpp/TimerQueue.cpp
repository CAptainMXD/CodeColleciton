//引用计数指针
//这个模板参数表示是否是多线程
template<bool MultiThread> class Refcounted
{};


//特化  不需要多线程
template<> class Refcounted<false>
{
	public:
		Refcounted()
		{
			m_nRefCnt = 0;
		}

		virtual ~Refcounted()
		{
		}

		virtual int DoRef()
		{
			++m_nRefCnt;
			return 0;
		}

		virtual int UnRef()
		{
			--m_nRefCnt;
			if(m_nRefCnt == 0)
				delete this;
			return 0;
		}
	private:
		int m_nRefCnt;
};



template<class T>
class RefcountedPtr
{
	public:
		RefcountedPtr(T *v=NULL)
			:m_pObj(v)
		{
			if(m_pObj!=NULL)
				m_pObj->DoRef();

		}


		RefcountedPtr(const RefcountedPtr<T>& v)
		{
			m_pObj = v.m_pObj;
			if(m_pObj != NULL)
				m_pObj->DoRef();
		}

		~RefcountedPtr()
		{
			if(m_pObj != NULL)
				m_pObj->UnRef();
			m_pObj = NULL;
		}

		T* Get()
		{
			return m_pObj;
		}

		operator T*()
		{
			return m_pObj;
		}

		T* operator ->()
		{
			return m_pObj;
		}

		operator T&()
		{
			return *m_pObj;
		}

		RefcountedPtr<T>& operator =(const RefcountedPtr<T>& v)
		{
			if(m_pObj == v.m_pObj)
				return *this;

			if(m_pObj != NULL)
				m_pObj->UnRef();
			m_pObj = v.m_pObj;
			if(m_pObj != NULL)
				m_pObj->DoRef();
			return *this;
		}


	public:
		void Reset(T* obj = NULL)
		{
			if(m_pObj == obj)
				return;
			if(m_pObj != NULL)
				m_pObj->UnRef();
			m_pObj = obj;
			if(m_pObj != NULL)
				m_pObj->DoRef();
		}
	private:
		T *m_pObj;
};


extern unsigned int g_TimerNum;
class Timer : public Refcounted<false>
{
	public:
		Timer()
			:m_nExpiredTime(0)
			 , m_nPeriodTime(0)
			 , m_pfnTimer(NULL)
			 , m_pContext(NULL)
			 , m_bCanceled(false) 
	{
		g_TimerNum++;
	}


		virtual ~Timer() 
		{
			g_TimerNum--;
		};

		int Execute()
		{
			return m_pfnTimer ? m_pfnTimer(this,m_pContext):-1;
		}
	public:
		typedef RefcountedPtr<Timer> TRef;
		typedef int(*pfnTimer)(void *timer,void *context);
	public:
		long long  m_nExpiredTime;
		long long  m_nPeriodTime;
		pfnTimer m_pfnTimer;
		void* m_pContext;
		bool m_bCanceled;
};


template<class T>
class ObjectTimer :public Timer
{
	public:
		typedef int (T::*pfnObjectTimer)(void *,void *);
	public:
		static int TimerFuncProxy(void *timer,void *context)
		{
			ObjectTimer<T> *pObjectTimer=(ObjectTimer<T>*)context;
			return (pObjectTimer->m_Object->*pObjectTimer->m_pfnObjectTimer)(timer,pObjectTimer->m_pObjectContext);
		}
	public:

		T *m_Object;
		pfnObjectTimer m_pfnObjectTimer;
		void *m_pObjectContext;

};
template<> struct less<mars_time::Timer*> : public binary_function<mars_time::Timer*, mars_time::Timer*, bool>
{
	bool operator()(mars_time::Timer*& l, mars_time::Timer*& r)
	{
		if(l->m_nExpiredTime == r->m_nExpiredTime)
			return l > r;
		return l->m_nExpiredTime > r->m_nExpiredTime;
	}
};



class TimerQueue
{
	public:
		TimerQueue();
		virtual ~TimerQueue();

	public:
		int CreateTimer(Timer::TRef* result,sint64 time,sint64 period,Timer::pfnTimer pfn,void* context);
		template<class T>
			int CreateTimer(Timer::TRef* result,sint64 time,sint64 period,T* obj,typename ObjectTimer<T>::pfnObjectTimer pfn,void* context)
			{
				ObjectTimer<T>* timer = new ObjectTimer<T>();
				timer->m_bCanceled = false;
				timer->m_nExpiredTime = mars_time::get() + time;
				timer->m_nPeriodTime = period;
				timer->m_Object = obj;
				timer->m_pfnObjectTimer = pfn;
				timer->m_pObjectContext = context;
				timer->m_pContext = timer;
				timer->m_pfnTimer = &ObjectTimer<T>::TimerFuncProxy;
				timer->DoRef();

				if(result)
				{
					(*result).Reset(timer);
				}
				m_deqTimers.push(timer);
				return 0;
			}
		int CancelTimer(Timer::TRef& ref)
		{
			if(ref.Get() != NULL && ref->m_bCanceled == false)
			{
				ref->m_bCanceled = true;
				//ref->UnRef();
				return 0;
			}
			return -1;
		}
	public:
		int Update(sint64 currtick = -1)
		{
			sint64 nCurrTick = currtick > 0 ? currtick : mars_time::get();
			while(!m_deqTimers.empty())
			{
				Timer* pTimer = m_deqTimers.top();
				if(nCurrTick < pTimer->m_nExpiredTime)
					break;

				m_deqTimers.pop();
				if (pTimer->m_bCanceled)
				{
					pTimer->UnRef();
					continue;
				}

				pTimer->Execute();
				if(pTimer->m_nPeriodTime != 0 && pTimer->m_bCanceled == false)
				{
					pTimer->m_nExpiredTime = nCurrTick + pTimer->m_nPeriodTime;
					m_deqTimers.push(pTimer);
				}
				else
				{
					pTimer->UnRef();
				}
			}
			return 0;


		}
		int Cleanup()
		{
			while(!m_deqTimers.empty())
			{
				Timer* pTimer = m_deqTimers.top();
				pTimer->UnRef();
				m_deqTimers.pop();
			}
			return 0;
		}
	private:
		typedef ::std::priority_queue<Timer*> CollectionDeqTimersT;
		CollectionDeqTimersT m_deqTimers;


};






