
Task 1 : generate the schema
```
./flatbuffers/flatc -b --cpp --schema --gen-mutable --reflect-names --reflect-names  property.fbs
```

Task 2 : 
run the Task2() function in main.cpp
property object is updated.
output:
```
 property tree name: name
property tree value: crypto
 property tree name: name
property tree value: Crypto
```

Task3 :
run the Task3Server() function in main.cpp
then run Task4() function in sender.cpp
sender.cpp send a nested property tree object and Task3Server() function receive and print out all the 
element in property object
output:
```
Name: TestName, Type: String, Value: TestValue
Name: TestName_children, Type: String, Value: TestValue_children


```

Task4 :
run the Task4() function in main.cpp
then run Task4() function in sender.cpp
sender.cpp send a nested property tree object and Task4 function receive and print out all the
element in property object using reflection API
output:
```
name is TestNametype isvalue is TestValue
child    
name is TestName_childrentype isvalue is TestValue_children



```

Task 5:
use boost asio. 
run the Task5() function in main.cpp
then run Task5() function in sender.cpp
sender.cpp will send a nested property tree every 0.5 seconds, and main.cpp will print Hello world 
every 0.5 seconds, but it also recieves any incoming property object from sender.cpp.once it receive the 
perperty object, it will print out all the elements of property tree. 

output for main.cpp
```
name is TestNametype isvalue is TestValue
child    
name is TestName_childrentype isvalue is TestValue_children
name is TestNametype isvalue is TestValue
child    
name is TestName_childrentype isvalue is TestValue_children
name is TestNametype isvalue is TestValue
child    
name is TestName_childrentype isvalue is TestValue_children
name is TestNametype isvalue is TestValue
child    
name is TestName_childrentype isvalue is TestValue_children
name is TestNametype isvalue is TestValue
child    
name is TestName_childrentype isvalue is TestValue_children
name is TestNametype isvalue is TestValue
child    
name is TestName_childrentype isvalue is TestValue_children
name is TestNametype isvalue is TestValue
child    
name is TestName_childrentype isvalue is TestValue_children
name is TestNametype isvalue is TestValue
child    
name is TestName_childrentype isvalue is TestValue_children
name is TestNametype isvalue is TestValue
child    
name is TestName_childrentype isvalue is TestValue_children
name is TestNametype isvalue is TestValue
child    
name is TestName_childrentype isvalue is TestValue_children
Hello world
name is TestNametype isvalue is TestValue
child    
name is TestName_childrentype isvalue is TestValue_children
Hello world

```

output for sender.cpp
```angular2html
Sent Property successfully
Sent Property successfully
Sent Property successfully
Sent Property successfully
Sent Property successfully
Sent Property successfully
Sent Property successfully
Sent Property successfully
Sent Property successfully
Sent Property successfully
Sent Property successfully
Sent Property successfully
Sent Property successfully
Sent Property successfully

```



