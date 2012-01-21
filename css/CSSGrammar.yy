/*
 * Copyright 2010-2012 Esrille Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

%name-prefix "CSS"

%parse-param {CSSParser* parser}
%lex-param   {CSSParser* parser}

%{

#include <iostream>

#include "utf.h"

#include "MediaListImp.h"
#include "css/CSSRuleImp.h"
#include "css/CSSSelector.h"
#include "css/CSSImportRuleImp.h"
#include "css/CSSMediaRuleImp.h"
#include "css/CSSStyleDeclarationImp.h"
#include "css/CSSStyleRuleImp.h"
#include "css/CSSStyleSheetImp.h"
#include "CSSGrammar.h"

#include <org/w3c/dom/stylesheets/MediaList.h>

using namespace org::w3c::dom::bootstrap;
using namespace org::w3c::dom::css;

%}

%union {
    long integer;
    org::w3c::dom::bootstrap::CSSParserNumber number;
    org::w3c::dom::bootstrap::CSSParserString text;
    org::w3c::dom::bootstrap::CSSParserTerm term;
    org::w3c::dom::bootstrap::CSSParserExpr* expr;
    org::w3c::dom::bootstrap::CSSPrimarySelector* primarySelector;
    org::w3c::dom::bootstrap::CSSSimpleSelector* simpleSelector;
    org::w3c::dom::bootstrap::CSSSelector* selector;
    org::w3c::dom::bootstrap::CSSSelectorsGroup* selectorsGroup;
    org::w3c::dom::bootstrap::CSSRuleImp* cssRule;
}

/* %expect 19 */

%token START_STYLESHEET
%token START_DECLARATION_LIST
%token START_EXPRESSION

%token END 0 "end of file"
%token <number> ANGLE_DEG
%token <number> ANGLE_RAD
%token <number> ANGLE_GRAD
%token CDC
%token CDO
%token CHARSET_SYM
%token DASHMATCH
%token <term> DIMEN  /* This is also an unknown unit. */
%token <number> EMS
%token <number> EXS
%token FONT_FACE_SYM
%token <number> FREQ_HZ
%token <number> FREQ_KHZ
%token <text> FUNCTION
%token <text> HASH_COLOR
%token <text> HASH_IDENT
%token <text> IDENT
%token IMPORT_SYM
%token <text> IMPORTANT_SYM
%token INCLUDES
%token <number> LENGTH_PX
%token <number> LENGTH_CM
%token <number> LENGTH_MM
%token <number> LENGTH_IN
%token <number> LENGTH_PT
%token <number> LENGTH_PC
%token MEDIA_SYM
%token NAMESPACE_SYM
%token NOT
%token <number> NUMBER
%token PAGE_SYM
%token <number> PERCENTAGE
%token PREFIXMATCH
%token <number> PX
%token S
%token <text> STRING
%token <text> BAD_STRING
%token SUBSTRINGMATCH
%token SUFFIXMATCH
%token <number> TIME_MS
%token <number> TIME_S
%token <text> UNICODERANGE
%token <text> URI

%type <text> class;
%type <text> id;
%type <text> element_name;
%type <text> namespace_prefix;
%type <text> universal;
%type <text> ident_term;
%type <text> property;
%type <text> prio;
%type <text> uri_term;
%type <integer> combinator
%type <integer> attrib_op
%type <integer> unary_operator
%type <integer> operator
%type <integer> declaration
%type <expr> expr
%type <expr> term_list
%type <expr> expression
%type <term> term
%type <term> numeric_term
%type <term> unary_term
%type <term> hexcolor
%type <term> function
%type <term> functional_pseudo
%type <term> expression_term
%type <cssRule> import
%type <cssRule> ruleset
%type <cssRule> media
%type <cssRule> page
%type <cssRule> font_face
%type <selectorsGroup> selectors_group
%type <selector> selector
%type <primarySelector> type_selector
%type <primarySelector> simple_selector
%type <simpleSelector> negation_arg
%type <simpleSelector> attrib
%type <simpleSelector> simple_selector_seq
%type <simpleSelector> optional_simple_selector_seq
%type <simpleSelector> simple_selector_term
%type <simpleSelector> pseudo
%type <simpleSelector> negation

