//klasy IPackageStockpile, IPackageQueue, PackageQueue
//typ wyliczeniowy PackageQueueType
#ifndef STORAGE_TYPES_HPP
#define STORAGE_TYPES_HPP
#include <list>
#include "package.hpp"

enum class PackageQueueType {
    FIFO,
    LIFO
};


class IPackageStockpile {
public:
    using  const_iterator = std::list<Package>::const_iterator;
    virtual void push(Package&& package) = 0;
    virtual const_iterator begin() const = 0;
    virtual const_iterator cbegin() const = 0;
    virtual const_iterator end() const = 0;
    virtual const_iterator cend() const = 0;
    virtual std::size_t size() const = 0;
    virtual bool empty() const = 0;
    virtual ~IPackageStockpile() = default;

};

class IPackageQueue : public IPackageStockpile {
public:
    virtual Package pop() = 0;
    virtual PackageQueueType get_queue_type() const = 0;

    virtual ~IPackageQueue() = default;

};


class PackageQueue : public IPackageQueue{
public:
    PackageQueue(PackageQueueType type) : type_(type) {}
    void push(Package&& package) override;
    Package pop() override;
    const_iterator begin() const override;
    const_iterator cbegin() const override;
    const_iterator end()  const override;
    const_iterator cend() const override;
    std::size_t size() const override;
    bool empty() const override;
    PackageQueueType get_queue_type() const override;
    ~PackageQueue() = default;

 

private:
    std::list<Package> pako_;
    PackageQueueType type_;
};



#endif