/*
	是对一块缓冲区的操作，没有分配内存空间，使用的是已经分配好的内存。

*/


typedef unsigned char data_t;
typedef unsigned char * pdata_t;

class ByteStream
{
public:
	ByteStream(pdata_t data,size_t bufsize)
		:data_(data)
		,cur_(0)
		,bufszie_(bufsize)
	{}
	
	ByteStream(const ByteStream & stream)
		:data_(stream.data_)
		,cur_(stream.cur_)
		,bufsize_(stream.bufsize_)
	{}
	
	~ByteStream()
	{
		data_ = NULL;
		cur_ = 0;
	}

protected:
	ByteStream()
	:data_(NULL)
	,cur_(0)
	,bufsize_(0){}


public:
	bool Write(const void *pIn,size_t size)
	{
		if(pIn==NULL) return false;
		if(size+cur_>bufsize_) return false;

		pdata_t pcur=(data_+cur_);
		memcpy((void*)pcur,pIn,size);
		cur_+=size;
		return true;
	}
	
	bool Read(void *pOut,size_t size)
	{
		if(pOut==NULL) return false;
		if(size+cur_ > bufsize_) return false;
		
		pdata_t pcur=(data_+cur_);
		memcpy((void*)pOut,(const void*)pcur,size);
		cur_+=size;
		return true;
	}
	//获取一段数据
	void GetData(pdata_t &data,size_t &cur,size_t size)const
	{
		data=data_;
		cur=cur_;
		size=bufsize_;
	}

public:
	inline void Empty(cur_=0;)
	inline bool IsEmpty const(){ return (data_==NULL||bufsize_==0);}
	inline bool IsReadEnd const(){ return cur_==bufszie_;}
	inline pdata_t GetBuff()const{return data_;}
	inline size_t GetCur()const{return cur_;}
	inline size_t GetBuffSize()const {return bufsize_;}
protected:
	//这个就是缓冲区指针
	pdata_t data_;
	//当前下标
	size_t cur_;
	//缓冲区大小
	size_t bufsize_;	
};

class ByteStreamPool
{
public:
	ByteStreamPool()
	{
		for(size_t i=0;i<buf_num_;++i)
		{
			//这样把整个内存空间给分割成几个独立的空间
			SData data;
			data.pdata_=buffer_+i*per_buf_size_;
			data.size_=per_buf_size_;		
			vecbuf_.push(data);
		}
	}
	~ByteStreamPool(){}
	
	bool PopByteStreamBuf(pdata_t& data, size_t& size)
	{
		if(vecbuf_.empty())return false;
		const SData &buf=vecbuf_.top();
		vecbuf_.pop();
		data=buf.pdata_;
		size=buf.size_;
		return true;
	}
	bool PushByteStreamBuf(pdata_t& data, size_t& size)
	{
		if(vecbuf_.size()>buf_num_)return false;
		SData buf;
		buf.pdata_=data;
		buf.size_=size;
		vecbuf_.push(buf);
		return true;
	}
private:
	struct SData
	{
		pdata_t pdata_;
		size_t size_;
	}
	typedef std::stack<SData> vec_buffer_;
private:
	static const size_t per_buf_size_ =1024 * 1024 * 8;
	static const size_t buf_num_=5;
private:
	data_t			buffer_[per_buf_size_ * buf_num_];
	vec_bufferT_	vecbuf_;
};















