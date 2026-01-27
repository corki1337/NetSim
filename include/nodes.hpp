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
#include <optional>


enum class ReceiverType{
    WORKER,
    STOREHOUSE
};

enum class NodeColor{
    UNVISITED,
    VISITED,
    VERIFIED
};

class IPackageReceiver{
public:
    virtual void receive_package(Package&& p) = 0;
    virtual ElementID get_id() const = 0;
    virtual ~IPackageReceiver() = default;
    virtual ReceiverType get_receiver_type() const = 0;

    virtual IPackageStockpile::const_iterator cbegin() const = 0;
    virtual IPackageStockpile::const_iterator cend() const = 0;
    virtual IPackageStockpile::const_iterator begin() const = 0;
    virtual IPackageStockpile::const_iterator end() const = 0;
};

class ReceiverPreferences {
    
    public:
        using preferences_t = std::map<IPackageReceiver*, double>;
        using const_iterator = preferences_t::const_iterator;

        explicit ReceiverPreferences(ProbabilityGenerator pg = probability_generator) : pg_(pg) {}

        void add_receiver(IPackageReceiver* receiver);
        void remove_receiver(IPackageReceiver* receiver);
        IPackageReceiver* choose_receiver();
        const preferences_t& get_preferences() const{ return preferences_; }

        const_iterator begin() const;
        const_iterator cbegin() const;
        const_iterator end() const;
        const_iterator cend() const;

    private:
        preferences_t preferences_; 
        ProbabilityGenerator pg_; 
};

class PackageSender{

    public: 
        PackageSender() = default;
        PackageSender(PackageSender&&) = default;

        ReceiverPreferences receiver_preferences_;

        void send_package();
        const std::optional<Package>& get_sending_buffer() const { return buffer_; }

    protected:
        void push_package(Package &&pack) {buffer_.emplace(std::move(pack));}     
        
    private:
        std::optional<Package> buffer_;
};

class Storehouse : public IPackageReceiver{

    public:
        Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d = std::make_unique<PackageQueue>(PackageQueueType::FIFO)): id_(id), d_(std::move(d)) {}

        void receive_package(Package&& p) override { d_->push(std::move(p)); }
        ElementID get_id() const override{ return id_; }
        ReceiverType get_receiver_type() const override {return ReceiverType::STOREHOUSE;}

        IPackageStockpile::const_iterator cbegin() const override;
        IPackageStockpile::const_iterator cend() const override;
        IPackageStockpile::const_iterator begin() const override;
        IPackageStockpile::const_iterator end() const override;

    private:
        ElementID id_;
        std::unique_ptr<IPackageStockpile> d_;
};

class Ramp : public PackageSender{

    public:
        Ramp(ElementID id, TimeOffset di): id_(id ), di_(di) {}

        void deliver_goods(Time t);
        TimeOffset get_delivery_interval() const { return di_; }
        ElementID get_id() const {return id_; }

    private:
        ElementID id_ ;
        TimeOffset di_;
};

class Worker : public IPackageReceiver, public PackageSender{
public:
    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q)
     : id_(id), pd_(pd), q_(std::move(q)), t_(0) {};
    void do_work(Time t);
    TimeOffset get_processing_duration() const {return pd_;}
    Time get_package_processing_start_time() const {return t_;}
    ReceiverType get_receiver_type() const override {return ReceiverType::WORKER;}

    void receive_package(Package&& p) override{q_->push(std::move(p));}
    ElementID get_id() const override{return id_;}
    IPackageQueue* get_queue() const {return q_.get();}
    

    IPackageStockpile::const_iterator cbegin() const override {return q_->cbegin();}
    IPackageStockpile::const_iterator cend() const override {return q_->cend();}
    IPackageStockpile::const_iterator begin() const override {return q_->begin();}
    IPackageStockpile::const_iterator end() const override {return q_->end();}


private:
    ElementID id_;
    TimeOffset pd_;
    std::unique_ptr<IPackageQueue> q_;
    Time t_;
    std::optional<Package> buffer_;
    

};



#endif