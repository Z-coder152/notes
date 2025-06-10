```c++
vector<int> findAnagrams(string s, string p) {
        unordered_map<char,int> map_p;
        unordered_map<char,int> window;
        int left = 0, right = 0;
        int valid = 0;
        vector<int> result;
        for(char c : p){
            map_p[c]++;
        }

        while(right < s.size()){
            char c = s[right];
            right++;
            if(map_p.count(c)){
                window[c]++;
                if(window[c] == map_p[c]){
                    valid++;
                }
            }
            while(right - left >= p.size()){
                if(valid == map_p.size()){
                    result.push_back(left);
                }
                char d = s[left];
                left++;
                if(map_p.count(d)){                                      
                    if(window[d] == map_p[d]){
                        valid--;
                    }
                    window[d]--; 
                }
            
            }            
        }
        return result;
    }
```

