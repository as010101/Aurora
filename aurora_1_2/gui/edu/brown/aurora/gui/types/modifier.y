%{
import java.lang.Math;
import java.io.*;
import java.util.StringTokenizer;
import edu.brown.aurora.gui.*;
%}


/* YACC Declarations */
%token FILTER
%token DROP
%token PASS
%token MAP
%token UNKNOWN
%token RESTREAM
%token AGGREGATE
%token JOIN
%token BSORT
%token UNION
%token UPDATE_RELATION
%token READ_RELATION

%token ID
%token LAST
%token ALL
%token WHENEVER
%token NONE
%token TIMEOUT
%token SATISFIES

%token INTEGER
%token FLOAT
%token STRING
%token INT2FLOAT
%token INT2DOUBLE
%token FLOAT2DOUBLE
%token FLOAT2INT

%token NOT
%token AND
%token OR

%token TUPLENUM
%token VALUES
%token TUPLES

%left '-' '+'
%left '*' '/'
%left NEG /* negation--unary minus */
%right '^' /* exponentiation */

/* Grammar follows */
%%
input: /* empty string */
 | box { parsed = $1; }
 ;

box: FILTER '(' predlist '~' DROP ')' { $$ = new ParserVal("FILTER(" + $3.str + '~' + "DROP)", new Integer( ((Integer)($3.obj)).intValue() )); }
 | FILTER '(' predlist '~' PASS ')' { $$ = new ParserVal("FILTER(" + $3.str + '~' + "PASS)", new Integer( ((Integer)($3.obj)).intValue()+1 )); }
 | DROP '(' timeout ')' { $$ = new ParserVal( "DROP(" + $3.str + ")", $3.obj); }
 | MAP '(' maplist ')' { $$ = new ParserVal( "MAP(" + $3.str + ")", $3.obj); }
 | RESTREAM '(' identlist ')' { $$ = new ParserVal( "RESTREAM(" + $3.str + ")", $3.obj); }
 | AGGREGATE '(' aggregateParams ')' { $$ = new ParserVal( "AGGREGATE(" + $3.str + ")", $3.obj); }
 | JOIN '(' joinParams ')' { $$ = new ParserVal( "JOIN(" + $3.str + ")", $3.obj); }
 | BSORT '(' bsortParams ')' { $$ = new ParserVal( "BSORT(" + $3.str + ")", $3.obj); }
 | UNION '('  ')' { $$ = new ParserVal( "UNION(" + ")", null); }
 | UPDATE_RELATION '(' updateRelationParams ')' { $$ = new ParserVal( "UPDATE_RELATION(" + $3.str + ")", $3.obj); }
 | READ_RELATION '(' readRelationParams ')' { $$ = new ParserVal( "READ_RELATION(" + $3.str + ")", $3.obj); }
 ;

maplist: { $$ = new ParserVal("", null); } /* empty string */
 | ID '=' expr
  {
    Type t = getOutputAttributeType($1.str);
    if (t != null)
    {
      if (isEncoding)
      {
        if (t.equals(TypeManager.FLOAT_TYPE))
        {
          if ($3.obj.equals(TypeManager.INTEGER_TYPE))
          {
            $3.str = "INT2FLOAT("+$3.str+")";
            $3.obj = TypeManager.FLOAT_TYPE;
          }
        }
        if (t.equals(TypeManager.DOUBLE_TYPE))
        {
          if ($3.obj.equals(TypeManager.INTEGER_TYPE))
          {
            $3.str = "INT2DOUBLE("+$3.str+")";
            $3.obj = TypeManager.DOUBLE_TYPE;
          }
          if ($3.obj.equals(TypeManager.FLOAT_TYPE))
          {
            $3.str = "FLOAT2DOUBLE("+$3.str+")";
            $3.obj = TypeManager.DOUBLE_TYPE;
          }
        }
      }
      if (!t.equals($3.obj)) throw new ParsingException("Invalid Assignment!");
    }
    CompositeType c = new CompositeType("", true);
    try
    {
       c.insertAttribute(0, $1.str, ((Type)($3.obj)), ((PrimitiveType)($3.obj)).getLength());
    }
    catch(Exception e)
    {
      System.out.println(e);
    }
    $$ = new ParserVal($1.str+" = "+$3.str, c);
  }
 | ID '=' expr ',' maplist
 {
     Type t = getOutputAttributeType($1.str);
    if (t != null)
    {
      if (isEncoding)
      {
        if (t.equals(TypeManager.FLOAT_TYPE))
        {
          if ($3.obj.equals(TypeManager.INTEGER_TYPE))
          {
            $3.str = "INT2FLOAT("+$3.str+")";
            $3.obj = TypeManager.FLOAT_TYPE;
          }
        }
        if (t.equals(TypeManager.DOUBLE_TYPE))
        {
          if ($3.obj.equals(TypeManager.INTEGER_TYPE))
          {
            $3.str = "INT2DOUBLE("+$3.str+")";
            $3.obj = TypeManager.DOUBLE_TYPE;
          }
          if ($3.obj.equals(TypeManager.FLOAT_TYPE))
          {
            $3.str = "FLOAT2DOUBLE("+$3.str+")";
            $3.obj = TypeManager.DOUBLE_TYPE;
          }
        }
      }
      if (!t.equals($3.obj)) throw new ParsingException("Invalid Assignment!");
    }
    CompositeType c = (CompositeType)($5.obj);
     try
     {
       c.insertAttribute(0, $1.str, ((Type)($3.obj)), ((PrimitiveType)($3.obj)).getLength());
     }
     catch (Exception e)
     {
       System.out.println(e);
     }
    $$ = new ParserVal($1.str+" = "+$3.str + "," + $5.str, c);
 }
 ;

