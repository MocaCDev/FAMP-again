#ifndef YAML_LEXER_H
#define YAML_LEXER_H
#include "common.hpp"

namespace YamlLexer
{

/* Some thigs that'll help us. */
#define is_ascii(x) (x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z')
#define is_ascii_WE(x, exc) (is_ascii(x)) || (x == exc)
#define is_number(x) (x >= '0' && x <= '9')
#define is_hex_dec(x) ((x >= 'A' && x <= 'F') || (x >= 'a' && x <= 'f'))
#define is_type_of_space(x) (x == ' ') || (x == '\t')

enum class YamlTokens
{
    user_def, /* The most common ordeal in a yaml file */
    colon,    /* `:` */
    lsqrbr,   /* `[ */
    rsqrbr,   /* `]` */
    string,   /* Denoted with opening/closing double quotes (`"`) */
    number,   /* Any decimal number. n can be >= 0 && n can be <= 0 */
    hex,      /* Number denoted with `0x` (hexadecimal value) */
    character,/* Any sort of value with an opening/closing single quote (`'`) */
    comma,    /* `,` */
    YamlEOF,  /* End of the yaml file (`\0`) */
    YamlTokenPlaceholder, /* Placeholder; assigned when struct is initiated */
};

struct token
{
    YamlTokens        id;
    puint8            token_value;

    token()
    {
        id = YamlTokens::YamlTokenPlaceholder;
        token_value = nullptr;
    }

    void new_token(YamlTokens tid, puint8 tval)
    {
        id = tid;

        /* If there is existing data, free it. */
        if(token_value)
        {
            memset(token_value, 0, strlen((cpint8) token_value));
            delete token_value;
            token_value = nullptr;
        }

        /* Allocate memory and copy over the values. */
        token_value = new uint8[strlen((cpint8) tval)];
        memcpy(token_value, tval, strlen((cpint8) tval));

        if(tval)
            delete tval;
    }

    ~token()
    {
        if(token_value) delete token_value;
    }
};

struct lexer
{
    puint8        src = nullptr;
    uint16        index;
    uint8         curr_char;
    uint16        line;

    lexer(cpint8 filename)
    {
        FILE *yaml_file = fopen(filename, "rb");
        YAML_ASSERT(yaml_file,
            "\nError opening the yaml file %s.\n",
            filename)

        fseek(yaml_file, 0, SEEK_END);
        usize yaml_filesize = ftell(yaml_file);
        fseek(yaml_file, 0, SEEK_SET);

        YAML_ASSERT(yaml_filesize > 1,
            "\nThe yaml file %s does not have much data.\n",
            filename)

        src = new uint8[yaml_filesize];
        YAML_ASSERT(src,
            "\nError allocating memory for the yaml source code.\n")

        YAML_ASSERT(fread(src, sizeof(*src), yaml_filesize, yaml_file) == yaml_filesize,
            "\nError reading %s. Not all data got read.\n",
            filename)
        fclose(yaml_file);

        line = 1;
        index = 0;
        curr_char = src[index];
    }

    template<typename T>
        requires std::is_same<T, struct lexer>::value
    void delete_instance(T *instance)
    {
        if(instance)
            delete instance;
        instance = nullptr;
    }

    ~lexer()
    {
        if(src) delete src;
    }
};

class yaml_lexer
{
private:
    struct lexer *ylexer = nullptr;
    struct token *ytoken = nullptr;

    inline void advance_lexer(bool auto_skip_newlines = true)
    {
        advance_lexer_beginning:
        ylexer->index++;
        if(ylexer->src[ylexer->index] == '\n' && auto_skip_newlines)
        {
            ylexer->line++;
            goto advance_lexer_beginning;
        }
        if(ylexer->src[ylexer->index] == '\0' || ylexer->index >= strlen((cpint8) ylexer->src))
        {
            ylexer->curr_char = '\0';
            return;
        }

        ylexer->curr_char = ylexer->src[ylexer->index];
    }

    inline void retreat_lexer()
    {
        retreat_lexer_beginning:
        ylexer->index--;
        if(ylexer->src[ylexer->index] == '\n')
        {
            ylexer->line--;
            goto retreat_lexer_beginning;
        }

        ylexer->curr_char = ylexer->src[ylexer->index];
    }

    inline bool lexer_peek(uint8 against)
    {
        bool matches = false;
        advance_lexer(false);

        if(ylexer->curr_char == against) matches = true;

        retreat_lexer();
        return matches;
    }

    inline bool lexer_peek_back(uint8 against)
    {
        bool matches = false;
        
        ylexer->index--;
        if(ylexer->src[ylexer->index] == against) matches = true;
        ylexer->index++;
        
        return matches;
    }

    inline puint8 make_str(uint8 val)
    {
        puint8 str_val = new uint8[2];
        str_val[0] = val;
        str_val[1] = '\0';
        return str_val;
    }

    inline puint8 get_string()
    {
        puint8 string = (puint8) calloc(1, sizeof(*string));
        uint16 index = 0;
        
        while(ylexer->curr_char != '"')
        {
            string[index] = ylexer->curr_char;
            index++;

            string = (puint8) realloc(
                string,
                (index + 1) * sizeof(*string)
            );
            advance_lexer();
        }
        string[index] = '\0';
        
        return string;
    }

