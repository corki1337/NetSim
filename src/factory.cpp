#include "factory.hpp"

bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors) {

    if (node_colors[sender] == NodeColor::VERIFIED) {
        return true;
    }    
    node_colors[sender] = NodeColor::VISITED; 

    if (sender->receiver_preferences_.get_preferences().empty()) {
        throw std::logic_error("No defined receivers");
    }
    bool has_other_receiver = false;

    for(const auto& receiver: sender->receiver_preferences_.get_preferences()){
    
        if(receiver.first->get_receiver_type() == ReceiverType::STOREHOUSE){
            has_other_receiver = true;
        }
        
        else if(receiver.first->get_receiver_type() == ReceiverType::WORKER){
            
            IPackageReceiver* receiver_ptr = receiver.first;
            auto worker_ptr = dynamic_cast<Worker*>(receiver_ptr);
            auto sendrecv_ptr =dynamic_cast<PackageSender*>(worker_ptr);

            if (worker_ptr == sender){
                continue;
            }
            has_other_receiver = true;

            if(node_colors[sendrecv_ptr] == NodeColor::UNVISITED){
                has_reachable_storehouse(sendrecv_ptr, node_colors);
            }
        }   
    }
    node_colors[sender] = NodeColor::VERIFIED;

    if (has_other_receiver) {
        return true;
    } 
    else {
        throw std::logic_error("No reachable magazine");
    }
}  

bool Factory::is_consistent() const {
    std::map<const PackageSender*, NodeColor> node_colors;

    for (const auto& ramp : ramps) {
        node_colors[&ramp] = NodeColor::UNVISITED;
    }
    for (const auto& worker : workers){
        node_colors[&worker] = NodeColor::UNVISITED;
    }

    try{        
        for (const auto& ramp : ramps) {
            has_reachable_storehouse(&ramp, node_colors);
        }
    }
    catch (const std::logic_error&) {
        return false;
    }

    return true;
}

void Factory::do_deliveries(Time t){
    for (auto& ramp: ramps){
        ramp.deliver_goods(t);
    }
}

void Factory::do_package_passing() {
    for (auto& ramp: ramps){
        ramp.send_package();
    }

    for (auto& worker: workers){
        worker.send_package();
    }
}

void Factory::do_work(Time t){
    for (auto& worker: workers){
        worker.do_work(t);
    }
}
