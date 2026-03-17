#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <limits>

using namespace std;

class Product
{
private:

    static int nextId;

    int id;
    string name;
    string category;
    string seller;

    double price;
    double rating;

    int stock;

    double discountPercent;

public:

    Product()
    {
        id = 0;
        price = 0;
        rating = 0;
        stock = 0;
        discountPercent = 0;
    }

    Product(string n,string cat,string s,double p,int st,double r=0,double d=0)
    {
        id = nextId++;

        name = n;
        category = cat;
        seller = s;

        price = p;
        stock = st;
        rating = r;

        discountPercent = d;
    }

    int getId() const { return id; }
    string getName() const { return name; }
    string getCategory() const { return category; }
    string getSeller() const { return seller; }

    double getPrice() const { return price; }
    double getRating() const { return rating; }

    int getStock() const { return stock; }

    double getDiscount() const { return discountPercent; }

    void setStock(int s){ stock=s; }

    void setDiscount(double d){ discountPercent=d; }

    double getDiscountedPrice() const
    {
        return price - (price * discountPercent / 100);
    }

    bool operator<(const Product& other) const
    {
        return getDiscountedPrice() < other.getDiscountedPrice();
    }

    bool operator>(const Product& other) const
    {
        return getDiscountedPrice() > other.getDiscountedPrice();
    }

    bool operator==(const Product& other) const
    {
        return id == other.id;
    }

    void display() const
    {
        cout<<fixed<<setprecision(2);

        cout<<"["<<setw(4)<<id<<"] "
            <<setw(20)<<left<<name
            <<" MRP:"<<setw(8)<<price
            <<" Discount:"<<setw(4)<<discountPercent<<"%"
            <<" Final:"<<setw(8)<<getDiscountedPrice()
            <<" Rating:"<<rating
            <<" Stock:"<<stock
            <<" Seller:"<<seller
            <<endl;
    }
};

int Product::nextId = 1001;

/**********************************************************************
 ORDER CLASS
**********************************************************************/

class Order
{
private:

    static int orderCounter;

    int orderId;
    string buyerName;

    vector<pair<Product,int>> items;

    double total;

public:

    Order(string buyer,vector<pair<Product,int>> cart)
    {
        orderId = orderCounter++;

        buyerName = buyer;
        items = cart;

        total = 0;

        for(auto &i:items)
            total += i.first.getDiscountedPrice() * i.second;
    }

    void display() const
    {
        cout<<"\nOrder #"<<orderId<<" Buyer:"<<buyerName<<endl;

        for(auto &i:items)
        {
            cout<<i.first.getName()
                <<" x"<<i.second
                <<" = Rs."
                <<i.first.getDiscountedPrice()*i.second
                <<endl;
        }

        cout<<"Total: Rs."<<total<<endl;
    }
};

int Order::orderCounter = 5001;

/**********************************************************************
 SEARCH FUNCTION
**********************************************************************/

void searchProduct(const vector<Product>& catalog,string keyword)
{
    bool found=false;

    transform(keyword.begin(),keyword.end(),keyword.begin(),::tolower);

    for(auto &p:catalog)
    {
        string name=p.getName();

        transform(name.begin(),name.end(),name.begin(),::tolower);

        if(name.find(keyword)!=string::npos)
        {
            p.display();
            found=true;
        }
    }

    if(!found)
        cout<<"Product not found\n";
}

/**********************************************************************
 SORT BY PRICE
**********************************************************************/

void sortByPrice(vector<Product>& catalog,bool asc=true)
{
    if(asc)
        sort(catalog.begin(),catalog.end());

    else
        sort(catalog.begin(),catalog.end(),greater<Product>());
}

/**********************************************************************
 TOP RATED PRODUCTS
**********************************************************************/

void topRated(vector<Product> catalog)
{
    sort(catalog.begin(),catalog.end(),
    [](Product a,Product b)
    {
        return a.getRating()>b.getRating();
    });

    cout<<"\nTop Rated Products\n";

    for(int i=0;i<5 && i<catalog.size();i++)
        catalog[i].display();
}

/**********************************************************************
 FIND CHEAPEST PRODUCT
**********************************************************************/

Product* findCheapest(vector<Product>& catalog)
{
    Product* cheapest=nullptr;

    for(auto &p:catalog)
    {
        if(!cheapest ||
           p.getDiscountedPrice()<cheapest->getDiscountedPrice())

            cheapest=&p;
    }

    return cheapest;
}

/**********************************************************************
 PRODUCT COMPARISON
**********************************************************************/

void compareProducts(vector<Product>& catalog)
{
    int id1,id2;

    cout<<"Enter first product ID: ";
    cin>>id1;

    cout<<"Enter second product ID: ";
    cin>>id2;

    Product *p1=nullptr,*p2=nullptr;

    for(auto &p:catalog)
    {
        if(p.getId()==id1) p1=&p;

        if(p.getId()==id2) p2=&p;
    }

    if(!p1 || !p2)
    {
        cout<<"Products not found\n";
        return;
    }

    p1->display();
    p2->display();

    if(*p1 < *p2)
        cout<<p1->getName()<<" is cheaper after discount\n";

    else if(*p2 < *p1)
        cout<<p2->getName()<<" is cheaper after discount\n";

    else
        cout<<"Both have same price\n";
}

/**********************************************************************
 USER BASE CLASS
**********************************************************************/

class User
{
protected:

    string username;
    string password;

public:

    User(string u,string p)
    {
        username=u;
        password=p;
    }

