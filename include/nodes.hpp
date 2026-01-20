//klasy ujęte na diagramie UML w pakiecie Nodes
//typ wyliczeniowy ReceiverType
//klasy IPackageReceiver, Storehouse, ReceiverPreferences, PackageSender, Ramp, Worker

#ifndef NODES_HPP
#define NODES_HPP

#include "types.hpp"
#include "storage_types.hpp"
#include <memory>

class IPackageReceiver{
public:
    virtual void receive_package(Package&& p) = 0;
    virtual ElementID get_id() const = 0;
    virtual ~IPackageReceiver() = default;
};



#endif