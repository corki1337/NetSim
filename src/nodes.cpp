#include "nodes.hpp"

void ReceiverPreferences::add_receiver(IPackageReceiver* receiver) {
    
    preferences_[receiver] = 0.0; 
    double new_prob = 1.0 / preferences_.size(); 
    for (auto& pair : preferences_) {
        pair.second = new_prob;
    }
}

void ReceiverPreferences::remove_receiver(IPackageReceiver* receiver) {

    if (preferences_.erase(receiver) > 0 && !preferences_.empty()) {
        double new_prob = 1.0 / preferences_.size();
        for (auto& item : preferences_) {
            item.second = new_prob;
        }
    }
}

IPackageReceiver* ReceiverPreferences::choose_receiver() {
    double p = pg_();
    double distribution = 0.0;

    for (const auto& item : preferences_) {
        distribution += item.second;
        if (p <= distribution) {
            return item.first;
        }
    }

    return nullptr;
}

ReceiverPreferences::const_iterator ReceiverPreferences::begin() const {
    return preferences_.begin();
}

ReceiverPreferences::const_iterator ReceiverPreferences::cbegin() const {
    return preferences_.cbegin();
}

ReceiverPreferences::const_iterator ReceiverPreferences::end() const {
    return preferences_.end();
}

ReceiverPreferences::const_iterator ReceiverPreferences::cend() const {
    return preferences_.cend();
}


void PackageSender :: send_package(){
    if (buffer_){
            IPackageReceiver* receiver = receiver_preferences_.choose_receiver();
            if (receiver){
                receiver->receive_package(std::move(*buffer_));
                buffer_.reset();
            }
        }
}

IPackageStockpile::const_iterator Storehouse::cbegin() const {
    return d_->cbegin();
}

IPackageStockpile::const_iterator Storehouse::cend() const {
    return d_->cend();
}

IPackageStockpile::const_iterator Storehouse::begin() const {
    return d_->begin();
}

IPackageStockpile::const_iterator Storehouse::end() const {
    return d_->end();
}


void Ramp :: deliver_goods(Time t){
    if ((t - 1) % di_ == 0) {
        push_package(Package());
    }
}