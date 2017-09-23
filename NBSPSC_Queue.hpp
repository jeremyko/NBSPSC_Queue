//Single Producer Single Consumer queue using vector
//Non-blocking operation

#include <atomic>
#include <vector>
#include <cstddef>

typedef enum __OPERATION_RESULT__
{
    QUEUE_OK  = 0,
    QUEUE_FULL  ,
    QUEUE_EMPTY

} OPERATION_RESULT;

template <typename T, int MAX_ARRAY_CNT>
class NBSPSC_Queue
{
    private:
        std::atomic<size_t> nWriteIndex_ ;
        std::atomic<size_t> nReadIndex_ ;
        std::vector<T> vec_data_ ;
        const int ST_QUEUE_DATA_LEN = sizeof (T);

    public:

        NBSPSC_Queue()
        {
            nWriteIndex_ = 0;
            nReadIndex_  = 0;
            vec_data_.reserve(MAX_ARRAY_CNT);
        }

        ///////////////////////////////////////////////////////////
        OPERATION_RESULT Push(T& queue_data )
        {
            if ( nWriteIndex_  >= MAX_ARRAY_CNT )
            {
                if( nReadIndex_ == 0 )
                {
                    return QUEUE_FULL; 
                }

                nWriteIndex_ = 0 ;

                return QUEUE_FULL; 
            }
            else if ( nWriteIndex_ < nReadIndex_ )
            {
                if ( nWriteIndex_ +1  >= nReadIndex_ )
                {
                    return QUEUE_FULL; 
                }
            }

            vec_data_[nWriteIndex_] = queue_data ;
            
            nWriteIndex_++  ;

            return QUEUE_OK ;
        }

        ///////////////////////////////////////////////////////////
        OPERATION_RESULT Pop(T** ppQueuedata)
        {
            if (nWriteIndex_ == nReadIndex_)
            {
                return QUEUE_EMPTY ;
            }
            else if ( nReadIndex_ >= MAX_ARRAY_CNT )
            {
                if( nWriteIndex_ == 0 )
                {
                    return QUEUE_EMPTY ; 
                }
                
                nReadIndex_ = 0 ;
            }

            *ppQueuedata = & vec_data_[nReadIndex_];
            
            return QUEUE_OK ;
        }

        ///////////////////////////////////////////////////////////
        void CommitPop()
        {
            nReadIndex_++ ;
        }

        ///////////////////////////////////////////////////////////
        int GetQueueTotalCapacity()
        {
            return vec_data_.capacity() ;
        }

};


