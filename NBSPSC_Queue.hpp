//Single Producer Single Consumer queue using vector
//Non-blocking operation

#include <atomic>
#include <vector>

#define CACHE_LINE_SIZE 64 

///////////////////////////////////////////////////////////////////////////////
typedef enum __OPERATION_RESULT__
{
    QUEUE_OK  = 0,
    QUEUE_FULL  ,
    QUEUE_EMPTY

} OPERATION_RESULT;

///////////////////////////////////////////////////////////////////////////////
template <typename T, int MAX_ARRAY_CNT>
class NBSPSC_Queue
{
    private:
        std::atomic<size_t> nWriteIndex_ alignas(CACHE_LINE_SIZE);
        std::atomic<size_t> nReadIndex_  alignas(CACHE_LINE_SIZE) ;
        std::vector<T> vec_data_         alignas(CACHE_LINE_SIZE);
        const int ST_QUEUE_DATA_LEN = sizeof (T);

    public:

        //-------------------------------------------------------------------
        NBSPSC_Queue()
        {
            nWriteIndex_ = 0;
            nReadIndex_  = 0;
            vec_data_.reserve(MAX_ARRAY_CNT);
        }

        //-------------------------------------------------------------------
        OPERATION_RESULT Push(T& queue_data )
        {
            size_t temp_read_index = nReadIndex_ ;

            if ( nWriteIndex_  >= MAX_ARRAY_CNT )
            {
                if( temp_read_index == 0 )
                {
                    return QUEUE_FULL; 
                }
                else
                {
                    nWriteIndex_ = 0 ;
                }
            }

            if ( nWriteIndex_ < temp_read_index )
            {
                if ( nWriteIndex_ +1  >= temp_read_index )
                {
                    return QUEUE_FULL; 
                }
            }

            vec_data_[nWriteIndex_] = queue_data ;
            
            nWriteIndex_++  ;

            return QUEUE_OK ;
        }

        //-------------------------------------------------------------------
        OPERATION_RESULT Pop(T** ppQueuedata)
        {
            size_t temp_write_index = nWriteIndex_ ;

            if (temp_write_index == nReadIndex_)
            {
                return QUEUE_EMPTY ;
            }
            else if ( nReadIndex_ >= MAX_ARRAY_CNT )
            {
                if( temp_write_index == 0 )
                {
                    return QUEUE_EMPTY ; 
                }
                
                nReadIndex_ = 0 ;
            }

            *ppQueuedata = & vec_data_[nReadIndex_];
            
            return QUEUE_OK ;
        }

        //-------------------------------------------------------------------
        void CommitPop()
        {
            nReadIndex_++ ;
        }

        //-------------------------------------------------------------------
        int GetQueueTotalCapacity()
        {
            return vec_data_.capacity() ;
        }
};


