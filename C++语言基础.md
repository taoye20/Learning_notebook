# 语言特性
## c++的运程序内存模型

内存分别分为：代码区，全局区，栈区，堆区。

* 代码区：存放可执行二进制码，具有共享特性和只读特性，共享方便了程序被调度执行。
* 全局区：存放全局变量，静态变量`static`，字符串常量如`cout << "hello" << endl;`，全局常量（注意只有全局的常量才会在这，局部常量在栈区）
* 栈区：内存在程序结束就释放地址，因此下面程序会出现问题
  ```c++
  int* func() {
    int a = 10;
    return &a;  
  }

  int main() {
    int* res = func();  //这之后就释放了内存，因此res指向的地址无法将正确数值传回
    cout << *res << endl;
    return 0; 
  }
  ```
* 堆区：注意堆区返回为指针，由程序员自己创建和释放。
  ```c++
  int* func() {
    int* a = new int(10);
    return a;  //返回了堆区的地址，但是堆区不会被自动优化掉，因此main中访问该地址中的值是正确的
  }
  ```
  关于堆区的释放：
  ```c++
  int main() {
    int* p = func();
    cout << *p << endl;
    delete p;
    return 0;
  }
  ```
  创建堆区数组：
  ```c++
  int* func() {
    int *a = new int[10];
    return a;
  }
  ```
  关于堆区数组的释放：
  ```c++
  int main() {
    int* p = func();
    cout << *p << endl;
    delete[] p;
    return 0;
  }
  ```

## 封装
### 基本
将属性和行为封装，并且设置访问权限，是封装的意义。一个class具有构造和析构函数，方便对象的创建和删除。其中浅拷贝深拷贝知识比较难理解。

### 静态成员
分为静态变量和静态函数。静态变量被保存在全局区，在**代码执行四步：预编译、编译、汇编、链接中处于编译阶段就分配内存。**所以在class中定义静态变量有**所有对象都共享一份数据**的属性。静态函数只能访问静态变量。

### this指针
**成员函数和成员变量是分开存储的。只有非静态成员变量才和类对象一起存储。**可以使用`Solution s; sizeof(s);`来验证。在对象或者还没创建对象的类中如果没有任何非静态变量时，对象只占有一个字节，占个坑而已。

那么多个同类型的对象会使用同一块代码，那么在调用这块代码时，怎么知道是哪个对象调用了代码呢？使用this指针，它指向了调用该成员函数的对象。在创建对象时，this就自动存在了。若传入形参与成员函数内参数同名，可以使用this区分。

  ```c++
  int val;
  void func(int val) {
    this->val = val;
  }
  ```

### 常函数常对象
指针（是）常量：指针指向的地址是不能变的。`Person * const this`

常量（的）指针：指针所指地址存的数值是不能变的。`const Person * this`

那么常函数的表达方式为：`void func() const {}` 表示函数内的值是不能修改的。

常对象：`const Solution s;`常对象只能调用常函数。


## 继承
### 友元
友元可以访问到该类中的private
```c++
class room{
  //全局函数作为友元
  friend void func();

  //类作为友元
  friend class gay();

  //类成员作为友元
  friend void gay::looklook();
};
```

### 继承
继承语法：
```c++
class A : pubilc class B{  //公有方式继承
  ...
}
```
关于继承的方式：
![Alt text](image-13.png)

继承中调用构造函数的顺序为：先调用父类，再调用子类。

子类父类中具有同名变量或同名函数的情况：
```c++
//父类
class Base{
public:
  int m_a = 10;

  void func(){
    cout << "base fun" << endl;
  }

  void func(int a){
    cout << "base int fun" << endl;
  }
};

//子类
class Son : pubilc class Base{  
public:
  int m_a = 20;

  void func(){
    cout << "son fun" << endl;
  }
};

void test(){
  Son s;
  int a = s.m_a;  //10
  int a = s.Base::m_a;  //20，这里的意思是加入作用域

  s.func();  //会隐藏所有的父类同名函数，输出为子函数
  s.Base::func();  //这样调用父类的函数
  s.Base::func(a);
  int a = Son::m_a;  //通过类名调用子类参数
  int a = Son::Base::m_a;  //想要调用父类同名参数
}
```

```c++
class A : pubilc class B, pubilc class C{  //多继承
  
}

void test(){
  //调用继承的同名变量和函数时应当加作用域
  A a;
  a.B::val = 10;
  a.C::val = 20;
}
```

## 多态
### 基础
* 静态多态：函数重载，运算符重载，复用函数名叫静态多态。
* 动态多态：派生类和虚函数实现运行时多态。

静态多态地址在编译阶段确定，动态多态地址在运行阶段确定。

```c++
class Animal {  
public:
  void speak(){   //它被早绑定了
    cout << "动物在说话" << endl;
  }

  //如果想要执行猫函数的地址，那么就要在运行时再绑定函数
  virtual void speak(){   //注释上面那个再运行，就成猫说话了
    cout << "动物在说话" << endl;
  }
};

class Cat : public Animal {  
public:
  void speak(){
    cout << "猫在说话喵喵喵" << endl;
  }
};

void doSpeak(Animal& animal){
  animal.speak();  //因为函数地址早绑定，因此传入不管是猫狗，都调用animal
}

void test(){
  Cat cat;    //创建和传入的是子类对象
  doSpeak(cat);  //执行输出是"动物在说话"
}
```

