# Skip C-style commenting
 /\/\*/ , /\*\//b
 /\/\/\(.*\)/b
 
 s/module/namespace/
 s/local//
 s/idempotent//
 s/dictionary/map/
 s/sequence/vector/
 
 #parse local interface
 /interface/ , /{/    {
     /interface/ {
         #change interface to class
         s/interface\(.*\)/class \1/
     }
 #change extends
     s/extends/: public/
 #add public before every keyword, but not {
     /{/!s/,\(.*\)/, public \1/g
     s/{/{public:/
 }
 
 s/nonmutating\(.*)\)/\1 const/
 
 /throws/,/;/ {
     s/throws/throw(/
     s/;/);/
 }
