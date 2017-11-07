#pragma once

void TestThreading()
{

    OS::ThreadPool& threadPool = Locator::getThreadManager().getThreadPool();


#define SIZE 100
    OS::JobPtr jobs[SIZE];

    for (int i = 0; i < SIZE; i++)
    {
        jobs[i] = ASYNC_JOB([=] {
            LOG("Job #" + TS(i) + " executing....", Color::VIOLET);
        });
    }

    for (int i = 0; i < SIZE; i++)
    {
        jobs[i]->wait();
    }

    OS::JobPtr i = threadPool.addJob([] {
        WARN("A super awesome job");
    });

}