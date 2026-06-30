#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace modelchecklib::algorithms::ltl {

    enum class Operator {
        // boolean operators
        NOT,
        AND,
        OR,
        IMPLIES,

        // ltl operators
        GLOBALLY,   // G
        EVENTUALLY, // F
        NEXT,       // X
        UNTIL,      // U
        RELEASE     // R
    };

    struct LTLNode {
        virtual ~LTLNode() = default;
        virtual std::string toString() const {
            return {};
        }
    };

    using LTLPtr = std::shared_ptr<LTLNode>;

    struct LTLAtom : LTLNode {
        std::string label;

        explicit LTLAtom(std::string l) : label(std::move(l)) {}

        [[nodiscard]] std::string toString() const override {
            return label;
        }
    };

    struct LTLBool : public LTLNode {
        bool value;

        explicit LTLBool(bool v) : value(v) {}

        [[nodiscard]] std::string toString() const override {
            return value ? "True" : "False";
        }
    };

    struct LTLUnaryOperator : public LTLNode {
        Operator op;
        LTLPtr child;

        ~LTLUnaryOperator() override = default;

        explicit LTLUnaryOperator(Operator o, LTLPtr c) : op(o), child(std::move(c)) {}

        [[nodiscard]] std::string toString() const override {
            std::string opStr;
            switch (op) {
                case Operator::NOT:
                    opStr = "!";
                    break;
                case Operator::NEXT:
                    opStr = "X";
                    break;
                case Operator::GLOBALLY:
                    opStr = "G";
                    break;
                case Operator::EVENTUALLY:
                    opStr = "F";
                    break;
                default:
                    throw std::logic_error("Invalid unary operator");
            }
            return opStr + "(" + child->toString() + ")";
        }
    };

    struct LTLBinaryOperator : public LTLNode {
        Operator op;
        LTLPtr leftChild;
        LTLPtr rightChild;

        ~LTLBinaryOperator() override = default;

        explicit LTLBinaryOperator(Operator o, LTLPtr lc, LTLPtr rc) :
            op(o),
            leftChild(std::move(lc)),
            rightChild(std::move(rc)) {}

        [[nodiscard]] std::string toString() const override {
            std::string opStr;
            switch (op) {
                case Operator::AND:
                    opStr = " && ";
                    break;
                case Operator::OR:
                    opStr = " || ";
                    break;
                case Operator::IMPLIES:
                    opStr = " -> ";
                    break;
                case Operator::UNTIL:
                    opStr = " U ";
                    break;
                case Operator::RELEASE:
                    opStr = " R ";
                    break;
                default:
                    throw std::logic_error("Invalid binary operator");
            }
            return "(" + leftChild->toString() + opStr + rightChild->toString() + ")";
        }
    };

    // helper functions to build the AST
    inline LTLPtr makeAtom(const std::string& label) {
        return std::make_shared<LTLAtom>(label);
    }

    inline LTLPtr makeBool(bool value) {
        return std::make_shared<LTLBool>(value);
    }

    // unary operators
    inline LTLPtr makeNot(LTLPtr child) {
        return std::make_shared<LTLUnaryOperator>(Operator::NOT, std::move(child));
    }

    inline LTLPtr makeNext(LTLPtr child) {
        return std::make_shared<LTLUnaryOperator>(Operator::NEXT, std::move(child));
    }

    inline LTLPtr makeGlobally(LTLPtr child) {
        return std::make_shared<LTLUnaryOperator>(Operator::GLOBALLY, std::move(child));
    }

    inline LTLPtr makeEventually(LTLPtr child) {
        return std::make_shared<LTLUnaryOperator>(Operator::EVENTUALLY, std::move(child));
    }

    // binary operators
    inline LTLPtr makeAnd(LTLPtr leftChild, LTLPtr rightChild) {
        return std::make_shared<LTLBinaryOperator>(Operator::AND, std::move(leftChild), std::move(rightChild));
    }

    inline LTLPtr makeOr(LTLPtr leftChild, LTLPtr rightChild) {
        return std::make_shared<LTLBinaryOperator>(Operator::OR, std::move(leftChild), std::move(rightChild));
    }

    inline LTLPtr makeImplies(LTLPtr leftChild, LTLPtr rightChild) {
        return std::make_shared<LTLBinaryOperator>(Operator::IMPLIES, std::move(leftChild), std::move(rightChild));
    }

    inline LTLPtr makeUntil(LTLPtr leftChild, LTLPtr rightChild) {
        return std::make_shared<LTLBinaryOperator>(Operator::UNTIL, std::move(leftChild), std::move(rightChild));
    }

    inline LTLPtr makeRelease(LTLPtr leftChild, LTLPtr rightChild) {
        return std::make_shared<LTLBinaryOperator>(Operator::RELEASE, std::move(leftChild), std::move(rightChild));
    }

}