动态多态的场景是：有继承，且子类重写父类函数（重写中函数返回类型，函数名，参数列表全部相同）。父类指针或引用，执行子类对象成员。

若加入了virtual后，sizeof(Animal)变成了4字节（32位系统），说明此时类中保存了一个指针。

![Alt text](image-22.png)

该图是虚函数的原理，是由于虚函数表中的内容在调用doSpeak(cat)时发生了重写，那么指针指向的表的内容就替换成了猫的，最后调用的也是猫的。

```c++
#include<iostream>
#include<vector>
using namespace std;

//class Calculator {
//public:
//	int m_num1;
//	int m_num2;
//	int getResult(string oper) {
//		if (oper == "+") return m_num1 + m_num2;
//		if (oper == "-") return m_num1 - m_num2;
//		if (oper == "*") return m_num1 * m_num2;
//		
//		//可此时想要扩展程序...
//};
//
//void test01() {
//	Calculator c;
//	c.m_num1 = 10;
//	c.m_num2 = 20;
//	string oper = "+";
//	cout << c.m_num1 << oper << c.m_num2 << "=" << c.getResult(oper) << endl;
//}

class AbstractCalculator {
public:
	//AbstractCalculator(int a, int b) : m_Num1(a), m_Num2(b) {};
	virtual int getResult() {
		return 0;
	}
	int m_Num1;
	int m_Num2;
};

class AddCalculator : public AbstractCalculator {
public:
	int getResult() {
		return m_Num1 + m_Num2;
	}
};

class SubCalculator : public AbstractCalculator {
public:
	int getResult() {
		return m_Num1 - m_Num2;
	}
};

class MutiCalculator : public AbstractCalculator {
public:
	int getResult() {
		return m_Num1 * m_Num2;
	}
};


void test01() {
	AbstractCalculator* val = new AddCalculator; //一定记得创建一个父类的指针，指向子类对象
	val->m_Num1 = 10;
	val->m_Num2 = 20;
	cout << val->m_Num1 << "+" << val->m_Num2 << "=" << val->getResult() << endl;
	delete val;   //记得移除堆区

	val = new SubCalculator; //一定记得创建一个父类的指针，指向子类对象，定义一次后不用再定义一次类型，即使delete了也只是移除了堆区没有修改栈区
	val->m_Num1 = 10;
	val->m_Num2 = 20;
	cout << val->m_Num1 << "-" << val->m_Num2 << "=" << val->getResult() << endl;
	delete val;
}

int main() {
	test01();
	return 0;
}
```

上面的父类中的方法等于没有作用，全部依靠子类重写，叫纯虚函数。当父类中有虚函数时，父类也叫抽象类，抽象类是无法实例化对象的。纯虚函数语法：`virtual void func() = 0;`

```c++
#include<iostream>
#include<vector>
using namespace std;

class AbstractDrinking {
public:
	virtual void Boil() = 0;
	virtual void Berw() = 0;
	virtual void Pourincup() = 0;
	virtual void Putotherthing() = 0;

	void makeDrink() {
		Boil();
		Berw();
		Pourincup();
		Putotherthing();
	}
};

class Coffee : public AbstractDrinking {
public:
	void Boil() {
		cout << "开水" << endl;
	}

	void Berw() {
		cout << "倒咖啡" << endl;
	}

	void Pourincup() {
		cout << "装杯" << endl;
	}

	void Putotherthing() {
		cout << "加奶" << endl;
	}
};

void doWork(AbstractDrinking* abs) {
	abs->makeDrink();
	delete abs;
}

int main() {
	doWork(new Coffee);
	return 0;
}
```

析构函数的知识点：
```c++
#include<iostream>
#include<vector>
using namespace std;

class Animal {
public:
    Animal() {
        cout << "动物构造函数" << endl;
    }

    virtual ~Animal() {   //父类析构函数使用virtual，可以实现子类的析构函数调用，你看看不加试试
        cout << "动物析构函数" << endl;
    }

    virtual void speak() {   
        cout << "动物在说话" << endl;
    }
};

class Cat : public Animal {
public:
    Cat(string name) {
        cout << "猫构造函数" << endl;
        m_Name = new string(name);  //在子类中开辟堆区
    }

    ~Cat() {
        cout << "猫析构函数" << endl;
        if (m_Name != NULL) {    //使用子类析构函数释放内存
            delete m_Name;
        }
    }

    void speak() {
        cout<< *m_Name << "猫在说话喵喵喵" << endl;
    }

    string* m_Name;
};

int main() {
    Animal* cat = new Cat("Tom");
    cat->speak();
    delete cat;     //这里delete的是父类指针，不会调用子类的析构函数，子类有创建堆区，即便在子类中写了析构函数delete堆区也没有用，导致内存泄漏，怎么办看上面
    return 0;
}
```

可以看到我多态也好，抽象类也好，其目的就是：搭建一个父类的样板，然后不同属性的子类分别继承并重写这些特定的成员，以达到实现不同功能的一类物品。最后采用封装使用的思想，通过其成员函数提供的功能一个个的使用这些物品。