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

// usage: re2c --case-insensitive -u -i -o CSSLex.cpp CSSLex.re

#include <cmath>

#include <Object.h>
#include <org/w3c/dom/css/CSSPrimitiveValue.h>

#include "css/CSSSelector.h"
#include "CSSGrammar.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

int CSSTokenizer::getToken()
{
    // cf. http://www.w3.org/TR/css3-syntax/#syntax
start:
    const char16_t* yytext = yyin;
    switch (mode) {
    case StartStyleSheet:
        mode = Normal;
        return START_STYLESHEET;
        break;
    case StartDeclarationList:
        mode = Normal;
        return START_DECLARATION_LIST;
        break;
    case StartExpression:
        mode = Normal;
        return START_EXPRESSION;
    case End:
        if (!openConstructs.empty()) {
            int token = openConstructs.front();
            openConstructs.pop_front();
            return token;
        }
        return END;
    default:
        break;
    }

/*!re2c

    h = [0-9a-fA-F];
    nonascii = [\X0080-\Xffff];
    unicode = "\\" h{1,6} [ \t\r\n\f]?;
    escape = unicode | "\\" [ -~\X0080-\Xffff];
    nmstart = [_a-zA-Z] | nonascii | escape;
    nmchar = [_a-zA-Z0-9-] | nonascii | escape;
    nl = "\n" | "\r\n" | "\r" | "\f";
    string1 = "\"" ([^\X0000\n\r\f\\"] | "\\" nl | "'" | nonascii | escape)* "\"";
    string2 = "'" ([^\X0000\n\r\f\\'] | "\\" nl | "\"" | nonascii | escape)* "'";
    ident = [-]? nmstart nmchar*;
    name = nmchar+;
    num = [0-9]+ | [0-9]* "." [0-9]+;
    integer = [0-9]+;
    string = string1 | string2;
    url = ([!#$%&*-~] | nonascii | escape)*;
    w =[ \t\r\n\f]*;
    range = "?"{1,6} | h ("?"{0,5} | h ("?"{0,4} | h ("?"{0,3} | h ("?"{0,2} | h ("?"? | h)))));
    hexcolor = h{3} | h{6};

    bad_string1 = "\"" ([^\X0000\n\r\f\\"] | "\\" nl | "'" | nonascii | escape)* (nl | "\X0000");
    bad_string2 = "'" ([^\X0000\n\r\f\\'] | "\\" nl | "\"" | nonascii | escape)* (nl | "\X0000");
    bad_string = bad_string1 | bad_string2;

    D = 'd' | "\\" "0"{0,4} ("44"|"64") ("\r\n" | [ \t\r\n\f])?;
    E = 'e' | "\\" "0"{0,4} ("45"|"65") ("\r\n" | [ \t\r\n\f])?;
    N = 'n' | "\\" "0"{0,4} ("4e"|"6e") ("\r\n" | [ \t\r\n\f])? | '\\n';
    O = 'o' | "\\" "0"{0,4} ("4f"|"6f") ("\r\n" | [ \t\r\n\f])? | '\\o';
    T = 't' | "\\" "0"{0,4} ("54"|"74") ("\r\n" | [ \t\r\n\f])? | '\\t';
    V = 'v' | "\\" "0"{0,4} ("58"|"78") ("\r\n" | [ \t\r\n\f])? | '\\v';

    re2c:define:YYCTYPE  = "char16_t";
    re2c:define:YYCURSOR = yyin;
    re2c:define:YYMARKER = yymarker;
    re2c:define:YYLIMIT = yylimit;
    re2c:yyfill:enable   = 0;
    re2c:indent:top      = 1;
    re2c:indent:string = "    " ;

    [ \t\r\n\f]+        {return S;}

    "/*" [^\X0000*]* "*"+ ([^\X0000/*][^\X0000*]* "*"+)* "/"   {goto start;}

    "<!--"              {
                            openConstructs.push_front(CDC);
                            return CDO;
                        }
    "-->"               {
                            if (!openConstructs.empty() && openConstructs.front() == CDC)
                                openConstructs.pop_front();
                            return CDC;
                        }
    "~="                {return INCLUDES;}
    "|="                {return DASHMATCH;}
    "^="                {return PREFIXMATCH;}
    "$="                {return SUFFIXMATCH;}
    "*="                {return SUBSTRINGMATCH;}

    string              {
                            CSSlval.text = { yytext + 1, yyin - yytext - 2 };
                            return STRING;
                        }

    ident               {
                            CSSlval.text = { yytext, yyin - yytext };
                            return IDENT;
                        }

    "#" hexcolor        {
                            CSSlval.text = { yytext + 1, yyin - yytext - 1 };
                            return HASH_COLOR;
                        }

    "#" ident           {
                            CSSlval.text = { yytext + 1, yyin - yytext - 1 };
                            return HASH_IDENT;
                        }

    ":" N O T "("       {
                            openConstructs.push_front(')');
                            return NOT;
                        }

    '@import'           {return IMPORT_SYM;}
    '@page'             {return PAGE_SYM;}
    '@media'            {return MEDIA_SYM;}
    '@font-face'        {return FONT_FACE_SYM;}
    '@charset'          {return CHARSET_SYM;}
    '@namespace'        {return NAMESPACE_SYM;}

    "!" w 'important'   {
                            CSSlval.text = { u"!important", 10 };
                            return IMPORTANT_SYM;
                        }

    num 'em'            {
                            CSSlval.number = parseNumber(yytext, yyin - yytext - 2);
                            return EMS;
                        }
    num 'ex'            {
                            CSSlval.number = parseNumber(yytext, yyin - yytext - 2);
                            return EXS;
                        }
    num 'px'            {
                            CSSlval.number = parseNumber(yytext, yyin - yytext - 2);
                            return LENGTH_PX;
                        }
    num 'cm'            {
                            CSSlval.number = parseNumber(yytext, yyin - yytext - 2);
                            return LENGTH_CM;
                        }
    num 'mm'            {
                            CSSlval.number = parseNumber(yytext, yyin - yytext - 2);
                            return LENGTH_MM;
                        }
    num 'in'            {
                            CSSlval.number = parseNumber(yytext, yyin - yytext - 2);
                            return LENGTH_IN;
                        }
    num 'pt'            {
                            CSSlval.number = parseNumber(yytext, yyin - yytext - 2);
                            return LENGTH_PT;
                        }
    num 'pc'            {
                            CSSlval.number = parseNumber(yytext, yyin - yytext - 2);
                            return LENGTH_PC;
                        }
    num 'deg'           {
                            CSSlval.number = parseNumber(yytext, yyin - yytext - 3);
                            return ANGLE_DEG;
                        }
    num 'rad'           {
                            CSSlval.number = parseNumber(yytext, yyin - yytext - 3);
                            return ANGLE_RAD;
                        }
    num 'grad'          {
                            CSSlval.number = parseNumber(yytext, yyin - yytext - 4);
                            return ANGLE_GRAD;
                        }
    num 'ms'            {
                            CSSlval.number = parseNumber(yytext, yyin - yytext - 2);
                            return TIME_MS;
                        }
    num 's'             {
                            CSSlval.number = parseNumber(yytext, yyin - yytext - 1);
                            return TIME_S;
                        }
    num 'Hz'            {
                            CSSlval.number = parseNumber(yytext, yyin - yytext - 2);
                            return FREQ_HZ;
                        }
    num 'kHz'           {
                            CSSlval.number = parseNumber(yytext, yyin - yytext - 3);
                            return FREQ_KHZ;
                        }
    num  ident          {
                            const char16_t* end;
                            CSSlval.term.unit = css::CSSPrimitiveValue::CSS_DIMENSION;
                            CSSlval.term.number = parseNumber(yytext, yyin - yytext, &end);
                            CSSlval.term.text = { end, yyin - end };
                            return DIMEN;
                        }
    num '%'             {
                            CSSlval.number = parseNumber(yytext, yyin - yytext - 1);
                            return PERCENTAGE;
                        }
    num                 {
                            CSSlval.number = parseNumber(yytext, yyin - yytext);
                            return NUMBER;
                        }

    integer             {
                            CSSlval.integer = parseInt(yytext, yyin - yytext);
                            return INTEGER;
                        }

    'url(' w string w ")"   {
                                parseURL(yytext, yyin - yytext, &CSSlval.text);
                                return URI;
                            }
    'url(' w url w ")"      {
                                parseURL(yytext, yyin - yytext, &CSSlval.text);
                                return URI;
                            }

    ident "("               {
                                openConstructs.push_front(')');
                                CSSlval.text = { yytext, yyin - yytext - 1 };
                                return FUNCTION;
                            }

    'U+' range                  {
                                    CSSlval.text = { yytext, yyin - yytext };
                                    return UNICODERANGE;
                                }
    'U+' h{1,6} "-" h{1,6}      {
                                    CSSlval.text = { yytext, yyin - yytext };
                                    return UNICODERANGE;
                                }
    bad_string          {
                            if (yylimit <= yyin) {
                                CSSlval.text = { yytext + 1, yylimit - yytext - 1 };
                                mode = End;
                                return STRING;
                            }
                            CSSlval.text = { yytext + 1, yyin - yytext - 1 };
                            return BAD_STRING;
                        }
    "{"                 {
                            openConstructs.push_front('}');
                            return *yytext;
                        }
    "}"                 {
                            if (!openConstructs.empty() && openConstructs.front() == '}')
                                openConstructs.pop_front();
                            return *yytext;
                        }
    "["                 {
                            openConstructs.push_front(']');
                            return *yytext;
                        }
    "]"                 {
                            if (!openConstructs.empty() && openConstructs.front() == ']')
                                openConstructs.pop_front();
                            return *yytext;
                        }
    "("                 {
                            openConstructs.push_front(')');
                            return *yytext;
                        }
    ")"                 {
                            if (!openConstructs.empty() && openConstructs.front() == ')')
                                openConstructs.pop_front();
                            return *yytext;
                        }
    "\X0000"            {
                            mode = End;
                            goto start;
                        }
    .                   {return *yytext;}

*/
    ;
}

void CSSTokenizer::parseURL(const char16_t* text, ssize_t length, CSSParserString* string)
{
    text += 4;
    length -= 5;
    while (isSpace(*text)) {
        ++text;
        --length;
    }
    while (isSpace(text[length - 1]))
        --length;
    if (*text == u'"' || *text == u'\'') {
        ++text;
        length -= 2;
    }
    string->text = text;
    string->length = length;
}

}}}}  // org::w3c::dom::bootstrap
