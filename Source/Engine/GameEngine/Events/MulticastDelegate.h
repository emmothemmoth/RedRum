#pragma once

#include <vector>
#include <functional>
#include <memory>

#include "DelegateHandle.h"

template<typename... Args>
class MulticastDelegate
{
    struct Binding
    {
        DelegateHandle Handle;
        std::function<void(Args...)> Function;
    };

public:
    // Equivalent to Unreal's .Broadcast()
    void Broadcast(Args... args)
    {
        for (size_t i = 0; i < myBindings.size(); ++i)
        {
            if (myBindings[i].Function)
            {
                myBindings[i].Function(args...);
            }
        }
    }

    // Equivalent to .AddLambda()
    DelegateHandle AddLambda(std::function<void(Args...)> aFunction)
    {
        size_t id = ++myCounter;
        DelegateHandle handle(id);
        myBindings.push_back({ handle, aFunction });
        return handle;
    }

    template<typename T>
    DelegateHandle AddRaw(T* anInstance, void(T::* aFunction)(Args...))
    {
        size_t id = ++myCounter;
        DelegateHandle handle(id);

        // We wrap the member function call in a lambda
        auto wrapper = [anInstance, aFunction](Args... args)
            {
                (anInstance->*aFunction)(args...);
            };

        myBindings.push_back({ handle, wrapper });
        return handle;
    }

    // Equivalent to .Remove()
    void Remove(DelegateHandle aHandle)
    {
        myBindings.erase(std::remove_if(myBindings.begin(), myBindings.end(),
            [&aHandle](const Binding& b) { return b.Handle == aHandle; }),
            myBindings.end());
    }

private:
    std::vector<Binding> myBindings;
    size_t myCounter = 0;
};