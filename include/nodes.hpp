//klasy ujęte na diagramie UML w pakiecie Nodes
//typ wyliczeniowy ReceiverType
//klasy IPackageReceiver, Storehouse, ReceiverPreferences, PackageSender, Ramp, Worker

#ifndef NODES_HPP
#define NODES_HPP

#include "types.hpp"
#include "storage_types.hpp"
#include "helpers.hpp"
#include <memory>
#include <map>

class IPackageReceiver{
public:
    virtual void receive_package(Package&& p) = 0;
    virtual ElementID get_id() const = 0;
    virtual ~IPackageReceiver() = default;
};

class ReceiverPreferences {
    
    public:
        using preferences_t = std::map<IPackageReceiver*, double>;
        using const_iterator = preferences_t::const_iterator;

        explicit ReceiverPreferences(ProbabilityGenerator pg = probability_generator) : pg_(pg) {}

        void add_receiver(IPackageReceiver* receiver);
        void remove_receiver(IPackageReceiver* receiver);
        IPackageReceiver* choose_receiver();
        const preferences_t& get_preferences() const{
            return preferences_;
        }

        const_iterator begin() const;
        const_iterator cbegin() const;
        const_iterator end() const;
        const_iterator cend() const;

    private:
    preferences_t preferences_; 
    ProbabilityGenerator pg_; 
};


#endif