    inline puint8 get_hexadecimal()
    {
        puint8 hex_val = (puint8) calloc(1, sizeof(*hex_val));
        uint16 index = 0;

        while(is_number(ylexer->curr_char) || is_hex_dec(ylexer->curr_char))
        {
            hex_val[index] = ylexer->curr_char;
            index++;

            hex_val = (puint8) realloc(
                hex_val,
                (index + 1) * sizeof(*hex_val)
            );
            advance_lexer();
        }
        hex_val[index] = '\0';

        YAML_ASSERT(lexer_peek_back('\n') || ylexer->curr_char == '\0',
            "\nExpected a newline (or EOF) following hexadecimal value `0x%s` on line %d.\n",
            hex_val, ylexer->line)
        
        return hex_val;
    }

    //inline puint8 get_decimal

    inline puint8 get_user_defined()
    {
        puint8 user_def_val = (puint8) calloc(1, sizeof(*user_def_val));
        uint16 index = 0;

        while((is_ascii_WE(ylexer->curr_char, '_')) && !(ylexer->curr_char == '\0'))
        {
            user_def_val[index] = ylexer->curr_char;
            index++;

            user_def_val = (puint8) realloc(
                user_def_val,
                (index + 1) * sizeof(*user_def_val)
            );
            advance_lexer();
        }
        user_def_val[index] = '\0';

        /* Shouldn't be anything but `:`. */
        YAML_ASSERT(ylexer->curr_char == ':',
            "\nExpected colon (`:`) after `%s` on line %d.\n",
            user_def_val, ylexer->line)

        return user_def_val;
    }

public:
    yaml_lexer(cpint8 filename)
    {
        ylexer = new struct lexer(filename);
        ytoken = new struct token;
    }

    inline struct token *return_token()
    { return ytoken; }

    void get_token()
    {
        get_token_beginning:
        if(is_type_of_space(ylexer->curr_char))
        {
            while(is_type_of_space(ylexer->curr_char))
                advance_lexer();
        }
        
        if(is_number(ylexer->curr_char))
        {
            if(lexer_peek('x')) goto get_hex;
            /* TODO: */
        }

        if(is_ascii(ylexer->curr_char))
        {
            puint8 user_defined = get_user_defined();
            ytoken->new_token(YamlTokens::user_def, user_defined);
            
            return;
        }

        goto check_char;

        {
            get_hex:
            advance_lexer(); /* 0 */
            advance_lexer(); /* x */
            
            /* Make sure there are values following `0x`. */
            YAML_ASSERT(is_number(ylexer->curr_char) || is_hex_dec(ylexer->curr_char),
                "\nInvalid value of `%c` following `0x` on line %d.\n",
                ylexer->curr_char, ylexer->line)

            puint8 hex = get_hexadecimal();
            ytoken->new_token(YamlTokens::hex, hex);
            return;
        }

        check_char:
        switch(ylexer->curr_char)
        {
            case '#': {
                advance_lexer();

                while(!lexer_peek('\n'))
                    advance_lexer();
                advance_lexer();
                goto get_token_beginning;
                break;
            }
            case '[': advance_lexer();ytoken->new_token(YamlTokens::lsqrbr, make_str('['));return;break;
            case ']': advance_lexer();ytoken->new_token(YamlTokens::rsqrbr, make_str(']'));return;break;
            case ':': advance_lexer();ytoken->new_token(YamlTokens::colon, make_str(':'));return;break;
            case ',': advance_lexer();ytoken->new_token(YamlTokens::comma, make_str(','));return;break;
            case '\'': {
                advance_lexer();

                /* Make sure the next value is a closing single quote. */
                YAML_ASSERT(lexer_peek('\'') == true,
                    "\nFound a single quote (denoting a character value) on line %d. Missing closing single quote following the character value %c.\n",
                    ylexer->line, ylexer->curr_char)

                uint8 char_val = ylexer->curr_char;

                /* This will give us `'`, so advance again after this. */
                advance_lexer();
                advance_lexer();

                ytoken->new_token(YamlTokens::character, make_str(char_val));
                return;
                break;
            }
            case '"': {
                advance_lexer();

                puint8 str_val = get_string();
                advance_lexer(); /* Skip the closing '"' */

                ytoken->new_token(YamlTokens::string, str_val);
                return;
                break;
            }
            case '\0': ytoken->new_token(YamlTokens::YamlEOF, make_str('\0'));return;break;
            default: break;
        }
    }

    template<typename T>
        requires std::is_same<T, struct lexer>::value
            || std::is_same<T, struct token>::value
            || std::is_same<T, yaml_lexer>::value
    void delete_instance(T *instance)
    {
        if(instance)
            delete instance;
        instance = nullptr;
    }

    ~yaml_lexer()
    {
        if(ylexer) ylexer->delete_instance<struct lexer> (ylexer);
        if(ytoken) delete ytoken;
    }
};

}

#endif