%%

start
  : START_STYLESHEET stylesheet
  | START_DECLARATION_LIST declaration_list
  | START_EXPRESSION expr {
        parser->setExpression($2);
    }
  ;

stylesheet
  : optional_sgml CHARSET_SYM optional_space STRING optional_space ';'
    optional_sgml optional_namespaces statement_list
  | optional_sgml optional_namespaces statement_list
  | optional_sgml CHARSET_SYM error_non_block ';'
    optional_sgml optional_namespaces statement_list
  | optional_sgml CHARSET_SYM error_block
    optional_sgml optional_namespaces statement_list
  ;
import
  : IMPORT_SYM optional_space uri_term optional_space medium_list ';' optional_space {
        if (!parser->isImportable())
            $$ = 0;
        else {
            CSSImportRuleImp* rule = new(std::nothrow) CSSImportRuleImp($3);
            if (rule) {
                if (MediaListImp* mediaList = parser->getMediaList()) {
                    rule->setMediaList(mediaList);
                    mediaList->clear();
                }
            }
            $$ = rule;
        }
    }
  | IMPORT_SYM optional_space uri_term optional_space             ';' optional_space {
        if (!parser->isImportable())
            $$ = 0;
        else {
            CSSImportRuleImp* rule = new(std::nothrow) CSSImportRuleImp($3);
            $$ = rule;
        }
    }
  | IMPORT_SYM error_block optional_sgml {
        $$ = 0;
    }
  | IMPORT_SYM error_non_block ';' optional_sgml {
        $$ = 0;
    }
  ;
namespace
  : NAMESPACE_SYM optional_space namespace_prefix optional_space uri_term optional_space ';' optional_space
  | NAMESPACE_SYM optional_space                                 uri_term optional_space ';' optional_space
  ;
namespace_prefix
  : IDENT
  ;
media
  : MEDIA_SYM optional_space {
        parser->setMediaRule(new(std::nothrow) CSSMediaRuleImp);
    }
    medium_list
   '{' optional_space optional_rulesets error_non_block '}' optional_space {
        CSSMediaRuleImp* mediaRule = parser->getMediaRule();
        if (mediaRule) {
            if (MediaListImp* mediaList = parser->getMediaList()) {
                mediaRule->setMediaList(mediaList);
                mediaList->clear();
            }
            parser->setMediaRule(0);
        }
        $$ = mediaRule;
    }
  | MEDIA_SYM error_block optional_sgml {
        $$ = 0;
    }
  | MEDIA_SYM error_non_block ';' optional_sgml {
        $$ = 0;
    }
  ;
medium
  : IDENT optional_space {
        if (MediaListImp* mediaList = parser->getMediaList())
            mediaList->appendMedium($1);
    }
  ;
page
  : PAGE_SYM optional_space IDENT pseudo_page optional_space '{' optional_space declaration_list '}' optional_space {
        $$ = 0;
    }
  | PAGE_SYM optional_space       pseudo_page optional_space '{' optional_space declaration_list '}' optional_space {
        $$ = 0;
    }
  | PAGE_SYM optional_space IDENT             optional_space '{' optional_space declaration_list '}' optional_space {
        $$ = 0;
    }
  | PAGE_SYM optional_space                                  '{' optional_space declaration_list '}' optional_space {
        $$ = 0;
    }
  | PAGE_SYM error_block optional_sgml {
        $$ = 0;
    }
  | PAGE_SYM error_non_block ';' optional_sgml {
        $$ = 0;
    }
  ;
pseudo_page
  : ':' IDENT
  ;