identlist: { $$ = new ParserVal("", new CompositeType("", true)); } /* empty string */
 | ID
{
     CompositeType c = new CompositeType("", true);
     try
     {
       String attributeName = "";
       if (isEncoding)
         attributeName = $1.str;
       else
         attributeName = getInputAttributeName($1.str);
       Type t = getInputAttributeType($1.str);
       c.insertAttribute(0, attributeName, t, ((PrimitiveType)t).getLength());
     }
     catch (Exception e)
     {
       System.out.println(e);
       throw new ParsingException("Duplicated attribute names");
     }
   $$ = new ParserVal(getInputAttributeName($1.str), c);
 }
 | ID ',' identlist
 {
     CompositeType c = (CompositeType)($3.obj);
     try
     {
       String attributeName = "";
       if (isEncoding)
         attributeName = $1.str;
       else
         attributeName = getInputAttributeName($1.str);
       Type t = getInputAttributeType($1.str);
       c.insertAttribute(0, attributeName, t, ((PrimitiveType)t).getLength());
     }
     catch (Exception e)
     {
       System.out.println(e);
       throw new ParsingException("Duplicated attribute names");
     }
   $$ = new ParserVal(getInputAttributeName($1.str) + "," + $3.str, c);
 }
 ;

aggregateParams: targetAttribute '=' aggregate '~' aggregateInputAttribute '~' windowSize '~' advance '~' timeout '~' bsortParams
{
  CompositeType c = (CompositeType)($13.obj);
  String attributeName = $1.str;

//  Object o = getOutputAttributeType($1.str);
  Object o = null;
  if (o != null)
  {
    $3.obj = getAggregateType($3.str, (Type)($5.obj));
    if ($3.obj == null) throw new ParsingException("Invalid Aggregate Function");
    if (isEncoding)
    {
      if (o.equals(TypeManager.FLOAT_TYPE))
      {
        if ($3.obj.equals(TypeManager.INTEGER_TYPE))
        {
          $3.str = "INT2FLOAT("+$3.str+")";
          $3.obj = TypeManager.FLOAT_TYPE;
        }
      }
      if (o.equals(TypeManager.DOUBLE_TYPE))
      {
        if ($3.obj.equals(TypeManager.INTEGER_TYPE))
        {
          $3.str = "INT2DOUBLE("+$3.str+")";
          $3.obj = TypeManager.DOUBLE_TYPE;
        }
        if ($3.obj.equals(TypeManager.FLOAT_TYPE))
        {
          $3.str = "FLOAT2DOUBLE("+$3.str+")";
          $3.obj = TypeManager.DOUBLE_TYPE;
        }
      }
    }
    if (!o.equals($3.obj)) throw new ParsingException("Invalid Assignment!");
    try
    {
       c.addAttribute(attributeName, ((Type)($3.obj)), ((PrimitiveType)($3.obj)).getLength());
    }
    catch (Exception e)
    {
      e.printStackTrace();
      throw new ParsingException("Duplicated attribute names");
    }
  }
  else /* if o == null*/
  {
    Type[] types = getAggregateTypes($3.str, (Type)($5.obj));
    if (types == null) throw new ParsingException("Invalid Aggregate Function");
    try
    {
      if (types.length==1)
         c.addAttribute(attributeName, types[0], ((PrimitiveType)(types[0])).getLength());
      else
        for (int i = 0; i < types.length; i++)
           c.addAttribute(attributeName+"."+i, types[i], ((PrimitiveType)(types[i])).getLength());
    }
    catch (Exception e)
    {
      e.printStackTrace();
      throw new ParsingException("Duplicated attribute names");
    }
  }
  $$ = new ParserVal(attributeName +  "=" + $3.str  + "~" + $5.str + "~" + $7.str + "~" + $9.str + "~" + $11.str + "~" + $13.str ,  c);
}
;

joinParams: pred '~' INTEGER '~' ID '~' slack '~'  ID '~' slack
{
  CompositeType c = new CompositeType("", true);
  try {
    if (inputTypes[0] != null)
      c.addAttributes(inputTypes[0], "LEFT.");
    if (inputTypes[1] != null)
      c.addAttributes(inputTypes[1], "RIGHT.");
  } catch (Exception e) {
//    e.printStackTrace();
    c = null;
  }
  String str = $1.str +  "~" + $3.str  + "~" + getInputAttributeName($5.str) + "~" + $7.str + "~" + getInputAttributeName($9.str) + "~" + $11.str;
  $$ = new ParserVal(str,  c);
}
;

bsortParams: orderOn '~' slack '~' groupByAttributes
{
     CompositeType c = (CompositeType)($5.obj);
     String attributeName = $1.str;
     try
     {
       if ($1.obj != null) {
         if (isEncoding)
           c.addAttribute($1.str,  (PrimitiveType)($1.obj) , ((PrimitiveType)($1.obj)).getLength());
        else
           c.addAttribute(getInputAttributeName($1.str),  (PrimitiveType)($1.obj) , ((PrimitiveType)($1.obj)).getLength());
         attributeName = getInputAttributeName($1.str);
       }
     }
     catch (Exception e)
     {
       System.out.println(e);
       throw new ParsingException("Duplicated attribute names");
     }
  $$ = new ParserVal(attributeName + "~" + $3.str + "~" +$5.str , c);
}
;

