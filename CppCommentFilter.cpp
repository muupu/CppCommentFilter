#include   <iostream>   
#include   <fstream>   
#include   <string>   
using   namespace   std;   

class   CppCommentFilter   
{   
public:   
        CppCommentFilter(istream *  i=0, ostream *  o1=0, ostream * o2=0);   
        //                输入源，         文件输出源，    注释输出源   
        bool   Work();   
        string::size_type   LineNumber()   const;   
        //出现错误时返回行数   
private:   
        istream   *from;   
        ostream   *to1,*to2;   
        string   Line;   
        string::size_type   pos,size,LineNum;   
        enum   Status{Normal,DivStart,DivDiv,Quotation,DoubleQuotation,Finish};   
        /*   
        枚举值代表     正常    /*      //        '             "         结束    状态   
        */   
        Status   S;   
        void   NewLine();   
        void   NormalFunc();    //正常状态   
        void   DivStartFunc();  //进入  /*的状态   
        void   DivDivFunc();    //进入  //的状态   
        void   QuotationFunc();//进入  '的状态   
        void   DoubleQuotationFunc();//进入   "的状态   
        typedef   void   (CppCommentFilter::*ptrf)();   
        ptrf   array[5];   
};   

CppCommentFilter::CppCommentFilter(istream*  i,ostream*  o1,ostream* o2)   
{   
        from=i;   
        to1=o1;   
        to2=o2;   
        S=Normal;   

        array[0]=&CppCommentFilter::NormalFunc;   
        array[1]=&CppCommentFilter::DivStartFunc;   
        array[2]=&CppCommentFilter::DivDivFunc;   
        array[3]=&CppCommentFilter::QuotationFunc;   
        array[4]=&CppCommentFilter::DoubleQuotationFunc;   
}   

string::size_type   CppCommentFilter::LineNumber()   const   
{   
        return   LineNum;   
}   

bool   CppCommentFilter::Work()   
{   
        LineNum=0;   
        if(!from   ||   !to1   &&   !to2)   return   false;   
        getline(*from,Line);   
        pos=0;   
        size=Line.size();   
        ++LineNum;   
        while(true){   
                if(S==Finish)   return   false;//文件中注释有错误   
                if(!*from)   return   true;//文件结束   
                (this->*array[S])();//该过程会修改S值   
        }
}   

void   CppCommentFilter::NewLine()   
{   
        if(getline(*from,Line)){   
                pos=0;   
                size=Line.size();   
                ++LineNum;   
                if(to1)   *to1<<'\n';   
                if(to2)   *to2<<'\n';   
        }   
}   

void   CppCommentFilter::NormalFunc()   //正常状态 
{   
        if(Line.empty()||pos==size){   
                NewLine();   
        }   
        else{   
                for(;pos<size;++pos){   
                        if(Line[pos]=='/'   &&   pos+1<size){   
                                if(Line[pos+1]=='*'){   
                                        if(to2)   *to2<<Line[pos]<<Line[pos+1];   
                                        pos+=2;   
                                        S=DivStart;   
                                        return;   
                                }   
                                if(Line[pos+1]=='/'){   
                                        if(to2)   *to2<<Line[pos]<<Line[pos+1];   
                                        pos+=2;   
                                        S=DivDiv;   
                                        return;   
                                }   
                        }
                        else   if(Line[pos]=='"'){   
                                if(to1)   *to1<<Line[pos];   
                                ++pos;   
                                S=DoubleQuotation;   
                                return;   
                        }   
                        else   if(Line[pos]=='\''){   
                                if(to1)   *to1<<Line[pos];   
                                ++pos;   
                                S=Quotation;   
                                return;   
                        }   
                        if(to1)   *to1<<Line[pos];   
                }  
        }   
}   

void   CppCommentFilter::DivStartFunc()   //进入   /*的状态 
{   
        for(;pos<size;++pos){   
                if(to2)   *to2<<Line[pos];   
                if(Line[pos]=='*'   &&   pos+1<size   &&   Line[pos+1]=='/'){   
                        if(to2)   *to2<<Line[pos+1];   
                        pos+=2;   
                        S=Normal;   
                        return;   
                }   
        }          
        if(pos==size)   NewLine();   
}   

void   CppCommentFilter::DivDivFunc()   //进入   //的状态  
{   
        for(;pos<size;++pos){   
                if(to2)   *to2<<Line[pos];   
        }   
        S=Normal;   
}   

void   CppCommentFilter::QuotationFunc()   //进入   '的状态 
{   
        if(Line[pos]=='\\'   &&   pos+2<size   &&   Line[pos+2]=='\''){   
                if(to1)   *to1<<Line[pos]<<Line[pos+1]<<Line[pos+2];   
                pos+=3;   
                S=Normal;   
                return;   
        }
        else   if(Line[pos]!='\\'   &&   pos+1<size   &&   Line[pos+1]=='\''){   
                if(to1)   *to1<<Line[pos]<<Line[pos+1];   
                pos+=2;   
                S=Normal;   
                return;   
        }   
        S=Finish;   
} 

void   CppCommentFilter::DoubleQuotationFunc()   //进入   "的状态 
{   
        for(;pos<size;++pos){   
                if(to1)   *to1<<Line[pos];   
                if(Line[pos]=='"'   &&   Line[pos-1]!='\\'){   
                        ++pos;   
                        S=Normal;   
                        return;   
                }   
        }   
        S=Finish;   
}   


int  main()   
{       
        cout << "Enter file name: ";
        string name;
	    cin >> name;
	
        ifstream   inf(name.c_str());   
        ofstream   outf1("删除注释后的代码.cpp");   //生成不含注释的cpp文件
        ofstream   outf2("注释.txt");   //生成只含注释的文件
        
	
        CppCommentFilter   T(&inf,&outf1,&outf2);   
        if(T.Work()==false)   cout<<"error   at   line#"<<T.LineNumber();   //输出错误的注释所在行
        return 0;
}
