#include<string>
#include<vector>
using std::string;
using std::vector;

class SemanticErrorInfo{
public:
    static string errorType[19];        //initial in cpp, include 17 types
    SemanticErrorInfo(){}
    SemanticErrorInfo(int lineNO, string &ObjectName, int errorNo);
    ~SemanticErrorInfo(){}

private:
    int lineNo;
    string ObjectName;
    int errorNo;        //from 1 to 17
};


/*error list, create a global variable to use*/
class SemanticErrorList{    
    void PrintErrorList();
    void AddError(SemanticErrorInfo &error);
private:
    vector<SemanticErrorInfo> errorList;
};

