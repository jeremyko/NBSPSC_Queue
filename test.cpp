#include "NBSPSC_Queue.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <thread>
#include <unistd.h>
#include <iostream>

//-----------------------------------------------------------------------------
// TEST 
//-----------------------------------------------------------------------------

const int MAX_TEST = 1000000 ;

std::chrono::microseconds sleep_duration_microsec (1);

typedef struct __ST_QUEUE_DATA__
{
    int  data_index;
    char data_record[4096];

} ST_QUEUE_DATA ;

///////////////////////////////////////////////////////////////////////////////
NBSPSC_Queue<ST_QUEUE_DATA, 10> gQueue ;

///////////////////////////////////////////////////////////////////////////////
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
                std::this_thread::sleep_for(sleep_duration_microsec);
                continue;
            }
            else
            {
                break;
            }
        }
    }
    std::cout<< "Producer Done\n" ;
}

///////////////////////////////////////////////////////////////////////////////
void Consumer()
{
    std::size_t nVerify = 0;

    ST_QUEUE_DATA* pQueueData;

    for ( int i =0; i < MAX_TEST; i++)
    {
        while(1)
        {
            OPERATION_RESULT nRslt = gQueue.Pop(& pQueueData) ;
            if( nRslt == QUEUE_EMPTY )
            {
                std::this_thread::sleep_for(sleep_duration_microsec);
                continue;
            }
            else
            {
                if( pQueueData->data_index != nVerify )
                {
                    std::cerr << "Pop errorr : data_index=" 
                        << pQueueData->data_index << "/ nVerify=" << nVerify 
                        << " / data_record=" 
                        << pQueueData->data_record  
                        << "\n" ; 
                    exit(0);
                }

                nVerify++;

                gQueue.CommitPop();

                break;
            }
        }
    }//for
    std::cout<< "Consumer OK\n";
}

///////////////////////////////////////////////////////////////////////////////
int main ()
{
    std::cout<< "Queue capacity :" << gQueue.GetQueueTotalCapacity() <<"\n";

    std::thread ConsumerThread(Consumer);
    std::thread ProducerThread(Producer);

    ConsumerThread.join();
    ProducerThread.join();

    return 0;
}

// g++ -Wall -O2 -g -std=c++11 -pthread -o test test.cpp

