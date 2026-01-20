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