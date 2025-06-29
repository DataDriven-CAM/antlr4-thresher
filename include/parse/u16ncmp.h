#pragma once

namespace std{
    size_t u16ncmp(const char16_t* a, const char16_t* b, size_t n){
        for(size_t i=0;i<n;i++){
            if(a[i]>b[i])return 1;
            else if(a[i]<b[i])return -1;
        }
        if((a[n]>=u'a' && a[n]<=u'z') || (a[n]>=u'A' && a[n]<=u'Z')|| (a[n]>=u'0' && a[n]<=u'9'))return -1;
        std::cout<<"u16ncmp "<<((a[n]>=u'a' && a[n]<=u'z'))<<std::endl;
        return 0;
    };

}


