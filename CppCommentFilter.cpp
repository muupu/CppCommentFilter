#include   <iostream>   
#include   <fstream>   
#include   <string>   
using   namespace   std;   

class   CppCommentFilter   
{   
public:   
        CppCommentFilter(istream *  i=0, ostream *  o1=0, ostream * o2=0);   
        //                ����Դ��         �ļ����Դ��    ע�����Դ   
        bool   Work();   
        string::size_type   LineNumber()   const;   
        //���ִ���ʱ��������   
private:   
        istream   *from;   
        ostream   *to1,*to2;   
        string   Line;   
        string::size_type   pos,size,LineNum;   
        enum   Status{Normal,DivStart,DivDiv,Quotation,DoubleQuotation,Finish};   
        /*   
        ö��ֵ����     ����    /*      //        '             "         ����    ״̬   
        */   
        Status   S;   
        void   NewLine();   
        void   NormalFunc();    //����״̬   
        void   DivStartFunc();  //����  /*��״̬   
        void   DivDivFunc();    //����  //��״̬   
        void   QuotationFunc();//����  '��״̬   
        void   DoubleQuotationFunc();//����   "��״̬   
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
                if(S==Finish)   return   false;//�ļ���ע���д���   
                if(!*from)   return   true;//�ļ�����   
                (this->*array[S])();//�ù��̻��޸�Sֵ   
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

void   CppCommentFilter::NormalFunc()   //����״̬ 
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

void   CppCommentFilter::DivStartFunc()   //����   /*��״̬ 
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

void   CppCommentFilter::DivDivFunc()   //����   //��״̬  
{   
        for(;pos<size;++pos){   
                if(to2)   *to2<<Line[pos];   
        }   
        S=Normal;   
}   

void   CppCommentFilter::QuotationFunc()   //����   '��״̬ 
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

void   CppCommentFilter::DoubleQuotationFunc()   //����   "��״̬ 
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
        ofstream   outf1("ɾ��ע�ͺ�Ĵ���.cpp");   //���ɲ���ע�͵�cpp�ļ�
        ofstream   outf2("ע��.txt");   //����ֻ��ע�͵��ļ�
        
	
        CppCommentFilter   T(&inf,&outf1,&outf2);   
        if(T.Work()==false)   cout<<"error   at   line#"<<T.LineNumber();   //��������ע��������
        return 0;
}
