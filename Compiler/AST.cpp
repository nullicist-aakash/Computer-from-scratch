#include "AST.h"
#include <iostream>
#include <cassert>
using namespace std;

std::ostream& operator<<(std::ostream& out, const ASTNode& node)
{
    out <<
        "{ symbol: '" <<
        parser.symbolType2symbolStr[node.sym_index] <<
        "', lexeme: '" <<
        (node.token ? node.token->lexeme : "") <<
        "'";

    out << " }";
    return out;
}

Token* copy_token(Token* input)
{
    assert(input != nullptr);
    assert(input->length > 0);

    auto* out = new Token;
    out->type = input->type;
    out->lexeme = input->lexeme;
    out->line_number = input->line_number;
    out->start_index = input->line_number;
    out->length = input->length;
    return out;
}

ASTNode* createAST(const ParseTreeNode* input, const ParseTreeNode* parent, ASTNode* inherited)
{
    assert(input != nullptr);

    if (input->isLeaf)
    {
        auto* node = new ASTNode;
        node->sym_index = input->symbol_index;
        node->token = copy_token(input->token);
        return node;
    }

    auto* node = new ASTNode;
    node->sym_index = input->symbol_index;

    if (input->productionNumber == 0)
    {
        // class ==> TK_CLASS TK_IDENTIFIER TK_CURO class_vars subroutineDecs TK_CURC TK_EOF
        node->token = copy_token(input->children[0]->token);
        node->children = {
            createAST(input->children[1]),
            createAST(input->children[3]),
            createAST(input->children[4]),
        };
    }
    else if (input->productionNumber == 1)
    {
        // class_vars ==> class_var class_vars
        delete node;
        auto child = createAST(input->children[0]);
        child->sibling = createAST(input->children[1]);
        return child;
    }
    else if (input->productionNumber == 2)
    {
        // class_vars ==> eps
        delete node;
        return nullptr;
    }
    else if (input->productionNumber == 3)
    {
        // subroutineDecs ==> subroutineDec subroutineDecs
        delete node;
        auto routine = createAST(input->children[0]);
        routine->sibling = createAST(input->children[1]);
        return routine;
    }
    else if (input->productionNumber == 4)
    {
        // subroutineDecs ==> eps
        delete node;
        return nullptr;
    }
    else if (input->productionNumber == 5)
    {
        // class_var ==> class_var_prefix type TK_IDENTIFIER more_identifiers TK_SEMICOLON
        node->children = {
            createAST(input->children[0]),
            createAST(input->children[1]),
            createAST(input->children[2])
        };
        node->children[2]->sibling = createAST(input->children[3]);
    }
    else if (input->productionNumber >= 6 && input->productionNumber <= 11)
    {
        // class_var_prefix TK_STATIC
        // class_var_prefix TK_FIELD
        // type TK_INT
        // type TK_CHAR
        // type TK_BOOLEAN
        // type TK_IDENTIFIER
        delete node;
        return createAST(input->children[0]);
    }
    else if (input->productionNumber == 12)
    {
        // more_identifiers ==> TK_COMMA TK_IDENTIFIER more_identifiers
        delete node;
        node = createAST(input->children[1]);
        node->sibling = createAST(input->children[2]);
    }
    else if (input->productionNumber == 13)
    {
        // more_identifiers ==> eps
        delete node;
        return nullptr;
    }
    else if (input->productionNumber == 14)
    {
        // subroutineDec ==> subroutine_prefix subroutine_type TK_IDENTIFIER TK_PARENO parameters TK_PARENC subroutine_body
        node->token = copy_token(input->children[2]->token);
        node->children = {
            createAST(input->children[0]),
            createAST(input->children[1]),
            createAST(input->children[4]),
            createAST(input->children[6])
        };
    }
    else if (input->productionNumber >= 15 && input->productionNumber <= 19)
    {
        // subroutine_prefix TK_CONSTRUCTOR
        // subroutine_prefix TK_FUNCTION
        // subroutine_prefix TK_METHOD
        // subroutine_type type
        // subroutine_type TK_VOID
        delete node;
        return createAST(input->children[0]);
    }
    else if (input->productionNumber == 20)
    {
        // parameters ==> TK_IDENTIFIER more_parameters
        node->token = copy_token(input->children[0]->token);
        node->sibling = createAST(input->children[1]);
    }
    else if (input->productionNumber == 21)
    {
        // parameters ==> eps
        delete node;
        return nullptr;
    }
    else if (input->productionNumber == 22)
    {
        // more_parameters ==> TK_COMMA TK_IDENTIFIER more_parameters
        node->token = copy_token(input->children[1]->token);
        node->sibling = createAST(input->children[2]);
    }
    else if (input->productionNumber == 23)
    {
        // more_parameters ==> eps
        delete node;
        return nullptr;
    }
    else if (input->productionNumber == 24)
    {
        // subroutine_body ==> TK_CURO routine_vars statements TK_CURC
        node->children = {
            createAST(input->children[1]),
            createAST(input->children[2]),
        };
    }
    else if (input->productionNumber == 25)
    {
        // routine_vars ==> routine_var routine_vars
        delete node;
        node = createAST(input->children[0]);
        node->sibling = createAST(input->children[1]);
    }
    else if (input->productionNumber == 26)
    {
        // routine_vars ==> eps
        delete node;
        return nullptr;
    }
    else if (input->productionNumber == 27)
    {
        // routine_var ==> TK_VAR type TK_IDENTIFIER more_identifiers TK_SEMICOLON
        node->token = copy_token(input->children[0]->token);
        node->children = {
            createAST(input->children[1]),
            createAST(input->children[2])
        };
        node->children[1]->sibling = createAST(input->children[3]);
    }
    else if (input->productionNumber == 28)
    {
        // statements ==> statement statements
        delete node;
        node = createAST(input->children[0]);
        node->sibling = createAST(input->children[1]);
    }
    else if (input->productionNumber == 29)
    {
        // statements ==> eps
        delete node;
        return nullptr;
    }
    else if (input->productionNumber >= 30 && input->productionNumber <= 34)
    {
        // statement let_statement
        // statement if_statement
        // statement while_statement
        // statement do_statement
        // statement return_statement
        delete node;
        return createAST(input->children[0]);
    }
    else if (input->productionNumber == 35)
    {
        // let_statement ==> TK_LET TK_IDENTIFIER identifier_suffix TK_EQ expression TK_SEMICOLON
        node->token = copy_token(input->children[0]->token);
        node->children = {
            createAST(input->children[1]),
            createAST(input->children[2]),
            createAST(input->children[4])
        };
    }
    else if (input->productionNumber == 36)
    {
        // identifier_suffix ==> eps
        delete node;
        return nullptr;
    }
    else if (input->productionNumber == 37)
    {
        // identifier_suffix ==> TK_BRACKO expression TK_BRACKC
        delete node;
        return createAST(input->children[1]);
    }
    else if (input->productionNumber == 38)
    {
        // if_statement TK_IF TK_PARENO expression TK_PARENC TK_CURO statements TK_CURC else_statement
        node->token = copy_token(input->children[0]->token);
        node->children = {
            createAST(input->children[2]),
            createAST(input->children[5]),
            createAST(input->children[7])
        };
    }
    else if (input->productionNumber == 39)
    {
        // else_statement ==> eps
        delete node;
        return nullptr;
    }
    else if (input->productionNumber == 40)
    {
        // else_statement ==> TK_ELSE TK_CURO statements TK_CURC
        delete node;
        return createAST(input->children[2]);
    }
    else if (input->productionNumber == 41)
    {
        // while_statement ==> TK_WHILE TK_PARENO expression TK_PARENC TK_CURO statements TK_CURC
        node->token = copy_token(input->children[0]->token);
        node->children = {
            createAST(input->children[2]),
            createAST(input->children[5])
        };
    }
    else if (input->productionNumber == 42)
    {
        // do_statement ==> TK_DO subroutine_call TK_SEMICOLON
        node->token = copy_token(input->children[0]->token);
        node->children = {createAST(input->children[1])};
    }
    else if (input->productionNumber == 43)
    {
        // return_statement ==> TK_RETURN return_suffix TK_SEMICOLON
        node->token = copy_token(input->children[0]->token);
        node->children = {createAST(input->children[1])};
    }
    else if (input->productionNumber == 44)
    {
        // return_suffix ==> eps
        delete node;
        return nullptr;
    }
    else if (input->productionNumber == 45)
    {
        // return_suffix ==> expression
        delete node;
        return createAST(input->children[0]);
    }
    else if (input->productionNumber == 46)
    {
        // expression ==> term expression_suffix
        delete node;
        auto term = createAST(input->children[0]);
        auto suffix = createAST(input->children[1]);
        if (suffix == nullptr)
            return term;

        suffix->children[0] = term;
        return suffix;
    }
    else if (input->productionNumber == 47)
    {
        // expression_suffix ==> op term expression_suffix
        delete node;
        auto op = createAST(input->children[0]);
        auto term = createAST(input->children[1]);
        auto suffix = createAST(input->children[2]);

        if (suffix == nullptr)
            op->children = { nullptr, term };
        else
        {
            op->children = { nullptr, suffix };
            suffix->children[0] = term;
        }

        return op;
    }
    else if (input->productionNumber == 48)
    {
        // expression_suffix ==> eps
        delete node;
        return nullptr;
    }
    else if (input->productionNumber >= 49 && input->productionNumber <= 54)
    {
        // term TK_NUM
        // term TK_STR
        // term TK_TRUE
        // term TK_FALSE
        // term TK_NULL
        // term TK_THIS
        delete node;
        return createAST(input->children[0]);
    }
    else if (input->productionNumber == 55)
    {
        // term ==> TK_IDENTIFIER term_sub_iden
        node->token = copy_token(input->children[0]->token);
        return createAST(input->children[1], nullptr, node);
    }
    else if (input->productionNumber == 56)
    {
        // term_sub_iden ==> eps
        delete node;
        return inherited;
    }
    else if (input->productionNumber == 57)
    {
        // term_sub_iden ==> TK_PARENO expression_list TK_PARENC
        node->token = copy_token(input->children[0]->token);
        node->children = {
            inherited,
            createAST(input->children[1])
        };
    }
    else if (input->productionNumber == 58)
    {
        // term_sub_iden ==> TK_DOT TK_IDENTIFIER TK_PARENO expression_list TK_PARENC
        node->token = copy_token(input->children[2]->token);

        auto caller = createAST(input->children[0]);
        caller->children = {
            inherited,
            createAST(input->children[1])
        };

        node->children = {
            caller,
            createAST(input->children[3])
        };
    }
    else if (input->productionNumber == 59)
    {
        // term_sub_iden ==> TK_BRACKO expression TK_BRACKC
        node->token = copy_token(input->children[0]->token);
        node->children = {
            inherited,
            createAST(input->children[1])
        };
    }
    else if (input->productionNumber == 60)
    {
        // term ==> TK_PARENO expression TK_PARENC
        delete node;
        return createAST(input->children[1]);
    }
    else if (input->productionNumber == 61)
    {
        // term ==> TK_MINUS term
        node->token = copy_token(input->children[0]->token);
        node->children = {
            createAST(input->children[1])
        };
    }
    else if (input->productionNumber == 62)
    {
        // term ==> TK_NOT term
        node->token = copy_token(input->children[0]->token);
        node->children = {
                createAST(input->children[1])
        };
    }
    else if (input->productionNumber >= 63 && input->productionNumber <= 71)
    {
        // op TK_PLUS
        // op TK_MINUS
        // op TK_MULT
        // op TK_DIV
        // op TK_AND
        // op TK_OR
        // op TK_LE
        // op TK_GE
        // op TK_EQ
        delete node;
        return createAST(input->children[0]);
    }
    else if (input->productionNumber == 72)
    {
        // subroutine_call ==> TK_IDENTIFIER subroutine_scope TK_PARENO expression_list TK_PARENC
        node->token = copy_token(input->children[2]->token);
        node->children = {
            createAST(input->children[1], nullptr, createAST(input->children[0])),
            createAST(input->children[3])
        };
    }
    else if (input->productionNumber == 73)
    {
        // subroutine_scope ==> TK_DOT TK_IDENTIFIER
        node->token = copy_token(input->children[0]->token);
        node->children = {
            inherited,
            createAST(input->children[1])
        };
    }
    else if (input->productionNumber == 74)
    {
        // subroutine_scope ==> eps
        delete node;
        return inherited;
    }
    else if (input->productionNumber == 75)
    {
        // expression_list ==> expression more_expressions
        delete node;
        auto expression = createAST(input->children[0]);
        expression->sibling = createAST(input->children[1]);
        return expression;
    }
    else if (input->productionNumber == 76)
    {
        // expression_list ==> eps
        delete node;
        return nullptr;
    }
    else if (input->productionNumber == 77)
    {
        // more_expressions ==> TK_COMMA expression more_expressions
        delete node;
        auto expression = createAST(input->children[1]);
        expression->sibling = createAST(input->children[2]);
        return expression;
    }
    else if (input->productionNumber == 78)
    {
        // more_expressions ==> eps
        delete node;
        return nullptr;
    }
    else
    {
        assert(false);
    }

    return node;
}