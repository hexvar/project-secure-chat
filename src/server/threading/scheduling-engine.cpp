#include "scheduling-engine.h"

void MethodThread::Execute(void* arg)
{
    thread_params* t_param = (thread_params*)arg;
    SchedulingEngine* sched_engine = t_param->ptr;
    while(1)
    {
        MethodRequest* meth = sched_engine->GetNextMethod();
        if (!meth)
            continue;

        meth->call();

        delete meth;
    }    
}

SchedulingEngine* SchedulingEngine::_instance = NULL;

SchedulingEngine::SchedulingEngine() : sem(m_mutex), b_active(false)
{

}

SchedulingEngine::~SchedulingEngine()
{
    while (!m_threads.empty())
    {
        delete m_threads.begin()->second;
        m_threads.erase(m_threads.begin());
    }   
}

void SchedulingEngine::Initialize(uint32 n_thread)
{
    Lock guard(m_mutex);
    for (uint32 i = 0; i < n_thread; i++)
    {
        thread_params* t_param = new thread_params;
        t_param->ptr = this;
        MethodThread* m_thread = new MethodThread(); 
        m_thread->Start(t_param);       
        m_threads.add(m_thread);
    }
    b_active = true;
}

int SchedulingEngine::Deactivate()
{
    // TODO
    b_active = false;
    return 0;
}

bool SchedulingEngine::IsActive()
{
    return b_active;
}

int SchedulingEngine::Execute(MethodRequest* m_req)
{   
    q_method.add(m_req);
    sem.Signal();
    return 0;
}

MethodRequest* SchedulingEngine::GetNextMethod()
{
    sem.Wait();
    MethodRequest* ret = NULL;
    q_method.next(ret);
    return ret;
}