updateRelationParams: ID '~' groupByAttributes
{
  $$ = new ParserVal($1.str + "~" + $3.str , null);
}
;

readRelationParams: ID '~' groupByAttributes
{
  $$ = new ParserVal($1.str + "~" + $3.str , null);
}
;

orderOn: ID
{
  String attributeName = "";
  if (isEncoding)
     attributeName = $1.str;
   else
     attributeName = getInputAttributeName($1.str);
  Type t = getInputAttributeType($1.str);
  $$ = new ParserVal($1.str, t);
}
| TUPLENUM
{
  $$ = new ParserVal("TUPLENUM", null);
};

targetAttribute: ID
{
  $$ = new ParserVal($1.str, null);
}
;

aggregate: ID
{
  chosenAggregate = $1.str;
  $$ = new ParserVal($1.str, null);
}
;

aggregateInputAttribute: ID
{
  String attributeName = getInputAttributeName($1.str);
  if (attributeName == null)
    throw new ParsingException("Invalid Identifier");
  chosenAggregateAttribute = attributeName;
  $$ = new ParserVal(attributeName, getInputAttributeType($1.str));
}
;

groupByAttributes: identlist
{
  $$ = new ParserVal($1.str, $1.obj);
}
;

windowSize: INTEGER '~' VALUES {$$ = new ParserVal($1.str+"~VALUES",
$1.obj); }
| INTEGER '~' TUPLES {$$ = new ParserVal($1.str+"~TUPLES", $1.obj); }
;

advance: INTEGER {$$ = new ParserVal($1.str, $1.obj); }
| pred {$$ = new ParserVal($1.str, $1.obj); }
;

slack: INTEGER {$$ = new ParserVal($1.str, $1.obj); }
;

timeout: FLOAT { $$ = new ParserVal($1.str, $1.obj); }
| INTEGER {$$ = new ParserVal($1.str+".0", TypeManager.FLOAT_TYPE); }
;

predlist: pred
 {
   Integer i = new Integer(1);
   $$ = new ParserVal($1.str, i);
 }
 | pred '&' predlist
 {
   Integer i = new Integer(((Integer)($3.obj)).intValue()+1);
   $$ = new ParserVal($1.str+"& "+$3.str, i);
  }
 ;

pred: spred { $$ = new ParserVal($1.str, $1.obj); }
 | NOT '(' pred ')' { $$ = new ParserVal( "NOT(" + $3.str + ")", $3.obj ); }
 | AND '(' pred ',' pred ')' { $$ = new ParserVal("AND(" + $3.str + ", " + $5.str + ")", $3.obj); }
 | OR '(' pred ',' pred ')' { $$ = new ParserVal("OR(" + $3.str + ", " + $5.str + ")", $3.obj); }
 ;

spred: expr '=' expr
{
  if (isEncoding)
    castTypeAutomatically($1, $3);
  $$ = new ParserVal($1.str + " = " + $3.str, TypeManager.BOOL_TYPE);
}
 | expr '>' expr
{
  if (isEncoding)
    castTypeAutomatically($1, $3);
  $$ = new ParserVal($1.str + " > " + $3.str, TypeManager.BOOL_TYPE);
}
 | expr '<' expr
 {
  if (isEncoding)
    castTypeAutomatically($1, $3);
  $$ = new ParserVal($1.str + " < " + $3.str, TypeManager.BOOL_TYPE);
}
 | expr '>' '=' expr
 {
  if (isEncoding)
    castTypeAutomatically($1, $4);
  $$ = new ParserVal($1.str + " >= " + $4.str, TypeManager.BOOL_TYPE);
}
 | expr '<' '=' expr
 {
  if (isEncoding)
    castTypeAutomatically($1, $4);
  $$ = new ParserVal($1.str + " <= " + $4.str, TypeManager.BOOL_TYPE);
}
 | expr '!' '='expr
 {
  if (isEncoding)
    castTypeAutomatically($1, $4);
  $$ = new ParserVal($1.str + " != " + $4.str, TypeManager.BOOL_TYPE);
}
 | expr '<' '>' expr
{
  if (isEncoding)
    castTypeAutomatically($1, $4);
  $$ = new ParserVal($1.str + " != " + $4.str, TypeManager.BOOL_TYPE);
}
;

