#include "NBSPSC_LockFreeQueue.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <thread>
#include <unistd.h>

const int MAX_TEST = 1000000 ;

typedef struct __ST_QUEUE_DATA__
{
    int  data_index;
    char data_record[4096];

} ST_QUEUE_DATA ;


//----------------------------------------------------------------------
// TEST 
//----------------------------------------------------------------------

///////////////////////////////////////////////
LockFreeQueueNBSPSC<ST_QUEUE_DATA, 1000> gQueue ;

///////////////////////////////////////////////
void Producer()
{
    ST_QUEUE_DATA queueData;
    char szTemp[20];

    for ( int i =0; i < MAX_TEST; i++)
    {
        queueData.data_index= i;
        snprintf(szTemp, sizeof(szTemp), "data-%d", i );
        memcpy(queueData.data_record, szTemp, strlen(szTemp));

        while(1)
        {
            OPERATION_RESULT nRslt = gQueue.Push(&queueData);
            if( nRslt != QUEUE_OK )
            {
                usleep(1);
                continue;
            }
            else
            {
                break;
            }
        }
    }
}

///////////////////////////////////////////////
void Consumer()
{
    size_t nVerify = 0;

    ST_QUEUE_DATA* pQueueData;

    for ( int i =0; i < MAX_TEST; i++)
    {
        while(1)
        {
            OPERATION_RESULT nRslt = gQueue.Pop(& pQueueData) ;
            if( nRslt != QUEUE_OK )
            {
                usleep(1);
                continue;
            }
            else
            {
                if( pQueueData->data_index != nVerify )
                {
                    printf("[%s-%d] Errorr %d-%zu [%s]\n", __func__, __LINE__, pQueueData->data_index, nVerify, pQueueData->data_record ); 
                    exit(0);
                }

                nVerify++;

                gQueue.CommitPop();

                //printf("[%s]  Pop: index=%d / %s\n", __func__,  i, pQueueData->data_record ); 

                break;
            }
        }
    }
}

///////////////////////////////////////////////
int main ()
{
    std::thread ConsumerThread(Consumer);
    std::thread ProducerThread(Producer);

    ConsumerThread.join();
    ProducerThread.join();

    return 0;
}

// g++ -std=c++11 -pthread -o NBSPSC_LockFreeQueueTest NBSPSC_LockFreeQueueTest.cpp

