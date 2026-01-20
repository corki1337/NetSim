#include "storage_types.hpp"


void PackageQueue::push(Package&& package){
    pako_.emplace_back(std::move(package));
}

Package PackageQueue::pop(){
    Package paka;
    switch(type_){

        case PackageQueueType::FIFO:
            paka = std::move(pako_.front());
            pako_.pop_front();

            break;
        case PackageQueueType::LIFO:
            paka = std::move(pako_.back());
            pako_.pop_back();
            break;

    

    }
    return paka;
}

IPackageStockpile::const_iterator PackageQueue::begin() const{
    return pako_.begin();
}

IPackageStockpile::const_iterator PackageQueue::cbegin() const{
    return pako_.cbegin();
}
IPackageStockpile::const_iterator PackageQueue::end() const{
    return pako_.end();
}
IPackageStockpile::const_iterator PackageQueue::cend() const{
    return pako_.cend();
}
std::size_t PackageQueue::size() const{
    return pako_.size();
}
bool PackageQueue::empty() const{
    return pako_.empty();
}
PackageQueueType PackageQueue::get_queue_type() const{
    return type_;
}