expr: ID
{
  String attributeName = getInputAttributeName($1.str);
  if (attributeName == null) throw new ParsingException("Invalid Identifier");
  $$ = new ParserVal(attributeName, getInputAttributeType($1.str));
}
 | const
 {
  $$ = new ParserVal($1.str, $1.obj);
}
 | '(' expr ')'
 {
  $$ = new ParserVal("(" + $2.str + ")", $2.obj);
}
 | '-' expr %prec NEG
 {
  if (!$2.obj.equals(TypeManager.INTEGER_TYPE) && !$2.obj.equals(TypeManager.FLOAT_TYPE) && !$2.obj.equals(TypeManager.DOUBLE_TYPE))
    throw new ParsingException("Invalid Expression");
   $$ = new ParserVal("-" + $2.str, $2.obj);
}
 | expr '+' expr
{
  if (!$1.obj.equals(TypeManager.INTEGER_TYPE) && !$1.obj.equals(TypeManager.FLOAT_TYPE) && !$1.obj.equals(TypeManager.DOUBLE_TYPE))
    throw new ParsingException("Invalid Expression");
  if (!$3.obj.equals(TypeManager.INTEGER_TYPE) && !$3.obj.equals(TypeManager.FLOAT_TYPE) && !$3.obj.equals(TypeManager.DOUBLE_TYPE))
    throw new ParsingException("Invalid Expression");
  castTypeAutomatically($1, $3);
  $$ = new ParserVal($1.str + " + " + $3.str, $1.obj);
}
 | expr '-' expr
{
  if (!$1.obj.equals(TypeManager.INTEGER_TYPE) && !$1.obj.equals(TypeManager.FLOAT_TYPE) && !$1.obj.equals(TypeManager.DOUBLE_TYPE))
    throw new ParsingException("Invalid Expression");
  if (!$3.obj.equals(TypeManager.INTEGER_TYPE) && !$3.obj.equals(TypeManager.FLOAT_TYPE) && !$3.obj.equals(TypeManager.DOUBLE_TYPE))
    throw new ParsingException("Invalid Expression");
  castTypeAutomatically($1, $3);
  $$ = new ParserVal($1.str + " - " + $3.str, $1.obj);
}
 | expr '*' expr
 {
  if (!$1.obj.equals(TypeManager.INTEGER_TYPE) && !$1.obj.equals(TypeManager.FLOAT_TYPE) && !$1.obj.equals(TypeManager.DOUBLE_TYPE))
    throw new ParsingException("Invalid Expression");
  if (!$3.obj.equals(TypeManager.INTEGER_TYPE) && !$3.obj.equals(TypeManager.FLOAT_TYPE) && !$3.obj.equals(TypeManager.DOUBLE_TYPE))
    throw new ParsingException("Invalid Expression");
  castTypeAutomatically($1, $3);
  $$ = new ParserVal($1.str + " * " + $3.str, $1.obj);
}
 | expr '/' expr
{
  if (!$1.obj.equals(TypeManager.INTEGER_TYPE) && !$1.obj.equals(TypeManager.FLOAT_TYPE) && !$1.obj.equals(TypeManager.DOUBLE_TYPE))
    throw new ParsingException("Invalid Expression");
  if (!$3.obj.equals(TypeManager.INTEGER_TYPE) && !$3.obj.equals(TypeManager.FLOAT_TYPE) && !$3.obj.equals(TypeManager.DOUBLE_TYPE))
    throw new ParsingException("Invalid Expression");
  castTypeAutomatically($1, $3);
  $$ = new ParserVal($1.str + " / " + $3.str, $1.obj);
}
 | expr '^' expr
 {
  if (!$1.obj.equals(TypeManager.INTEGER_TYPE) && !$1.obj.equals(TypeManager.FLOAT_TYPE) && !$1.obj.equals(TypeManager.DOUBLE_TYPE))
    throw new ParsingException("Invalid Expression");
  if (!$3.obj.equals(TypeManager.INTEGER_TYPE) && !$3.obj.equals(TypeManager.FLOAT_TYPE) && !$3.obj.equals(TypeManager.DOUBLE_TYPE))
    throw new ParsingException("Invalid Expression");
  castTypeAutomatically($1, $3);
  $$ = new ParserVal($1.str + " ^ " + $3.str, $1.obj);
}
| INT2FLOAT '(' expr ')'
{
  $$ = new ParserVal($3.str, $3.obj);
}
| INT2DOUBLE '(' expr ')'
{
  $$ = new ParserVal($3.str, $3.obj);
}
| FLOAT2INT '(' expr ')'
{
  $$ = new ParserVal( $3.str, $3.obj);
}
| FLOAT2DOUBLE '(' expr ')'
{
  $$ = new ParserVal( $3.str, $3.obj);
}
| ID '(' ID ')'
{
  if (!isAggregateFunction($1.str)) throw new ParsingException("Invalid Aggregate Function!");
  String attributeName = getInputAttributeName($3.str);
  if (attributeName == null) throw new ParsingException("Invalid Identifier");
  Type returnType = getAggregateType($1.str, getInputAttributeType($3.str));
  if (returnType == null) throw new ParsingException("Invalid Aggregate Function");
  if (!chosenAggregateAttribute.equals(attributeName)) throw new ParsingException("Invalid Aggregate Attribute In the Predicate!");
  if (!chosenAggregate.equals($1.str)) throw new ParsingException("Invalid Aggregate Function In the Predicate!");
  $$ = new ParserVal( $1.str+"("+attributeName+")", returnType);
}
;

const:  INTEGER { $$.str = $1.str; $$.obj = TypeManager.INTEGER_TYPE; }
 | FLOAT { $$.str = $1.str; $$.obj = TypeManager.FLOAT_TYPE;}
 | STRING { $$.str = $1.str; $$.obj = TypeManager.STRING_TYPE; }
 ;

%%

boolean isEncoding;
BoxNode box;
CompositeType[] inputTypes;
CompositeType[] outputTypes;
String chosenAggregateAttribute = null;
String chosenAggregate = null;

StreamTokenizer tokenizer;
ParserVal parsed;

void yyerror(String s) throws ParsingException
{
 throw new ParsingException(s);
}

