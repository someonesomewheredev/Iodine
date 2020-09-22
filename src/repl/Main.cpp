#include <memory>
#include <parser.hpp>
#include <iostream>
#include <unordered_map>

using namespace iodine;

void printIndents(int indents) {
    for (int i = 0; i < indents; i++) {
        std::cout << "  ";
    }
}

std::string valueToStr(const Value& val) {
    switch (val.type) {
        case DataType::F32:
            return std::to_string(val.floatVal);
            break;
        case DataType::F64:
            return std::to_string(val.doubleVal);
        case DataType::Int32:
            return std::to_string(val.intVal);
            break;
        default:
            return "";
    }
}

std::unordered_map<std::string, Value> iodine::variables;

void printASTNode(std::shared_ptr<ASTNode> node, int indentDepth = 0) {
    printIndents(indentDepth);
        std::cout << "Node type: " << nodeTypeNames[node->type] << "\n";
    switch (node->type) {
        case ASTNodeType::ConstVal:
            printIndents(indentDepth);
            std::cout << "val: " << valueToStr(std::static_pointer_cast<ConstValNode>(node)->val) << "\n";
            break;
        case ASTNodeType::Arithmetic:
        {
            auto aNode = std::static_pointer_cast<ArithmeticNode>(node);
            printIndents(indentDepth);
            std::cout << "a: \n";
            printASTNode(aNode->a, indentDepth + 1);

            printIndents(indentDepth);
            std::cout << "b: \n";
            printASTNode(aNode->b, indentDepth + 1);

            printIndents(indentDepth);
            std::cout << "operation: " << arithOperationNames[aNode->operation] << "\n";
            break;
        }
        case ASTNodeType::UnaryOp:
        {
            printIndents(indentDepth);
            auto uNode = std::static_pointer_cast<UnaryOpNode>(node);
            std::cout << "operation: " << unaryOperationNames[uNode->operation] << "\n";
            printIndents(indentDepth);
            std::cout << "child:\n";
            printASTNode(uNode->valNode, indentDepth + 1);
            break;
        }
        case ASTNodeType::VarAssignment:
        {
            printIndents(indentDepth);
            auto assignmentNode = std::static_pointer_cast<VarAssignmentNode>(node);

            std::cout << "variable name: " << assignmentNode->varName << "\n";

            printIndents(indentDepth);
            std::cout << "value:\n";
            printASTNode(assignmentNode->valNode);
            break;
        }
        case ASTNodeType::VariableReference:
        {
            printIndents(indentDepth);
            auto refNode = std::static_pointer_cast<VariableReferenceNode>(node);
            std::cout << "varname: " << refNode->varName << "\n";
            printIndents(indentDepth);
            std::cout << "varinfo:" << (int)variables.at(refNode->varName).type << "\n";
            break;
        }
        default:
            break;
    }
}

Value evalAST(std::shared_ptr<ASTNode> exprRoot) {
    if (exprRoot->type == ASTNodeType::VarAssignment) {
        auto assignNode = std::static_pointer_cast<VarAssignmentNode>(exprRoot);

        variables[assignNode->varName] = assignNode->valNode->getValue();
        return assignNode->valNode->getValue();
    }
    return std::static_pointer_cast<ProducesValueNode>(exprRoot)->getValue();
}

void printTokens(std::vector<Token>& tokens) {
    for (auto& token : tokens) {
        std::cout << "Token: " << tokenNames[token.type];
        std::cout << " (" << token.val << ")";
        std::cout << "\n";
    }
}

int main(int argc, char** argv) {
    while (true) {
        std::string line;

        std::cout << ">";

        std::getline(std::cin, line);

        if (std::cin.eof())
            break;

        try {
            std::vector<Token> tokens = parseTokens(line);

            std::shared_ptr<ASTNode> n = parseScript(tokens);

            if (n == nullptr) {
                std::cout << "AST is empty\n";
            } else {
                printTokens(tokens);
                printASTNode(n);
                std::cout << valueToStr(evalAST(n)) << "\n";
            }
        } catch (std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    } 
    return 0;
}
