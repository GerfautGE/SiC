#include <vector>
#include <string>

enum class NodeType
{
    program,
    function,
    expression,
    integer,
};

class AstNode
{
    public :
        virtual ~AstNode();
        /**
         * @brief Construct a new Ast Node object
         * @param type the type of the node
         * @param value the value of the node
        */
        AstNode(NodeType type, std::string value);
        NodeType type;
        std::string value;
};

class Integer : public AstNode
{
    public :
        Integer(int value);
        int value;
};
