#include "NBSPSC_Queue.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <thread>
#include <unistd.h>
#include <iostream>

const int MAX_TEST = 100 ;

typedef struct __ST_QUEUE_DATA__
{
    int  data_index;
    char data_record[4096];

} ST_QUEUE_DATA ;


//----------------------------------------------------------------------
// TEST 
//----------------------------------------------------------------------

///////////////////////////////////////////////
NBSPSC_Queue<ST_QUEUE_DATA, 10> gQueue ;

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
            OPERATION_RESULT nRslt = gQueue.Push(queueData);
            if( nRslt == QUEUE_FULL )
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
            if( nRslt == QUEUE_EMPTY )
            {
                usleep(1);
                continue;
            }
            else
            {
                if( pQueueData->data_index != nVerify )
                {
                    std::cerr << "Errorr : " << pQueueData->data_index << "/" << nVerify << "/" << pQueueData->data_record << "\n" ; 
                    exit(0);
                }

                nVerify++;

                gQueue.CommitPop();

                std::cout << "Pop: index=" << i << " / " << pQueueData->data_record << "\n" ; 

                break;
            }
        }
    }
}

///////////////////////////////////////////////
int main ()
{
    std::cout<< "Queue capacity :" << gQueue.GetQueueTotalCapacity() <<"\n";

    std::thread ConsumerThread(Consumer);
    std::thread ProducerThread(Producer);

    ConsumerThread.join();
    ProducerThread.join();

    return 0;
}

// g++ -std=c++11 -pthread -o test test.cpp