    virtual void showMenu(vector<Product>& catalog,
                          vector<Order>& orders)=0;
};

/**********************************************************************
 BUYER CLASS
**********************************************************************/

class Buyer : public User
{
private:

    vector<pair<Product,int>> cart;

public:

    Buyer(string u,string p) : User(u,p) {}

    void browse(vector<Product>& catalog)
    {
        for(auto &p:catalog)
            p.display();
    }

    void addToCart(vector<Product>& catalog)
    {
        int id,qty;

        cout<<"Product ID:";
        cin>>id;

        cout<<"Quantity:";
        cin>>qty;

        for(auto &p:catalog)
        {
            if(p.getId()==id && p.getStock()>=qty)
            {
                cart.push_back({p,qty});

                cout<<"Added to cart\n";
                return;
            }
        }

        cout<<"Product not available\n";
    }

    void viewFinalPrice()
    {
        if(cart.empty())
        {
            cout<<"Cart empty\n";
            return;
        }

        double total=0;

        cout<<"\nCart Summary\n";

        for(auto &i:cart)
        {
            double itemTotal =
            i.first.getDiscountedPrice()*i.second;

            cout<<i.first.getName()
                <<" x"<<i.second
                <<" = Rs."<<itemTotal
                <<endl;

            total += itemTotal;
        }

        cout<<"--------------------\n";
        cout<<"Final Price: Rs."<<total<<endl;
    }

    void checkout(vector<Product>& catalog,
                  vector<Order>& orders)
    {
        if(cart.empty())
        {
            cout<<"Cart empty\n";
            return;
        }

        for(auto &i:cart)
        {
            for(auto &p:catalog)
            {
                if(p.getId()==i.first.getId())
                    p.setStock(p.getStock()-i.second);
            }
        }

        orders.push_back(Order(username,cart));

        cart.clear();

        cout<<"Order placed successfully\n";
    }

    void showMenu(vector<Product>& catalog,
                  vector<Order>& orders)
    {
        int c;

        while(true)
        {
            cout<<"\nBUYER MENU\n";

            cout<<"1 Browse Products\n";
            cout<<"2 Search\n";
            cout<<"3 Add To Cart\n";
            cout<<"4 Sort By Price\n";
            cout<<"5 Top Rated\n";
            cout<<"6 Cheapest Product\n";
            cout<<"7 Compare Products\n";
            cout<<"8 View Final Price\n";
            cout<<"9 Checkout\n";
            cout<<"0 Logout\n";

            cin>>c;

            if(c==0) break;

            else if(c==1) browse(catalog);

            else if(c==2)
            {
                string k;
                cin>>k;

                searchProduct(catalog,k);
            }

            else if(c==3)
                addToCart(catalog);

            else if(c==4)
            {
                int s;
                cin>>s;

                sortByPrice(catalog,s==1);
            }

            else if(c==5)
                topRated(catalog);

            else if(c==6)
            {
                Product* p=findCheapest(catalog);

                if(p) p->display();
            }

            else if(c==7)
                compareProducts(catalog);

            else if(c==8)
                viewFinalPrice();

            else if(c==9)
                checkout(catalog,orders);
        }
    }
};

/**********************************************************************
 SELLER CLASS
**********************************************************************/

class Seller : public User
{
public:

    Seller(string u,string p) : User(u,p) {}

    void addProduct(vector<Product>& catalog)
    {
        string name,cat;

        double price,discount;

        int stock;

        cin.ignore(numeric_limits<streamsize>::max(),'\n');

        cout<<"Name:"; getline(cin,name);

        cout<<"Category:"; getline(cin,cat);

        cout<<"Price:"; cin>>price;

        cout<<"Stock:"; cin>>stock;

        cout<<"Discount %:"; cin>>discount;

        catalog.emplace_back(name,cat,username,price,stock,0,discount);

        cout<<"Product added\n";
    }

    void showMenu(vector<Product>& catalog,
                  vector<Order>& orders)
    {
        int c;

        while(true)
        {
            cout<<"\nSELLER MENU\n";

            cout<<"1 Add Product\n";
            cout<<"2 View Catalog\n";
            cout<<"0 Logout\n";

            cin>>c;

            if(c==0) break;

            else if(c==1)
                addProduct(catalog);

            else if(c==2)
                for(auto&p:catalog) p.display();
        }
    }
};

/**********************************************************************
 DEMO PRODUCTS
**********************************************************************/

void seedCatalog(vector<Product>& catalog)
{
    catalog.emplace_back("iPhone 15","Electronics","seller",120000,10,4.8,10);
    catalog.emplace_back("Galaxy S24","Electronics","seller",90000,12,4.6,8);
    catalog.emplace_back("Nike Shoes","Footwear","seller",6000,30,4.2,20);
    catalog.emplace_back("Backpack","Accessories","seller",2000,25,4.1,15);
}

/**********************************************************************
 MAIN PROGRAM
**********************************************************************/

int main()
{
    vector<Product> catalog;

    vector<Order> orders;

    seedCatalog(catalog);

    Buyer buyer("buyer","123");

    Seller seller("seller","123");

    int choice;

    while(true)
    {
        cout<<"\n==== MINI AMAZON ====\n";

        cout<<"1 Buyer Login\n";

        cout<<"2 Seller Login\n";

        cout<<"0 Exit\n";

        cout<<"Enter choice: ";

        cin>>choice;

        if(choice==0)
            break;

        if(choice==1)
            buyer.showMenu(catalog,orders);

        else if(choice==2)
            seller.showMenu(catalog,orders);
    }

    return 0;
}