font_face
  : FONT_FACE_SYM optional_space '{' optional_space declaration_list '}' optional_space
  | FONT_FACE_SYM error_block optional_sgml {
        $$ = 0;
    }
  | FONT_FACE_SYM error_non_block ';' optional_sgml {
        $$ = 0;
    }
  ;
operator
  : '/' optional_space {
        $$ = '/';
    }
  | ',' optional_space {
        $$ = ',';
    }
  | /* empty */ {
        $$ = 0;
    }
  ;
combinator
  : '+' optional_space {
        $$ = CSSPrimarySelector::AdjacentSibling;
  }
  | '>' optional_space {
        $$ = CSSPrimarySelector::Child;
  }
  | '~' optional_space {
        $$ = CSSPrimarySelector::GeneralSibling;
  }
  | /* empty */ {
        $$ = CSSPrimarySelector::Descendant;
  }
  ;
unary_operator
  : '-' {
        $$ = -1;
    }
  | '+' {
        $$ = 1;
    }
  ;
property
  : IDENT optional_space
  ;
ruleset
  : selectors_group {
        if ($1) {
            if (!$1->isValid()) {
                delete $1;
                $1 = 0;
            } else
                parser->setStyleDeclaration(new(std::nothrow) CSSStyleDeclarationImp);
        }
    }
    '{' optional_space declaration_list '}' optional_space {
        if ($1) {
            $$ = new(std::nothrow) CSSStyleRuleImp($1, parser->getStyleDeclaration());
            parser->setStyleDeclaration(0);
        } else
            $$ = 0;
    }
  ;
selectors_group
  : selector {
        $$ = new(std::nothrow) CSSSelectorsGroup($1);
    }
  | selectors_group ',' optional_space selector {
        if ($1)
            $1->append($4);
        $$ = $1;
    }
  | error {
        $$ = 0;
    }
  ;
selector
  : simple_selector {
        $$ = new(std::nothrow) CSSSelector($1);
    }
  | selector combinator simple_selector {
        if ($1)
            $1->append($2, $3);
        $$ = $1;
    }
  ;
simple_selector
  : type_selector optional_simple_selector_seq optional_space {
        if (!$2)
            $$ = $1;
        else if (CSSPrimarySelector* primary = dynamic_cast<CSSPrimarySelector*>($2)) {
            if ($1) {
                primary->setNamespacePrefix($1->getNamespacePrefix());
                primary->setName($1->getName());
            }
            $$ = primary;
        } else {
            if ($1)
                $1->append($2);
            $$ = $1;
        }
    }
  | universal     optional_simple_selector_seq optional_space {
        CSSPrimarySelector* primary;
        if (primary = dynamic_cast<CSSPrimarySelector*>($2))
            primary->setNamespacePrefix($1);
        else {
            primary = new(std::nothrow) CSSPrimarySelector($1);
            if (primary)
                primary->append($2);
        }
        $$ = primary;
    }
  |                        simple_selector_seq optional_space {
        CSSPrimarySelector* primary;
        if (!(primary = dynamic_cast<CSSPrimarySelector*>($1))) {
            primary = new(std::nothrow) CSSPrimarySelector;
            if (primary)
                primary->append($1);
        }
        $$ = primary;
    }
  ;
class
  : '.' IDENT {
        $$ = $2;
    }
  | '.' error IDENT {
        $3.clear();
        $$ = $3;
    }
  ;
id
  : HASH_IDENT
  | '#' error IDENT {
        $3.clear();
        $$ = $3;
    }
  ;
element_name
  : IDENT
  ;
