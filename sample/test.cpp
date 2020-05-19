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

const int MAX_TEST = 10 ;

std::chrono::microseconds sleep_duration_microsec (1);

struct ST_QUEUE_DATA
{
    size_t      data_index;
    char        data_record1 [100];
    std::string data_record2; // this causes a non-trivial type.
} ;

///////////////////////////////////////////////////////////////////////////////
NBSPSC_Queue<ST_QUEUE_DATA, 10> g_queue ;

///////////////////////////////////////////////////////////////////////////////
void Producer()
{
    ST_QUEUE_DATA queue_data;
    std::cout << "is_trivial :" <<std::is_trivial<ST_QUEUE_DATA>::value << '\n'; 

    for ( size_t i =0; i < MAX_TEST; i++) {
        queue_data.data_index= i;
        snprintf(queue_data.data_record1, sizeof(queue_data.data_record1), "data-%ld", i );
        queue_data.data_record2 = queue_data.data_record1 ;

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
                std::cout << "data_index=" << queue_data->data_index  
                    << " / data_record1=" << queue_data->data_record1  
                    << " / data_record2=" << queue_data->data_record2 << "\n" ; 
                if( queue_data->data_index != verify ) {
                    std::cerr << "Pop errorr : data_index=" 
                        << queue_data->data_index << "/ verify=" << verify 
                        << " / data_record1=" << queue_data->data_record1  
                        << " / data_record2=" << queue_data->data_record2 << "\n" ; 
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


