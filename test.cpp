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
    size_t  data_index;
    char data_record[4096];

} ST_QUEUE_DATA ;

///////////////////////////////////////////////////////////////////////////////
NBSPSC_Queue<ST_QUEUE_DATA, 10> g_queue ;

///////////////////////////////////////////////////////////////////////////////
void Producer()
{
    ST_QUEUE_DATA queue_data;

    for ( size_t i =0; i < MAX_TEST; i++) {
        queue_data.data_index= i;
        snprintf(queue_data.data_record, sizeof(queue_data.data_record), "data-%ld", i );

        while(1) {
            OPERATION_RESULT rslt = g_queue.Push(queue_data);
            if( rslt == QUEUE_FULL ) {
                std::this_thread::sleep_for(sleep_duration_microsec);
                continue;
            } else {
                break;
            }
        }
    }
    std::cout<< "Producer Done\n" ;
}

///////////////////////////////////////////////////////////////////////////////
void Consumer()
{
    std::size_t verify = 0;
    ST_QUEUE_DATA* queue_data;

    for ( int i =0; i < MAX_TEST; i++) {
        while(1) {
            OPERATION_RESULT nRslt = g_queue.Pop(& queue_data) ;
            if( nRslt == QUEUE_EMPTY ) {
                std::this_thread::sleep_for(sleep_duration_microsec);
                continue;
            } else {
                if( queue_data->data_index != verify ) {
                    std::cerr << "Pop errorr : data_index=" 
                        << queue_data->data_index << "/ verify=" << verify 
                        << " / data_record=" 
                        << queue_data->data_record  
                        << "\n" ; 
                    exit(0);
                }
                verify++;

                g_queue.CommitPop();
                break;
            }
        }
    }//for
    std::cout<< "Consumer OK\n";
}

///////////////////////////////////////////////////////////////////////////////
int main ()
{
    std::cout<< "Queue capacity :" << g_queue.GetQueueTotalCapacity() <<"\n";

    std::thread ConsumerThread(Consumer);
    std::thread ProducerThread(Producer);

    ConsumerThread.join();
    ProducerThread.join();

    return 0;
}

// g++ -Wall -O2 -g -std=c++11 -pthread -o test test.cpp

