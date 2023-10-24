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

## 实战问题散记
### const用法
代码double real () const {return val;}中，const位于函数签名的末尾，表示该方法是一个常量成员函数，这意味着这个成员函数不能修改它所属的对象。因此，这个函数可以被一个const对象调用，而且在函数体内不能修改任何非静态成员变量。这对于保证数据的安全性和一致性非常有用。

### Singleton设计模式
将构造函数放入private中：
```c++
class A {
public:
  static A& getInstance();
  setup(){...}

private:
  A();
  A(const A& rhs);
  ...

  A& A::getInstance() {
    static A a;
    return a;
  }
};
```

首先getInstance函数的返回类型是通过reference来接收的，通常提倡使用引用传递参数或接收返回参数。

### 运算符重载的本质
```c++
//一般理解的写法
class Complex{
public:
  Complex Addfunc(const Complex& p){
    Complex res;
    res.real = this->real + p.real;
    res.imaginary = this->imaginary + p.imaginary;
    return res;
  }

private:
  int real;
  int imaginary;
};

void test(){
  ...
  Complex p3 = p1.Addfunc(p2);
}


//调用库给出的operator的写法
class complex
{
public:
  complex (double r = 0, double i = 0): re (r), im (i) { }
  complex& operator += (const complex&);
  private:
  double re, im;

  friend complex& __doapl (complex *, const complex&);  //只有friend后，才能直接访问complex中的参数
};

inline complex   //这里的返回参数不是引用而是对象，是因为返回对象在函数内创建，调用完函数后函数结束回收变量，那么如果调用引用指向函数内分配的那个地址就是个bad值
operator + (const complex& x, const complex& y)
{
  return complex (real (x) + real (y), imag (x) + imag (y));
}

void test(){
  complex p3 = p1 + p2; 
}


//更明显的例子
inline complex&  //返回引用是因为对象在函数外被创建的，因此使用引用传递快，且可以直接改变对象值
__doapl (complex* ths, const complex& r)  //注意这里加入const的原因应当提前考虑，不希望改变被加数，那么就养成良好的习惯
{
  ths->re += r.re;
  ths->im += r.im;
  return *ths;
}
 
inline complex&
complex::operator += (const complex& r)
{
  return __doapl (this, r);
  //this->re += r.re;
  //this->im += r.im;
  //return *this;
}

void test(){
  complex p2 += p1; //p2调用+=成员，p2在operator += 中是this
}
```

### 链式编程
同样举例为上面的例子：
```c++
inline complex&  
__doapl (complex* ths, const complex& r)  被加数，那么就养成良好的习惯
{
  ths->re += r.re;  //因为传入指针，所以操作是在原地址上操作的。
  ths->im += r.im;
  return *ths;
}
 
inline complex&  //这里返回类型为void可以不？可以！因为this中的参数在上面的函数中已经被改变了，那为什么要返回complex&类型呢？
complex::operator += (const complex& r)
{
  return __doapl (this, r);
}

void test(){
  complex p2 += p1; 
  //考虑下面一行代码
  complex p3 += p2 += p1;  //如果p2+=p1返回的是void，那么p3就无法继续进行了，返回complex&，那么p2+=p1又可以看成一个complex类型的值，继续参与p3的计算
}
```

### new malloc和实际物理分配地址
new时会在堆区分配所需大小的内存空间，在这片空间的前后还要分别加入四个字节的名为cookie的空间，用于标志这片内存空间的大小，以便在free的时候可以分辨出删除的大小。

同时在分配地址不足16字节倍数大小时，会添加pad以达到16字节的倍数，因为地址指针指定的地址的物理间隔是那么多。

![Alt text](image-24.png)

### 静态变量和静态函数的讨论
在一般非静态成员函数中系统会自动分配this指针，每次一个新的对象调用该函数时，创建的this指针指向都是不一样的，都是创建了新的地址和空间（浅拷贝除外）。

但static变量是存在全局区的，所有调用都指向同一个地址，一个对象对它做的改变，在其他对象中也会受到影响。

静态函数没有this指针，调用它的对象是指不到自己的，所以静态函数只能用于处理静态变量。

回到singleton的例子：

```c++
class A {
public:
  static A& getInstance();  //该函数由于static，只能用于处理全局区的static
  setup(){...}

private:
  A();   //构造函数在private中，无法被外界用于创建对象，只能自己创建
  A(const A& rhs);
  ...

  A& A::getInstance() {   //通过类名调用成员函数
    static A a;    //创建全局区唯一一个对象，使用static，由类自己创建
    return a;
  }
};

void test(){
  A::getInstance().setup();  //在使用时，通过类名创建那个唯一的对象，并使用其中的功能
}
```
### 类模板，函数模板和成员模板
类模板：对于一个类，其里面的参数类型可以后面指定，调用时就是类名加<...>

```c++
template<typename T>  //T在下面的类中就是一个类型名的标志，不指定是int，double或float
class complex
{
public:
  complex (T r = 0, T i = 0) : re (r), im (i) { }
  complex& operator += (const complex&);
  T real () const { return re; }
  T imag () const { return im; }

private:
  T re, im;
  friend complex& __doapl (complex*, const complex&); 
};

//调用
complex<double> c1(2.5,1.5);  //这时再指定变量类型
complex<int> c2(2,6);
```

函数模板：函数只是一个半成品，只定义功能，而不需要管进来和返回的是什么类，之后再指定。使用时函数只用传递进相应的类型就可以实现功能输出。

![Alt text](image-25.png)

成员模板：在类中的成员函数接收不同于类模板类型的模板，其在标准库中的构造函数中比较常见，主要是增加弹性，如下面代码可以实现子对象拷贝构造一个父对象。

![Alt text](image-30.png)

### 委托和复合
* 委托：我类中有一个指针指向另一个类，但是这个类什么时候创建，里面有什么和我关系不大。
* 复合：在我这个类里创建了一个另一个类的对象，并调用这个对象中的功能来实现我的一些功能。
* 委托是虚的，提供了一个指针接口，只有当另一个类实例化了对象，来调用我的函数实现我内部的操作（两个类的对象创建没有直接的依赖关系，可以不同时出现）。复合是实的，先创建被复合的对象，再创建我的对象（一个嵌套关系，queue和deque的关系）；

### 一份数据多份访问的代码（委托）
![Alt text](image-26.png)

### win下的文件系统
![Alt text](image-27.png)



### 转换函数
```c++
class Fraction{
public:
  Fraction(int a, int b) : m_a(a), m_b(b) {}
  operator double() const {
    return (double) (a/b);
  }
private:
  int m_a;
  int m_b;
}

Fraction f(3, 5);
double d = 4 + f; //其含义为：编译器寻找上面的f的定义，发现返回double类型，4也可以是double，因此将double类型的f和double的4相加。
```

关于该知识点可以看侯捷pdf这里：

![Alt text](image-28.png)

### 迭代器和仿函数
![Alt text](image-29.png)

```c++
//考虑上图中的使用方法：
list<Foo>::iterator tie;  
*tie;  //需要返回节点的data
tie->method();  //需要返回节点的指针
```



