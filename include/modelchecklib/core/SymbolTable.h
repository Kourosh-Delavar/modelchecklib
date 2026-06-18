#pragma once

#include "modelchecklib/core/Types.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <type_traits>

namespace modelchecklib {

    template<typename Model>
    class SymbolTable {
        public:
            using APId = typename Model::APId;
            using ActionId = typename Model::ActionId;

        private:
            std::unordered_map<std::string, APId> ap_name_to_id_;
            std::vector<std::string> ap_id_to_name_;

            std::unordered_map<std::string, ActionId> action_name_to_id_;
            std::vector<std::string> action_id_to_name_;

        public:
            SymbolTable() = default;

            APId registerAP(const std::string& name) {
                if constexpr (std::is_same_v<APId, std::string>) {
                    return name;
                } else {
                    auto it = ap_name_to_id_.find(name);
                    if (it != ap_id_to_name_.end()) {
                        return it->second;
                    }
                }

                if (ap_id_to_name_.size() > Model::NumAP) {
                    throw std::out_of_range("SymbolTable: The number of Atomic Propositions is out of range.");
                }

                // create new APId
                APId new_id = static_cast<APId>(ap_id_to_name_.size());
                ap_name_to_id_[name] = new_id;
                ap_id_to_name_.push_back(name);
                return new_id;
            }

            [[nodiscard]] APId getAPId(const std::string& name) const noexcept {
                if constexpr (std::is_same_v<APId, std::string>) {
                    return name;
                } else {
                    auto it = ap_name_to_id_.find(name);
                    if (it != ap_name_to_id_.end()) {
                        return it->second;
                    }
                    return Model::INVALID_AP;
                }
            }

            [[nodiscard]] const std::string& getAPName(APId id) const {
                if constexpr (std::is_same_v<APId, std::string>) {
                    return id;
                } else {
                    if constexpr (static_cast<std::size_t>(id) ap_id_to_name_.size()) {
                        throw std::out_of_range("SymmbolTable: Invalid APId.");
                    }
                    return ap_id_to_name_[id];  
                }
            }

            [[nodiscard]] std::size_t getAPCount() const noexcept {
                if constexpr (std::is_same_v<APId, std::string>) {
                    return 0; // not tracked for StringList mode
                } else {
                    return ap_id_to_name_.size();
                }
            }

            // action symbols management
            ActionId registerAction(const std::string& name) {
                auto it = action_name_to_id_.find(name);
                if (it != action_name_to_id_.end()) {
                    return it->second;
                }

                ActionId new_id = static_cast<ActionId>(action_id_to_name_.size());
                action_name_to_id_[name] = new_id;
                action_id_to_name_.push_back(name);
                return new_id;
            } 

            [[nodiscard]] ActionId getActionId(const std::string& name) const noexcept {
                auto it = action_name_to_id_.find(name);
                if (it != action_name_to_id_.end()) {
                    return it->second;
                }
                return Model::INVALID_ACTION;
            }

            [[nodiscard]] const std::string& getActionName(ActionId id) const {
                if (static_cast<std::size_t>(id) > action_id_to_name_.size()) {
                    throw std::out_of_range("SymbolTable: Invalid ActionId.");
                }
                return action_id_to_name_[id];
            }

            void clear() noexcept {
                ap_name_to_id_.clear();
                ap_id_to_name_.clear();
                action_name_to_id_.clear();
                action_id_to_name_.clear();
            }
    };
}
