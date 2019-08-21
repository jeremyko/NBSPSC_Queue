//Single Producer Single Consumer queue using vector
//Non-blocking operation

#include <atomic>
#include <vector>

const size_t CACHE_LINE_SIZE = 64 ; 

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
        std::atomic<std::size_t> write_index_ alignas(CACHE_LINE_SIZE);
        std::atomic<std::size_t> read_index_  alignas(CACHE_LINE_SIZE) ;
        std::vector<T> vec_data_         alignas(CACHE_LINE_SIZE);
        const int ST_QUEUE_DATA_LEN = sizeof (T);
    public:
        //-------------------------------------------------------------------
        NBSPSC_Queue() {
            write_index_ = 0;
            read_index_  = 0;
            vec_data_.reserve(MAX_ARRAY_CNT);
        }
        //-------------------------------------------------------------------
        OPERATION_RESULT Push(T& queue_data ) {
            std::size_t temp_read_index = read_index_ ;
            if ( write_index_  >= MAX_ARRAY_CNT ) {
                if( temp_read_index == 0 ) {
                    return QUEUE_FULL; 
                } else {
                    write_index_ = 0 ;
                }
            }
            if ( write_index_ < temp_read_index ) {
                if ( write_index_ +1  >= temp_read_index ) {
                    return QUEUE_FULL; 
                }
            }
            vec_data_[write_index_] = queue_data ;
            write_index_++  ;
            return QUEUE_OK ;
        }
        //-------------------------------------------------------------------
        OPERATION_RESULT Pop(T** out_data) {
            std::size_t temp_write_index = write_index_ ;
            if (temp_write_index == read_index_) {
                return QUEUE_EMPTY ;
            } else if ( read_index_ >= MAX_ARRAY_CNT ) {
                if( temp_write_index == 0 ) {
                    return QUEUE_EMPTY ; 
                }
                read_index_ = 0 ;
            }
            *out_data = & vec_data_[read_index_];
            return QUEUE_OK ;
        }
        //-------------------------------------------------------------------
        void CommitPop() {
            read_index_++ ;
        }
        //-------------------------------------------------------------------
        int GetQueueTotalCapacity() {
            return vec_data_.capacity() ;
        }
};


