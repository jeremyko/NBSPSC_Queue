//Single Producer Single Consumer lock free queue using array
//Non-blocking operation

#include <atomic>

//const int MAX_ARRAY_CNT = 1000;

typedef enum __OPERATION_RESULT__
{
    QUEUE_OK  = 0,
    QUEUE_FULL  ,
    QUEUE_EMPTY

} OPERATION_RESULT;

template <typename T, int MAX_ARRAY_CNT>
class LockFreeQueueNBSPSC
{
    private:
        std::atomic<size_t> nWriteIndex_ ;
        std::atomic<size_t> nReadIndex_ ;
        T arrayOfData_[MAX_ARRAY_CNT];
        const int ST_QUEUE_DATA_LEN = sizeof (T);

    public:

        LockFreeQueueNBSPSC()
        {
            nWriteIndex_ = 0;
            nReadIndex_  = 0;
        }

        ///////////////////////////////////////////////////////////
        OPERATION_RESULT Push(T* pQueuedata )
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

            memcpy( & arrayOfData_[nWriteIndex_], pQueuedata, ST_QUEUE_DATA_LEN);
            
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

            *ppQueuedata = & arrayOfData_[nReadIndex_];
            
            return QUEUE_OK ;
        }

        ///////////////////////////////////////////////////////////
        void CommitPop()
        {
            nReadIndex_++ ;
        }

};


