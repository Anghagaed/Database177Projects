 # Updating what a SQL is
 SQL ::= SELECT SelectAtts FROM Tables WHERE AndList |  
 SELECT SelectAtts FROM Tables WHERE AndList GROUP BY Atts |
 CREATE TABLE TableName ( AttsExpression ) |
 LOAD DATA TableName FROM TextFile |
 CREATE INDEX IndexName FROM TableName ON AttributeName

 AttsExpression ::= AttributeName TypeName | AttsExpression ',' AttributeName TypeName
 AttributeName ::= YY_NAME
 TypeName ::= YY_NAME
 TableName ::= YY_NAME
 TextFile ::= YY_NAME
 IndexName ::= YY_NAME