int yylex() throws ParsingException
{
  Double d;
  try
  {
    int token = tokenizer.nextToken();
    switch(token)
    {
      case StreamTokenizer.TT_EOF:
        return 0;
      case StreamTokenizer.TT_WORD:
        try
        {
          Integer i = new Integer(tokenizer.sval);
          yylval = new ParserVal(tokenizer.sval, TypeManager.INTEGER_TYPE);
          return INTEGER;
        } catch(NumberFormatException e) {}

        try
        {
          Float f = new Float(tokenizer.sval);
          yylval = new ParserVal(tokenizer.sval, TypeManager.FLOAT_TYPE);
          return FLOAT;
        } catch(NumberFormatException e) {}

        if (tokenizer.sval.toUpperCase().equals("NOT"))
          return NOT;
        else if (tokenizer.sval.toUpperCase().equals("AND"))
          return AND;
        else if (tokenizer.sval.toUpperCase().equals("OR"))
          return OR;
        else if (tokenizer.sval.toUpperCase().equals("FILTER"))
          return FILTER;
        else if (tokenizer.sval.toUpperCase().equals("DROP"))
          return DROP;
        else if (tokenizer.sval.toUpperCase().equals("PASS"))
          return PASS;
        else if (tokenizer.sval.toUpperCase().equals("MAP"))
          return MAP;
        else if (tokenizer.sval.toUpperCase().equals("UNKNOWN"))
          return UNKNOWN;
        else if (tokenizer.sval.toUpperCase().equals("RESTREAM"))
          return RESTREAM;
        else if (tokenizer.sval.toUpperCase().equals("AGGREGATE"))
          return AGGREGATE;
        else if (tokenizer.sval.toUpperCase().equals("JOIN"))
          return JOIN;
        else if (tokenizer.sval.toUpperCase().equals("BSORT"))
          return BSORT;
        else if (tokenizer.sval.toUpperCase().equals("UNION"))
          return UNION;
        else if (tokenizer.sval.toUpperCase().equals("UPDATE_RELATION"))
          return UPDATE_RELATION;
        else if (tokenizer.sval.toUpperCase().equals("READ_RELATION"))
          return READ_RELATION;
        else if (tokenizer.sval.toUpperCase().equals("LAST"))
          return LAST;
        else if (tokenizer.sval.toUpperCase().equals("ALL"))
          return ALL;
        else if (tokenizer.sval.toUpperCase().equals("WHENEVER"))
          return WHENEVER;
        else if (tokenizer.sval.toUpperCase().equals("NONE"))
          return NONE;
        else if (tokenizer.sval.toUpperCase().equals("TIMEOUT"))
          return TIMEOUT;
        else if (tokenizer.sval.toUpperCase().equals("SATISFIES"))
          return SATISFIES;
        else if (tokenizer.sval.toUpperCase().equals("INT2FLOAT"))
          return INT2FLOAT;
        else if (tokenizer.sval.toUpperCase().equals("INT2DOUBLE"))
          return INT2DOUBLE;
        else if (tokenizer.sval.toUpperCase().equals("FLOAT2INT"))
          return FLOAT2INT;
        else if (tokenizer.sval.toUpperCase().equals("FLOAT2DOUBLE"))
          return FLOAT2DOUBLE;
        else if (tokenizer.sval.toUpperCase().equals("TUPLENUM"))
          return TUPLENUM;
        else if (tokenizer.sval.toUpperCase().equals("VALUES"))
          return VALUES;
        else if (tokenizer.sval.toUpperCase().equals("TUPLES"))
          return TUPLES;
        else
        {
          yylval = new ParserVal(tokenizer.sval, null);
          return ID;
        }
      default:
        if ((char)token == '\'')
        {
          String s = tokenizer.sval;
          String r = "";
          for (int i = 0; i < s.length(); i++)
          {
            char c = s.charAt(i);
            if (c == '\'' )
              r += "\\" + "'";
            else
              r += c;
          }
          yylval = new ParserVal("'"+r+"'", TypeManager.STRING_TYPE);
          return STRING;
        }

        return (char)token;
    }
  }
  catch (Exception e)
  {}
  return 0;
}

protected String getInputAttributeName(String word)
{
  return getAttributeName(word, inputTypes);
}

protected String getOutputAttributeName(String word)
{
  return getAttributeName(word, outputTypes);
}

protected Type getInputAttributeType(String word)
{
  return getAttributeType(word, inputTypes);
}

protected Type getOutputAttributeType(String word)
{
  return getAttributeType(word, outputTypes);
}

protected String getAttributeName(String word, CompositeType[] types)
{
  int typeIndex = -1;
  try
  {
    if (isEncoding)
    {
      String s = ":";
      CompositeType c = new CompositeType("", true);
      if (types.length == 1)
      {
        c = types[0];
        s += 0;
      }
      else if (types.length == 2)
      {
        if (word.toUpperCase().indexOf("LEFT") >= 0 )
        {
          c = types[0];
          s += 0;
        }
        else
        {
          c = types[1];
          s += 1;
        }
        int offset = word.indexOf('.');
        if (offset == -1)
          return null;
        word = word.substring(offset+1);
      }
      s += ':';
      int i = c.getAttributeIndex(word);
      if (i < 0)
        return null;
      s += c.getAttributeType(i).toString().toLowerCase().charAt(0);
      s += ':';
      s += c.getAttributeOffset(i);
      s += ':';
      s += c.getAttributeLength(i);
      s += ':';
      return s;
    }
    else // if decoding
    {
      int offset = word.indexOf(':');
      String s = word.substring(offset+1, word.indexOf(':', offset+1));
      typeIndex = new Integer(s).intValue();
      CompositeType c = types[typeIndex];
      offset = word.indexOf(':', offset+1);
      offset = word.indexOf(':', offset+1);
      s = word.substring(offset+1, word.indexOf(':', offset+1));
      offset = new Integer(s).intValue();
      int i = c.getAttributeIndex(offset);
      if (types.length == 2)
      {
        if (typeIndex == 0)
          s = "LEFT."+c.getAttributeName(i);
        else
          s = "RIGHT."+c.getAttributeName(i);
      }
      else
        s = c.getAttributeName(i);
      return s;
    }
  }
  catch (Exception e)
  {
//    e.printStackTrace();
  }
  return null;
}

