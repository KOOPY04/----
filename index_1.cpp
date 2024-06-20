#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

// 定義記錄結構
struct Record {
    string student_id;  // 學生ID
    string course_id;   // 課程ID
    string course_name; // 課程名稱
};

// 定義索引條目結構
struct IndexEntry {
    string key;  // 索引鍵值（學生ID或課程ID）
    string data; // 相關資料（課程ID或學生ID）
    long offset; // 在資料檔案中的偏移量
};

// 定義二元搜尋樹結構
struct TreeNode {
    IndexEntry entry;
    TreeNode* left;
    TreeNode* right;

    TreeNode(IndexEntry e) : entry(e), left(nullptr), right(nullptr) {}
};

// 插入二元搜尋樹節點
TreeNode* insert(TreeNode* root, IndexEntry entry) {
    if (root == nullptr) {
        return new TreeNode(entry);
    }
    if (entry.key < root->entry.key) {
        root->left = insert(root->left, entry);
    } else {
        root->right = insert(root->right, entry);
    }
    return root;
}

// 將二元搜尋樹寫入索引文件
void writeTreeToFile(TreeNode* root, ofstream& outFile) {
    if (root == nullptr) {
        return;
    }
    outFile << root->entry.key << " " << root->entry.data << " " << root->entry.offset << '\n';
    writeTreeToFile(root->left, outFile);
    writeTreeToFile(root->right, outFile);
    delete root; // 釋放節點
}

// 根據學生ID建立索引
void createStudentIndex(const vector<string>& dataFiles, const string& indexFile) {
    ofstream indexFp(indexFile);

    if (!indexFp.is_open()) {
        cerr << "無法打開索引文件" << endl;
        exit(EXIT_FAILURE);
    }

    TreeNode* root = nullptr;

    for (const auto& dataFile : dataFiles) {
        ifstream dataFp(dataFile);
        if (!dataFp.is_open()) {
            cerr << "無法打開數據文件: " << dataFile << endl;
            continue;
        }

        string line;
        long offset = 0;
        IndexEntry entry;

        // 逐行讀取資料檔案
        while (getline(dataFp, line)) {
            Record record;
            istringstream iss(line);
            getline(iss, record.student_id, ',');
            getline(iss, record.course_id, ',');
            getline(iss, record.course_name);

            if (record.student_id == "student_id") continue; // 跳過第一行

            entry.key = record.student_id;
            entry.data = record.course_id;
            entry.offset = offset;
            root = insert(root, entry);

            offset += line.length() + 1; // 更新偏移量（包含換行符）
        }

        dataFp.close();
    }

    writeTreeToFile(root, indexFp);
    indexFp.close();
}

// 根據課程ID建立索引
void createCourseIndex(const vector<string>& dataFiles, const string& indexFile) {
    ofstream indexFp(indexFile);

    if (!indexFp.is_open()) {
        cerr << "無法打開索引文件" << endl;
        exit(EXIT_FAILURE);
    }

    unordered_map<string, long> courseIndex; // 課程ID -> 偏移量

    for (const auto& dataFile : dataFiles) {
        ifstream dataFp(dataFile);
        if (!dataFp.is_open()) {
            cerr << "無法打開數據文件: " << dataFile << endl;
            continue;
        }

        string line;
        long offset = 0;

        // 逐行讀取資料檔案
        while (getline(dataFp, line)) {
            Record record;
            istringstream iss(line);
            getline(iss, record.student_id, ',');
            getline(iss, record.course_id, ',');
            getline(iss, record.course_name);

            if (record.student_id == "student_id") continue; // 跳過第一行

            courseIndex[record.course_id] = offset;

            offset += line.length() + 1; // 更新偏移量（包含換行符）
        }

        dataFp.close();
    }

    // 課程ID索引寫入文件
    for (const auto& entry : courseIndex) {
        indexFp << entry.first << " " << entry.second << '\n';
    }

    indexFp.close();
}

// 構造數據文件名稱列表
vector<string> generateDataFileNames(int fileCount) {
    vector<string> dataFiles;
    for (int i = 1; i <= fileCount; i++) {
        stringstream fileNo;
        if (i < 10) {
            fileNo << "data_big5/000" << i;
        } else if (i < 100) {
            fileNo << "data_big5/00" << i;
        } else {
            fileNo << "data_big5/0" << i;
        }
        dataFiles.push_back(fileNo.str());
    }
    return dataFiles;
}

int main() {
    // 讀取多個檔案
    vector<string> dataFiles = generateDataFileNames(466);
    const string studentIndexFile = "student_index.txt"; // 學生ID索引檔案名稱
    const string courseIndexFile = "course_index.txt";   // 課程ID索引檔案名稱

    // 建立學生ID索引
    createStudentIndex(dataFiles, studentIndexFile);
    cout << "學生ID索引建立完成" << endl;

    // 建立課程ID索引
    createCourseIndex(dataFiles, courseIndexFile);
    cout << "課程ID索引建立完成" << endl;

    return 0;
}
