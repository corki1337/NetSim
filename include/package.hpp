//klasa Package
#ifndef PACKAGE_HPP
#define PACKAGE_HPP
#include "types.hpp"

#include <set>

class Package {
public:
    Package();
    Package(ElementID id);
    Package(Package&& paka);
    
    Package& operator= (Package&& paka);
    ElementID get_id() const {return id_;}


    ~Package();


private:
    ElementID id_;
    static std::set<ElementID> assigned_IDs;
    static std::set<ElementID> freed_IDs;

};





#endif