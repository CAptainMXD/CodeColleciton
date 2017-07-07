//单链表结点
struct node_t
{
	void*	value_;
	node_t*	next_;
};



//队列有头结点和尾结点和两个锁
struct queue_t
{
	node_t*	head_;
	node_t* tail_;
	pthread_mutex_t	h_lock;
	pthread_mutex_t	t_lock;
};



class ConcurrentQueue
{
	public:
	       ConcurrentQueue()
	       {
		       node_t *node=new node_t();
		       node->next_=NULL;
		       queue_.head_=queue.tail_=node;
		       push_=0;
		       pop_=0;
		       pthread_mutex_init(&queue_.h_lock,NULL);
		       pthread_mutex_init(&queue_.t_lock,NULL);
	       }


	       ~ConcurrentQueue()
	       {
		       delete queue_.head_;
		       queue.head_=NULL;
		       queue.tail=NULL;
		       push_=0;
		       pop_=0;
		       pthread_mutex_destroy(&queue_.h_lock);
		       pthread_mutex_destroy(&queue_.t_lock);
	       }
	public:
	       //头出尾进,在进和出时会加锁
		bool try_pop(void *&result)
		{
			pthread_mutex_lock(&queue_.h_lock);
			node_t * node=queue_.head_;
			node_t * new_head=node->next;
			if(new_head==NULL)
			{
				pthread_mutex_unlock(&queue_.h_lock);
				return false;
			}
			result=new_head->value_;
			queue_.head_=new_head;
			pop_++;

			pthread_mutex_unlock(&queue_.h_lock);
			delete node;
			return true;

		}
		
		
		void push(void *source)
		{
			node_t*node=new node_t();
			node->value=source;
			node->next=NULL;

			pthread_mutex_lock(&queue_.t_lock);
			push++;
			queue_.tail_->next=node;
			queue_.tail_=node;	
			pthread_mutex_unlock(&queue_.t_lock);
		}

		size_t pushsize()const{return push_;}
		size_t popsize()const{return pop_;}
		int unsafe_size()const{return push_-pop_;}
	private:
		queue_t queue_;
		volatile size_t push_;
		volatile size_t pop_;
};




