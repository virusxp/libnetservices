#ifndef CONTAINER_H_
#define CONTAINER_H_

#include <vector>
#include <mutex>

template<typename T>
class SimpleConcurrentQueue
{
    private:
        std::vector<T> container;
        std::mutex m;

    public:
        SimpleConcurrentQueue() {}
        ~SimpleConcurrentQueue() {}

        void push(T element)
        {
            std::lock_guard<std::mutex> lock(this->m);
            container.push_back(element);
        }

        void pushn(T* elements, size_t n)
        {
            if(elements == NULL)
            {
                return;
            }

            std::lock_guard<std::mutex> lock(this->m);
            for(size_t i = 0; i < n; i++)
            {
                container.push_back(elements[i]);
            }
        }

        void pop(T* element)
        {            
            if(container.size() == 0)
            {
                return;
            }

            std::lock_guard<std::mutex> lock(this->m);
            *element = container.front();
            container.erase(0);
        }

        void popn(T* elements, size_t n)
        {
            if(container.size() == 0)
            {
                return;
            }

            std::lock_guard<std::mutex> lock(this->m);
            for(size_t i = 0; i < n; i++)
            {
                elements[i] = container[i];
            }
            container.erase(container.begin(),container.begin() + n);
        }

        size_t size()
        {
            std::lock_guard<std::mutex> lock(this->m);
            return container.size();
        }
};

#endif // CONTAINER_H_