attrib
  : '[' optional_space namespace_prefix IDENT optional_space attrib_op optional_space ident_term optional_space ']' {
        $$ = new(std::nothrow) CSSAttributeSelector($3, $4.toString(parser->getCaseSensitivity()), $6, $8.toString(true));
    }
  | '[' optional_space namespace_prefix IDENT optional_space                                                    ']' {
        $$ = new(std::nothrow) CSSAttributeSelector($3, $4.toString(parser->getCaseSensitivity()));
    }
  | '[' optional_space                  IDENT optional_space attrib_op optional_space ident_term optional_space ']' {
        $$ = new(std::nothrow) CSSAttributeSelector($3.toString(parser->getCaseSensitivity()), $5, $7.toString(true));
    }
  | '[' optional_space                  IDENT optional_space                                                    ']' {
        $$ = new(std::nothrow) CSSAttributeSelector($3.toString(parser->getCaseSensitivity()));
    }
  ;
pseudo
  : ':' IDENT {
        $$ = CSSPseudoSelector::createPseudoSelector(CSSPseudoSelector::PseudoClass, $2.toString(false));
    }
  | ':' functional_pseudo {
        $$ = CSSPseudoSelector::createPseudoSelector(CSSPseudoSelector::PseudoClass, $2);
    }
  | ':' ':' IDENT {
        $$ = CSSPseudoSelector::createPseudoSelector(CSSPseudoSelector::PseudoElement, $3.toString(false));
    }
  | ':' ':' functional_pseudo {
        $$ = CSSPseudoSelector::createPseudoSelector(CSSPseudoSelector::PseudoElement, $3);
    }
  ;
declaration
  : property ':' optional_space expr prio {
        if (CSSStyleDeclarationImp* decl = parser->getStyleDeclaration())
            $$ = decl->appendProperty($1, $4, $5);
        else
            $$ = CSSStyleDeclarationImp::Unknown;
    }
  | property ':' optional_space expr {
        if (CSSStyleDeclarationImp* decl = parser->getStyleDeclaration())
            decl->appendProperty($1, $4);
        else
            $$ = CSSStyleDeclarationImp::Unknown;
    }
  | /* empty */ {
        if (CSSStyleDeclarationImp* decl = parser->getStyleDeclaration())
            $$ = decl->cancelAppend();
        else
            $$ = CSSStyleDeclarationImp::Unknown;
    }
  ;
prio
  : IMPORTANT_SYM optional_space
  ;
expr
  : term {
        $$ = new(std::nothrow) CSSParserExpr;
        if ($$) {
            $1.op = '\0';
            $$->push_front($1);
        }
    }
  | term term_list {
        if ($2) {
            $1.op = '\0';
            $2->push_front($1);
        }
        $$ = $2;
    }
  ;
term
  : unary_term
  | STRING       optional_space {
        $$.unit = CSSPrimitiveValue::CSS_STRING;
        $$.text = $1;
    }
  | IDENT        optional_space {
        $$.unit = CSSPrimitiveValue::CSS_IDENT;
        $$.text = $1;
    }
  | URI          optional_space {
        $$.unit = CSSPrimitiveValue::CSS_URI;
        $$.text = $1;
    }
  | UNICODERANGE optional_space {
        $$.unit = CSSPrimitiveValue::CSS_UNICODE_RANGE;
        $$.text = $1;
    }
  | hexcolor
  | function /* In CSS3, function seems to be moved to numeric_term... */
  ;
function
  : FUNCTION optional_space expr ')' optional_space {
        $$.unit = CSSParserTerm::CSS_TERM_FUNCTION;
        $$.text = $1;
        $$.expr = $3;
    }
  | FUNCTION optional_space error_parenthesis ')' optional_space {
        $$.unit = CSSParserTerm::CSS_TERM_FUNCTION;
        $$.text = $1;
        $$.expr = 0;
    }
  ;
/*
 * There is a constraint on the color that it must
 * have either 3 or 6 hex-digits (i.e., [0-9a-fA-F])
 * after the "#"; e.g., "#000" is OK, but "#abcd" is not.
 */
hexcolor
  : HASH_COLOR optional_space {
        $$.unit = CSSPrimitiveValue::CSS_RGBCOLOR;
        $$.text = $1;
        $$.rgb = $$.text.toRGB();
    }
  ;

/*
 * CSS 3 Selectors
 */
