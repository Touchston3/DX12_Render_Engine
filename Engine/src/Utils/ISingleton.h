#pragma once

class ISingleton
{
public:
    virtual ISingleton* get() = 0;
    virtual ~ISingleton() = default;
};
