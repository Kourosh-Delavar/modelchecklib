#pragma once

#include "AST.h"
#include <memory>
#include <stdexcept>

namespace modelchecklib::algorithms::ltl {

    class NNFTransformer {
        public:
            // converts an LTL formula to NNF
            static LTLPtr transform(const LTLPtr& node, bool negate = false) {
                if (!node) return nullptr;

                // make negation of atom node 
                if (auto atom = std::dynamic_pointer_cast<LTLAtom>(node)) {
                    if (negate) return makeNot(makeAtom(atom->label));
                    return makeAtom(atom->label);
                }

                // make negation of boolean node
                if (auto boolean = std::dynamic_pointer_cast<LTLBool>(node)) {
                    if (negate) return makeBool(!boolean->value);
                    return makeBool(boolean->value);
                }

                // make negation of unary node
                if (auto unary = std::dynamic_pointer_cast<LTLUnaryOperator>(node)) {
                    switch (unary->op) {
                        case Operator::NOT:
                            return transform(unary->child, !negate);

                        case Operator::NEXT:
                            return makeNext(transform(unary->child, negate));

                        case Operator::GLOBALLY:
                            if (negate) return makeEventually(transform(unary->child, true));
                            return makeGlobally(transform(unary->child, false));

                        case Operator::EVENTUALLY:
                            if (negate) return makeGlobally(transform(unary->child, true));
                            return makeEventually(transform(unary->child, false));

                        default:
                            break;
                    }
                }

                // make negation of binary node
                if (auto binary = std::dynamic_pointer_cast<LTLBinaryOperator>(node)) {
                    switch (binary->op) {
                        case Operator::IMPLIES:
                            if (negate) {
                                return makeAnd(transform(binary->leftChild, false), transform(binary->rightChild, true));
                            } 
                            return makeOr(transform(binary->leftChild, true), transform(binary->rightChild, false));

                        case Operator::AND:
                            if (negate) {
                                return makeOr(transform(binary->leftChild, true), transform(binary->rightChild, true));
                            }    
                            return makeAnd(transform(binary->leftChild, false), transform(binary->rightChild, false));

                        case Operator::OR:
                            if (negate) {
                                return makeAnd(transform(binary->leftChild, true), transform(binary->rightChild, true));
                            }
                            return makeOr(transform(binary->leftChild, false), transform(binary->rightChild, false));

                        case Operator::UNTIL:
                            if (negate) {
                                return makeRelease(transform(binary->leftChild, true), transform(binary->rightChild, true));
                            }
                            return makeUntil(transform(binary->leftChild, false), transform(binary->rightChild, false));

                        case Operator::RELEASE:
                            if (negate) {
                                return makeUntil(transform(binary->leftChild, true), transform(binary->rightChild, true));
                            }
                            return makeRelease(transform(binary->leftChild, false), transform(binary->rightChild, false));

                        default:
                    }
                }

                throw std::runtime_error("NNFTransformer Error: Unknown AST model");
            }
    };
}