type_selector
  : namespace_prefix element_name {
        $$ = new(std::nothrow) CSSPrimarySelector($1, $2.toString(parser->getCaseSensitivity()));
    }
  |                  element_name {
        $$ = new(std::nothrow) CSSPrimarySelector(u"*", $1.toString(parser->getCaseSensitivity()));  // TODO: support css3-selectors later...
    }
  ;
namespace_prefix
  : IDENT '|'
  | '*'   '|' {
        $$ = { u"*", 1 };
    }
  |       '|' {
        $$ = { u"", 0 };
    }
  ;
universal
  : namespace_prefix '*'
  |                  '*' {
        $$ = { u"*", 0 };  // TODO: support css3-selectors later...
    }
  ;
functional_pseudo
  : FUNCTION optional_space expression ')' {
        $$.unit = CSSParserTerm::CSS_TERM_FUNCTION;
        $$.text = $1;
        $$.expr = $3;
    }
  ;
expression
  /* In CSS3, the expressions are identifiers, strings, */
  /* or of the form "an+b" */
  : expression_term optional_space {
        $$ = new(std::nothrow) CSSParserExpr;
        if ($$)
            $$->push_front($1);
    }
  | expression expression_term optional_space {
        if ($1)
            $1->push_back($2);
        $$ = $1;
    }
  ;
expression_term
  : '+' {
        $$.unit = CSSParserTerm::CSS_TERM_OPERATOR;
        $$.op = '+';
    }
  | '-' {
        $$.unit = CSSParserTerm::CSS_TERM_OPERATOR;
        $$.op = '-';
    }
  | DIMEN {
        $$ = $1;
    }
  | NUMBER {
        $$.unit = CSSPrimitiveValue::CSS_NUMBER;
        $$.number = $1;
    }
  | STRING {
        $$.unit = CSSPrimitiveValue::CSS_STRING;
        $$.text = $1;
    }
  | IDENT {
        $$.unit = CSSPrimitiveValue::CSS_IDENT;
        $$.text = $1;
    }
  ;
negation
  : NOT optional_space negation_arg optional_space ')' {
        $$ = new(std::nothrow) CSSNegationPseudoClassSelector($3);
    }
  ;
negation_arg
  : type_selector
  | universal {
        $$ = new(std::nothrow) CSSPrimarySelector($1);
    }
  | id {
        $$ = new(std::nothrow) CSSIDSelector($1.toString(true));
    }
  | class {
        $$ = new(std::nothrow) CSSClassSelector($1.toString(true));
    }
  | attrib
  | pseudo
  ;

/*
 * Extended rules for bison
 */
statement_list
  : /* empty */
  | statement_list import optional_sgml {
        if (CSSStyleSheetImp* styleSheet = parser->getStyleSheet()) {
            styleSheet->append($2, parser->getDocument());
        }
    }
  | statement_list ruleset   optional_sgml {
        if (CSSStyleSheetImp* styleSheet = parser->getStyleSheet()) {
            if ($2) {
                styleSheet->append($2, parser->getDocument());
                parser->disableImport();
            }
        }
    }
  | statement_list media     optional_sgml {
        if (CSSStyleSheetImp* styleSheet = parser->getStyleSheet()) {
            if ($2) {
                styleSheet->append($2, parser->getDocument());
                parser->disableImport();
            }
        }
    }
  | statement_list page      optional_sgml {
        if (CSSStyleSheetImp* styleSheet = parser->getStyleSheet()) {
            if ($2) {
                styleSheet->append($2, parser->getDocument());
                parser->disableImport();
            }
        }
    }
  | statement_list font_face optional_sgml {
        if (CSSStyleSheetImp* styleSheet = parser->getStyleSheet()) {
            if ($2) {
                styleSheet->append($2, parser->getDocument());
                parser->disableImport();
            }
        }
    }
  | statement_list '@' IDENT error_block optional_sgml {
        CSSerror(parser, "syntax error, invalid at rule");
    }
  | statement_list '@' IDENT error_non_block ';' optional_sgml {
        CSSerror(parser, "syntax error, invalid at rule");
    }
  | statement_list CHARSET_SYM error_block optional_sgml {
        CSSerror(parser, "syntax error, invalid charset rule");
    }
  | statement_list CHARSET_SYM error_non_block ';' optional_sgml {
        CSSerror(parser, "syntax error, invalid charset rule");
    }
  | statement_list error_block optional_sgml {
        CSSerror(parser, "syntax error, invalid rule set");
    }
  ;