protected Type getAttributeType(String word, CompositeType[] types)
{
  try
  {
    if (isEncoding)
    {
      String s = ":";
      CompositeType c = new CompositeType("", true);
      if (types.length == 1)
      {
        c = types[0];
        s += 0;
      }
      else if (types.length == 2)
      {
        if (word.toUpperCase().indexOf("LEFT") >= 0 )
        {
          c = types[0];
          s += 0;
        }
        else
        {
          c = types[1];
          s += 1;
        }
        int offset = word.indexOf('.');
        if (offset == -1)
          return null;
        word = word.substring(offset+1);
      }
      s += ':';
      int i = c.getAttributeIndex(word);
      if (i < 0)
        return null;
      return c.getAttributeType(i);
    }
    else // if decoding
    {
      int offset = word.indexOf(':');
      String s = word.substring(offset+1, word.indexOf(':', offset+1));
      int typeIndex = new Integer(s).intValue();
      CompositeType c = types[typeIndex];
      offset = word.indexOf(':', offset+1);
      offset = word.indexOf(':', offset+1);
      s = word.substring(offset+1, word.indexOf(':', offset+1));
      offset = new Integer(s).intValue();
      int i = c.getAttributeIndex(offset);
      return c.getAttributeType(i);
    }
  }
  catch (Exception e)  { }
  return null;
}


public static final AggregateFunction AVG_AGGREGATE = new AggregateFunction("AVG",
  new Type[] {TypeManager.INTEGER_TYPE, TypeManager.FLOAT_TYPE, TypeManager.DOUBLE_TYPE},
  new Type[] {TypeManager.FLOAT_TYPE, TypeManager.FLOAT_TYPE, TypeManager.DOUBLE_TYPE});

public static final AggregateFunction COUNT_AGGREGATE = new AggregateFunction("COUNT",
  new Type[] {TypeManager.STRING_TYPE, TypeManager.INTEGER_TYPE, TypeManager.FLOAT_TYPE, TypeManager.DOUBLE_TYPE},
  new Type[] {TypeManager.INTEGER_TYPE, TypeManager.INTEGER_TYPE, TypeManager.INTEGER_TYPE, TypeManager.INTEGER_TYPE});

public static final AggregateFunction MIN_AGGREGATE = new AggregateFunction("MIN",
  new Type[] {TypeManager.INTEGER_TYPE, TypeManager.FLOAT_TYPE, TypeManager.DOUBLE_TYPE},
  new Type[] {TypeManager.INTEGER_TYPE, TypeManager.FLOAT_TYPE, TypeManager.DOUBLE_TYPE});

public static final AggregateFunction MAX_AGGREGATE = new AggregateFunction("MAX",
  new Type[] {TypeManager.INTEGER_TYPE, TypeManager.FLOAT_TYPE, TypeManager.DOUBLE_TYPE},
  new Type[] {TypeManager.INTEGER_TYPE, TypeManager.FLOAT_TYPE, TypeManager.DOUBLE_TYPE});

public static final AggregateFunction SUM_AGGREGATE = new AggregateFunction("SUM",
  new Type[] {TypeManager.INTEGER_TYPE, TypeManager.FLOAT_TYPE, TypeManager.DOUBLE_TYPE},
  new Type[] {TypeManager.INTEGER_TYPE, TypeManager.FLOAT_TYPE, TypeManager.DOUBLE_TYPE});

public static final AggregateFunction FIRSTVAL_AGGREGATE = new AggregateFunction("FIRSTVAL",
  new Type[] {TypeManager.INTEGER_TYPE, TypeManager.FLOAT_TYPE},
  new Type[] {TypeManager.INTEGER_TYPE, TypeManager.FLOAT_TYPE});

public static final AggregateFunction LASTVAL_AGGREGATE = new AggregateFunction("LASTVAL",
  new Type[] {TypeManager.INTEGER_TYPE, TypeManager.FLOAT_TYPE},
  new Type[] {TypeManager.INTEGER_TYPE, TypeManager.FLOAT_TYPE});

public static final AggregateFunction DELTA_AGGREGATE = new AggregateFunction("DELTA",
  new Type[] {TypeManager.INTEGER_TYPE, TypeManager.FLOAT_TYPE},
  new Type[] {TypeManager.INTEGER_TYPE, TypeManager.FLOAT_TYPE});

public static final AggregateFunction LASTTWO_AGGREGATE = new AggregateFunction("LASTTWO",
  new Type[] {TypeManager.FLOAT_TYPE},
  new Type[] {TypeManager.FLOAT_TYPE});

/////////////////////////////////////

