#define MY_MIN(a, b) ((a) < (b) ? (a) : (b))
#define MY_MAX(a, b) ((a) > (b) ? (a) : (b))
#include<string.h>
#include<stdlib.h>



/*  the circle buffer , we judge the buffer is empty by m_datasize==0
 *
 */

/*four types
 *
 *type:           [1]
 *iter:     begin(2)     end(8)
 *            |           |
 *data:   _ _ * * * * * * _ _ _ 
 *buffer: _ _ _ _ _ _ _ _ _ _ _ 
 *index:  0 1 2 3 4 5 6 7 8 9 10
 *
 *
 *type:           [2]
 *iter:       end(2)   begin(7)
 *            |         |
 *data:   * * _ _ _ _ _ * * * *  
 *buffer: _ _ _ _ _ _ _ _ _ _ _ 
 *index:  0 1 2 3 4 5 6 7 8 9 10
 *
 *
 *type:           [3] empty
 *iter:      begin(4),end(4)
 *                |
 *data:   _ _ _ _ _ _ _ _ _ _ _ 
 *buffer: _ _ _ _ _ _ _ _ _ _ _ 
 *index:  0 1 2 3 4 5 6 7 8 9 10
 *
 *
 *type:           [4] full
 *iter:      begin(4),end(4)
 *                |
 *data:   * * * * * * * * * * * 
 *buffer: _ _ _ _ _ _ _ _ _ _ _ 
 *index:  0 1 2 3 4 5 6 7 8 9 10
 *
 * */

struct circle_buffer
{
    void ini(int len)
    {
        m_size=len;
        m_buffer=(char *)malloc(len);
        m_datasize=0;
        m_begin=0;
        m_end=0;
        m_store_begin=0;
        m_store_end=0;
        m_store_datasize=0;
    }

    void fini()
    {
       free(m_buffer);
    }
    
    bool can_write(int size)
    {
        return m_datasize+size<=m_size;
    }

    bool can_read(int size)
    {
        return m_datasize>=size;
    }
    

    void skip_write(int size)
    {
        m_datasize+=size;
        m_end+=size;
        if (m_end>=m_size) {
            m_end-=m_size;
        }
    }


    bool write(const char *p,int size)
    {
        if(!can_write(size))
        {
            return false;
        }
        //first write data to buffer
        real_write(p,size);

        //second  change the index 
        skip_write(size);
        return true;
    }

    void skip_read(int size)
    {
        m_datasize-=size;
        m_begin+=size;
        
        if(m_begin>=m_size)
        {
            m_begin-=m_size;
        }
        
        //if m_datasize == 0 mean the buffer is empty
        if(!m_datasize)
        {
            m_begin=0;
            m_end=0;
        }
    }
      
    bool read(char *out,int size)
    {
        if(!can_read(size))
        {
            return false;
        }
        real_read(out,size);
        skip_read(size);
        return true;
    }
    
    void store()
    {
        m_store_datasize=m_datasize;
        m_store_begin=m_begin;
        m_store_end=m_end;
    }

    void restore()
    {
        m_datasize=m_store_datasize;
        m_begin=m_store_begin;
        m_end=m_store_end;
    }

    bool clear()
    {
        m_datasize=0;
        m_begin=0;
        m_end=0;
        return true;
    }

    int size() const
    {
        return m_datasize;
    }

    int capacity() const
    {
        return m_size;
    }

    bool empty()const
    {
        return m_datasize==0;
    }

    bool full()const
    {
        return m_datasize==m_size;
    }

    char *get_read_line_buffer()
    {
        return m_buffer+m_begin;
    }

    int get_read_line_size()
    {
        return MY_MIN(m_datasize,m_size-m_begin);
    }

    char *get_write_line_buffer()
    {
        return m_buffer+m_end;
    }

    int get_write_line_size()
    {
        return MY_MIN(m_size-m_datasize,m_size-m_end);
    }

private:
    void real_write(const char *p,int size)
    {
        
        if (m_end>=m_begin) //[1][3]
        {
            //can store
            if (m_size-m_end>=size) 
            {
                memcpy(m_buffer+m_end,p,size*sizeof(char));
            }
            else
            {
               //first store the data to the last space 
               memcpy(m_buffer+m_end,p,(m_size-m_end)*sizeof(char));
               //second store the no store dat to the space 
               memcpy(m_buffer,p+(m_size-m_end),(size-(m_size-m_end))*sizeof(char));
            }
        }
        else //[2]
        {
            memcpy(m_buffer+m_end,p,size*sizeof(char));
        }
    }


    void real_read(char *out,int size)
    {
        if (m_begin>=m_end) 
        {
             if (m_size-m_begin>=size) 
             {
                 memcpy(out,m_buffer+m_begin,size*sizeof(char));
             }
             else 
             {
                 memcpy(out, m_buffer + m_begin, (m_size - m_begin) * sizeof(char));
                 memcpy(out + (m_size - m_begin), m_buffer, (size - (m_size - m_begin)) * sizeof(char));
             }
        }
        else 
        {
          memcpy(out,m_buffer+m_begin,size*sizeof(char));   
        }

    }


private:
    char *m_buffer;  //the buffer ptr
    int m_size;      //buffer size
    
    int m_datasize;  //the size of storage data
    int m_begin;     
    int m_end;

    int m_store_datasize; 
    int m_store_begin;
    int m_store_end;
};


