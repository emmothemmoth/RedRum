#pragma once

class DelegateHandle
{
public:
    DelegateHandle() : myID(0) {}
    explicit DelegateHandle(size_t anID) : myID(anID) {}
    bool IsValid() const { return myID != 0; }
    bool operator==(const DelegateHandle& other) const { return myID == other.myID; }
private:
    size_t myID;
};