public static final AdhocAggregateFunction SEG1AF = new AdhocAggregateFunction("SEG1AF",
  new Type[] {TypeManager.INTEGER_TYPE, TypeManager.INTEGER_TYPE});

public static final AdhocAggregateFunction MITRE1AF = new AdhocAggregateFunction("MITRE1AF",
  new Type[] {TypeManager.FLOAT_TYPE, TypeManager.FLOAT_TYPE, TypeManager.FLOAT_TYPE, TypeManager.FLOAT_TYPE});

public static final AdhocAggregateFunction MITRE2AF = new AdhocAggregateFunction("MITRE2AF",
  new Type[] {TypeManager.INTEGER_TYPE, TypeManager.TIMESTAMP_TYPE, TypeManager.FLOAT_TYPE, TypeManager.FLOAT_TYPE, TypeManager.INTEGER_TYPE});

public static final AdhocAggregateFunction MITRECOMAF = new AdhocAggregateFunction("CENTER_OF_MASS_AF",
  new Type[] {TypeManager.FLOAT_TYPE, TypeManager.FLOAT_TYPE});

public static final AdhocAggregateFunction SEG2AF = new AdhocAggregateFunction("SEG2AF",
  new Type[] {TypeManager.INTEGER_TYPE, TypeManager.INTEGER_TYPE});

public static final AdhocAggregateFunction SEG3AF = new AdhocAggregateFunction("SEG3AF",
  new Type[] {TypeManager.INTEGER_TYPE, TypeManager.INTEGER_TYPE, TypeManager.INTEGER_TYPE});

public static final AdhocAggregateFunction ACC1AF = new AdhocAggregateFunction("ACC1AF",
  new Type[] {TypeManager.INTEGER_TYPE,
	      TypeManager.INTEGER_TYPE,
	      TypeManager.INTEGER_TYPE,
	      TypeManager.INTEGER_TYPE,
	      TypeManager.INTEGER_TYPE});

public static final AdhocAggregateFunction ACC2AF = new AdhocAggregateFunction("ACC2AF",
  new Type[] {TypeManager.INTEGER_TYPE, TypeManager.INTEGER_TYPE});

public static final AggregateFunction FIDELITY1 = new AdhocAggregateFunction("FIDELITY1",
									     new Type[] {TypeManager.INTEGER_TYPE, TypeManager.INTEGER_TYPE,TypeManager.INTEGER_TYPE});

public static final AggregateFunction FIDELITYALARM = new AdhocAggregateFunction("FIDELITYALARM",
  new Type[] {TypeManager.INTEGER_TYPE});

public static final AggregateFunction[] aggregateFunctions = {AVG_AGGREGATE, COUNT_AGGREGATE, MIN_AGGREGATE, MAX_AGGREGATE, SUM_AGGREGATE, FIRSTVAL_AGGREGATE, LASTVAL_AGGREGATE, LASTTWO_AGGREGATE, DELTA_AGGREGATE,
  FIDELITY1,FIDELITYALARM, MITRECOMAF, MITRE1AF, MITRE2AF, SEG1AF, SEG2AF, SEG3AF, ACC1AF, ACC2AF};

protected static boolean isAggregateFunction(String name)
{
  for (int i = 0; i < aggregateFunctions.length; i++)
    if (aggregateFunctions[i].getName().equals(name))
      return true;
   return false;
}

protected static Type[] getAggregateTypes(String name, Type inputAttributeType)
{
  for (int i = 0; i < aggregateFunctions.length; i++)
    if (aggregateFunctions[i].getName().equals(name))
    {
      if (aggregateFunctions[i] instanceof AdhocAggregateFunction)
        return ((AdhocAggregateFunction)(aggregateFunctions[i])).getOutputTypes();
      else
        return new Type[] {aggregateFunctions[i].getOutputType(inputAttributeType)};
    }
   return null;
}

protected static Type getAggregateType(String name, Type inputAttributeType)
{
  for (int i = 0; i < aggregateFunctions.length; i++)
    if (aggregateFunctions[i].getName().equals(name))
    {
      return aggregateFunctions[i].getOutputType(inputAttributeType);
    }
   return null;
}

