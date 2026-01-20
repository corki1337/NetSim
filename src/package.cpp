#include "package.hpp"

std::set<ElementID> Package::assigned_IDs;
std::set<ElementID> Package::freed_IDs;


Package::Package(){
    if (freed_IDs.empty()){
        if (assigned_IDs.empty()) id_ = 1;
        else id_ = *assigned_IDs.rbegin() + 1;
        
    }
    else {
        id_ = *freed_IDs.begin();
        freed_IDs.erase(id_);
    }
    assigned_IDs.insert(id_);
    
    
}

Package::Package(ElementID id) : id_(id){
    assigned_IDs.insert(id);
    freed_IDs.erase(id);
}


Package::Package(Package&& paka){
    
    id_ = paka.get_id();
    paka.id_ = 0;
}

Package& Package::operator= (Package&& paka){

    if (this == &paka) return *this;

    if (id_ != 0){
        assigned_IDs.erase(id_);
        freed_IDs.insert(id_);        
    }
    id_ = paka.id_;
    paka.id_ = 0;
    
    return *this;

}




Package::~Package(){
    if (id_ != 0){
        assigned_IDs.erase(id_);
        freed_IDs.insert(id_);
    }
}