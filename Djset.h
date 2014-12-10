#include <vector>
#include <opencv2\opencv.hpp>

class djset{
public:
	typedef struct _node{
		int parent;
		int nodeno;
		cv::Point mini;
		cv::Point maxi;
		cv::Point minj;
		cv::Point maxj;
		int count;
		int level;
		bool objcolor;
	}node;

	std::vector<node> arr;
	int rows;
	int cols;

	djset(){
		//default constructor
	}

	void Initialize(int rows, int cols){
		this->rows = rows;
		this->cols = cols;
		arr.resize(rows * cols);
	}

	void Join(int x, int y){
		int a = FindParent(x);
		int b = FindParent(y);
		int root, child;
		if(a == b) return ;
		if(arr[a].level > arr[b].level){
			root = a; child = b;
		}
		else if(arr[a].level < arr[b].level){
			root = b; child = a;
		}
		else{
			root = a; child = b;
			arr[a].level++;
		}

		arr[child].parent = root;
		arr[root].count += arr[child].count;
		if(arr[child].mini.x < arr[root].mini.x) arr[root].mini = arr[child].mini;
		if(arr[child].maxi.x > arr[root].maxi.x) arr[root].maxi = arr[child].maxi;
		if(arr[child].minj.y < arr[root].minj.y) arr[root].minj = arr[child].minj;
		if(arr[child].maxj.y > arr[root].maxj.y) arr[root].maxj = arr[child].maxj;
	}

	int FindParent(int x){
		while(arr[x].parent != x) x = arr[x].parent;
		return x;
	}

	void SetAt(int i, int j , bool objcolor){
		int idx = (i * cols) + j;
		arr[idx].objcolor = objcolor;
		arr[idx].mini = arr[idx].minj = arr[idx].maxi = arr[idx].maxj = cv::Point(j, i);
		arr[idx].count = 1;
		arr[idx].level = 1;
		arr[idx].nodeno = arr[idx].parent = idx;

		if(objcolor){
			if(j-1 > 0) 
				if(arr[idx-1].objcolor) Join(idx, idx-1);
			if(i-1 > 0)
				if(arr[idx-cols].objcolor) Join(idx, idx-cols);
		}
	}
};