public void parse(boolean isEncoding, BoxNode box, String modifier) throws ParsingException
{
  chosenAggregateAttribute = null;
  chosenAggregate = null;

  this.isEncoding = isEncoding;
  this.box = box;
  if (modifier == null) throw new ParsingException("Null Modifier Exception");
  if (box instanceof UnaryBoxNode)
  {
    this.inputTypes = new CompositeType[] {(((UnaryBoxNode)box).getInputPort()).getType()};
    this.outputTypes = new CompositeType[] {(((UnaryBoxNode)box).getOutputPort()).getType()};
    if (box instanceof FilterBoxNode)
      modifier = "FILTER("+modifier+")";
    if (box instanceof DropBoxNode)
      modifier = "DROP("+modifier+")";
    if (box instanceof MapBoxNode)
      modifier = "MAP("+modifier+")";
    if (box instanceof UnknownBoxNode) {
      parsed = new ParserVal(modifier, null);
      parsed.str = modifier;
      return;
    }
    if (box instanceof RestreamBoxNode)
      modifier = "RESTREAM("+modifier+")";
    if (box instanceof AggregateBoxNode)
      modifier = "AGGREGATE("+modifier+")";
    if (box instanceof BsortBoxNode)
      modifier = "BSORT("+modifier+")";
    if (box instanceof UnionBoxNode)
      modifier = "UNION("+modifier+")";
    if (box instanceof UpdateRelationBoxNode)
      modifier = "UPDATE_RELATION("+modifier+")";
    if (box instanceof ReadRelationBoxNode)
      modifier = "READ_RELATION("+modifier+")";
  }
  else if (box instanceof BinaryBoxNode)
  {
    BinaryBoxNode binaryBox = ((BinaryBoxNode)box);
    inputTypes = new CompositeType[2];
    inputTypes[0] = binaryBox.getLeftInputPort().getType();
    inputTypes[1] = binaryBox.getRightInputPort().getType();
    outputTypes = new CompositeType[] {(((BinaryBoxNode)box).getOutputPort()).getType()};
    if (box instanceof JoinBoxNode)
      modifier = "join("+modifier+")";
    else if (box instanceof ResampleBoxNode)
      modifier = "resample("+modifier+")";
  }
  try
  {
    tokenizer = new StreamTokenizer(new StringReader(modifier));
    tokenizer.ordinaryChar('0');
    tokenizer.ordinaryChar('1');
    tokenizer.ordinaryChar('2');
    tokenizer.ordinaryChar('3');
    tokenizer.ordinaryChar('4');
    tokenizer.ordinaryChar('5');
    tokenizer.ordinaryChar('6');
    tokenizer.ordinaryChar('7');
    tokenizer.ordinaryChar('8');
    tokenizer.ordinaryChar('9');
    tokenizer.ordinaryChar('.');
    tokenizer.ordinaryChar('-');
    tokenizer.ordinaryChar(':');
    tokenizer.ordinaryChar('/');
    tokenizer.ordinaryChar('_');
    tokenizer.wordChars('0', '9');
    tokenizer.wordChars('.', '.');
    tokenizer.wordChars(':', ':');
    tokenizer.quoteChar('\'');
    tokenizer.wordChars('_', '_');
  }
  catch (Exception e)
  {
    throw new ParsingException(e.getMessage());
  }
  yyparse();
  parsed.str = parsed.str.substring(parsed.str.indexOf('(')+1, parsed.str.lastIndexOf(')'));
}

public void parse(boolean isEncoding, BoxNode box) throws ParsingException
{
  parse(isEncoding, box, box.getModifier());
}

public CompositeType getInferredType()
{
  if (box instanceof FilterBoxNode)
    return ((FilterBoxNode)box).getInputPort().getType();
  if (box instanceof DropBoxNode)
    return ((DropBoxNode)box).getInputPort().getType();
  if (box instanceof RestreamBoxNode)
    return ((RestreamBoxNode)box).getInputPort().getType();
  if (box instanceof UnionBoxNode)
    return ((UnionBoxNode)box).getInputPort().getType();
  if (box instanceof BsortBoxNode)
    return ((BsortBoxNode)box).getInputPort().getType();
  if (box instanceof UpdateRelationBoxNode)
    return ((UpdateRelationBoxNode)box).getInputPort().getType();
  if (box instanceof ReadRelationBoxNode)
    return ((ReadRelationBoxNode)box).getInputPort().getType();
  if (box instanceof UnknownBoxNode)
    return ((UnknownBoxNode)box).getOutputPort().getType();
  return (CompositeType)(parsed.obj); // Aggregate, Map, Join
}

public Object getParsedObject()
{
  return parsed.obj;
}

public String getTransformedString()
{
  return parsed.str;
}

protected void castTypeAutomatically(ParserVal v1, ParserVal v2) throws ParsingException
{
  if (!isEncoding) return;
  if (v1.obj.equals(TypeManager.FLOAT_TYPE))
  {
    if (v2.obj.equals(TypeManager.INTEGER_TYPE))
    {
      v2.str = "INT2FLOAT("+v2.str+")";
      v2.obj = TypeManager.FLOAT_TYPE;
    }
  }
  if (v1.obj.equals(TypeManager.DOUBLE_TYPE))
  {
    if (v2.obj.equals(TypeManager.INTEGER_TYPE))
    {
      v2.str = "INT2DOUBLE("+v2.str+")";
      v2.obj = TypeManager.DOUBLE_TYPE;
    }
    if (v2.obj.equals(TypeManager.FLOAT_TYPE))
    {
      v2.str = "FLOAT2DOUBLE("+v2.str+")";
      v2.obj = TypeManager.DOUBLE_TYPE;
    }
  }
  if (v2.obj.equals(TypeManager.FLOAT_TYPE))
  {
    if (v1.obj.equals(TypeManager.INTEGER_TYPE))
    {
      v1.str = "INT2FLOAT("+v1.str+")";
      v1.obj = TypeManager.FLOAT_TYPE;
    }
  }
  if (v2.obj.equals(TypeManager.DOUBLE_TYPE))
  {
    if (v1.obj.equals(TypeManager.INTEGER_TYPE))
    {
      v1.str = "INT2DOUBLE("+v1.str+")";
      v1.obj = TypeManager.DOUBLE_TYPE;
    }
    if (v1.obj.equals(TypeManager.FLOAT_TYPE))
    {
      v1.str = "FLOAT2DOUBLE("+v1.str+")";
      v1.obj = TypeManager.DOUBLE_TYPE;
    }
  }
  if (!v1.obj.equals(v2.obj)) throw new ParsingException("Incompatible Types: " +v1.obj +", "+v2.obj);
}