optional_namespaces
  : /* empty */
  | optional_namespaces namespace optional_sgml
  ;
uri_term
  : STRING
  | URI
  ;
optional_rulesets
  : /* empty */
  | optional_rulesets ruleset {
        if (CSSMediaRuleImp* mediaRule = parser->getMediaRule()) {
            mediaRule->append($2);
        }
    }
  | optional_rulesets error_block optional_sgml {
        CSSerror(parser, "syntax error, invalid ruleset");
    }
  | optional_rulesets error_non_block ';' optional_sgml {
        CSSerror(parser, "syntax error, invalid ruleset");
    }
  ;
medium_list
  : medium
  | medium_list ',' optional_space medium
  ;
declaration_list
  : declaration {
        if (CSSStyleDeclarationImp* decl = parser->getStyleDeclaration())
            decl->commitAppend();
    }
  | declaration_list ';' optional_space declaration {
        if (CSSStyleDeclarationImp* decl = parser->getStyleDeclaration())
            decl->commitAppend();
    }
  | declaration error {
        if (CSSStyleDeclarationImp* decl = parser->getStyleDeclaration())
            decl->cancelAppend();
    }
  | declaration_list ';' optional_space declaration error {
        if (CSSStyleDeclarationImp* decl = parser->getStyleDeclaration())
            decl->cancelAppend();
    }
  | declaration_list ';' optional_space error invalid_construct_list error
  | declaration_list ';' optional_space error
  | error invalid_construct_list error
  | error
  ;
simple_selector_term
  : id {
        $$ = new(std::nothrow) CSSIDSelector($1.toString(true));
    }
  | class {
        $$ = new(std::nothrow) CSSClassSelector($1.toString(true));
    }
  | attrib
  | pseudo
  | negation
  ;
simple_selector_seq
  : simple_selector_term
  | simple_selector_seq simple_selector_term {
        CSSPrimarySelector* primary;
        if (primary = dynamic_cast<CSSPrimarySelector*>($1))
            primary->append($2);
        else {
            primary = new(std::nothrow) CSSPrimarySelector();
            if (primary) {
                primary->append($1);
                primary->append($2);
            }
        }
        $$ = primary;
    }
  ;
optional_simple_selector_seq
  : /* empty */ {
        $$ = 0;
    }
  | simple_selector_seq
  ;
ident_term
  : IDENT
  | STRING
  ;
term_list
  : operator term {
        $2.op = $1;
        $$ = new(std::nothrow) CSSParserExpr;
        if ($$)
            $$->push_front($2);
    }
  | term_list operator term {
        $3.op = $2;
        if ($1)
            $1->push_back($3);
        $$ = $1;
    }
  ;
unary_term
  : unary_operator numeric_term {
        $2.number.number *= $1;
        $$ = $2;
    }
  | numeric_term
  ;
numeric_term
  : NUMBER optional_space {
        $$.unit = CSSPrimitiveValue::CSS_NUMBER;
        $$.number = $1;
    }
  | PERCENTAGE optional_space {
        $$.unit = CSSPrimitiveValue::CSS_PERCENTAGE;
        $$.number = $1;
    }
  | LENGTH_PX optional_space {
        $$.unit = CSSPrimitiveValue::CSS_PX;
        $$.number = $1;
    }
  | LENGTH_CM optional_space {
        $$.unit = CSSPrimitiveValue::CSS_CM;
        $$.number = $1;
    }
  | LENGTH_MM optional_space {
        $$.unit = CSSPrimitiveValue::CSS_MM;
        $$.number = $1;
    }
  | LENGTH_IN optional_space {
        $$.unit = CSSPrimitiveValue::CSS_IN;
        $$.number = $1;
    }
  | LENGTH_PT optional_space {
        $$.unit = CSSPrimitiveValue::CSS_PT;
        $$.number = $1;
    }
  | LENGTH_PC optional_space {
        $$.unit = CSSPrimitiveValue::CSS_PC;
        $$.number = $1;
    }
  | EMS optional_space {
        $$.unit = CSSPrimitiveValue::CSS_EMS;
        $$.number = $1;
    }
  | EXS optional_space {
        $$.unit = CSSPrimitiveValue::CSS_EXS;
        $$.number = $1;
    }
  | ANGLE_DEG optional_space {
        $$.unit = CSSPrimitiveValue::CSS_DEG;
        $$.number = $1;
    }
  | ANGLE_RAD optional_space {
        $$.unit = CSSPrimitiveValue::CSS_RAD;
        $$.number = $1;
    }
  | ANGLE_GRAD optional_space {
        $$.unit = CSSPrimitiveValue::CSS_GRAD;
        $$.number = $1;
    }
  | TIME_MS optional_space {
        $$.unit = CSSPrimitiveValue::CSS_MS;
        $$.number = $1;
    }
  | TIME_S optional_space {
        $$.unit = CSSPrimitiveValue::CSS_S;
        $$.number = $1;
    }
  | FREQ_HZ optional_space {
        $$.unit = CSSPrimitiveValue::CSS_HZ;
        $$.number = $1;
    }
  | FREQ_KHZ optional_space {
        $$.unit = CSSPrimitiveValue::CSS_KHZ;
        $$.number = $1;
    }
  ;
attrib_op
  : PREFIXMATCH {
        $$ = CSSAttributeSelector::PrefixMatch;
    }
  | SUFFIXMATCH {
        $$ = CSSAttributeSelector::SuffixMatch;
    }
  | SUBSTRINGMATCH {
        $$ = CSSAttributeSelector::SubstringMatch;
    }
  | '=' {
        $$ = CSSAttributeSelector::Equals;
    }
  | INCLUDES {
        $$ = CSSAttributeSelector::Includes;
    }
  | DASHMATCH {
        $$ = CSSAttributeSelector::DashMatch;
    }
  ;
optional_sgml
  : /* empty */
  | optional_sgml S
  | optional_sgml CDO
  | optional_sgml CDC
  ;
optional_space
  : /* empty */
  | optional_space S
  ;
invalid_construct
  : '{' error invalid_construct_list error '}'
  | '{' error '}'
  | '(' error invalid_construct_list error ')'
  | '(' error ')'
  | '[' error invalid_construct_list error ']'
  | '[' error ']'
  ;
invalid_construct_list
  : invalid_construct
  | invalid_construct_list error invalid_construct
  ;
invalid_block
  : '{' error invalid_construct_list error '}'
  | '{' error '}'
  ;
invalid_non_block
  : '(' error invalid_non_block_list error ')'
  | '(' error ')'
  | '[' error invalid_non_block_list error ']'
  | '[' error ']'
  ;
invalid_non_block_list
  : invalid_non_block
  | invalid_non_block_list error invalid_non_block
  ;
invalid_parenthesis
  : '(' error invalid_parenthesis_list error ')'
  | '(' error ')'
invalid_parenthesis_list
  : invalid_parenthesis
  | invalid_parenthesis_list error invalid_parenthesis
  ;
error_block
  : error invalid_non_block_list error invalid_block
  | error invalid_block
  | invalid_block
  ;
error_non_block
  : error invalid_non_block_list error
  | error
  ;
error_parenthesis
  : error invalid_parenthesis_list error
  | error
  ;
